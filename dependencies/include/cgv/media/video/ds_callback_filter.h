#pragma once

#pragma warning(disable:4996)
#pragma warning(disable:4995)
#pragma warning(disable:4275)
#pragma warning(disable:4251)

#include <cgv/media/video/ds_common.h>
#include <cgv/media/video/ds_capture.h>
#include <cgv/utils/stopwatch.h>
#include <cgv/media/video/ds/streams.h>
#include <cgv/media/video/lib_begin.h>

namespace cgv {
	namespace media {
		namespace video {

class ds_cb_sink;
class ds_cb_sink_pin;

class CGV_API ds_cb_filter : public CTransInPlaceFilter
{
	/// use only this format type in the transformation
	ds_capture::format_type ft;
public:
	DECLARE_IUNKNOWN
	ds_cb_filter(LPUNKNOWN pUnk, HRESULT *phr, ds_capture::format_type _ft = ds_capture::UNDEFINED_FORMAT_TYPE);
	static CUnknown * WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT *phr);
	HRESULT CheckInputType(const CMediaType *mtIn);
	HRESULT Transform(IMediaSample *pSource);
};


// Main filter object
class CGV_API ds_cb_sink_filter : public CBaseFilter
{
    const ds_cb_sink* cb;

public:

    // Constructor
    ds_cb_sink_filter(ds_cb_sink *_cb,
                LPUNKNOWN pUnk,
                CCritSec *pLock,
				HRESULT *phr);

    // Pin enumeration
	CBasePin * GetPin(int n);
	int GetPinCount();

    // Open and close the file as necessary
    STDMETHODIMP Run(REFERENCE_TIME tStart);
    STDMETHODIMP Pause();
    STDMETHODIMP Stop();
};


//  Pin object

class CGV_API ds_cb_sink_pin : public CRenderedInputPin
{
    ds_cb_sink* cb;
    CCritSec * const m_pReceiveLock;    // Sample critical section
    REFERENCE_TIME m_tLast;             // Last sample receive time
public:
    ds_cb_sink_pin(ds_cb_sink* _cb,
                 LPUNKNOWN pUnk,
                 CBaseFilter *pFilter,
                 CCritSec *pLock,
                 CCritSec *pReceiveLock,
				 HRESULT *phr);
    // perform ds_cb_sink
	STDMETHODIMP Receive(IMediaSample *pSample);
    STDMETHODIMP EndOfStream(void);
    STDMETHODIMP ReceiveCanBlock();
    // Write detailed information about this sample to a file
    HRESULT WriteStringInfo(IMediaSample *pSample);

    // Check if the pin can support this specific proposed type and format
    HRESULT CheckMediaType(const CMediaType *);
    // Break connection
    HRESULT BreakConnect();
    // Track NewSegment
    STDMETHODIMP NewSegment(REFERENCE_TIME tStart,
                            REFERENCE_TIME tStop,
                            double dRate);
};


//  CDump object which has filter and pin members

class CGV_API ds_cb_sink : public CUnknown//, public IFileSinkFilter
{
public:
    friend class ds_cb_sink_filter;
    friend class ds_cb_sink_pin;

    ds_cb_sink_filter *filter;
    ds_cb_sink_pin    *pin;

    CCritSec m_Lock;                // Main renderer critical section
    CCritSec m_ReceiveLock;         // Sublock for received samples
	cgv::utils::stopwatch watch;
	double time;
	int count;
public:

    DECLARE_IUNKNOWN

	ds_cb_sink(LPUNKNOWN pUnk, HRESULT *phr);
    ~ds_cb_sink();
	void init() { count = 0; time = 0; }

    static CUnknown * WINAPI CreateInstance(LPUNKNOWN punk, HRESULT *phr);
    // handle ds_cb_sink
    HRESULT handle(PBYTE pbData, LONG lDataLength);

private:

    // Overriden to say what interfaces we support where
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

};

		}
	}
}

#include <cgv/config/lib_end.h>
