#include "Utility.h"
#include "Debug.h"
#include <QImage>
#include <linux/fb.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#define FBDEV_PATH "/dev/fb0"

namespace Util {
#if defined(CONFIG_CAPTURE_SCREEN) && defined(Q_WS_QWS)
QImage captureScreen()
{
    int fd;
    int xres = 0;
    int yres = 0;
    int screenSize = 0;
    int bpp = 0;
    int lineLength;
    struct fb_fix_screeninfo finfo;
    struct fb_var_screeninfo vinfo;
    char * fbp;
    QImage::Format format;
    fd = open(FBDEV_PATH, O_RDONLY);
    if (fd >= 0)
    {
         if (ioctl(fd, FBIOGET_FSCREENINFO, &finfo) < 0 || ioctl(fd, FBIOGET_VSCREENINFO, &vinfo))
         {
             qDebug()<<Q_FUNC_INFO<<"ioctl: FBIOGET_FSCREENNFO or FBIOGET_VSCREENINFO failed.";
             close(fd);
             return QImage();
         }
         xres = vinfo.xres;
         yres = vinfo.yres;
         bpp = vinfo.bits_per_pixel;
         lineLength = finfo.line_length;
         screenSize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

         fbp = (char *) mmap(0, screenSize, PROT_READ, MAP_SHARED, fd, 0);
         if ((reinterpret_cast<long>(fbp) == -1))
         {
             qDebug()<<Q_FUNC_INFO<<"mmap failed.";
             close(fd);
             return QImage();
         }

         switch(bpp)
         {
             case 16:
                 format = QImage::Format_RGB16;
                 break;
             case 24:
                 format = QImage::Format_RGB888;
                 break;
             case 32:
                 format = QImage::Format_ARGB32;
                 break;
             default:
                 format = QImage::Format_Invalid;
                 break;
         }

         QImage screen;
         if(format != QImage::Format_Invalid)
         {
             screen = QImage((const uchar *)fbp, xres, yres, lineLength, format).copy();
         }

         munmap(fbp, screenSize);
         close(fd);
         return screen;
    }
    else
    {
        qDebug()<<Q_FUNC_INFO<<"Open "<<FBDEV_PATH<<" Failed.";
        return QImage();
    }

}
#endif
}
