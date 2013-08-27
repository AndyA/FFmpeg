#ifndef __CAPTURE_H__
#define __CAPTURE_H__

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <fcntl.h>
#include <pthread.h>
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

struct {
  pthread_mutex_t mutex;
  pthread_cond_t got_input;
  AVFifoBuffer   *fifo;;
} decklink_pipe;

int dl_startup(void);

#ifdef __cplusplus
}
#endif

#endif
