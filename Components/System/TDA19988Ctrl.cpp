/**
 ** This file is part of the Project project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by lianghuan <lianghuan@blmed.cn>, 2021/3/18
 **/

#include "TDA19988Ctrl.h"
#include "tda19988.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <errno.h>
#include <QDebug>
#include <QTimer>
#include <QProcess>
#include <QApplication>
#include <QDesktopWidget>

#define I2C_BUS_DEV "/dev/i2c-0"
#define BIT(n) (1 << n)
#define EDID_LENGTH 128

typedef quint8 u8;
typedef quint32 u32;

struct DisplayTimings
{
    int pixelClock;
    int hActive;
    int vActive;
    int hFrontPorch;
    int hBackPorch;
    int hSyncLen;
    int vBackPorch;
    int vFrontPorch;
    int vSyncLen;
    int hSyncActive;
    int vSyncActive;
    int doubleScan;
    int doubleClk;
};

enum TimingType {
    TIMING_1280_800,
    TIMING_1024_768,
    TIMING_1366_768,
    TIMING_NR
};

/*
 * NOTE: this setting is similar to the setting in the device tree,
 * see panel setting in am335x-pmos.dts
 * However, there's some different bellow:
 * The hFrontPorch and hBackPorch is swapped
 * The vFrontPorch and vBackPorch is swapped
 * The hSyncActive is toggled
 */
static DisplayTimings supportTimings[TIMING_NR] =
{
    /* 1280x800 */
    {
        .pixelClock = 68200000,
        .hActive = 1280,
        .vActive = 800,
        .hFrontPorch = 5,
        .hBackPorch = 64,
        .hSyncLen = 128,
        .vBackPorch = 40,
        .vFrontPorch = 10,
        .vSyncLen = 6,
        .hSyncActive = 1,
        .vSyncActive = 1,
        .doubleScan = 0,
        .doubleClk = 0,
    },
    /* 1024x768 */
    {
        .pixelClock = 65000000,
        .hActive = 1024,
        .vActive = 768,
        .hFrontPorch = 160,
        .hBackPorch = 112,
        .hSyncLen = 48,
        .vBackPorch = 15,
        .vFrontPorch = 15,
        .vSyncLen = 8,
        .hSyncActive = 1,
        .vSyncActive = 1,
        .doubleScan = 0,
        .doubleClk = 0,
    },
    /* 1366x768 */
    {
        .pixelClock = 75000000,
        .hActive = 1360,
        .vActive = 768,
        .hFrontPorch = 78,
        .hBackPorch = 100,
        .hSyncLen = 4,
        .vBackPorch = 22,
        .vFrontPorch = 20,
        .vSyncLen = 4,
        .hSyncActive = 1,
        .vSyncActive = 1,
        .doubleScan = 0,
        .doubleClk = 0,
    }
};

/* running timings */
static DisplayTimings dispTimings =
{
    .pixelClock = 68200000,
    .hActive = 1280,
    .vActive = 800,
    .hFrontPorch = 5,
    .hBackPorch = 64,
    .hSyncLen = 128,
    .vBackPorch = 40,
    .vFrontPorch = 10,
    .vSyncLen = 6,
    .hSyncActive = 1,
    .vSyncActive = 1,
    .doubleScan = 0,
    .doubleClk = 0,
};

struct HdmiAviInfoframe
{
    int type;
    unsigned char version;
    unsigned char length;
    int colorspace;
    int scan_mode;
    int colorimetry;
    int picture_aspect;
    int active_aspect;
    bool itc;
    int extended_colorimetry;
    int quantization_range;
    int nups;
    unsigned char video_code;
    int ycc_quantization_range;
    int content_type;
    unsigned char pixel_repeat;
    unsigned short top_bar;
    unsigned short bottom_bar;
    unsigned short left_bar;
    unsigned short right_bar;
};

struct est_timings
{
    u8 t1;
    u8 t2;
    u8 mfg_rsvd;
} __attribute__((packed));

struct std_timing
{
    u8 hsize; /* need to multiply by 8 then add 248 */
    u8 vfreq_aspect;
} __attribute__((packed));

struct cvt_timing
{
    u8 code[3];
} __attribute__((packed));

/* If detailed data is pixel timing */
struct detailed_pixel_timing
{
    u8 hactive_lo;
    u8 hblank_lo;
    u8 hactive_hblank_hi;
    u8 vactive_lo;
    u8 vblank_lo;
    u8 vactive_vblank_hi;
    u8 hsync_offset_lo;
    u8 hsync_pulse_width_lo;
    u8 vsync_offset_pulse_width_lo;
    u8 hsync_vsync_offset_pulse_width_hi;
    u8 width_mm_lo;
    u8 height_mm_lo;
    u8 width_height_mm_hi;
    u8 hborder;
    u8 vborder;
    u8 misc;
} __attribute__((packed));


/* If it's not pixel timing, it'll be one of the below */
struct detailed_data_string
{
    u8 str[13];
} __attribute__((packed));

struct detailed_data_monitor_range
{
    u8 min_vfreq;
    u8 max_vfreq;
    u8 min_hfreq_khz;
    u8 max_hfreq_khz;
    u8 pixel_clock_mhz; /* need to multiply by 10 */
    u8 flags;
    union
    {
        struct
        {
            u8 reserved;
            u8 hfreq_start_khz; /* need to multiply by 2 */
            u8 c; /* need to divide by 2 */
            __le16 m;
            u8 k;
            u8 j; /* need to divide by 2 */
        } __attribute__((packed)) gtf2;
        struct
        {
            u8 version;
            u8 data1; /* high 6 bits: extra clock resolution */
            u8 data2; /* plus low 2 of above: max hactive */
            u8 supported_aspects;
            u8 flags; /* preferred aspect and blanking support */
            u8 supported_scalings;
            u8 preferred_refresh;
        } __attribute__((packed)) cvt;
    } formula;
} __attribute__((packed));


struct detailed_data_wpindex
{
    u8 white_yx_lo; /* Lower 2 bits each */
    u8 white_x_hi;
    u8 white_y_hi;
    u8 gamma; /* need to divide by 100 then add 1 */
} __attribute__((packed));

struct detailed_non_pixel
{
    u8 pad1;
    u8 type; /* ff = serial, fe = string, fd = monitor range, fc = monitor name
            fb = color point data, fa = standard timing data,
            f9 = undefined, f8 = mfg. reserved */
    u8 pad2;
    union
    {
        struct detailed_data_string str;
        struct detailed_data_monitor_range range;
        struct detailed_data_wpindex color;
        struct std_timing timings[6];
        struct cvt_timing cvt[4];
    } data;
} __attribute__((packed));

struct detailed_timing
{
    __le16 pixel_clock; /* need to multiply by 10 KHz */
    union
    {
        struct detailed_pixel_timing pixel_data;
        struct detailed_non_pixel other_data;
    } data;
} __attribute__((packed));

struct edid
{
    u8 header[8];
    /* Vendor & product info */
    u8 mfg_id[2];
    u8 prod_code[2];
    u32 serial; /* FIXME: byte order */
    u8 mfg_week;
    u8 mfg_year;
    /* EDID version */
    u8 version;
    u8 revision;
    /* Display info: */
    u8 input;
    u8 width_cm;
    u8 height_cm;
    u8 gamma;
    u8 features;
    /* Color characteristics */
    u8 red_green_lo;
    u8 black_white_lo;
    u8 red_x;
    u8 red_y;
    u8 green_x;
    u8 green_y;
    u8 blue_x;
    u8 blue_y;
    u8 white_x;
    u8 white_y;
    /* Est. timings and mfg rsvd timings*/
    struct est_timings established_timings;
    /* Standard timings 1-8*/
    struct std_timing standard_timings[8];
    /* Detailing timings 1-4 */
    struct detailed_timing detailed_timings[4];
    /* Number of 128 byte ext. blocks */
    u8 extensions;
    /* Checksum */
    u8 checksum;
} __attribute__((packed));

static int i2c_smbus_access(int fd, char read_write, quint8 cmd,
                            int size, union i2c_smbus_data *data)
{
    i2c_smbus_ioctl_data args;
    args.read_write = read_write;
    args.command = cmd;
    args.size = size;
    args.data = data;

    return ioctl(fd, I2C_SMBUS, &args);
}

static int i2c_smbus_read_byte_data(int fd, quint8 cmd)
{
    i2c_smbus_data data;
    int err;
    err = i2c_smbus_access(fd, I2C_SMBUS_READ, cmd, I2C_SMBUS_BYTE_DATA, &data);
    if (err < 0)
    {
        return err;
    }
    return data.byte;
}

static inline int i2c_smbus_write_byte_data(int fd, quint8 cmd, quint8 val)
{
    i2c_smbus_data data;
    data.byte = val;
    return i2c_smbus_access(fd, I2C_SMBUS_WRITE, cmd, I2C_SMBUS_BYTE_DATA, &data);
}

static inline int i2c_smbus_write_word_data(int fd, quint8 cmd, quint16 val)
{
    i2c_smbus_data data;
    data.word = val;
    return i2c_smbus_access(fd, I2C_SMBUS_WRITE, cmd, I2C_SMBUS_WORD_DATA, &data);
}

static inline int i2c_smbus_write_block_data(int fd, quint8 cmd, quint8 length, const quint8 *values)
{
    i2c_smbus_data data;
    if (length > I2C_SMBUS_BLOCK_MAX)
    {
        qWarning("I2C write block size(%d) larger than %d.", length, I2C_SMBUS_BLOCK_MAX);
        length = I2C_SMBUS_BLOCK_MAX;
    }
    memcpy(data.block + 1, values, length);
    data.block[0] = length;
    return i2c_smbus_access(fd, I2C_SMBUS_WRITE, cmd, I2C_SMBUS_I2C_BLOCK_BROKEN, &data);
}

static inline int i2c_smbus_read_i2c_block_data(int fd, quint8 cmd, quint8 length, u8 *values)
{
    i2c_smbus_data data;
    if (length > I2C_SMBUS_BLOCK_MAX)
    {
        qWarning("I2C read block size(%d) larger than %d.", length, I2C_SMBUS_BLOCK_MAX);
        length = I2C_SMBUS_BLOCK_MAX;
    }
    data.block[0] = length;
    int err = i2c_smbus_access(fd, I2C_SMBUS_READ, cmd,
                               length == 32 ? I2C_SMBUS_I2C_BLOCK_BROKEN :
                               I2C_SMBUS_I2C_BLOCK_DATA, &data);
    if (err < 0)
    {
        return err;
    }

    for (int i = 1; i <= data.block[0]; i++)
    {
        *values++ = data.block[i];
    }

    return data.block[0];
}

class TDA19988CtrlPrivate
{
public:
    TDA19988CtrlPrivate()
        : fd(-1), currentSlaveAddr(0), currentPage(0xff), vipCntrl_0(0),
          vipCntrl_1(0), vipCntrl_2(0), hdmiAviInfoframe(NULL), timer(NULL),
          connected(false)
    {
        hdmiAviInfoframe = new HdmiAviInfoframe();

        memset(hdmiAviInfoframe, 0, sizeof(HdmiAviInfoframe));

        hdmiAviInfoframe->type = 0x82;
        hdmiAviInfoframe->version = 2;
        hdmiAviInfoframe->length = 13;
        hdmiAviInfoframe->pixel_repeat = 0;
        hdmiAviInfoframe->video_code = 0;
        hdmiAviInfoframe->picture_aspect = 0;
        hdmiAviInfoframe->active_aspect = 8;
        hdmiAviInfoframe->scan_mode = 2;
        hdmiAviInfoframe->quantization_range = 2;
    }

    ~TDA19988CtrlPrivate()
    {
        delete hdmiAviInfoframe;
    }

    bool setSlaveAddr(quint8 slaveAddr)
    {
        if (slaveAddr == currentSlaveAddr)
        {
            return true;
        }

        if (ioctl(fd, I2C_SLAVE, slaveAddr))
        {
            qCritical() << "set slave address failed: " << strerror(errno);
            return false;
        }
        currentSlaveAddr = slaveAddr;
        return true;
    }

    void cecWrite(quint8 addr, quint8 val)
    {
        setSlaveAddr(CEC_ADDR);
        i2c_smbus_write_byte_data(fd, addr, val);
    }

    char cecRead(quint8 addr)
    {
        setSlaveAddr(CEC_ADDR);
        quint8 val;
        val = i2c_smbus_read_byte_data(fd, addr);
        return val;
    }

    void setPage(quint16 reg)
    {
        setSlaveAddr(HDMI_ADDR);
        if (REG2PAGE(reg) != currentPage)
        {
            i2c_smbus_write_byte_data(fd, REG_CURPAGE, REG2PAGE(reg));
        }
        currentPage = REG2PAGE(reg);
    }

    void regWrite(quint16 reg, quint8 val)
    {
        setPage(reg);
        i2c_smbus_write_byte_data(fd, REG2ADDR(reg), val);
    }

    void regWrite16(quint16 reg, quint16 val)
    {
        setPage(reg);
        // switch to big endian
        quint16 newVal = ((val & 0xff) << 8) | ((val & 0xff00) >> 8);
        i2c_smbus_write_word_data(fd, REG2ADDR(reg), newVal);
    }

    void regWriteRange(quint16 reg, quint8 *buf, quint8 size)
    {
        setPage(reg);
        i2c_smbus_write_block_data(fd, REG2ADDR(reg), size, buf);
    }

    int regRead(quint16 reg)
    {
        setPage(reg);
        return i2c_smbus_read_byte_data(fd, REG2ADDR(reg));
    }

    int regReadRange(quint16 reg, u8 *buf, int cnt)
    {
        setPage(reg);
        u8 addr = REG2ADDR(reg);
        int tmp;
        int left = cnt;
        int curRead = 0;
        for (u8 curaddr = addr; curaddr < addr + cnt && left > 0; curaddr += tmp)
        {
            u8 curLen;
            if (left > I2C_SMBUS_BLOCK_MAX)
            {
                curLen = I2C_SMBUS_BLOCK_MAX;
            }
            else
            {
                curLen = left;
            }
            curLen = left > I2C_SMBUS_BLOCK_MAX ? I2C_SMBUS_BLOCK_MAX : left;
            left -= curLen;
            tmp = i2c_smbus_read_i2c_block_data(fd,
                                                curaddr,
                                                curLen, buf + curRead);
            if (tmp <= 0)
            {
                return -1;
            }
            curRead += tmp;
        }

        return curRead;
    }


    void regSet(quint16 reg, quint8 val)
    {
        int old_val = regRead(reg);
        if (old_val >= 0)
        {
            regWrite(reg, old_val | val);
        }
    }

    void regClear(quint16 reg, quint8 val)
    {
        int old_val = regRead(reg);
        if (old_val >= 0)
        {
            regWrite(reg, old_val & (~val));
        }
    }


    void reset();

    void setMode();

    int getEdidBlock(u8 *buf, unsigned int blk, size_t length);

    struct edid *getEdid();


    int fd;
    quint8 currentSlaveAddr;
    quint8 currentPage;
    quint8 vipCntrl_0;
    quint8 vipCntrl_1;
    quint8 vipCntrl_2;
    HdmiAviInfoframe *hdmiAviInfoframe;
    QTimer *timer;
    bool connected;
private:
    TDA19988CtrlPrivate(const TDA19988CtrlPrivate &other);  /* disable copy */
};

void TDA19988CtrlPrivate::reset()
{
    /* reset audio and i2c master: */
    regWrite(REG_SOFTRESET, SOFTRESET_AUDIO | SOFTRESET_I2C_MASTER);
    usleep(50000);
    regWrite(REG_SOFTRESET, 0);
    usleep(50000);

    /* reset transmitter: */
    regSet(REG_MAIN_CNTRL0, MAIN_CNTRL0_SR);
    regClear(REG_MAIN_CNTRL0, MAIN_CNTRL0_SR);

    /* PLL registers common configuration */
    regWrite(REG_PLL_SERIAL_1, 0x00);
    regWrite(REG_PLL_SERIAL_2, PLL_SERIAL_2_SRL_NOSC(1));
    regWrite(REG_PLL_SERIAL_3, 0x00);
    regWrite(REG_SERIALIZER,   0x00);
    regWrite(REG_BUFFER_OUT,   0x00);
    regWrite(REG_PLL_SCG1,     0x00);
    regWrite(REG_AUDIO_DIV,    AUDIO_DIV_SERCLK_8);
    regWrite(REG_SEL_CLK,      SEL_CLK_SEL_CLK1 | SEL_CLK_ENA_SC_CLK);
    regWrite(REG_PLL_SCGN1,    0xfa);
    regWrite(REG_PLL_SCGN2,    0x00);
    regWrite(REG_PLL_SCGR1,    0x5b);
    regWrite(REG_PLL_SCGR2,    0x00);
    regWrite(REG_PLL_SCG2,     0x10);

    /* Write the default value MUX register */
    regWrite(REG_MUX_VP_VIP_OUT, 0x24);
}

void TDA19988CtrlPrivate::setMode()
{
    int hdisplay = dispTimings.hActive;
    int hsync_start = hdisplay + dispTimings.hBackPorch;
    int hsync_end = hsync_start + dispTimings.hSyncLen;
    int htotal = hsync_end + dispTimings.hFrontPorch;

    int vdisplay = dispTimings.vActive;
    int vsync_start = vdisplay + dispTimings.vBackPorch;
    int vsync_end = vsync_start + dispTimings.vSyncLen;
    int vtotal = vsync_end + dispTimings.vFrontPorch;

    int clock = dispTimings.pixelClock / 1000;

    quint16 n_pix = htotal;
    quint16 n_line = vtotal;

    /*
     * Internally TDA998x is using ITU-R BT.656 style sync but
     * we get VESA style sync. TDA998x is using a reference pixel
     * relative to ITU to sync to the input frame and for output
     * sync generation. Currently, we are using reference detection
     * from HS/VS, i.e. REFPIX/REFLINE denote frame start sync point
     * which is position of rising VS with coincident rising HS.
     *
     * Now there is some issues to take care of:
     * - HDMI data islands require sync-before-active
     * - TDA998x register values must be > 0 to be enabled
     * - REFLINE needs an additional offset of +1
     * - REFPIX needs an addtional offset of +1 for UYUV and +3 for RGB
     *
     * So we add +1 to all horizontal and vertical register values,
     * plus an additional +3 for REFPIX as we are using RGB input only.
     */
    quint16 hs_pix_e = hsync_end - hdisplay;
    quint16 hs_pix_s = hsync_start - hdisplay;
    quint16 de_pix_e = htotal;
    quint16 de_pix_s = htotal - hdisplay;
    quint16 ref_pix = 3 + hs_pix_s;


    /*
     * Attached LCD controllers may generate broken sync. Allow
     * those to adjust the position of the rising VS edge by adding
     * HSKEW to ref_pix.
     */
    ref_pix += hsync_end - hsync_start;

    quint16 ref_line = 1 + vsync_start - vdisplay;
    quint16 vwin1_line_s = vtotal - vdisplay - 1;
    quint16 vwin1_line_e = vwin1_line_s + vdisplay;
    quint16 vs1_pix_s = hs_pix_s;
    quint16 vs1_pix_e = hs_pix_s;
    quint16 vs1_line_s = vsync_start - vdisplay;
    quint16 vs1_line_e = vs1_line_s + vsync_end - vsync_start;
    quint16 vwin2_line_s = 0, vwin2_line_e = 0, vs2_pix_s = 0,
            vs2_pix_e = 0, vs2_line_s = 0, vs2_line_e = 0;

    quint8 div = 148500 / clock;
    if (div != 0)
    {
        div--;
        if (div > 3)
        {
            div = 3;
        }
    }

    /* mute the audio FIFO: */
    regSet(REG_AIP_CNTRL_0, AIP_CNTRL_0_RST_FIFO);

    /* set HDMI HDCP mode off: */
    regWrite(REG_TBG_CNTRL_1, TBG_CNTRL_1_DWIN_DIS);
    regClear(REG_TX33, TX33_HDMI);
    regWrite(REG_ENC_CNTRL, ENC_CNTRL_CTL_CODE(0));

    /* no pre-filter or interpolator: */
    regWrite(REG_HVF_CNTRL_0, HVF_CNTRL_0_PREFIL(0) |
             HVF_CNTRL_0_INTPOL(0));
    regWrite(REG_VIP_CNTRL_5, VIP_CNTRL_5_SP_CNT(0));
    regWrite(REG_VIP_CNTRL_4, VIP_CNTRL_4_BLANKIT(0) |
             VIP_CNTRL_4_BLC(0));

    regClear(REG_PLL_SERIAL_1, PLL_SERIAL_1_SRL_MAN_IZ);
    regClear(REG_PLL_SERIAL_3, PLL_SERIAL_3_SRL_CCIR |
             PLL_SERIAL_3_SRL_DE);
    regWrite(REG_SERIALIZER, 0);
    regWrite(REG_HVF_CNTRL_1, HVF_CNTRL_1_VQR(0));

    /* TODO enable pixel repeat for pixel rates less than 25Msamp/s */
    quint8 rep = 0;
    regWrite(REG_RPT_CNTRL, 0);
    regWrite(REG_SEL_CLK, SEL_CLK_SEL_VRF_CLK(0) |
             SEL_CLK_SEL_CLK1 | SEL_CLK_ENA_SC_CLK);

    regWrite(REG_PLL_SERIAL_2, PLL_SERIAL_2_SRL_NOSC(div) |
             PLL_SERIAL_2_SRL_PR(rep));

    /* set color matrix bypass flag: */
    regWrite(REG_MAT_CONTRL, MAT_CONTRL_MAT_BP |
             MAT_CONTRL_MAT_SC(1));

    /* set BIAS tmds value: */
    regWrite(REG_ANA_GENERAL, 0x09);

    /*
     * Sync on rising HSYNC/VSYNC
     */
    quint8 reg = VIP_CNTRL_3_SYNC_HS;

    /*
     * TDA19988 requires high-active sync at input stage,
     * so invert low-active sync provided by master encoder here
     */
    if (dispTimings.hSyncActive == 0)
    {
        reg |= VIP_CNTRL_3_H_TGL;
    }
    if (dispTimings.vSyncActive == 0)
    {
        reg |= VIP_CNTRL_3_V_TGL;
    }
    regWrite(REG_VIP_CNTRL_3, reg);

    regWrite(REG_VIDFORMAT, 0x00);
    regWrite16(REG_REFPIX_MSB, ref_pix);
    regWrite16(REG_REFLINE_MSB, ref_line);
    regWrite16(REG_NPIX_MSB, n_pix);
    regWrite16(REG_NLINE_MSB, n_line);
    regWrite16(REG_VS_LINE_STRT_1_MSB, vs1_line_s);
    regWrite16(REG_VS_PIX_STRT_1_MSB, vs1_pix_s);
    regWrite16(REG_VS_LINE_END_1_MSB, vs1_line_e);
    regWrite16(REG_VS_PIX_END_1_MSB, vs1_pix_e);
    regWrite16(REG_VS_LINE_STRT_2_MSB, vs2_line_s);
    regWrite16(REG_VS_PIX_STRT_2_MSB, vs2_pix_s);
    regWrite16(REG_VS_LINE_END_2_MSB, vs2_line_e);
    regWrite16(REG_VS_PIX_END_2_MSB, vs2_pix_e);
    regWrite16(REG_HS_PIX_START_MSB, hs_pix_s);
    regWrite16(REG_HS_PIX_STOP_MSB, hs_pix_e);
    regWrite16(REG_VWIN_START_1_MSB, vwin1_line_s);
    regWrite16(REG_VWIN_END_1_MSB, vwin1_line_e);
    regWrite16(REG_VWIN_START_2_MSB, vwin2_line_s);
    regWrite16(REG_VWIN_END_2_MSB, vwin2_line_e);
    regWrite16(REG_DE_START_MSB, de_pix_s);
    regWrite16(REG_DE_STOP_MSB, de_pix_e);

    /* let incoming pixels fill the active space (if any) */
    regWrite(REG_ENABLE_SPACE, 0x00);

    /*
     * Always generate sync polarity relative to input sync and
     * revert input stage toggled sync at output stage
     */
    reg = TBG_CNTRL_1_DWIN_DIS | TBG_CNTRL_1_TGL_EN;
    if (dispTimings.hSyncActive == 0)
    {
        reg |= TBG_CNTRL_1_H_TGL;
    }
    if (dispTimings.vSyncActive == 0)
    {
        reg |= TBG_CNTRL_1_V_TGL;
    }
    regWrite(REG_TBG_CNTRL_1, reg);

    /* must be last register set: */
    regWrite(REG_TBG_CNTRL_0, 0);

    /* We need to turn HDMI HDCP stuff on to get audio through */
    reg &= ~TBG_CNTRL_1_DWIN_DIS;
    regWrite(REG_TBG_CNTRL_1, reg);
    regWrite(REG_ENC_CNTRL, ENC_CNTRL_CTL_CODE(1));
    regSet(REG_TX33, TX33_HDMI);

    unsigned char aviInfoFrame[17];
    unsigned char *ptr = aviInfoFrame;
    memset(aviInfoFrame, 0, sizeof(aviInfoFrame));

    /* fill header */
    ptr[0] = hdmiAviInfoframe->type;
    ptr[1] = hdmiAviInfoframe->version;
    ptr[2] = hdmiAviInfoframe->length;
    ptr[3] = 0; /* checksum */

    ptr += 4; /* offset to content */
    ptr[0] = ((hdmiAviInfoframe->colorspace & 0x3) << 5) | (hdmiAviInfoframe->scan_mode & 0x3);

    /*
     * Data byte 1, bit 4 has to be set if we provide the active format
     * aspect ratio
     */
    if (hdmiAviInfoframe->active_aspect & 0xf)
    {
        ptr[0] |= BIT(4);
    }

    /* Bit 3 and 2 indicate if we transmit horizontal/vertical bar data */
    if (hdmiAviInfoframe->top_bar || hdmiAviInfoframe->bottom_bar)
    {
        ptr[0] |= BIT(3);
    }

    if (hdmiAviInfoframe->left_bar || hdmiAviInfoframe->right_bar)
    {
        ptr[0] |= BIT(2);
    }

    ptr[1] = ((hdmiAviInfoframe->colorimetry & 0x3) << 6) |
             ((hdmiAviInfoframe->picture_aspect & 0x3) << 4) |
             (hdmiAviInfoframe->active_aspect & 0xf);

    ptr[2] = ((hdmiAviInfoframe->extended_colorimetry & 0x7) << 4) |
             ((hdmiAviInfoframe->quantization_range & 0x3) << 2) |
             (hdmiAviInfoframe->nups & 0x3);

    if (hdmiAviInfoframe->itc)
    {
        ptr[2] |= BIT(7);
    }

    ptr[3] = hdmiAviInfoframe->video_code & 0x7f;

    ptr[4] = ((hdmiAviInfoframe->ycc_quantization_range & 0x3) << 6) |
             ((hdmiAviInfoframe->content_type & 0x3) << 4) |
             (hdmiAviInfoframe->pixel_repeat & 0xf);

    ptr[5] = hdmiAviInfoframe->top_bar & 0xff;
    ptr[6] = (hdmiAviInfoframe->top_bar >> 8) & 0xff;
    ptr[7] = hdmiAviInfoframe->bottom_bar & 0xff;
    ptr[8] = (hdmiAviInfoframe->bottom_bar >> 8) & 0xff;
    ptr[9] = hdmiAviInfoframe->left_bar & 0xff;
    ptr[10] = (hdmiAviInfoframe->left_bar >> 8) & 0xff;
    ptr[11] = hdmiAviInfoframe->right_bar & 0xff;
    ptr[12] = (hdmiAviInfoframe->right_bar >> 8) & 0xff;

    // calculate checksum
    unsigned char  csum = 0;
    for (unsigned int i = 0; i < sizeof(aviInfoFrame); i++)
    {
        csum += aviInfoFrame[i];
    }
    aviInfoFrame[3] = 256 - csum;

    regClear(REG_DIP_IF_FLAGS, DIP_IF_FLAGS_IF2);
    regWriteRange(REG_IF2_HB0, aviInfoFrame, sizeof(aviInfoFrame));
    regSet(REG_DIP_IF_FLAGS, DIP_IF_FLAGS_IF2);
}

int TDA19988CtrlPrivate::getEdidBlock(u8 *buf, unsigned int blk, size_t length)
{
    u8 offset, segptr;
    int ret, i;

    offset = (blk & 1) ? 128 : 0;
    segptr = blk / 2;

    regWrite(REG_DDC_ADDR, 0xa0);
    regWrite(REG_DDC_OFFS, offset);
    regWrite(REG_DDC_SEGM_ADDR, 0x60);
    regWrite(REG_DDC_SEGM, segptr);

    /* enable reading EDID: */
    regWrite(REG_EDID_CTRL, 0x1);

    /* flag must be cleared by sw: */
    regWrite(REG_EDID_CTRL, 0x0);

    /* wait for block read to complete: */
    for (i = 100; i > 0; i--)
    {
        usleep(2000);
        ret = regRead(REG_INT_FLAGS_2);
        if (ret < 0)
        {
            return ret;
        }
        if (ret & INT_FLAGS_2_EDID_BLK_RD)
        {
            break;
        }
    }

    if (i == 0)
    {
        qWarning("read edid timeout\n");
        return -ETIMEDOUT;
    }

    ret = regReadRange(REG_EDID_DATA_0, buf, length);
    if (ret != static_cast<int>(length))
    {
        qWarning("failed to read edid block %d: %d\n", blk, ret);
        return ret;
    }

    return 0;
}

/*** DDC fetch and block validation ***/

static const u8 edid_header[] =
{
    0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00
};

/**
 * drm_edid_header_is_valid - sanity check the header of the base EDID block
 * @raw_edid: pointer to raw base EDID block
 *
 * Sanity check the header of the base EDID block.
 *
 * Return: 8 if the header is perfect, down to 0 if it's totally wrong.
 */
static int drm_edid_header_is_valid(const u8 *raw_edid)
{
    int i, score = 0;

    for (i = 0; i < static_cast<int>(sizeof(edid_header)); i++)
        if (raw_edid[i] == edid_header[i])
        {
            score++;
        }

    return score;
}


static int drm_edid_block_checksum(const u8 *raw_edid)
{
    int i;
    u8 csum = 0;
    for (i = 0; i < EDID_LENGTH; i++)
    {
        csum += raw_edid[i];
    }

    return csum;
}


/**
 * drm_edid_block_valid - Sanity check the EDID block (base or extension)
 * @raw_edid: pointer to raw EDID block
 * @block: type of block to validate (0 for base, extension otherwise)
 * @edid_corrupt: if true, the header or checksum is invalid
 *
 * Validate a base or extension EDID block and optionally dump bad blocks to
 * the console.
 *
 * Return: True if the block is valid, false otherwise.
 */
bool drm_edid_block_valid(u8 *raw_edid, int block, bool *edid_corrupt)
{
    u8 csum;
    struct edid *edid = (struct edid *)raw_edid;

    if (!raw_edid)
    {
        return false;
    }

    if (block == 0)
    {
        int score = drm_edid_header_is_valid(raw_edid);
        if (score == 8)
        {
            if (edid_corrupt)
            {
                *edid_corrupt = false;
            }
        }
        else
        {
            if (edid_corrupt)
            {
                *edid_corrupt = true;
            }
            goto bad;
        }
    }

    csum = drm_edid_block_checksum(raw_edid);
    if (csum)
    {
        if (edid_corrupt)
        {
            *edid_corrupt = true;
        }

        /* allow CEA to slide through, switches mangle this */
        if (raw_edid[0] != 0x02)
        {
            goto bad;
        }
    }

    /* per-block-type checks */
    switch (raw_edid[0])
    {
    case 0: /* base */
        if (edid->version != 1)
        {
            qDebug("EDID has major version %d, instead of 1\n", edid->version);
            goto bad;
        }

        if (edid->revision > 4)
        {
            qDebug("EDID minor > 4, assuming backward compatibility\n");
        }
        break;

    default:
        break;
    }

    return true;

bad:
    return false;
}

static bool drm_edid_is_zero(const u8 *in_edid, int length)
{
    for (int i = 0; i < length; i++)
    {
        if (in_edid[i] != 0)
        {
            return false;
        }
    }

    return true;
}

struct edid *TDA19988CtrlPrivate::getEdid()
{
    int i, j = 0, valid_extensions = 0;
    u8 *block, *new_block;
    bool edid_corrupt = false;
    if ((block = reinterpret_cast<u8 *>(malloc(EDID_LENGTH))) == NULL)
    {
        return NULL;
    }

    /* base block fetch */
    for (i = 0; i < 4; i++)
    {
        if (getEdidBlock(block, 0, EDID_LENGTH))
        {
            goto out;
        }
        if (drm_edid_block_valid(block, 0, &edid_corrupt))
        {
            break;
        }
        if (i == 0 && drm_edid_is_zero(block, EDID_LENGTH))
        {
            goto out;
        }
    }
    if (i == 4)
    {
        goto out;
    }

    /* if there's no extensions, we're done */
    if (block[0x7e] == 0)
    {
        return (struct edid *)block;
    }

    new_block = reinterpret_cast<u8 *>(realloc(block, (block[0x7e] + 1) * EDID_LENGTH));
    if (!new_block)
    {
        goto out;
    }
    block = new_block;

    for (j = 1; j <= block[0x7e]; j++)
    {
        for (i = 0; i < 4; i++)
        {
            if (getEdidBlock(block + (valid_extensions + 1) * EDID_LENGTH,
                             j, EDID_LENGTH))
            {
                goto out;
            }
            if (drm_edid_block_valid(block + (valid_extensions + 1)
                                     * EDID_LENGTH, j,
                                     NULL))
            {
                valid_extensions++;
                break;
            }
        }
    }

    if (valid_extensions != block[0x7e])
    {
        block[EDID_LENGTH - 1] += block[0x7e] - valid_extensions;
        block[0x7e] = valid_extensions;
        new_block = reinterpret_cast<u8 *>(realloc(block, (valid_extensions + 1) * EDID_LENGTH));
        if (!new_block)
        {
            goto out;
        }
        block = new_block;
    }

    return (struct edid *)block;

out:
    free(block);
    return NULL;
}

TDA19988Ctrl::TDA19988Ctrl(QObject *parent)
    : QObject(parent), d_ptr(new TDA19988CtrlPrivate())
{
    /* load screen timming */
    QDesktopWidget *pDesk = QApplication::desktop();
    int screenWidth = pDesk->width();
    int screenHeight = pDesk->height();
    if (screenWidth == 1024 && screenHeight == 768)
    {
        qMemCopy(&dispTimings, &supportTimings[TIMING_1024_768], sizeof(DisplayTimings));
    }
    else if ((screenWidth == 1366 && screenHeight == 768) ||
             (screenWidth == 1360 && screenHeight == 768))
    {
        qMemCopy(&dispTimings, &supportTimings[TIMING_1366_768], sizeof(DisplayTimings));
    }

    int fd = open(I2C_BUS_DEV, O_RDWR);
    if (fd < 0)
    {
        qCritical() << "open dev file error: " << strerror(errno);
        return;
    }
    d_ptr->fd = fd;

    d_ptr->setSlaveAddr(CEC_ADDR);

    if (!isValid())
    {
        return;
    }

    /* wave up the device */
    d_ptr->cecWrite(REG_CEC_ENAMODS, CEC_ENAMODS_EN_RXSENS | CEC_ENAMODS_EN_HDMI);

    d_ptr->reset();

    quint8 revLo = d_ptr->regRead(REG_VERSION_LSB);
    quint8 revHi = d_ptr->regRead(REG_VERSION_MSB);

    quint16 rev = (revHi << 8) | revLo;
    qDebug("HDMI framer rev: 0x%04x", rev);

    /* after reset, enable DDC */
    d_ptr->regWrite(REG_DDC_DISABLE, 0x00);

    /* set clock on DDC channel */
    d_ptr->regWrite(REG_TX3, 39);

    d_ptr->cecWrite(REG_CEC_FRO_IM_CLK_CTRL,
                    CEC_FRO_IM_CLK_CTRL_GHOST_DIS | CEC_FRO_IM_CLK_CTRL_IMCLK_SEL);

    /* enable EDID read irq */
    d_ptr->regSet(REG_INT_FLAGS_2, INT_FLAGS_2_EDID_BLK_RD);

#if 1
    d_ptr->vipCntrl_0 = VIP_CNTRL_0_SWAP_A(2) | VIP_CNTRL_0_SWAP_B(3);
    d_ptr->vipCntrl_1 = VIP_CNTRL_1_SWAP_C(0) | VIP_CNTRL_1_SWAP_D(1);
    d_ptr->vipCntrl_2 = VIP_CNTRL_2_SWAP_E(4) | VIP_CNTRL_2_SWAP_F(5);
#else
    // this setting is used for beagle bone black
    d_ptr->vipCntrl_0 = VIP_CNTRL_0_SWAP_A(2) | VIP_CNTRL_0_SWAP_B(3);
    d_ptr->vipCntrl_1 = VIP_CNTRL_1_SWAP_C(4) | VIP_CNTRL_1_SWAP_D(5);
    d_ptr->vipCntrl_2 = VIP_CNTRL_2_SWAP_E(0) | VIP_CNTRL_2_SWAP_F(1);
#endif

    d_ptr->timer = new QTimer(this);

    connect(d_ptr->timer, SIGNAL(timeout()), this, SLOT(checkHDMIConnection()));

    d_ptr->timer->start(2000);
}


TDA19988Ctrl::~TDA19988Ctrl()
{
}

bool TDA19988Ctrl::isValid() const
{
    return d_ptr->fd != -1 && d_ptr->currentSlaveAddr != 0;
}

bool TDA19988Ctrl::isConnected() const
{
    return d_ptr->connected;
}

void TDA19988Ctrl::setSignalOutput(bool onOff)
{
    if (onOff)
    {
        /* enable video ports */
        d_ptr->regWrite(REG_ENA_VP_0, 0xff);
        d_ptr->regWrite(REG_ENA_VP_1, 0xff);
        d_ptr->regWrite(REG_ENA_VP_2, 0xff);
        /* set muxing after enabling ports */
        d_ptr->regWrite(REG_VIP_CNTRL_0, d_ptr->vipCntrl_0);
        d_ptr->regWrite(REG_VIP_CNTRL_1, d_ptr->vipCntrl_1);
        d_ptr->regWrite(REG_VIP_CNTRL_2, d_ptr->vipCntrl_2);
    }
    else
    {
        /* diable video ports */
        d_ptr->regWrite(REG_ENA_VP_0, 0x0);
        d_ptr->regWrite(REG_ENA_VP_1, 0x0);
        d_ptr->regWrite(REG_ENA_VP_2, 0x0);
    }
}

void TDA19988Ctrl::checkHDMIConnection()
{
    unsigned char lvl;

    lvl = d_ptr->cecRead(REG_CEC_RXSHPDLEV);

    if (lvl & CEC_RXSHPDLEV_HPD)
    {
        if (!d_ptr->connected)
        {
            qDebug() << "HDMI connected";
            d_ptr->connected = true;

            // read edid
            d_ptr->regClear(REG_TX4, TX4_PD_RAM);

            struct edid *edid;
            edid = d_ptr->getEdid();

            d_ptr->regSet(REG_TX4, TX4_PD_RAM);

            if (!edid)
            {
                qWarning() << "Failed to read EDID";
                return;
            }

            /* TODO: print the EDID info */

            free(edid);

            d_ptr->setMode();


            setSignalOutput(true);
        }
    }
    else
    {
        if (d_ptr->connected)
        {
            qDebug() << "HDMI disconnected";
            d_ptr->connected = false;
            setSignalOutput(false);
        }
    }
}

