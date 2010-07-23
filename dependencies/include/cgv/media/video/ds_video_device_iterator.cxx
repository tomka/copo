#include <cgv/media/video/ds_video_device_iterator.h>
#include <cgv/media/video/ds_common.h>
#include <iostream>
#include <atlstr.h>

namespace cgv {
	namespace media {
		namespace video {

ds_video_device_iterator::ds_video_device_iterator()
{
	ensure_com();
	// Create the system device enumerator
	if (FAILED(hr = CoCreateInstance (CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC,
		IID_ICreateDevEnum, (void **) &pDevEnum))) {
		std::cerr << "couldn't create system enumerator!  hr=" << hr << std::endl;
		return;
	}
	// Create an enumerator for the video capture devices
	if (FAILED(hr = pDevEnum->CreateClassEnumerator (CLSID_VideoInputDeviceCategory, &pClassEnum, 0))) {
		std::cerr << "couldn't create class enumerator!  hr=" << hr << std::endl;
		return;
	}
	// If there are no enumerators for the requested type, then 
	// CreateClassEnumerator will succeed, but pClassEnum will be NULL.
	pMoniker = NULL;
	++(*this);
}

ds_video_device_iterator::~ds_video_device_iterator()
{
	if (pMoniker != NULL)
		pMoniker->Release();
	if (pDevEnum != NULL)
		pDevEnum->Release();
	if (pClassEnum != NULL)
		pClassEnum->Release();
}
void ds_video_device_iterator::operator ++()
{
	if (pMoniker != NULL)
		pMoniker->Release();
	if (pClassEnum == NULL) {
		pMoniker = NULL;
		return;
	}
	ULONG cFetched;
	if (S_OK != (pClassEnum->Next (1, &pMoniker, &cFetched)))
		pMoniker = NULL;
}
bool ds_video_device_iterator::at_end() const
{
	return pMoniker == NULL;
}
IMoniker* ds_video_device_iterator::operator -> ()
{
	return pMoniker;
}
std::string ds_video_device_iterator::get_name() 
{
	if (pMoniker == NULL)
		return std::string();

    IPropertyBag *pPropBag;

    // Associate moniker with a file
    if (FAILED(hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropBag)))
		return std::string();
    // Read filter name from property bag
	VARIANT varName={0};
	varName.vt = VT_BSTR;
    hr = pPropBag->Read(L"FriendlyName", &varName, 0);
    if (FAILED(hr = pPropBag->Read(L"FriendlyName", &varName, 0)))
		return std::string();
    // Get filter name (converting BSTR name to a CString)
	LPCWSTR str(varName.bstrVal);
	USES_CONVERSION;
	LPSTR astr = W2A(str);
    SysFreeString(varName.bstrVal);
	pPropBag->Release();
	return std::string(astr);
}
		}
	}
}