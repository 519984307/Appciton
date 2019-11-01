/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/10/16
 **/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <QFile>
#include <QImage>
#include "Utility.h"
#include <QDebug>
#include <QHash>
#include <QApplication>

// we are using the median cut method to convert true color to 224 color
// most code is migrated from https://rosettacode.org/wiki/Color_quantization/C

#define MAX_LINUX_LOGO_COLORS 224

struct linux_logo {
    int type;       /* always 3, LINUX_LOGO_CLUT224 */
    unsigned int width;
    unsigned int height;
    unsigned int clutsize;
    const unsigned char *clut;  /* always NULL, should be filled in kernel */
    const unsigned char *data;  /* always NULL, should be filled in kernel */
};

#define LINUX_LOGO_CLUT224 3

typedef struct
{
    int w, h;
    unsigned char *pix;
} Image, *ImagePtr;

ImagePtr img_new(int w, int h)
{
    ImagePtr im = reinterpret_cast<ImagePtr>(malloc(sizeof(Image) + h * w * 3));
    im->w = w; im->h = h;
    im->pix = (unsigned char *)(im + 1);
    return im;
}

#define ON_INHEAP   1

typedef struct oct_node_t oct_node_t, *oct_node;
struct oct_node_t
{
    int64_t r, g, b; /* sum of all child node colors */
    int count, heap_idx;
    unsigned char n_kids, kid_idx, flags, depth;
    oct_node kids[8], parent;
};

typedef struct
{
    int alloc, n;
    oct_node *buf;
} node_heap;

static int cmp_node(oct_node a, oct_node b)
{
    if (a->n_kids < b->n_kids)
    {
        return -1;
    }
    if (a->n_kids > b->n_kids)
    {
        return 1;
    }

    int ac = a->count >> a->depth;
    int bc = b->count >> b->depth;
    return ac < bc ? -1 : ac > bc;
}

static void down_heap(node_heap *h, oct_node p)
{
    int n = p->heap_idx;
    while (1)
    {
        int m = n * 2;
        if (m >= h->n)
        {
            break;
        }
        if (m + 1 < h->n && cmp_node(h->buf[m], h->buf[m + 1]) > 0)
        {
            m++;
        }

        if (cmp_node(p, h->buf[m]) <= 0)
        {
            break;
        }

        h->buf[n] = h->buf[m];
        h->buf[n]->heap_idx = n;
        n = m;
    }
    h->buf[n] = p;
    p->heap_idx = n;
}

static void up_heap(node_heap *h, oct_node p)
{
    int n = p->heap_idx;
    oct_node prev;

    while (n > 1)
    {
        prev = h->buf[n / 2];
        if (cmp_node(p, prev) >= 0)
        {
            break;
        }

        h->buf[n] = prev;
        prev->heap_idx = n;
        n /= 2;
    }
    h->buf[n] = p;
    p->heap_idx = n;
}

static void heap_add(node_heap *h, oct_node p)
{
    if ((p->flags & ON_INHEAP))
    {
        down_heap(h, p);
        up_heap(h, p);
        return;
    }

    p->flags |= ON_INHEAP;
    if (!h->n)
    {
        h->n = 1;
    }
    if (h->n >= h->alloc)
    {
        while (h->n >= h->alloc)
        {
            h->alloc += 1024;
        }
        h->buf = reinterpret_cast<oct_node *>(realloc(h->buf, sizeof(oct_node) * h->alloc));
    }

    p->heap_idx = h->n;
    h->buf[h->n++] = p;
    up_heap(h, p);
}

static oct_node pop_heap(node_heap *h)
{
    if (h->n <= 1)
    {
        return 0;
    }

    oct_node ret = h->buf[1];
    h->buf[1] = h->buf[--h->n];

    h->buf[h->n] = 0;

    h->buf[1]->heap_idx = 1;
    down_heap(h, h->buf[1]);

    return ret;
}

static oct_node pool = 0;
static int len = 0;
static oct_node node_new(unsigned char idx, unsigned char depth, oct_node p)
{
    if (len <= 1)
    {
        oct_node p = reinterpret_cast<oct_node>(calloc(sizeof(oct_node_t), 2048));
        p->parent = pool;
        pool = p;
        len = 2047;
    }

    oct_node x = pool + len--;
    x->kid_idx = idx;
    x->depth = depth;
    x->parent = p;
    if (p)
    {
        p->n_kids++;
    }
    return x;
}

static void node_free()
{
    oct_node p;
    while (pool)
    {
        p = pool->parent;
        free(pool);
        pool = p;
    }
}

static oct_node node_insert(oct_node root, unsigned char *pix)
{
    unsigned char bit, depth = 0;

    for (bit = 1 << 7; ++depth < 8; bit >>= 1)
    {
        unsigned char i = !!(pix[1] & bit) * 4 + !!(pix[0] & bit) * 2 + !!(pix[2] & bit);
        if (!root->kids[i])
        {
            root->kids[i] = node_new(i, depth, root);
        }

        root = root->kids[i];
    }

    root->r += pix[0];
    root->g += pix[1];
    root->b += pix[2];
    root->count++;
    return root;
}

static oct_node node_fold(oct_node p)
{
    if (p->n_kids)
    {
        abort();
    }
    oct_node q = p->parent;
    q->count += p->count;

    q->r += p->r;
    q->g += p->g;
    q->b += p->b;
    q->n_kids--;
    q->kids[p->kid_idx] = 0;
    return q;
}

static void color_replace(oct_node root, unsigned char *pix)
{
    unsigned char bit;

    for (bit = 1 << 7; bit; bit >>= 1)
    {
        unsigned char i = !!(pix[1] & bit) * 4 + !!(pix[0] & bit) * 2 + !!(pix[2] & bit);
        if (!root->kids[i])
        {
            break;
        }
        root = root->kids[i];
    }

    pix[0] = root->r;
    pix[1] = root->g;
    pix[2] = root->b;
}

static oct_node nearest_color(int *v, node_heap *h)
{
    int i;
    int max = 100000000;
    oct_node o = 0;
    for (i = 1; i < h->n; i++)
    {
        int diff = 3 * abs(h->buf[i]->r - v[0])
                  + 5 * abs(h->buf[i]->g - v[1])
                  + 2 * abs(h->buf[i]->b - v[2]);
        if (diff < max)
        {
            max = diff;
            o = h->buf[i];
        }
    }
    return o;
}

static void error_diffuse(ImagePtr im, node_heap *h)
{
#define POS(i, j) (3 * ((i) * im->w + (j)))
    int i, j;
    int *npx = reinterpret_cast<int *>(calloc(sizeof(int), im->h * im->w * 3)), *px;
    int v[3];
    unsigned char *pix = im->pix;
    oct_node nd;

#define C10 7
#define C01 5
#define C11 2
#define C00 1
#define CTOTAL (C00 + C11 + C10 + C01)

    for (px = npx, i = 0; i < im->h; i++)
    {
        for (j = 0; j < im->w; j++, pix += 3, px += 3)
        {
            px[0] = static_cast<int>(pix[0]) * CTOTAL;
            px[1] = static_cast<int>(pix[1]) * CTOTAL;
            px[2] = static_cast<int>(pix[2]) * CTOTAL;
        }
    }
#define clamp(x, i) if (x[i] > 255) x[i] = 255; if (x[i] < 0) x[i] = 0
    pix = im->pix;
    for (px = npx, i = 0; i < im->h; i++)
    {
        for (j = 0; j < im->w; j++, pix += 3, px += 3)
        {
            px[0] /= CTOTAL;
            px[1] /= CTOTAL;
            px[2] /= CTOTAL;
            clamp(px, 0);
            clamp(px, 1);
            clamp(px, 2);

            nd = nearest_color(px, h);

            v[0] = px[0] - nd->r;
            v[1] = px[1] - nd->g;
            v[2] = px[2] - nd->b;

            pix[0] = nd->r;
            pix[1] = nd->g;
            pix[2] = nd->b;
            if (j < im->w - 1)
            {
                npx[POS(i, j + 1) + 0] += v[0] * C10;
                npx[POS(i, j + 1) + 1] += v[1] * C10;
                npx[POS(i, j + 1) + 2] += v[2] * C10;
            }
            if (i >= im->h - 1)
            {
                continue;
            }

            npx[POS(i + 1, j) + 0] += v[0] * C01;
            npx[POS(i + 1, j) + 1] += v[1] * C01;
            npx[POS(i + 1, j) + 2] += v[2] * C01;

            if (j < im->w - 1)
            {
                npx[POS(i + 1, j + 1) + 0] += v[0] * C11;
                npx[POS(i + 1, j + 1) + 1] += v[1] * C11;
                npx[POS(i + 1, j + 1) + 2] += v[2] * C11;
            }
            if (j)
            {
                npx[POS(i + 1, j - 1) + 0] += v[0] * C00;
                npx[POS(i + 1, j - 1) + 1] += v[1] * C00;
                npx[POS(i + 1, j - 1) + 2] += v[2] * C00;
            }
        }
    }
    free(npx);
}

static void color_quant(ImagePtr im, int n_colors, int dither)
{
    int i;
    unsigned char *pix = im->pix;
    node_heap heap = { 0, 0, 0 };

    oct_node root = node_new(0, 0, 0), got;
    for (i = 0; i < im->w * im->h; i++, pix += 3)
    {
        heap_add(&heap, node_insert(root, pix));
    }

    while (heap.n > n_colors + 1)
    {
        heap_add(&heap, node_fold(pop_heap(&heap)));
    }

    for (i = 1; i < heap.n; i++)
    {
        got = heap.buf[i];
        double c = got->count;
        got->r = got->r / c + .5;
        got->g = got->g / c + .5;
        got->b = got->b / c + .5;
    }

    if (dither)
    {
        error_diffuse(im, &heap);
    }
    else
    {
        for (i = 0, pix = im->pix; i < im->w * im->h; i++, pix += 3)
        {
            color_replace(root, pix);
        }
    }

    node_free();
    free(heap.buf);
    len = 0;
}

namespace Util {

/**
 * @brief generateKernelLogo convert the logo image to 224 color levels
 *        and save in the logo file
 * @param file the file to store the data
 * @param logo the image logo
 */
bool generateKernelLogo(QFile *logoFile, const QImage &logoImage)
{
    if (logoImage.isNull())
    {
        return false;
    }

    int w = logoImage.width();
    int h = logoImage.height();

    ImagePtr im = ::img_new(w, h);
    unsigned char *pix = im->pix;

    /* get the image data */
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++, pix += 3)
        {
           QRgb rgb = logoImage.pixel(j, i);
           pix[0] = qRed(rgb);
           pix[1] = qGreen(rgb);
           pix[2] = qBlue(rgb);
        }
    }

    ::color_quant(im, MAX_LINUX_LOGO_COLORS, 0);

    QRgb logoClut[MAX_LINUX_LOGO_COLORS];
    int logoClutsize = 0;
    QHash<QRgb, int> indexs;

    /* validate image and generate the clut table */
    pix = im->pix;
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++, pix += 3)
        {
            QRgb rgb = qRgb(pix[0], pix[1], pix[2]);
            int k = 0;
            for (; k < logoClutsize; k++)
            {
                if (logoClut[k] == rgb)
                {
                    break;
                }
            }

            if (k == logoClutsize)
            {
                if (logoClutsize == MAX_LINUX_LOGO_COLORS)
                {
                    qWarning("Logo has more than %d colors after quantization!", MAX_LINUX_LOGO_COLORS);
                    return false;
                }

                logoClut[logoClutsize] = qRgb(pix[0], pix[1], pix[2]);
                indexs.insert(logoClut[logoClutsize], logoClutsize);
                logoClutsize++;
            }
        }
        QApplication::processEvents();
    }


    linux_logo logoheader;
    logoheader.type = LINUX_LOGO_CLUT224;
    logoheader.width = w;
    logoheader.height = h;
    logoheader.clutsize = logoClutsize;
    logoheader.clut = NULL;
    logoheader.data = NULL;

    if (!logoFile->seek(0))
    {
        qDebug() << "Seek fail!";
        return false;
    }

    /* write linux logo info data */
    if (logoFile->write(reinterpret_cast<char *>(&logoheader), sizeof(linux_logo)) != sizeof(linux_logo))
    {
        qDebug() << "Incomplete write!";
        return false;
    }

    /* write the pixel data */
    pix = im->pix;
    for (int i = 0; i < h; i++)
    {
        QByteArray pixelIndexData(w, 0x20);
        for (int j = 0; j < w; j++, pix += 3)
        {
           QRgb rgb = qRgb(pix[0], pix[1], pix[2]);
           int index = indexs.value(rgb, 0);
           pixelIndexData[j] = index + 0x20;
        }

        if (logoFile->write(pixelIndexData.constData(), pixelIndexData.length()) != pixelIndexData.length())
        {
            qDebug() << "Incomplete write!";
            return false;
        }
        QApplication::processEvents();
    }

    /* write clut data */
    QByteArray clutdata;
    for (int i = 0; i < logoClutsize; i++)
    {
        clutdata.append(qRed(logoClut[i]));
        clutdata.append(qGreen(logoClut[i]));
        clutdata.append(qBlue(logoClut[i]));
    }

    if (logoFile->write(clutdata.constData(), clutdata.length()) != clutdata.length())
    {
        qDebug() << "Incomplete write!";
        return false;
    }

    free(im);
    return true;
}

}   // namespace Util
