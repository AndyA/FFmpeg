/*
 * Blackmagic Decklink grab
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "avdevice.h"

#undef __STRICT_ANSI__ //workaround due to broken kernel headers
#include "config.h"
#include "libavutil/rational.h"
#include "libavutil/imgutils.h"
#include "libavutil/log.h"
#include "libavutil/opt.h"
#include "libavformat/internal.h"
#include "libavcodec/dsputil.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>
#define _LINUX_TIME_H 1
#include <time.h>
#include "avdevice.h"

typedef struct {
  AVClass *cl;
} VideoData;

static int grab_read_header(AVFormatContext *s1) {
  return 1;
}

static int grab_read_packet(AVFormatContext *s1, AVPacket *pkt) {
  return 1;
}

static int grab_read_close(AVFormatContext *s1) {
  return 1;
}

static const AVOption options[] = {
  { NULL },
};

static const AVClass decklink_class = {
  .class_name = "Decklink indev",
  .item_name  = av_default_item_name,
  .option     = options,
  .version    = LIBAVUTIL_VERSION_INT,
};

AVInputFormat ff_decklink_demuxer = {
  .name           = "decklink",
  .long_name      = NULL_IF_CONFIG_SMALL("Blackmagic Decklink device grab"),
  .priv_data_size = sizeof(VideoData),
  .read_header    = grab_read_header,
  .read_packet    = grab_read_packet,
  .read_close     = grab_read_close,
  .flags          = AVFMT_NOFILE,
  .priv_class     = &decklink_class,
};
