#pragma once

#ifndef LONESTAR_DXGDECL_H_INCLUDED

// If this file is compiled without the engine header file, define some required things here

typedef CComPtr<IDirect3DTexture8> IDirect3DTexture8Ptr;

namespace LoneStar
{

#endif

	//
	// CD3DBitmapFont -- A back-ported OpenGL port of D3DFont
	//
	// A bitmap font class that uses runtime texture generation.
	// See d3dbitmapfont.cpp for some features supported by this class
	//

	// Define required types, RECTFLOAT and SIZEFLOAT
	// They are equivalent to RECT and SIZE types, except they have floating point coordinates

#ifndef RECTFLOAT_DEFINED
#define RECTFLOAT_DEFINED
	// RECT structure for floating point coordinates
	struct RECTFLOAT
	{
		float left, top, right, bottom;
	};

	struct CRectFloat : public RECTFLOAT
	{
		CRectFloat() {}
		CRectFloat(float left, float top, float right, float bottom)
		{
			this->left = left;
			this->top = top;
			this->right = right;
			this->bottom = bottom;
		}
	};

#endif

#ifndef SIZEFLOAT_DEFINED
#define SIZEFLOAT_DEFINED
	// SIZE structure for floating point coordinates
	struct SIZEFLOAT
	{
		float cx, cy;
	};

	struct CSizeFloat : public SIZEFLOAT
	{
		CSizeFloat() {}
		CSizeFloat(float cx, float cy)
		{
			this->cx = cx;
			this->cy = cy;
		}
	};
#endif

	// Prototype for the state procedure
	// It will be called after standard states have been applied and before actual rendering

	typedef void(APIENTRY *APPLYSTATEPROC)(IDirect3DDevice8 *pd3ddev);

	// This structure has all information necessary to describe a request issued to D3DFont
	// Originally, I had several overloaded Print and GetExtent functions, but as the number of
	// possible arguments increased as I added features, I decided to combine all arguments in a
	// single structure. Several constructors are provided for ease of use, and the main class
	// only needs one entry point per function, which is convenient for the developer (me)

	struct BITMAP_FONT_DRAW_STRUCT
	{
		// Flags specifying which other members of this struct are filled and how they are interpreted
		DWORD Flags;
		// Starting position for text rendering.
		// Text will be positioned so that (x, y, z) will be its
		// * left, top boundary for left aligned text
		// * right, top boundary for right aligned text
		// * center, top boundary for center aligned text
		// Rendering position grows in
		// * positive y direction for transformed text
		// * negative y direction for untransformed text
		float x, y, z;
		// Height of the text
		// Setting this parameter to zero will render text at the size specified during creation
		// If text is viewport-scaled, this number is the ratio of text height to viewport height
		float Height;
		// Text color
		DWORD color;
		// Text to render
		CString str;
		// Length of the text to render. Passing -1 will render entire string
		int len;
		// Clipping or wrapping rectangle
		RECTFLOAT ClipRect;
		// State block or state proc
		// If a state block is specified, it will be applied before any text is rendered and
		// after standard block has been applied. In particular, custom state blocks
		// only affect rendering of the text they are specified for
		// Only standard states are restored. If the state block has a state that is not normally
		// changed by CD3DBitmapFont, it will not be restored.
		// If a state proc is specified, it will be called instead of
		// pd3ddev->ApplyStateBlock(StateBlock)
		union
		{
			DWORD StateBlock;
			APPLYSTATEPROC ApplyStateProc;
		};
		// Viewport height, for viewport scaling
		// If you know the height, set it here so that the class doesn't have to retrieve it
		// You should always set this member if you're using viewport scaling with multiple viewports
		// on pure devices, because GetViewport doesn't work for pure devices
		// Note that window height is precalculated and using it is about as fast as using this member
		SIZE ViewportSize;

		// Defalult clipping/wrapping rectangle
		// Its values are meaningless and are not used anywhere
		// Whether ClipRect has significant values or not is determined by Flags, not by
		// ClipRect's contents
		static RECTFLOAT DefaultRect;
		// Default viewport size
		static SIZE DefaultSize;

		// A bunch of constructors to mostly accomodate different flag combinations

		BITMAP_FONT_DRAW_STRUCT()
		{
		}

		BITMAP_FONT_DRAW_STRUCT(DWORD Flags, float x, float y, float z, float Height, DWORD color,
								const CString &str, int len, const RECTFLOAT &ClipRect, DWORD StateBlock = 0, const SIZE &ViewportSize = DefaultSize)
			: Flags(Flags), x(x), y(y), z(z), Height(Height), color(color), str(str), len(len), ClipRect(ClipRect), StateBlock(StateBlock), ViewportSize(ViewportSize)
		{
		}

		BITMAP_FONT_DRAW_STRUCT(DWORD Flags, float x, float y, float z, float Height, DWORD color,
								const CString &str, int len, const RECTFLOAT &ClipRect, APPLYSTATEPROC ApplyStateProc, const SIZE &ViewportSize = DefaultSize)
			: Flags(Flags), x(x), y(y), z(z), Height(Height), color(color), str(str), len(len), ClipRect(ClipRect), ApplyStateProc(ApplyStateProc), ViewportSize(ViewportSize)
		{
		}

		BITMAP_FONT_DRAW_STRUCT(DWORD Flags, float x, float y, float z, float Height, DWORD color,
								const CString &str, int len, const SIZE &ViewportSize)
			: Flags(Flags), x(x), y(y), z(z), Height(Height), color(color), str(str), len(-1), ClipRect(DefaultRect), StateBlock(0), ViewportSize(ViewportSize)
		{
		}

		BITMAP_FONT_DRAW_STRUCT(DWORD Flags, float x, float y, float z, float Height, DWORD color,
								const CString &str, int len = -1)
			: Flags(Flags), x(x), y(y), z(z), Height(Height), color(color), str(str), len(len), ClipRect(DefaultRect), StateBlock(0), ViewportSize(DefaultSize)
		{
		}

		BITMAP_FONT_DRAW_STRUCT(DWORD Flags, float x, float y, DWORD color, const CString &str, int len = -1)
			: Flags(Flags), x(x), y(y), z(0), Height(0), color(color), str(str), len(len), ClipRect(DefaultRect), StateBlock(0), ViewportSize(DefaultSize)
		{
		}
	};

	// Rendering flags
	// If this flag is set, culling is set to none
	const DWORD LSBF_TWO_SIDED = 0x00000001;
	// This flag enables ZENABLE and ZWRITEENABLE
	const DWORD LSBF_WITH_DEPTH = 0x00000002;
	// This flag causes linear filtering of text, otherwise point filtering is used
	const DWORD LSBF_FILTER = 0x00000004;
	// This flag specifies that transformed vertices should be output
	// This flag renders in D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 FVF
	const DWORD LSBF_TRANSFORMED = 0x00000000;
	// The two following flags specify that untransformed vertices should be output
	// This flag renders in D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 FVF
	const DWORD LSBF_TRANSFORM_N = 0x00100000;
	// This flag renders in D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 FVF
	const DWORD LSBF_TRANSFORM_C = 0x00200000;
	// This flag renders in D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1 FVF
	const DWORD LSBF_TRANSFORM_NC = 0x00300000;
	// Default: don't perform either clipping or wrapping
	const DWORD LSBF_DONT_CLIP = 0x00000000;
	// Clip text outside ClipRect
	const DWORD LSBF_CLIP_RECT = 0x00000010;
	// Wrap text so that it appears inside ClipRect
	// If the text is left-aligned, it is wrapped on the .right side of the ClipRect
	// If the text is right-aligned, it is wrapped on the .left side of the ClipRect
	// If the text is center-aligned, 'x' component of BITMAP_FONT_DRAW_STRUCT is ignored
	// and the text is fit between .left and .right sides of ClipRect
	// No vertical clipping is performed unless requested
	const DWORD LSBF_WRAP_RECT = 0x00000020;
	// Enables special first lines. This flag is only applicable to left aligned text.
	// Ordinarily, if 'x' field of BITMAP_FONT_DRAW_STRUCT is not the same as ClipRect.left
	// field, either may be used as the beginning column of subsequent lines of text,
	// both for wrapping and if newline is embedded into the text.
	// This flag uses 'x' as the offset for the first line and all subsequent lines after hardcoded
	// newline, and ClipRect.left as the offset for all lines that were created during wrapping process
	const DWORD LSBF_FIRST_LINE_SPECIAL = 0x00000040;
	// This flag will cause StateBlock to be applied after standard state blocks are applied
	// for this request only
	const DWORD LSBF_CUSTOM_STATE_BLOCK = 0x00001000;
	// The same behavior, except StateProc will be called instead of pd3ddev->ApplyStateBlock
	const DWORD LSBF_CUSTOM_STATE_PROC = 0x00002000;
	// Implementation detail: these flags are handled outside the main rendering algorithm
	// This flag left-aligns text (default)
	const DWORD LSBF_ALIGN_LEFT = 0x00000000;
	// This flag right-aligns text. Text ends at 'x' offset and starts to the right of 'x'
	const DWORD LSBF_ALIGN_RIGHT = 0x00000100;
	// This flag center-aligns text. Text's center conincides with 'x' offset
	const DWORD LSBF_ALIGN_CENTER = 0x00000200;
	// This flag requests viewport-scaling of the text. Height field of BITMAP_FONT_DRAW_STRUCT
	// must be nonzero and must contain the height of the text relative to viewport height
	const DWORD LSBF_VIEWPORT_SCALE = 0x00080000;
	// Use GetViewport to obtain viewport height. Note: doesn't work on pure devices
	const DWORD LSBF_USE_VIEWPORT_HEIGHT = 0x00010000;
	// Use the window height for scaling
	const DWORD LSBF_USE_WINDOW_HEIGHT = 0x00000000;
	// Use the provided ViewportHeight member for scaling
	const DWORD LSBF_PROVIDED_VIEWPORT_HEIGHT = 0x00020000;

	// CD3DBitmapFont -- the font class

	struct BATCH_INFO;

	class CD3DBitmapFont : public CSimpleD3DDeviceDependentObject
	{
	public:
		// Construction and destruction
		CD3DBitmapFont();
		~CD3DBitmapFont();

		// Creation and deletion
		// WARNING: this class can only work with ONE Direct3D device.
		// Using it with multiple devices will lead to unpredictable results.
		// You wouldn't want to use more than one device in a single application due to
		// performance considerations anyway; use multiple viewports instead.
		HRESULT Create(IDirect3DDevice8 *pd3ddev, const LOGFONT &LogFont, BOOL DoMipMap = FALSE, UINT nFirstChar = 32, UINT nCharCount = 127 - 32);
		void Delete();

		// Wrap text rendering calls with these two functions to batch them together
		// Note: internally, batching is invoked for certain flag combinations
		// Don't batch requests that need different transformations, etc.
		// You can safely batch untransformed/transformed requests, as long as you don't change
		// transformations
		//
		// If CanSort is true, the requests will be sorted by texture and state changes
		// This might increase performance if you render text using first font, then second font,
		// then first font again, etc. Keep in mind that sorting takes time, so if you don't need it,
		// don't use it.
		//
		// An internal reference count is maintained for batch requests.
		// This means you can safely nest BeginBatch/EndBatch calls.
		// This also means that CanSort will only be considered for the outermost BeginBatch call:
		// once batching is activated, you cannot switch from sorting to not-sorting or back.

		static void BeginBatch(bool CanSort = false);
		static void EndBatch();

		// Text printing
		// This function will print everything according to the parameters specified in the req structure
		// This is the only entry point for printing
		void PrintText(const BITMAP_FONT_DRAW_STRUCT &req) const;

		// Text extent determination
		// SIZEFLOAT version will return the width and height of the text as printed.
		// RECTFLOAT version will return the bounds of the text before any D3D transforms, as printed
		// These functions handle all flags that PrintText can handle.
		//
		// Note that determining text extent can be both quite fast and quite slow, depending on
		// the flags you specified. Wrapping and clipping will significantly slow down the
		// calculations. Note that for simple (left aligned, not clipped, not wrapped) text
		// SIZEFLOAT will be faster, while for the complicated text (clipped/wrapped) RECTFLOAT
		// version will be faster. It's not recommended to recalculate text extent for static strings
		// every frame, especially if you're using heavy flags (clipping/wrapping), as the performance
		// penalty could be severe.
		//
		// Note on LSBF_FIRST_LINE_SPECIAL flag: the extent returned will not account for
		// outstanding first line. You need to calculate the gap/lead manually and add it to the
		// calculated extent. These functions can return negative dimensions if the string fits
		// in the first line lead.

		void GetTextExtent(const BITMAP_FONT_DRAW_STRUCT &req, RECTFLOAT &r) const;
		void GetTextExtent(const BITMAP_FONT_DRAW_STRUCT &req, SIZEFLOAT &sz) const;

		// Returns the number of letters that fits in the specified width
		// The dimensions of the string are returned in sz
		// Note: this method does not handle clipping, wrapping, and it ignores alignment.
		// Clipping will lead to a total mess, there's no clear way to support it.
		// With wrapping, all text will fit whatever width is specified, except if the width is
		// ridiculously small. Alignment doesn't matter and thus is ignored.
		int GetNumFittingChars(const BITMAP_FONT_DRAW_STRUCT &req, float MaxWidth, SIZEFLOAT &sz) const;

		// Device objects creation/deletion
		// Call these for every font that you have created
		// The class maintains a reference count on global device objects
		void OnLostDevice();
		void OnResetDevice();

	private:
		// The range of characters contained in this font
		UINT m_nFirstChar, m_nCharCount;
		// Texture coordinates for every letter
		std::vector<RECTFLOAT> m_CharTexRects;
		// Real dimensions for every letter
		std::vector<ABCFLOAT> m_CharWidths;
		// Assuming all characters have the same height
		float m_CharHeight;
		// Height specified in LOGFONT during creation, used for scaling
		float m_FontHeight;
		// D3D texture
		IDirect3DTexture8Ptr m_pTexture;
		// Cached width and height of our font texture, for clipping
		UINT m_TexWidth, m_TexHeight;

		// Helpers. These are member functions since they need access to per-instance data
		// This function is called when batch limit is exceeded, or when batching is complete
		// It parses the batches, converts them to simple text, and sends the text to BatchDataToVertex
		static void FlushBatchData();
		// This function converts strings to vertices and writes them to vertex buffer
		void BatchDataToVertex(BATCH_INFO &BatchData, int StringLength, int &NumChars, void *&pv) const;
		// This member is needed to prevent release of global batch data if it was never allocated
		// It indicates whether this font object is holding a reference to the global batch data
		bool m_BatchDataRef;

		// This function takes a right- or center-aligned request and "unaligns" it,
		// producing several strings of left-aligned text starting at appropriate offsets
		void InternalUnalignPrint(const BITMAP_FONT_DRAW_STRUCT &req) const;
		// The same thing for wrapped text. This function splits text up so that split strings fit in the
		// specified width
		void InternalUnwrapPrint(const BITMAP_FONT_DRAW_STRUCT &req) const;

		// Internal methods handling text extent calculations
		// This method calculates text extent of unclipped and unwrapped text
		void InternalSimpleGetTextExtent(const BITMAP_FONT_DRAW_STRUCT &req, SIZEFLOAT &sz) const;
		// This one calls its SIZEFLOAT counterpart to do the work
		void InternalSimpleGetTextExtent(const BITMAP_FONT_DRAW_STRUCT &req, RECTFLOAT &r) const;
		// This method converts wrapped text to several strings of unwrapped text
		void InternalUnwrapGetExtent(const BITMAP_FONT_DRAW_STRUCT &req, RECTFLOAT &r) const;
		// This helper is called by InternalUnwrapGetExtent and it basically just calls other
		// methods depending on the flags. It's here for speed optimization, mostly.
		void InternalPostUnwrapGetExtent(const BITMAP_FONT_DRAW_STRUCT &req, RECTFLOAT &r) const;
		// This method convers right- or center-aligned text to left-aligned text
		void InternalUnalignGetExtent(const BITMAP_FONT_DRAW_STRUCT &req, RECTFLOAT &r) const;
		// This method does the grunt work of determining text extent for text requiring clipping
		void InternalClipGetTextExtent(const BITMAP_FONT_DRAW_STRUCT &req, RECTFLOAT &r) const;

		// This method does the work of determining how many characters fit in the specified width
		int InternalGetNumFittingChars(float ScaleFactor, const CString &str, int len, float ResetX, float MaxWidth, SIZEFLOAT &sz) const;

		// This method verifies that flags combination is valid, takes care of viewport scaling,
		// and writes actual string length into req if it wasn't specified (-1).
		void PreProcessDrawStruct(BITMAP_FONT_DRAW_STRUCT &req, float *pInvScale = 0) const;

		// Be careful with assignment and copy construction. Presently, thanks to usage of smart
		// pointers and STL containers, compiler-generated assignment and copy-constructors are
		// just fine
		//	CD3DBitmapFont(const CD3DBitmapFont& other);
		//	CD3DBitmapFont& operator =(const CD3DBitmapFont& other);
	};

#ifndef LONESTAR_DXGDECL_H_INCLUDED

} // end namespace LoneStar
using namespace LoneStar;

#endif
