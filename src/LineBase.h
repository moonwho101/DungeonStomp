// LineBase.h: interface for the CLineBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LINEBASE_H__3035B763_37A3_11D3_8D13_F5AB64295028__INCLUDED_)
#define AFX_LINEBASE_H__3035B763_37A3_11D3_8D13_F5AB64295028__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define LINETYPE_LINE3D 1
#define LINETYPE_RAY3D 2
#define LINETYPE_SEGMENT3D 3

class CLineBase
{
public:
	int nLineType;
	CLineBase();
	virtual ~CLineBase();
};

#endif // !defined(AFX_LINEBASE_H__3035B763_37A3_11D3_8D13_F5AB64295028__INCLUDED_)
