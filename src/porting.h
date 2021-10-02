#pragma once

// porting header
// allows CD3DBitmapFont to be used without other LoneStar engine files and without WTL
// note: if any of these definitions conflict with your other includes, such as WTL or MFC, feel free to remove the definitions in this file

#ifdef UNICODE
#define _tstring std::wstring
#else
#define _tstring std::string
#endif

struct CString : _tstring
{
	CString() {}
	CString(LPCTSTR Start) : _tstring(Start) {}
	// len = npos isn't valid?
	CString(LPCTSTR Start, int len) : _tstring(Start, len) {}
	operator LPCTSTR() const { return c_str(); }
	int GetLength() const { return (int)size(); }
	int IsEmpty() const { return empty(); }
	char operator[](int n) const { return _tstring::operator[](n); }
	// why i use CString instead of std::string: CString has 'fully functional' LoadString/Format members,
	// that handle anysize strings, std::string doesn't.
	void LoadString(UINT id)
	{
		TCHAR storage[1000];
		::LoadString(GetModuleHandle(0), id, storage, 1000);
		*this = storage;
	}
	void Format(LPCTSTR fmt, ...)
	{
		va_list v;
		va_start(v, fmt);
		TCHAR storage[1000];
		_vsntprintf(storage, 1000, fmt, v);
		*this = storage;
	}
};

struct ATL_NO_VTABLE CSimpleD3DDeviceDependentObject
{
	virtual void OnResetDevice()
	{
	}

	virtual void OnLostDevice()
	{
	}
};

struct CFont
{
	CFont(HFONT hf) : hf(hf) {}
	~CFont()
	{
		if (hf)
			DeleteObject(hf);
	}
	operator HFONT() { return hf; }
	HFONT hf;
};

#undef SelectFont
#undef SelectBitmap

struct CDC
{
	CDC(HDC hdc) : hdc(hdc) {}
	~CDC()
	{
		if (hdc)
			DeleteDC(hdc);
	}
	operator HDC() { return hdc; }
	HFONT SelectFont(HFONT newfont) { return (HFONT)SelectObject(hdc, newfont); }
	HBITMAP SelectBitmap(HBITMAP newbitmap) { return (HBITMAP)SelectObject(hdc, newbitmap); }
	HDC hdc;
};

struct CBitmap
{
	CBitmap(HBITMAP hf) : hf(hf) {}
	~CBitmap()
	{
		if (hf)
			DeleteObject(hf);
	}
	operator HBITMAP() { return hf; }
	HBITMAP hf;
};

inline void ThrowIfFailed(HRESULT hr, LPCTSTR msg)
{
	if (FAILED(hr))
		throw exception(msg);
}

inline void ThrowExceptionFromHresult(HRESULT hr, LPCTSTR msg)
{
	throw exception(msg);
}

template <class VERTEX>
class CD3DVertexBufferLock
{
public:
	CD3DVertexBufferLock(IDirect3DVertexBuffer8 *pVB, UINT OffsetToLock = 0, UINT SizeToLock = 0, DWORD Flags = 0)
		: m_pVB(pVB)
	{
		HRESULT hr = pVB->Lock(OffsetToLock, SizeToLock, (BYTE **)&m_pData, Flags);
		ATLASSERT(SUCCEEDED(hr));
	}

	~CD3DVertexBufferLock()
	{
		m_pVB->Unlock();
	}

	operator VERTEX *()
	{
		return m_pData;
	}

private:
	IDirect3DVertexBuffer8 *m_pVB;
	VERTEX *m_pData;
};

template <class INDEX = WORD>
class CD3DIndexBufferLock
{
public:
	CD3DIndexBufferLock(IDirect3DIndexBuffer8 *pIB, UINT OffsetToLock = 0, UINT SizeToLock = 0, DWORD Flags = 0)
		: m_pIB(pIB)
	{
		HRESULT hr = pIB->Lock(OffsetToLock, SizeToLock, (BYTE **)&m_pData, Flags);
		ATLASSERT(SUCCEEDED(hr));
	}

	~CD3DIndexBufferLock()
	{
		m_pIB->Unlock();
	}

	operator INDEX *()
	{
		return m_pData;
	}

private:
	IDirect3DIndexBuffer8 *m_pIB;
	INDEX *m_pData;
};

struct CRect : RECT
{
	SIZE Size()
	{
		SIZE s = {right - left, bottom - top};
		return s;
	}
};
