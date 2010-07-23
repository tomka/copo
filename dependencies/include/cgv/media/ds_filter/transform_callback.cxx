#pragma warning (disable:4996)
#pragma warning (disable:4995)
#include <windows.h>
//#include <cvstreams.h>
#include <initguid.h>
#include <olectl.h>
#include <cgv/media/ds_filter/transform_callback.h>
#include <cgv/media/ds_filter/transform_callback_uids.h>

// setup data
const AMOVIESETUP_MEDIATYPE sudPinTypes =
{
    &MEDIATYPE_Video,       // Major type
    &MEDIASUBTYPE_NULL      // Minor type
};

const AMOVIESETUP_PIN psudPins[] =
{
    {
        L"Input",           // String pin name
        FALSE,              // Is it rendered
        FALSE,              // Is it an output
        FALSE,              // Allowed none
        FALSE,              // Allowed many
        &CLSID_NULL,        // Connects to filter
        L"Output",          // Connects to pin
        1,                  // Number of types
        &sudPinTypes },     // The pin details
      { L"Output",          // String pin name
        FALSE,              // Is it rendered
        TRUE,               // Is it an output
        FALSE,              // Allowed none
        FALSE,              // Allowed many
        &CLSID_NULL,        // Connects to filter
        L"Input",           // Connects to pin
        1,                  // Number of types
        &sudPinTypes        // The pin details
    }
};

const AMOVIESETUP_FILTER sud_callback =
{
    &CLSID_transform_callback,                 // Filter CLSID
    L"cgv_media_ds_filter_transform_callback", // Filter name
    MERIT_DO_NOT_USE,                // Its merit
    2,                               // Number of pins
    psudPins                         // Pin details
};

// List of class IDs and creator functions for the class factory. This
// provides the link between the OLE entry point in the DLL and an object
// being created. The class factory will call the static CreateInstance

CFactoryTemplate g_Templates[1] = {

    { L"cgv_media_ds_filter_transform_callback"
    , &CLSID_transform_callback
    , transform_callback::CreateInstance
    , NULL
    , &sud_callback }
};
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);

// constructor
transform_callback::transform_callback(TCHAR *tszName,LPUNKNOWN punk,HRESULT *phr) :
    CTransInPlaceFilter(tszName, punk, CLSID_transform_callback,phr), cb(0)
{
    ASSERT(tszName);
    ASSERT(phr);
}

// instance creation
CUnknown * WINAPI transform_callback::CreateInstance(LPUNKNOWN punk, HRESULT *phr) {

    transform_callback *cb = new transform_callback(L"cgv_media_ds_filter_transform_callback", punk, phr);
    if (cb == NULL) {
        *phr = E_OUTOFMEMORY;
    }
    return cb;

}

// provide ICallback interface
STDMETHODIMP transform_callback::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
    CheckPointer(ppv,E_POINTER);

    if (riid == IID_ICallback)
        return GetInterface((ICallback*) this, ppv);
    else
        return CTransInPlaceFilter::NonDelegatingQueryInterface(riid, ppv);
}

#include <cgv/media/image/basic_image.h>
#include <cgv/scene/mat/color.h>

// perform transform_callback on image
HRESULT transform_callback::Transform(IMediaSample *pMediaSample)
{
    BYTE*    pData;
    pMediaSample->GetPointer(&pData);
	cgv::media::image::basic_image<cgv::scene::mat::color<unsigned char, cgv::scene::mat::RGB> > image;
    
    AM_MEDIA_TYPE* pType = &m_pInput->CurrentMediaType();
    VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *) pType->pbFormat;
    
    if(pvi->bmiHeader.biBitCount != 24) {
		std::cerr << "cgv/media/ds_filter/transform_callback received media sample of non 24 bit type; skipping that." << std::endl;
        return NOERROR;
    }

    // Get the image properties from the BITMAPINFOHEADER
	image.width  = pvi->bmiHeader.biWidth;
	image.height = abs(pvi->bmiHeader.biHeight);
	image.set_data_ptr((cgv::scene::mat::color<unsigned char, cgv::scene::mat::RGB>*)pData);

    if(cb)
		(*cb)(&image);
	else {
		for (int i=0; i<1000; ++i)
			pData[i] = i%100;
	}
    return NOERROR;
}


// check input media type
HRESULT transform_callback::CheckInputType(const CMediaType *mtIn)
{
    // ensure video format
    if (*mtIn->FormatType() != FORMAT_VideoInfo)
        return E_INVALIDARG;
	// ensure 24 bit header
    VIDEOINFO* vi = (VIDEOINFO*)mtIn->Format();
    if(vi->bmiHeader.biBitCount != 24)
        return E_INVALIDARG;
    return NOERROR;
}

// check transformation types
HRESULT transform_callback::CheckTransform(const CMediaType *mtIn,const CMediaType *mtOut)
{
    HRESULT hr;
    if (FAILED(hr = CheckInputType(mtIn)))
		return hr;

    // format must be a VIDEOINFOHEADER
    if (*mtOut->FormatType() != FORMAT_VideoInfo)
		return E_INVALIDARG;
    
    // formats must be big enough 
    if (mtIn->FormatLength() < sizeof(VIDEOINFOHEADER) ||
		mtOut->FormatLength() < sizeof(VIDEOINFOHEADER))
		return E_INVALIDARG;
    
    VIDEOINFO *pInput = (VIDEOINFO *) mtIn->Format();
    VIDEOINFO *pOutput = (VIDEOINFO *) mtOut->Format();
    if (memcmp(&pInput->bmiHeader,&pOutput->bmiHeader,sizeof(BITMAPINFOHEADER)) == 0)
		return NOERROR;
    return E_INVALIDARG;
}

HRESULT transform_callback::DecideBufferSize(IMemAllocator *pAlloc,ALLOCATOR_PROPERTIES *pProperties)
{
    // Is the input pin connected
    if (m_pInput->IsConnected() == FALSE)
        return E_UNEXPECTED;

    ASSERT(pAlloc);
    ASSERT(pProperties);
    return NOERROR;
}

HRESULT transform_callback::GetMediaType(int iPosition, CMediaType *pMediaType)
{
    // Is the input pin connected
    if (m_pInput->IsConnected() == FALSE)
        return E_UNEXPECTED;

    // This should never happen
    if (iPosition < 0)
        return E_INVALIDARG;

    // Do we have more items to offer
    if (iPosition > 0)
        return VFW_S_NO_MORE_ITEMS;

    *pMediaType = m_pInput->CurrentMediaType();
    return NOERROR;
}

// set transform_callback
STDMETHODIMP transform_callback::set_callback(void (__cdecl *_cb)(void*))
{
    CAutoLock cAutoLock(&my_lock);
    cb = _cb;
    return NOERROR;
}

// Handle registration of this filter
STDAPI DllRegisterServer()
{
	return AMovieDllRegisterServer2( TRUE );
}

STDAPI DllUnregisterServer()
{
	return AMovieDllRegisterServer2( FALSE );
}
