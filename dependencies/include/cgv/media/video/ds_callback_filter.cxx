#include <iostream>
#include <cgv/media/video/ds_common.h>
#include <cgv/media/video/ds_callback_filter.h>

namespace cgv {
	namespace media {
		namespace video {


CUnknown * WINAPI ds_cb_filter::CreateInstance(LPUNKNOWN lpunk, HRESULT *phr)
{
    ASSERT(phr);
    
    CUnknown *punk = new ds_cb_filter(lpunk, phr);
    if (punk == NULL) 
    {
		if (phr)
	        *phr = E_OUTOFMEMORY;
    }
    return punk;
}

CUnknown * WINAPI ds_cb_sink::CreateInstance(LPUNKNOWN lpunk, HRESULT *phr)
{
    ASSERT(phr);
    
    CUnknown *punk = new ds_cb_sink(lpunk, phr);
    if (punk == NULL) 
    {
		if (phr)
	        *phr = E_OUTOFMEMORY;
    }
    return punk;
}

ds_cb_filter::ds_cb_filter(LPUNKNOWN pUnk, HRESULT *phr, ds_capture::format_type _ft) : ft(_ft), CTransInPlaceFilter(NAME("ds_cb_filter"), pUnk, CLSID_dc_cb_filter, phr)
{ 
}

HRESULT ds_cb_filter::CheckInputType(const CMediaType *mtIn)
{
	if ((mtIn->majortype != MEDIATYPE_Video) ||
		(mtIn->formattype != FORMAT_VideoInfo) || 
		(ft != ds_capture::UNDEFINED_FORMAT_TYPE) && 
		(mtIn->subtype != format_type_to_subtype(ft)) ||
		(mtIn->cbFormat < sizeof(VIDEOINFOHEADER)))
	{
		return VFW_E_TYPE_NOT_ACCEPTED;
	}
	return S_OK;
}
HRESULT ds_cb_filter::Transform(IMediaSample *pSource)
{
	// Get pointers to the underlying buffers.
	BYTE *pBufferIn;
	HRESULT hr = pSource->GetPointer(&pBufferIn);
	if (FAILED(hr))
	{
		return hr;
	}
	std::cout << "transform callback" << std::endl;
//	pDest->SetSyncPoint(TRUE);
	return S_OK;
}

// Constructor
ds_cb_sink_filter::ds_cb_sink_filter(ds_cb_sink *_cb,
            LPUNKNOWN pUnk,
            CCritSec *pLock,
			HRESULT *phr) :
	CBaseFilter(NAME("CallbackFilter"), pUnk, pLock, CLSID_Callback), cb(_cb)
{
}

// Pin enumeration
CBasePin * ds_cb_sink_filter::GetPin(int n) { return (n == 0) ? cb->pin : NULL; }
int ds_cb_sink_filter::GetPinCount()        { return 1; }

STDMETHODIMP ds_cb_sink_filter::Run(REFERENCE_TIME tStart)
{
	CAutoLock cObjectLock(m_pLock);
//	if (cb)
//		cb->start();
	return CBaseFilter::Run(tStart);
}

STDMETHODIMP ds_cb_sink_filter::Stop() 
{
	CAutoLock cObjectLock(m_pLock);
//	if (cb)
//		cb->Stop();
	return CBaseFilter::Stop();
}
STDMETHODIMP ds_cb_sink_filter::Pause()
{
	CAutoLock cObjectLock(m_pLock);
	return CBaseFilter::Pause();
}

//  Pin object

ds_cb_sink_pin::ds_cb_sink_pin(ds_cb_sink* _cb,
             LPUNKNOWN pUnk,
             CBaseFilter *pFilter,

             CCritSec *pLock,
             CCritSec *pReceiveLock,
			 HRESULT *phr) : 
	CRenderedInputPin(NAME("CallbackPin"),
				  pFilter,                   // Filter
				  pLock,                     // Locking
				  phr,                       // Return code
				  L"Input"),                 // Pin name
	m_pReceiveLock(pReceiveLock),
	cb(_cb),
	m_tLast(0)
{
}
// perform ds_cb_sink
STDMETHODIMP ds_cb_sink_pin::EndOfStream(void)
{
	CAutoLock lock(m_pReceiveLock);
	return CRenderedInputPin::EndOfStream();
}
STDMETHODIMP ds_cb_sink_pin::ReceiveCanBlock()
{
	return S_FALSE;
}

// Check if the pin can support this specific proposed type and format
HRESULT ds_cb_sink_pin::CheckMediaType(const CMediaType *)
{
	return S_OK;
}
// Break connection
HRESULT ds_cb_sink_pin::BreakConnect()
{
	return CRenderedInputPin::BreakConnect();
}
// Track NewSegment
STDMETHODIMP ds_cb_sink_pin::NewSegment(REFERENCE_TIME tStart,
                        REFERENCE_TIME tStop,
                        double dRate)
{
	m_tLast = 0;
	return S_OK;

}

ds_cb_sink::ds_cb_sink(LPUNKNOWN pUnk, HRESULT *phr) :
	CUnknown(NAME("ds_cb_sink"), pUnk),
	filter(NULL),
	pin(NULL),
	watch(&time)
{
	time = 0;
	count = 0;

	ASSERT(phr);
    
	filter = new ds_cb_sink_filter(this, GetOwner(), &m_Lock, phr);
	if (filter == NULL) {
		if (phr)
			*phr = E_OUTOFMEMORY;
		return;
	}
	pin = new ds_cb_sink_pin(this,GetOwner(),
							   filter,
							   &m_Lock,
							   &m_ReceiveLock,
							   phr);
	if (pin == NULL) {
		if (phr)
			*phr = E_OUTOFMEMORY;
		return;
	}
}

ds_cb_sink::~ds_cb_sink()
{
	delete pin;
	delete filter;
}


// handle ds_cb_sink
HRESULT ds_cb_sink::handle(PBYTE pbData, LONG lDataLength)
{
	if (count == 0) {
		watch.add_time();
		time = 0;
	}
	++count;
	if (count % 25 == 0) {
		watch.add_time();
		std::cout << "fps = " << count/time << std::endl;
	}
	//std::cout << "received " << lDataLength << " bytes" << std::endl;
	return S_OK;
}


// Overriden to say what interfaces we support where
STDMETHODIMP ds_cb_sink::NonDelegatingQueryInterface(REFIID riid, void ** ppv)
{
	CheckPointer(ppv,E_POINTER);
	CAutoLock lock(&m_Lock);

	// Do we have this interface

//		if (riid == IID_IFileSinkFilter) {
//		return GetInterface((IFileSinkFilter *) this, ppv);
//	} 
//	else
	if (riid == IID_IBaseFilter || riid == IID_IMediaFilter || riid == IID_IPersist) {
		return filter->NonDelegatingQueryInterface(riid, ppv);
	} 
	return CUnknown::NonDelegatingQueryInterface(riid, ppv);

} // NonDelegatingQueryInterface

STDMETHODIMP ds_cb_sink_pin::Receive(IMediaSample *pSample)
{
	CheckPointer(pSample,E_POINTER);
	CAutoLock lock(m_pReceiveLock);
	PBYTE pbData;
	REFERENCE_TIME tStart, tStop;
	pSample->GetTime(&tStart, &tStop);
	m_tLast = tStart;
	HRESULT hr = pSample->GetPointer(&pbData);
	if (FAILED(hr))
		return hr;
	return cb->handle(pbData, pSample->GetActualDataLength());
}

		}
	}
}