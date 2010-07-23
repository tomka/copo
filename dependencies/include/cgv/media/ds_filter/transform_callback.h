#pragma once

#include <3rd/strmbase/streams.h>
#include <cgv/media/ds_filter/icallback.h>

class transform_callback : public CTransInPlaceFilter,
				 public ICallback
{

public:
    
    // Constructor
    transform_callback( TCHAR *tszName, LPUNKNOWN punk, HRESULT *phr);

    static CUnknown * WINAPI CreateInstance(LPUNKNOWN punk, HRESULT *phr);

    // Reveals ProxyTransform & ISpecifyPropertyPages
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

    DECLARE_IUNKNOWN;

    HRESULT Transform(IMediaSample *pSample);
    HRESULT CheckInputType(const CMediaType *mtIn);
    HRESULT GetMediaType(int iPosition, CMediaType *pMediaType);
    HRESULT DecideBufferSize(IMemAllocator *pAlloc,ALLOCATOR_PROPERTIES *pProperties);
    HRESULT CheckTransform(const CMediaType *mtIn,const CMediaType *mtOut);

    // ICallback methods

    STDMETHODIMP set_callback(void (__cdecl *cb)(void*));
	STDMETHODIMP SetVideoWindowSize(HWND hWnd);
    STDMETHODIMP GetVideoWindowSize(HWND hWnd, RECT* rect);

private:
   
    // Non interface locking critical section
    CCritSec         my_lock;
    void             (*cb)(void*); 
/*    IBaseFilter*     m_SourceFilter;
    IProxyTransform* m_ProxyTrans;
    IGraphBuilder*   m_GraphBuilder;
    IMediaControl*   m_MediaControl;
    IVideoWindow*    m_VideoWindow;
    IFilterGraph*    m_FilterGraph;
    IMediaEventEx*   m_MediaEventEx;
    BOOL             m_DSOn;
    BOOL             m_isDSRunning;*/

};

