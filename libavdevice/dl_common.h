#ifndef __DL_COMMON_H__
#define __DL_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "avdevice.h"
#include "config.h"
#include "libavcodec/dsputil.h"
#include "libavformat/avformat.h"
#include "libavformat/internal.h"
#include "libavutil/fifo.h"
#include "libavutil/imgutils.h"
#include "libavutil/log.h"
#include "libavutil/opt.h"
#include "libavutil/rational.h"

  struct decklink_work;

  struct decklink_pipe {
    AVClass *cl;
    struct decklink_work *w;
    pthread_t worker;
    pthread_mutex_t mutex;
    pthread_cond_t non_empty, non_full;
    AVFifoBuffer *fifo;
  };

  int dl_startup(AVFormatContext *ctx);
  int dl_shutdown(AVFormatContext *ctx);

#ifdef __cplusplus
}
#endif

#endif
