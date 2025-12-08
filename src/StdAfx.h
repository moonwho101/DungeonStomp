// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__D74B7B03_00D1_11D4_8B0F_0050BAC83302__INCLUDED_)
#define AFX_STDAFX_H__D74B7B03_00D1_11D4_8B0F_0050BAC83302__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Stdlib.h>
#include <Stdio.h>
#include <String.h>
#include "IceTypes.h"

#define DELETEARRAY(x)  \
	{                   \
		if (x != null)  \
			delete[] x; \
		x = null;       \
	}
#define CHECKALLOC(x) \
	if (!x)           \
		return false;

//!	A function to clear a buffer.
//!	\param	addr		buffer address
//!	\param	size		buffer length
//!	\see	FillMemory
//!	\see	CopyMemory
//!	\see	MoveMemory
__forceinline void ZeroMemory(void *addr, udword size) {
	memset(addr, 0, size);
}

#include "IceRevisitedRadix.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__D74B7B03_00D1_11D4_8B0F_0050BAC83302__INCLUDED_)
