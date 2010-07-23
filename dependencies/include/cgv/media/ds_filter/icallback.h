#pragma once

extern "C" {
	DEFINE_GUID(IID_ICallback, 0xcb1f62c7, 0x605c, 0x4fd2, 0x80, 0xb2, 0xe0, 0x72, 0x34, 0xdc, 0x5, 0x73);
    DECLARE_INTERFACE_(ICallback, IUnknown)
    {
	    STDMETHOD(set_callback) (THIS_
			void (*cb)(void*)
		) PURE;
    };
}

