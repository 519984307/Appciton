/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/9/9
 **/


#include "TSCalibrationWindow.h"
#include <QDesktopWidget>
#include <QApplication>
#include <QThread>
#include <Debug.h>
#include <tslib.h>
#include <QFile>
#include <QTextStream>
#include <QPainter>
#include "FontManager.h"
#include "LanguageManager.h"
#include <QFontMetrics>
#include <QVector>
#include <errno.h>

#define DEF_TS_POINTERCAL "/etc/pointercal"
#define CROSSHAIR_FRAME_COLOR "#E0C0A0"

struct Calibration {
    int x[5], xfb[5];
    int y[5], yfb[5];
    int a[7];
};

static int perform_calibration(Calibration *cal)
{
    int j;
    float n, x, y, x2, y2, xy, z, zx, zy;
    float det, a, b, c, e, f, i;
    float scaling = 65536.0;

    /* Get sums for matrix */
    n = x = y = x2 = y2 = xy = 0;
    for (j = 0; j < 5; j++)
    {
        n += 1.0;
        x += static_cast<float>(cal->x[j]);
        y += static_cast<float>(cal->y[j]);
        x2 += static_cast<float>(cal->x[j]*cal->x[j]);
        y2 += static_cast<float>(cal->y[j]*cal->y[j]);
        xy += static_cast<float>(cal->x[j]*cal->y[j]);
    }

    /* Get determinant of matrix -- check if determinant is too small */
    det = n*(x2*y2 - xy*xy) + x*(xy*y - x*y2) + y*(x*xy - y*x2);
    if (det < 0.1 && det > -0.1) {
        qdebug("determinant is too small -- %f\n", det);
        return 0;
    }

    /* Get elements of inverse matrix */
    a = (x2*y2 - xy*xy)/det;
    b = (xy*y - x*y2)/det;
    c = (x*xy - y*x2)/det;
    e = (n*y2 - y*y)/det;
    f = (x*y - n*xy)/det;
    i = (n*x2 - x*x)/det;

    /* Get sums for x calibration */
    z = zx = zy = 0;
    for (j = 0; j < 5; j++) {
        z += static_cast<float>(cal->xfb[j]);
        zx += static_cast<float>(cal->xfb[j]*cal->x[j]);
        zy += static_cast<float>(cal->xfb[j]*cal->y[j]);
    }

    /* Now multiply out to get the calibration for framebuffer x coord */
    cal->a[0] = static_cast<int>((a*z + b*zx + c*zy)*(scaling));
    cal->a[1] = static_cast<int>((b*z + e*zx + f*zy)*(scaling));
    cal->a[2] = static_cast<int>((c*z + f*zx + i*zy)*(scaling));

    qdebug("%f %f %f\n", (a*z + b*zx + c*zy),
                 (b*z + e*zx + f*zy),
                 (c*z + f*zx + i*zy));

    /* Get sums for y calibration */
    z = zx = zy = 0;
    for (j = 0; j < 5; j++) {
        z += static_cast<float>(cal->yfb[j]);
        zx += static_cast<float>(cal->yfb[j]*cal->x[j]);
        zy += static_cast<float>(cal->yfb[j]*cal->y[j]);
    }

    /* Now multiply out to get the calibration for framebuffer y coord */
    cal->a[3] = static_cast<int>((a*z + b*zx + c*zy)*(scaling));
    cal->a[4] = static_cast<int>((b*z + e*zx + f*zy)*(scaling));
    cal->a[5] = static_cast<int>((c*z + f*zx + i*zy)*(scaling));

    qdebug("%f %f %f\n", (a*z + b*zx + c*zy),
                 (b*z + e*zx + f*zy),
                 (c*z + f*zx + i*zy));

    /* If we got here, we're OK, so assign scaling to a[6] and return */
    cal->a[6] = static_cast<int>(scaling);

    return 1;
}

static int sort_by_x(const void* a, const void *b)
{
    return (((struct ts_sample *)a)->x - ((struct ts_sample *)b)->x);
}

static int sort_by_y(const void* a, const void *b)
{
    return (((struct ts_sample *)a)->y - ((struct ts_sample *)b)->y);
}

/* Waits for the screen to be touched, averages x and y sample
 * coordinates until the end of contact */
static void getxy(tsdev *ts, int *x, int *y)
{
#define MAX_SAMPLES 128
    struct ts_sample samp[MAX_SAMPLES];
    int index, middle;

    do {
        if (ts_read_raw(ts, &samp[0], 1) < 0) {
            perror("ts_read");
            exit(1);
        }
    } while (samp[0].pressure == 0);

    /* Now collect up to MAX_SAMPLES touches into the samp array. */
    index = 0;
    do {
        if (index < MAX_SAMPLES-1)
            index++;
        if (ts_read_raw(ts, &samp[index], 1) < 0) {
            perror("ts_read");
            exit(1);
        }
    } while (samp[index].pressure > 0);
    qdebug("Took %d samples...\n", index);

    /*
     * At this point, we have samples in indices zero to (index-1)
     * which means that we have (index) number of samples.  We want
     * to calculate the median of the samples so that wild outliers
     * don't skew the result.  First off, let's assume that arrays
     * are one-based instead of zero-based.  If this were the case
     * and index was odd, we would need sample number ((index+1)/2)
     * of a sorted array; if index was even, we would need the
     * average of sample number (index/2) and sample number
     * ((index/2)+1).  To turn this into something useful for the
     * real world, we just need to subtract one off of the sample
     * numbers.  So for when index is odd, we need sample number
     * (((index+1)/2)-1).  Due to integer division truncation, we
     * can simplify this to just (index/2).  When index is even, we
     * need the average of sample number ((index/2)-1) and sample
     * number (index/2).  Calculate (index/2) now and we'll handle
     * the even odd stuff after we sort.
     */
    middle = index/2;
    if (x) {
        ::qsort(samp, index, sizeof(struct ts_sample), sort_by_x);
        if (index & 1)
            *x = samp[middle].x;
        else
            *x = (samp[middle-1].x + samp[middle].x) / 2;
    }
    if (y) {
        ::qsort(samp, index, sizeof(struct ts_sample), sort_by_y);
        if (index & 1)
            *y = samp[middle].y;
        else
            *y = (samp[middle-1].y + samp[middle].y) / 2;
    }
}

static void clearTsBuf(tsdev *ts)
{
    int fd = ts_fd(ts);
    fd_set fdset;
    struct timeval tv;
    struct ts_sample sample;

    while (1) {
        FD_ZERO(&fdset);
        FD_SET(fd, &fdset);

        tv.tv_sec = 0;
        tv.tv_usec = 0;

        int nfds = select(fd + 1, &fdset, NULL, NULL, &tv);
        if (nfds == 0)
            break;

        if (ts_read_raw(ts, &sample, 1) < 0) {
            perror("ts_read_raw");
            return;
        }
    }
}

class TSCalibrationWindowPrivate
{
public:
    enum CalibrationPoint {
        TOP_LEFT = 0,
        TOP_RIGHT,
        BOTTOM_RIGHT,
        BOTTOM_LEFT,
        CENTER,
        CALBRATION_POINT_NR,
    };

    explicit TSCalibrationWindowPrivate(TSCalibrationWindow * const q_ptr)
        : q_ptr(q_ptr),
          calPoint(TOP_LEFT),
          thread(NULL)
    {
        memset(&cal, 0, sizeof(Calibration));
    }

    void drawCrosshair(QPainter &painter, int x, int y);
    QPoint point(CalibrationPoint calPoint);
    QString pointName(CalibrationPoint calPoint);

    void saveResult();

    TSCalibrationWindow * const q_ptr;
    Calibration cal;
    CalibrationPoint calPoint;
    TSCalibrationThread *thread;
};

void TSCalibrationWindowPrivate::drawCrosshair(QPainter &painter, int x, int y)
{
    QVector<QLine> lines;
    lines.append(QLine(x - 10, y, x - 2, y));
    lines.append(QLine(x + 2, y, x + 10, y));
    lines.append(QLine(x, y - 10, x, y - 2));
    lines.append(QLine(x, y + 2, x, y + 10));
    painter.drawLines(lines);


    lines.clear();
    painter.save();
    painter.setPen(QColor(CROSSHAIR_FRAME_COLOR));
    lines.append(QLine(x - 6, y - 9, x - 9, y - 9));
    lines.append(QLine(x - 9, y - 8, x - 9, y - 6));
    lines.append(QLine(x - 9, y + 6, x - 9, y + 9));
    lines.append(QLine(x - 8, y + 9, x - 6, y + 9));
    lines.append(QLine(x + 6, y + 9, x + 9, y + 9));
    lines.append(QLine(x + 9, y + 8, x + 9, y + 6));
    lines.append(QLine(x + 9, y - 6, x + 9, y - 9));
    lines.append(QLine(x + 8, y - 9, x + 6, y - 9));
    painter.drawLines(lines);
    painter.restore();
}

QPoint TSCalibrationWindowPrivate::point(TSCalibrationWindowPrivate::CalibrationPoint calPoint)
{
    switch (calPoint)
    {
    case TOP_LEFT:
        return QPoint(50, 50);
        break;
    case TOP_RIGHT:
        return QPoint(q_ptr->width() - 50, 50);
        break;
    case BOTTOM_RIGHT:
        return QPoint(q_ptr->width() - 50, q_ptr->height() - 50);
        break;
    case BOTTOM_LEFT:
        return QPoint(50,  q_ptr->height() - 50);
        break;
    case CENTER:
        return QPoint(q_ptr->width() / 2, q_ptr->height() / 2);
        break;
    default:
        return QPoint();
        break;
    }
}

QString TSCalibrationWindowPrivate::pointName(TSCalibrationWindowPrivate::CalibrationPoint calPoint)
{
    switch (calPoint)
    {
    case TOP_LEFT:
        return "Top Left";
    case TOP_RIGHT:
        return "Top Right";
    case BOTTOM_RIGHT:
        return "Bottom Right";
    case BOTTOM_LEFT:
        return "Bottom Left";
    case CENTER:
        return "Center";
    default:
        return "Unknown";
    }
}


void TSCalibrationWindowPrivate::saveResult()
{
    const char *calfile = getenv("TSLIB_CALIBFILE");
    if (calfile == NULL)
    {
        calfile = DEF_TS_POINTERCAL;
    }

    QFile file(calfile);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&file);
        stream << cal.a[1] << " " << cal.a[2] << " " << cal.a[0] <<" ";
        stream << cal.a[4] << " " << cal.a[5] << " " << cal.a[3] <<" ";
        stream << cal.a[6] << " ";
        stream << q_ptr->width() << " " << q_ptr->height();
        stream.flush();
        file.close();
    }
    else
    {
        qdebug("Write calibration file failed.");
    }
}

TSCalibrationWindow::TSCalibrationWindow()
    :QDialog(NULL, Qt::FramelessWindowHint),
      d_ptr(new TSCalibrationWindowPrivate(this))
{
    // full screen dialog
    this->resize(qApp->desktop()->screen()->size());
    move(0, 0);
    QPalette pal = this->palette();
    pal.setBrush(QPalette::Window, Qt::black);
    pal.setBrush(QPalette::WindowText, Qt::white);
    setPalette(pal);
    setAutoFillBackground(true);

    QFont font = fontManager.textFont(24);
    setFont(font);

    d_ptr->thread = new TSCalibrationThread(this);
    connect(d_ptr->thread, SIGNAL(getTouchPos(QPoint)), this, SLOT(onGetTouchPos(QPoint)), Qt::QueuedConnection);
}

TSCalibrationWindow::~TSCalibrationWindow()
{
}

void TSCalibrationWindow::showEvent(QShowEvent *ev)
{
    QDialog::showEvent(ev);
    d_ptr->thread->start();
}

void TSCalibrationWindow::paintEvent(QPaintEvent *ev)
{
    Q_UNUSED(ev);

    QPainter painter(this);

    QString text = trs("TouchCrosshairToCalibrate");
    QFontMetrics fm = fontMetrics();
    QRect textRect = fm.boundingRect(text);
    textRect.moveCenter(QPoint(width() / 2, height() / 4 + 20));
    painter.drawText(textRect, Qt::AlignCenter, text);
    switch (d_ptr->calPoint)
    {
    case TSCalibrationWindowPrivate::TOP_LEFT:
        d_ptr->drawCrosshair(painter, 50, 50);
        break;
    case TSCalibrationWindowPrivate::TOP_RIGHT:
        d_ptr->drawCrosshair(painter, width() - 50, 50);
        break;
    case TSCalibrationWindowPrivate::BOTTOM_RIGHT:
        d_ptr->drawCrosshair(painter, width() - 50, height() -  50);
        break;
    case TSCalibrationWindowPrivate::BOTTOM_LEFT:
        d_ptr->drawCrosshair(painter, 50, height() - 50);
        break;
    case TSCalibrationWindowPrivate::CENTER:
        d_ptr->drawCrosshair(painter, width() / 2, height() / 2);
        break;
    default:
        break;
    }

    QPoint p = d_ptr->point(d_ptr->calPoint);
    if (!p.isNull())
    {
        d_ptr->drawCrosshair(painter, p.x(), p.y());
    }
}

void TSCalibrationWindow::onGetTouchPos(const QPoint &pos)
{
    d_ptr->thread->wait();

    d_ptr->cal.x[d_ptr->calPoint] = pos.x();
    d_ptr->cal.y[d_ptr->calPoint] = pos.y();

    QPoint screenPos = d_ptr->point(d_ptr->calPoint);
    d_ptr->cal.xfb[d_ptr->calPoint] = screenPos.x();
    d_ptr->cal.yfb[d_ptr->calPoint] = screenPos.y();

    qDebug() << d_ptr->pointName(d_ptr->calPoint) << ": " << pos;

    if (d_ptr->calPoint == TSCalibrationWindowPrivate::CENTER)
    {
        // finish get touch point
        if (perform_calibration(&d_ptr->cal))
        {
            QString str;
            for (int i = 0; i<= 6; i++)
            {
                str += QString::number(d_ptr->cal.a[i]);
                str += " ";
            }
            qDebug() << "Result: " << str;
            d_ptr->saveResult();
            done(1);
        }
        else
        {
            done(0);
        }
    }
    else
    {
        // next calibration point
        d_ptr->calPoint = static_cast<TSCalibrationWindowPrivate::CalibrationPoint>(d_ptr->calPoint + 1);
        update();
        d_ptr->thread->start();
    }
}


TSCalibrationThread::TSCalibrationThread(QObject *parent)
    :QThread(parent)
{
}

void TSCalibrationThread::run()
{
    tsdev *ts = ts_setup(NULL, 0);
    if (!ts)
    {
        perror("ts_setup");
        return;
    }

    clearTsBuf(ts);

    int x;
    int y;

    getxy(ts, &x, &y);

    ts_close(ts);

    emit getTouchPos(QPoint(x, y));
}
