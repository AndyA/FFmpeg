#ifndef __DL_INPUT_H__
#define __DL_INPUT_H__

#include "DeckLinkAPI.h"
#include "dl_common.h"

class DeckLinkCaptureDelegate : public IDeckLinkInputCallback {
public:
  DeckLinkCaptureDelegate(AVFormatContext *ctx);
  ~DeckLinkCaptureDelegate();

  virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID *ppv) {
    return E_NOINTERFACE;
  }
  virtual ULONG STDMETHODCALLTYPE AddRef(void);
  virtual ULONG STDMETHODCALLTYPE Release(void);
  virtual HRESULT STDMETHODCALLTYPE VideoInputFormatChanged(
    BMDVideoInputFormatChangedEvents,
    IDeckLinkDisplayMode *,
    BMDDetectedVideoInputFormatFlags);
  virtual HRESULT STDMETHODCALLTYPE VideoInputFrameArrived(
    IDeckLinkVideoInputFrame *,
    IDeckLinkAudioInputPacket *);

private:
  ULONG m_refCount;
  pthread_mutex_t m_mutex;
  AVFormatContext *m_ctx;
};

struct decklink_work {
  IDeckLink *dl;
  IDeckLinkInput *dli;
  DeckLinkCaptureDelegate *delegate;
};

#endif
