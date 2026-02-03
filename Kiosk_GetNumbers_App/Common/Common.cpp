#include "pch.h"
#include "Common.h"

Gdiplus::Image* Common::LoadPNGFromResource(UINT nIDResource)
{
	HRSRC hRes = FindResource(AfxGetInstanceHandle(), MAKEINTRESOURCE(nIDResource), _T("PNG"));
	if (!hRes) return nullptr;

	DWORD dwSize = SizeofResource(AfxGetInstanceHandle(), hRes);
	HGLOBAL hResData = LoadResource(AfxGetInstanceHandle(), hRes);
	if (!hResData) return nullptr;

	void* pBuffer = LockResource(hResData);
	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, dwSize);
	if (!hMem) return nullptr;

	void* pData = GlobalLock(hMem);
	memcpy(pData, pBuffer, dwSize);
	GlobalUnlock(hMem);

	IStream* pStream = nullptr;
	Gdiplus::Image* pFinalImg = nullptr;

	if (CreateStreamOnHGlobal(hMem, TRUE, &pStream) == S_OK)
	{
		Gdiplus::Image* pTempImg = Gdiplus::Image::FromStream(pStream);
		if (pTempImg && pTempImg->GetLastStatus() == Gdiplus::Ok)
		{
			pFinalImg = pTempImg->Clone();
			delete pTempImg;
		}
		pStream->Release();
	}
	return pFinalImg;
}