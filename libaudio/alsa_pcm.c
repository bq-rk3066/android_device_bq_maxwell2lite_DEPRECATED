/*
** Copyright 2010, The Android Open-Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#define LOG_TAG "alsa_pcm"
//#define LOG_NDEBUG 0
#include <cutils/log.h>
#include <cutils/config_utils.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>

#include <linux/ioctl.h>

#include "alsa_audio.h"

#define __force
#define __bitwise
#define __user
#include "asound.h"

#define DEBUG 0

#undef ALOGV
#define ALOGV ALOGD
#define PARAM_MAX SNDRV_PCM_HW_PARAM_LAST_INTERVAL

static inline int param_is_mask(int p)
{
    return (p >= SNDRV_PCM_HW_PARAM_FIRST_MASK) &&
        (p <= SNDRV_PCM_HW_PARAM_LAST_MASK);
}

static inline int param_is_interval(int p)
{
    return (p >= SNDRV_PCM_HW_PARAM_FIRST_INTERVAL) &&
        (p <= SNDRV_PCM_HW_PARAM_LAST_INTERVAL);
}

static inline struct snd_interval *param_to_interval(struct snd_pcm_hw_params *p, int n)
{
    return &(p->intervals[n - SNDRV_PCM_HW_PARAM_FIRST_INTERVAL]);
}

static inline struct snd_mask *param_to_mask(struct snd_pcm_hw_params *p, int n)
{
    return &(p->masks[n - SNDRV_PCM_HW_PARAM_FIRST_MASK]);
}

static void param_set_mask(struct snd_pcm_hw_params *p, int n, unsigned bit)
{
    if (bit >= SNDRV_MASK_MAX)
        return;
    if (param_is_mask(n)) {
        struct snd_mask *m = param_to_mask(p, n);
        m->bits[0] = 0;
        m->bits[1] = 0;
        m->bits[bit >> 5] |= (1 << (bit & 31));
    }
}

static void param_set_min(struct snd_pcm_hw_params *p, int n, unsigned val)
{
    if (param_is_interval(n)) {
        struct snd_interval *i = param_to_interval(p, n);
        i->min = val;
    }
}

static void param_set_max(struct snd_pcm_hw_params *p, int n, unsigned val)
{
    if (param_is_interval(n)) {
        struct snd_interval *i = param_to_interval(p, n);
        i->max = val;
    }
}

static void param_set_int(struct snd_pcm_hw_params *p, int n, unsigned val)
{
    if (param_is_interval(n)) {
        struct snd_interval *i = param_to_interval(p, n);
        i->min = val;
        i->max = val;
        i->integer = 1;
    }
}

static void param_init(struct snd_pcm_hw_params *p)
{
    int n;
    memset(p, 0, sizeof(*p));
    for (n = SNDRV_PCM_HW_PARAM_FIRST_MASK;
         n <= SNDRV_PCM_HW_PARAM_LAST_MASK; n++) {
            struct snd_mask *m = param_to_mask(p, n);
            m->bits[0] = ~0;
            m->bits[1] = ~0;
    }
    for (n = SNDRV_PCM_HW_PARAM_FIRST_INTERVAL;
         n <= SNDRV_PCM_HW_PARAM_LAST_INTERVAL; n++) {
            struct snd_interval *i = param_to_interval(p, n);
            i->min = 0;
            i->max = ~0;
    }
}

#if DEBUG
static const char *param_name[PARAM_MAX+1] = {
    [SNDRV_PCM_HW_PARAM_ACCESS] = "access",
    [SNDRV_PCM_HW_PARAM_FORMAT] = "format",
    [SNDRV_PCM_HW_PARAM_SUBFORMAT] = "subformat",

    [SNDRV_PCM_HW_PARAM_SAMPLE_BITS] = "sample_bits",
    [SNDRV_PCM_HW_PARAM_FRAME_BITS] = "frame_bits",
    [SNDRV_PCM_HW_PARAM_CHANNELS] = "channels",
    [SNDRV_PCM_HW_PARAM_RATE] = "rate",
    [SNDRV_PCM_HW_PARAM_PERIOD_TIME] = "period_time",
    [SNDRV_PCM_HW_PARAM_PERIOD_SIZE] = "period_size",
    [SNDRV_PCM_HW_PARAM_PERIOD_BYTES] = "period_bytes",
    [SNDRV_PCM_HW_PARAM_PERIODS] = "periods",
    [SNDRV_PCM_HW_PARAM_BUFFER_TIME] = "buffer_time",
    [SNDRV_PCM_HW_PARAM_BUFFER_SIZE] = "buffer_size",
    [SNDRV_PCM_HW_PARAM_BUFFER_BYTES] = "buffer_bytes",
    [SNDRV_PCM_HW_PARAM_TICK_TIME] = "tick_time",
};

static void param_dump(struct snd_pcm_hw_params *p)
{
    int n;

    for (n = SNDRV_PCM_HW_PARAM_FIRST_MASK;
         n <= SNDRV_PCM_HW_PARAM_LAST_MASK; n++) {
            struct snd_mask *m = param_to_mask(p, n);
            ALOGV("%s = %08x%08x\n", param_name[n],
                   m->bits[1], m->bits[0]);
    }
    for (n = SNDRV_PCM_HW_PARAM_FIRST_INTERVAL;
         n <= SNDRV_PCM_HW_PARAM_LAST_INTERVAL; n++) {
            struct snd_interval *i = param_to_interval(p, n);
            ALOGV("%s = (%d,%d) omin=%d omax=%d int=%d empty=%d\n",
                   param_name[n], i->min, i->max, i->openmin,
                   i->openmax, i->integer, i->empty);
    }
    ALOGV("info = %08x\n", p->info);
    ALOGV("msbits = %d\n", p->msbits);
    ALOGV("rate = %d/%d\n", p->rate_num, p->rate_den);
    ALOGV("fifo = %d\n", (int) p->fifo_size);
}

static void info_dump(struct snd_pcm_info *info)
{
    ALOGV("device = %d\n", info->device);
    ALOGV("subdevice = %d\n", info->subdevice);
    ALOGV("stream = %d\n", info->stream);
    ALOGV("card = %d\n", info->card);
    ALOGV("id = '%s'\n", info->id);
    ALOGV("name = '%s'\n", info->name);
    ALOGV("subname = '%s'\n", info->subname);
    ALOGV("dev_class = %d\n", info->dev_class);
    ALOGV("dev_subclass = %d\n", info->dev_subclass);
    ALOGV("subdevices_count = %d\n", info->subdevices_count);
    ALOGV("subdevices_avail = %d\n", info->subdevices_avail);
}
#else
static void param_dump(struct snd_pcm_hw_params *p) {}
static void info_dump(struct snd_pcm_info *info) {}
#endif

#define PCM_ERROR_MAX 128

struct pcm {
    int fd;
    unsigned flags;
    int running:1;
    int underruns;
    unsigned buffer_size;
    char error[PCM_ERROR_MAX];
};

unsigned pcm_buffer_size(struct pcm *pcm)
{
    return pcm->buffer_size;
}

const char* pcm_error(struct pcm *pcm)
{
    return pcm->error;
}

static int oops(struct pcm *pcm, int e, const char *fmt, ...) __attribute__((format(printf, 3, 4)));
static int oops(struct pcm *pcm, int e, const char *fmt, ...)
{
    va_list ap;
    int sz;

    va_start(ap, fmt);
    vsnprintf(pcm->error, PCM_ERROR_MAX, fmt, ap);
    va_end(ap);
    sz = strlen(pcm->error);

    if (errno)
        snprintf(pcm->error + sz, PCM_ERROR_MAX - sz,
                 ": %s", strerror(e));
    return -1;
}

int pcm_write(struct pcm *pcm, void *data, unsigned count)
{
    struct snd_xferi x;

    if (pcm->flags & PCM_IN)
        return -EINVAL;

    x.buf = data;
    x.frames = (pcm->flags & PCM_MONO) ? (count / 2) : (count / 4);

    for (;;) {
        if (!pcm->running) {
            if (ioctl(pcm->fd, SNDRV_PCM_IOCTL_PREPARE))
                return oops(pcm, errno, "cannot prepare channel");
            if (ioctl(pcm->fd, SNDRV_PCM_IOCTL_WRITEI_FRAMES, &x))
                return oops(pcm, errno, "cannot write initial data");
            pcm->running = 1;
            return 0;
        }
        if (ioctl(pcm->fd, SNDRV_PCM_IOCTL_WRITEI_FRAMES, &x)) {
            pcm->running = 0;
            if (errno == EPIPE) {
                pcm->underruns++;
                continue;
            }
            return oops(pcm, errno, "cannot write stream data");
        }
        return 0;
    }
}

#define SAMPLECOUNT 441*5*2*2
int channalFlags = -1;

int startCheckCount = 0;

int channel_check(void * data,int len )
{
	short * pcmLeftChannel = (short *)data;
	short * pcmRightChannel = pcmLeftChannel+1;
	unsigned index = 0;
	int leftValid = 0x0;
	int rightValid = 0x0;
	short checkValue = 0;
	
	checkValue = *pcmLeftChannel;

	for(index = 0; index < len; index += 2)
	{
		
		if((pcmLeftChannel[index] >= checkValue+50)||(pcmLeftChannel[index] <= checkValue-50))
		{
			leftValid = 0x01;
			ALOGI("-->pcmLeftChannel[%d] = %d checkValue %d",index,pcmLeftChannel[index],checkValue);
			break;
		}	
	}
	checkValue = *pcmRightChannel;

	for(index = 0; index < len; index += 2)
	{
		
		if((pcmRightChannel[index] >= checkValue+50)||(pcmRightChannel[index] <= checkValue-50))
		{
			rightValid = 0x02;
			ALOGI("-->pcmRightChannel[%d] = %d checkValue %d",index,pcmRightChannel[index],checkValue);
			break;
		}	
	}
	ALOGI("leftValid %d rightValid %d",leftValid,rightValid);
	return leftValid|rightValid;
}

void channel_fixed(void * data,int len, int chFlag)
{
	if(chFlag <= 0 || chFlag > 2 )
		return;

	short * pcmValid = (short *)data;
	short * pcmInvalid = pcmValid;
	
	if(chFlag == 1)
		pcmInvalid += 1;
	else if (chFlag == 2)
		pcmValid += 1;
	
	unsigned index ;
	
	for(index = 0; index < len; index += 2)
	{
		int tmp = (int)pcmValid[index] + pcmValid[index]>>1;
		pcmValid[index] = tmp > 32767 ? 32767 : (tmp < -32768 ? -32768 : tmp);
		pcmInvalid[index] = pcmValid[index];
	}
	return;
}
int pcm_read(struct pcm *pcm, void *data, unsigned count)
{
    struct snd_xferi x;

    if (!(pcm->flags & PCM_IN))
        return -EINVAL;

    x.buf = data;
    x.frames = (pcm->flags & PCM_MONO) ? (count / 2) : (count / 4);

    for (;;) {
        if (!pcm->running) {
            if (ioctl(pcm->fd, SNDRV_PCM_IOCTL_PREPARE))
                return oops(pcm, errno, "cannot prepare channel");
            if (ioctl(pcm->fd, SNDRV_PCM_IOCTL_START))
                return oops(pcm, errno, "cannot start channel");
            pcm->running = 1;
        }
        if (ioctl(pcm->fd, SNDRV_PCM_IOCTL_READI_FRAMES, &x)) {
            pcm->running = 0;
            if (errno == EPIPE) {
                pcm->underruns++;
                continue;
            }
            return oops(pcm, errno, "cannot read stream data");
        }
		if(!(pcm->flags & PCM_MONO))
		{
			if(channalFlags == -1 )	
			{
				if(startCheckCount < SAMPLECOUNT)
				{
					startCheckCount += count;
				}
				else
				{
					channalFlags = channel_check(data,count/2);
				}
			}
			channel_fixed(data,count/2, channalFlags);
		}
        return 0;
    }
}

static struct pcm bad_pcm = {
    .fd = -1,
};

int pcm_close(struct pcm *pcm)
{
    if (pcm == &bad_pcm)
        return 0;

    if (pcm->fd >= 0)
        close(pcm->fd);
    pcm->running = 0;
    pcm->buffer_size = 0;
    pcm->fd = -1;
    return 0;
}

struct pcm *pcm_open(unsigned flags)
{
    const char *dname;
    struct pcm *pcm;
    struct snd_pcm_info info;
    struct snd_pcm_hw_params params;
    struct snd_pcm_sw_params sparams;
    unsigned period_sz;
    unsigned period_cnt;

    ALOGV("pcm_open(0x%08x)",flags);

    pcm = calloc(1, sizeof(struct pcm));
    if (!pcm)
        return &bad_pcm;

    if (flags & PCM_IN) {
        dname = "/dev/snd/pcmC0D0c";
		channalFlags = -1;
		startCheckCount = 0;
    } else {
#ifdef SUPPORT_USB
        dname = "/dev/snd/pcmC1D0p";
#else
		dname = "/dev/snd/pcmC0D0p";
#endif
    }

    ALOGV("pcm_open() period sz multiplier %d",
         ((flags & PCM_PERIOD_SZ_MASK) >> PCM_PERIOD_SZ_SHIFT) + 1);
    period_sz = PCM_PERIOD_SZ_MIN * (((flags & PCM_PERIOD_SZ_MASK) >> PCM_PERIOD_SZ_SHIFT) + 1);
    ALOGV("pcm_open() period cnt %d",
         ((flags & PCM_PERIOD_CNT_MASK) >> PCM_PERIOD_CNT_SHIFT) + PCM_PERIOD_CNT_MIN);
    period_cnt = ((flags & PCM_PERIOD_CNT_MASK) >> PCM_PERIOD_CNT_SHIFT) + PCM_PERIOD_CNT_MIN;

    pcm->flags = flags;
    pcm->fd = open(dname, O_RDWR);
    if (pcm->fd < 0) {
        oops(pcm, errno, "cannot open device '%s'", dname);
        return pcm;
    }

    if (ioctl(pcm->fd, SNDRV_PCM_IOCTL_INFO, &info)) {
        oops(pcm, errno, "cannot get info - %s", dname);
        goto fail;
    }
    info_dump(&info);

    ALOGV("pcm_open() period_cnt %d period_sz %d channels %d",
         period_cnt, period_sz, (flags & PCM_MONO) ? 1 : 2);

    param_init(&params);
    param_set_mask(&params, SNDRV_PCM_HW_PARAM_ACCESS,
                   SNDRV_PCM_ACCESS_RW_INTERLEAVED);
    param_set_mask(&params, SNDRV_PCM_HW_PARAM_FORMAT,
                   SNDRV_PCM_FORMAT_S16_LE);
    param_set_mask(&params, SNDRV_PCM_HW_PARAM_SUBFORMAT,
                   SNDRV_PCM_SUBFORMAT_STD);
	
    param_set_min(&params, SNDRV_PCM_HW_PARAM_PERIOD_SIZE, period_sz);
    param_set_int(&params, SNDRV_PCM_HW_PARAM_SAMPLE_BITS, 16);
    param_set_int(&params, SNDRV_PCM_HW_PARAM_FRAME_BITS,
                  (flags & PCM_MONO) ? 16 : 32);
    param_set_int(&params, SNDRV_PCM_HW_PARAM_CHANNELS,
                  (flags & PCM_MONO) ? 1 : 2);
    param_set_int(&params, SNDRV_PCM_HW_PARAM_PERIODS, period_cnt);
    param_set_int(&params, SNDRV_PCM_HW_PARAM_RATE, 44100);
	

    if (ioctl(pcm->fd, SNDRV_PCM_IOCTL_HW_PARAMS, &params)) {
        oops(pcm, errno, "cannot set hw params");
        goto fail;
    }
    param_dump(&params);

    memset(&sparams, 0, sizeof(sparams));
    sparams.tstamp_mode = SNDRV_PCM_TSTAMP_NONE;
    sparams.period_step = 1;
    sparams.avail_min = 1;
    sparams.start_threshold = period_cnt * period_sz;
    sparams.stop_threshold = period_cnt * period_sz;
    sparams.xfer_align = period_sz / 2; /* needed for old kernels */
    sparams.silence_size = 0;
    sparams.silence_threshold = 0;

    if (ioctl(pcm->fd, SNDRV_PCM_IOCTL_SW_PARAMS, &sparams)) {
        oops(pcm, errno, "cannot set sw params");
        goto fail;
    }

    pcm->buffer_size = period_cnt * period_sz;
    pcm->underruns = 0;
    return pcm;

fail:
    close(pcm->fd);
    pcm->fd = -1;
    return pcm;
}

int pcm_ready(struct pcm *pcm)
{
    return pcm->fd >= 0;
}
