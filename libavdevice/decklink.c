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
#include <time.h>
#include "avdevice.h"

#include "dl_common.h"

static int dl_init(AVFormatContext *ctx) {
  struct decklink_pipe *self = ctx->priv_data;

  pthread_mutex_init(&self->mutex, NULL);
  pthread_cond_init(&self->non_empty, NULL);
  pthread_cond_init(&self->non_full, NULL);

  self->fifo = av_fifo_alloc(100 * sizeof(AVPacket));

  dl_startup(ctx);

  return 0;
}

static int dl_destroy(AVFormatContext *ctx) {
  struct decklink_pipe *self = ctx->priv_data;

  pthread_mutex_lock(&self->mutex);

  dl_shutdown(ctx); /* Wait? */

  av_fifo_free(self->fifo);
  self->fifo = NULL;

  pthread_mutex_unlock(&self->mutex);

  pthread_mutex_destroy(&self->mutex);
  pthread_cond_destroy(&self->non_empty);
  pthread_cond_destroy(&self->non_full);

  return 0;
}

static int dl_read_header(AVFormatContext *ctx) {
  AVStream *stm;
  dl_init(ctx);

  if (stm = avformat_new_stream(ctx, NULL), !stm) {
    dl_destroy(ctx);
    return AVERROR(ENOMEM);
  }

  stm->codec->codec_type = AVMEDIA_TYPE_VIDEO;
  stm->codec->codec_id = AV_CODEC_ID_RAWVIDEO;
  /*  stm->codec->time_base = av_inv_q(25);*/
  stm->codec->width = 1920;
  stm->codec->height = 1080;
  stm->codec->pix_fmt = AV_PIX_FMT_UYVY422;
  stm->codec->codec_tag = avcodec_pix_fmt_to_codec_tag(stm->codec->pix_fmt);
  stm->codec->bit_rate = 1920 * 1080 * 2 * 25 * 8;
  stm->avg_frame_rate.num = 25;
  stm->avg_frame_rate.den = 1;
  stm->codec->time_base = stm->avg_frame_rate;

  avpriv_set_pts_info(stm, 64, 1, 1000000);

  if (stm = avformat_new_stream(ctx, NULL), !stm) {
    dl_destroy(ctx);
    return AVERROR(ENOMEM);
  }

  stm->codec->codec_type = AVMEDIA_TYPE_AUDIO;
  stm->codec->codec_id = AV_CODEC_ID_PCM_S16LE;
  stm->codec->sample_rate = 48000;
  stm->codec->channels = 2;

  avpriv_set_pts_info(stm, 64, 1, 1000000);

  return 0;
}

static int dl_read_packet(AVFormatContext *ctx, AVPacket *pkt) {
  struct decklink_pipe *self = ctx->priv_data;

  pthread_mutex_lock(&self->mutex);

  while (!av_fifo_size(self->fifo))
    pthread_cond_wait(&self->non_empty, &self->mutex);

  av_fifo_generic_read(self->fifo, pkt, sizeof(*pkt), NULL);
  pthread_cond_signal(&self->non_full);

  pthread_mutex_unlock(&self->mutex);

  return 1;
}

static int dl_read_close(AVFormatContext *ctx) {
  dl_destroy(ctx);
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
  .long_name      = NULL_IF_CONFIG_SMALL("Blackmagic Decklink device input"),
  .priv_data_size = sizeof(struct decklink_pipe),
  .read_header    = dl_read_header,
  .read_packet    = dl_read_packet,
  .read_close     = dl_read_close,
  .flags          = AVFMT_NOFILE,
  .priv_class     = &decklink_class,
};
