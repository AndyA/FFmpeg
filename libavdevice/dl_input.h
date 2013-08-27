#ifndef __CAPTURE_H__
#define __CAPTURE_H__

#include <stdio.h>

#include "DeckLinkAPI.h"
#include "dl_common.h"

class DeckLinkCaptureDelegate : public IDeckLinkInputCallback {
public:
  DeckLinkCaptureDelegate(struct decklink_pipe *pipe);
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
  struct decklink_pipe *m_pipe;
};

#endif
