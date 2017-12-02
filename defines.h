/*!
 * \file defines.h
 * \brief Definition Support File
 *
 * This file contains constant, alias and variable type defintions.
 *
 * \author David Legault ( clickhere at scriptsdb dot org )
 *
 * � ScriptsDB.org - 2006
 */

/*
Some useful values for _MSC_VER if you need to target to a specific compiler.
http://symbiancorner.blogspot.com/2007/05/how-to-detect-version-of-ms-visual.html

#if _MSC_VER >= 1911 // Visual C++ 2017.3
#elif _MSC_VER >= 1910 // Visual C++ 2017
#elif _MSC_VER >= 1900 // Visual C++ 2015
#elif _MSC_VER >= 1800 // Visual C++ 2013
#elif _MSC_VER >= 1700 // Visual C++ 2012
#elif _MSC_VER >= 1600 // Visual C++ 2010 ?
#elif _MSC_VER >= 1500 // Visual C++ 2008
#elif _MSC_VER >= 1400 // Visual C++ 2005
#elif _MSC_VER >= 1310 // Visual C++ .NET 2003
#elif _MSC_VER > 1300 // Visual C++ .NET 2002
#endif
*/
#pragma once

#ifndef _DEFINES_H_
#define _DEFINES_H_

// VS2017+ only
#if !defined(_MSC_FULL_VER) || _MSC_FULL_VER < 191125542
#error "This version of DCX needs Visual Studio 2017.4.0 or newer"
#endif

#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( disable : 1195 )
#pragma warning( disable : 504 )
#pragma warning( disable : 1563 )
#pragma warning( disable : 869 )	// remark #869: parameter "doc" was never referenced
#pragma warning( disable : 981 )	// remark #981: operands are evaluated in unspecified order
#pragma warning( disable : 1419 )	// remark #1419: external declaration in primary source file
#pragma warning( disable : 1418 )	// remark #1418: external function definition with no prior declaration http://software.intel.com/en-us/articles/cdiag1418/
#pragma warning( disable : 383 )	// remark #383: value copied to temporary, reference to temporary used
#pragma warning( disable : 2292 )	// warning #2292: destructor is declared but copy constructor and assignment operator are not
#pragma warning( disable : 654 )	// warning #654: overloaded virtual function "DcxControl::toXml" is only partially overridden in class
#pragma warning( disable : 444 )	// remark #444: destructor for base class "DWebBrowserEvents2" (declared at line 1948 of "C:\Program Files\Microsoft SDKs\Windows\v7.0\\include\exdisp.h") is not virtual
// intel compiler has problems with .def file
#define _INTEL_DLL_ __declspec(dllexport)
#else
#pragma warning( disable : 4100 ) // unreferenced formal parameter
#pragma warning( disable : 4530 )
#pragma warning( disable : 4995 ) // name was marked as #pragma deprecated
#pragma warning( disable : 4820 ) // 'x' bytes padding added after data member '....'
#define _INTEL_DLL_ __declspec(dllexport)
#endif
#pragma warning( disable : 26499 ) //  warning C26499 : Could not find any lifetime tracking information for '........'
#pragma warning( disable : 26481 ) //  warning C26481 : Don't use pointer arithmetic. Use span instead. (bounds.1: http://go.microsoft.com/fwlink/p/?LinkID=620413)
#pragma warning( disable : 26482 ) //  warning C26482 : Only index into arrays using constant expressions. (bounds.2: http://go.microsoft.com/fwlink/p/?LinkID=620414)

// --------------------------------------------------
// Optional build libraries for DCX
// --------------------------------------------------
// DCX using DirectX SDK? (Required for DirectShow)
// This is included as part of the win10 sdk with vs2017 no separate install needed.
#define DCX_USE_DXSDK 1
// end of DirectX SDK

// always 1
#define DCX_USE_WINSDK 1

// DCX using GDI+? (Required for advanced graphics routines) (replace with Direct2D ?)
#define DCX_USE_GDIPLUS 1
#define DCX_MAX_GDI_ERRORS 21
// end of GDI+

// use string hashes for compares etc..? (non-hashing code removed, always use this now)
#define DCX_USE_HASHING 1

// DCX Using C++11 regex
// NB: Can't be used with either BOOST OR PCRE enabled.
#define DCX_USE_CREGEX 0

// --------------------------------------------------
// Some compiler/library definitions
// --------------------------------------------------
//#define WIN32_LEAN_AND_MEAN //!< blah

// Windows 98 + IE V5.01 + GDI+ 1.0
//#define _WIN32_WINDOWS 0x0410
//#define _WIN32_WINNT 0x0410
//#define _WIN32_IE 0x0501
//#define WINVER 0x0410
//#define GDIPVER 0x0100

// Windows XP + IE V5.01 + GDI+ 1.0
//#define _WIN32_WINNT 0x0501
//#define _WIN32_IE 0x0501
//#define WINVER 0x0501
//#define GDIPVER 0x0100

// Windows XP SP1 + IE V6 + GDI+ 1.0
//#define _WIN32_WINNT 0x0502
//#define _WIN32_IE 0x0600
//#define WINVER 0x0502
//#define GDIPVER 0x0100

// Windows XP SP2 + IE V6 + GDI+ 1.1
//#define _WIN32_WINNT 0x0503
//#define _WIN32_IE 0x0600
//#define WINVER 0x0503
//#define GDIPVER 0x0110

// Windows Vista + IE V7 + GDI+ 1.1
//#define _WIN32_WINNT 0x0600
//#define _WIN32_IE 0x0700
//#define WINVER 0x0600
//#define GDIPVER 0x0110

// Windows 7 + IE V8 + GDI+ 1.1
//#define _WIN32_WINNT 0x0601
//#define _WIN32_IE 0x0800
//#define WINVER 0x0601
//#define GDIPVER 0x0110

// Windows 7 + IE V10 + GDI+ 1.1
//#define _WIN32_WINNT 0x0601
//#define _WIN32_IE 0x0A00
//#define WINVER 0x0601
//#define GDIPVER 0x0110

// Windows 8 + IE V10 + GDI+ 1.1
//#define _WIN32_WINNT 0x0602
//#define _WIN32_IE 0x0A00
//#define WINVER 0x0602
//#define GDIPVER 0x0110

// Windows 8.1 + IE V10 + GDI+ 1.1
//#define _WIN32_WINNT 0x0603
//#define _WIN32_IE 0x0A00
//#define WINVER 0x0603
//#define GDIPVER 0x0110

// Windows 10 + IE V11 + GDI+ 1.1
#define _WIN32_WINNT 0x0A00
#define _WIN32_IE 0x0A00
#define WINVER 0x0A00
#define GDIPVER 0x0110

#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_SECURE_NO_WARNINGS 1

#define _CRT_RAND_S 1

// Includes created git build info header...
#include "gitBuild.h"

// --------------------------------------------------
// DCX build version
// --------------------------------------------------
#define DLL_VERSION    GIT_DESCRIBE
#define DLL_BUILD      GIT_HASH
#define DLL_DEV_BUILD  52

#ifdef NDEBUG
#ifdef DCX_DEV_BUILD
// Dev Build, enable debug output.
#define DCX_DEBUG_OUTPUT 1
#define DCX_DEBUG(x,y,z) x((y), (z));
#define DLL_STATE      TEXT("Development Build")
// Link with DirectX error lib, enables extra error reporting.
#define DCX_DX_ERR	1
#else
// Release Build, disable debug info.
#define DCX_DEBUG_OUTPUT 0
#define DCX_DEBUG(x,y,z)
#define DLL_STATE      TEXT("Release Build")
#define _SECURE_SCL 0 // disables checked iterators
#endif
#else
// Debug Build, enable debug output.
#define DCX_DEBUG_OUTPUT 1
#define DCX_DEBUG(x,y,z) x((y), (z));
#define DLL_STATE      TEXT("Debug Build")
// Link with DirectX error lib, enables extra error reporting.
#define DCX_DX_ERR	1
// Use Object switch code (testing only)
#define DCX_SWITCH_OBJ 0
#endif

#if DCX_SWITCH_OBJ
#if !__has_include("SwitchObjects.h")
#error "Switch Objects enabled: "SwitchObjects.h" Required!"
#endif
#endif

// --------------------------------------------------
// Include files
// --------------------------------------------------
#define INITGUID
#define NOCOMM
#define NOSOUND
#define NOKANJI
#define NOHELP
#define NOMCX
#define OEMRESOURCE
//#define NOMINMAX

#pragma warning(push)
#pragma warning(disable: 4668)	//is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
#include <windows.h>
#include <windowsx.h>
#pragma warning(pop)
#include <vector>
#include <map>
#include <unordered_map>
#include <commctrl.h>
#include <richedit.h>
#include <cguid.h>

#if defined(DCX_USE_CREGEX)
#include <regex>
#endif

#include <memory>
#include <experimental\filesystem>
#include <optional>

// BrowseFolder
#include <shlobj.h>

#include <uxtheme.h>

#if WINVER >= 0x600
#include <vssym32.h>
#else
#include <tmschema.h>
#endif

#include <dwmapi.h>

#ifdef DCX_USE_GDIPLUS
// VS 2015 generates C4458 errors in GDI+ includes
//c:\program files\windows kits\10\include\10.0.10240.0\um\GdiplusHeaders.h(701): warning C4458: declaration of 'nativeCap' hides class member
// VS 2015 generates C4365 errors in GDI+ includes
//c:\program files(x86)\windows kits\10\include\10.0.14393.0\um\GdiplusBitmap.h(1008) : warning C4365 : 'initializing' : conversion from 'INT' to 'unsigned int', signed / unsigned mismatch

#pragma warning(push)
#pragma warning(disable: 4458)
#pragma warning(disable: 4365)
#include <gdiplus.h>
#pragma warning(pop)
//using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")
#endif

#ifdef DCX_USE_DXSDK
#if !__has_include(<Dxsdkver.h>)
#error "DirectX SDK Required!"
#endif
#include <Dxsdkver.h>
#ifdef DCX_DX_ERR
#include <Dxerr.h>
#pragma comment(lib, "DxErr.lib")
//#define DX_ERR(prop,cmd,hr) showErrorEx((prop), (cmd), TEXT("%s: %s"), DXGetErrorString((hr)), DXGetErrorDescription((hr)))
#define DX_ERR(prop,cmd,hr) showError((prop), (cmd), TEXT("%: %"), DXGetErrorString((hr)), DXGetErrorDescription((hr)))
#else
#define DX_ERR(prop,cmd,hr)
#endif
#endif

// max() macro causes issues with GSL, just use std::max/min instead
//#pragma push_macro("max")
#undef max
#undef min
#define GSL_THROW_ON_CONTRACT_VIOLATION 1
#include "GSL\gsl"
//#pragma pop_macro("max")
//using gsl::byte;
//using std::byte;
//using gsl::to_integer;

#include "AutoRelease.h"

#include "Classes/TString/tstring.h"
#include "simpleString.h"
#include "refString.h"
#include "Classes/tstring/hashString.h"

#include "XSwitchFlags.h"

#include "classes/dcxdialogcollection.h"
#include "Classes\tinyxml\tinyxml.h"

//#include "AggressiveOptimize.h"

#include "dcxExceptions.h"
#include "Classes\WindowStyles.h"

// Win2000+ stuff for Win98+ compat (only used during testing)
//#ifndef ICC_STANDARD_CLASSES
//#define ICC_STANDARD_CLASSES		0x00004000
//#endif
//#ifndef ICC_LINK_CLASS
//#define ICC_LINK_CLASS					0x00008000
//#endif
//#ifndef WS_EX_COMPOSITED
//#define WS_EX_COMPOSITED        0x02000000L
//#endif
//#ifndef WS_EX_NOACTIVATE
//#define WS_EX_NOACTIVATE        0x08000000L
//#endif
//#ifndef WS_EX_LAYERED
//#define WS_EX_LAYERED           0x00080000
//#endif
//#ifndef PWINDOWINFO
//#define PWINDOWINFO LPVOID
//#endif
//#ifndef PFLASHWINFO
//#define PFLASHWINFO LPVOID
//#endif
#ifndef WM_DWMCOMPOSITIONCHANGED
#define WM_DWMCOMPOSITIONCHANGED 0x031E
#endif

// --------------------------------------------------
// Listview stuff
// --------------------------------------------------
#ifndef LPNMLVDISPINFO
#define LPNMLVDISPINFO LPNMLVDISPINFOW
#endif

// --------------------------------------------------
// DCX defines
// --------------------------------------------------
#define WM_MCOMMAND		(WM_USER + 200)
#define WM_MEVALUATE	(WM_USER + 201)
#define MIRCF_EDITBOX	1
#define MIRCF_CMD		2
#define MIRCF_FLOOD		4
#define MIRCF_UNICODE	8
// causes mIRC 7.33Beta+ to return more detailed error information
#define MIRCF_ENHANCEDERRORS	16
// stand alone flag used by mIRC 7.33Beta+ to get info about mIRC
#define MIRCF_INFO		-1
// mIRC v7.33Beta+ SendMessage() enhanced error codes
//The return values are: 0 = success, 1 = failure or'd with 2 = bad mapfile name, 4 = bad mapfile size, 8 = bad eventid, 16 = bad server, 32 = bad script.
#define MIRCF_ERR_FAILED		1
#define MIRCF_ERR_MAP_NAME		2
#define MIRCF_ERR_MAP_SIZE		4
#define MIRCF_ERR_EVENTID		8
#define MIRCF_ERR_SERVER		16
#define MIRCF_ERR_SCRIPT		32
#define MIRCF_ERR_DISABLED		64

enum class mIRC_SendMessage_Flags : UINT {
	EDITBOX = 1,
	CMD = 2,
	FLOOD = 4,
	UNI_CODE = 8,
	ENHANCEDERRORS = 16 // causes mIRC 7.33Beta+ to return more detailed error information
};

enum class mIRC_SendMessage_ErrorCodes : UINT {
	OK,
	ERR_FAILED = 1,
	ERR_MAP_NAME = 2,
	ERR_MAP_SIZE = 4,
	ERR_EVENTID = 8,
	ERR_SERVER = 16,
	ERR_SCRIPT = 32,
	ERR_DISABLED = 64
};

// size of data buffer for mirc in characters!
#define MIRC_BUFFER_SIZE_CCH	4100
// size of same data buffer in bytes.
#define MIRC_BUFFER_SIZE_BYTES (MIRC_BUFFER_SIZE_CCH * sizeof(TCHAR))
// size of the mirc mapfile we will use in bytes.
#define MIRC_MAP_SIZE		(8192 * sizeof(TCHAR))
#define mIRC_ID_OFFSET 6000U //!< mIRC Dialog ID Offset
#define mIRC_ID_MAX (UINT_MAX -1)	//!< Highest ID allowed.
#define mIRC_MAX_CONTROLS	10000U	//!< Max number of controls allowed per dialog.
#define mIRC_PALETTE_SIZE	100U	// Number of colours in mIRC's palette (mIRC v7.51.212+ support 100 colours, 0 - 15 same as old mirc, 16-99 new colour palette)

#define DCX_LISTVIEWCLASS    TEXT("DCXListViewClass")     //!< DCX Listview Class Name
#define DCX_PROGRESSBARCLASS TEXT("DCXProgressBarClass")  //!< DCX ProgressBar Class Name
#define DCX_TREEVIEWCLASS    TEXT("DCXTreeViewClass")     //!< DCX TreeView Class Name
#define DCX_TOOLBARCLASS     TEXT("DCXToolBarClass")      //!< DCX ToolBar Class Name
#define DCX_RICHEDITCLASS    TEXT("DCXRichEditClass")     //!< DCX RichEdit Class Name
#define DCX_TABCTRLCLASS     TEXT("DCXTabCtrlClass")      //!< DCX Tab Class Name
#define DCX_REBARCTRLCLASS   TEXT("DCXRebarCtrlClass")    //!< DCX Rebar Class Name
#define DCX_COMBOEXCLASS     TEXT("DCXComboExClass")      //!< DCX ComboEx Class Name
#define DCX_BUTTONCLASS      TEXT("DCXButtonClass")       //!< DCX Button Class Name
#define DCX_STATUSBARCLASS   TEXT("DCXStatusBarClass")    //!< DCX StatusBar Class Name
#define DCX_COLORCOMBOCLASS  TEXT("DCXColorComboClass")   //!< DCX ColorCombo Class Name
#define DCX_TRACKBARCLASS    TEXT("DCXTrackBarClass")     //!< DCX TrackBar Class Name
#define DCX_UPDOWNCLASS      TEXT("DCXUpDownClass")       //!< DCX Updown Class Name
#define DCX_IPADDRESSCLASS   TEXT("DCXIpAddressClass")    //!< DCX IP Address Class Name
#define DCX_DIVIDERCLASS     TEXT("DCXDividerClass")      //!< DCX Divider Class Name
#define DCX_PANELCLASS       TEXT("DCXPanelClass")        //!< DCX Panel Class Name
#define DCX_CALENDARCLASS    TEXT("DCXCalendarClass")     //!< DCX Panel Class Name
#define DCX_DATETIMECLASS    TEXT("DCXDateTimeClass")     //!< DCX DateTime Class Name
#define DCX_PAGERCLASS       TEXT("DCXPagerClass")        //!< DCX Panel Class Name
#define DCX_BOXCLASS         TEXT("DCXBoxClass")          //!< DCX Box Class Name
#define DCX_RADIOCLASS       TEXT("DCXRadioClass")        //!< DCX Radio Class Name
#define DCX_CHECKCLASS       TEXT("DCXCheckClass")        //!< DCX Check Class Name
#define DCX_SCROLLBARCLASS   TEXT("DCXScrollBarClass")    //!< DCX ScrollBar Class Name
//#define DCX_SHADOWCLASS				TEXT("DCXShadowClass")			//!< DCX Shadow Class Name
#define DCX_VISTACLASS			TEXT("DCXVistaClass")     //!< DCX Vista Dialog Class Name
#define DCX_STACKERCLASS		TEXT("DCXStackerClass")   //!< DCX Stacker Class Name
#define DCX_IMAGECLASS			TEXT("DCXImageClass")     //!< DCX Image Class Name
#define DCX_EDITCLASS			TEXT("DCXEditClass")      //!< DCX Edit Class Name
#define DCX_WEBCLASS			TEXT("DCXWebClass")       //!< DCX Web Class Name
#define DCX_LINECLASS			TEXT("DCXLineClass")      //!< DCX Line Class Name
#define DCX_LINKCLASS			TEXT("DCXLinkClass")      //!< DCX Link Class Name
#define DCX_LISTCLASS			TEXT("DCXListClass")      //!< DCX List Class Name
#define DCX_SCROLLCLASS			TEXT("DCXScrollClass")    //!< DCX Text Class Name
#define DCX_TEXTCLASS			TEXT("DCXTextClass")      //!< DCX Text Class Name
#define DCX_DIRECTSHOWCLASS		TEXT("DCXDirectShowClass") //!< DCX Text Class Name

// --------------------------------------------------
// CLA constants
// --------------------------------------------------
enum CLATypes: UINT {
	LAYOUTFIXED	= 0x01,  //!< Layout Cell Fixed Type
	LAYOUTFILL	= 0x02,  //!< Layout Cell Fill Type
	LAYOUTPANE	= 0x04,  //!< Layout Cell Pane Type
	LAYOUTID	= 0x08,  //!< Layout Control ID is Valid
	LAYOUTDIM	= 0x10,  //!< Layout Control Dimensions are Valid (Only works with LayoutCellFixed)
	LAYOUTVERT	= 0x20,  //!< Layout Cell Vertical Style (LayoutCellPane and LayoutCellFixed)
	LAYOUTHORZ	= 0x40  //!< Layout Cell Horizontal Style (LayoutCellPane and LayoutCellFixed)
};

// --------------------------------------------------
// DCX Font stuff
// --------------------------------------------------
enum dcxFontFlags: UINT {
	DCF_ANTIALIASE	= 0x01, //!< Control Font Anti-Aliase Style
	DCF_BOLD		= 0x02, //!< Control Font Bold Style
	DCF_ITALIC		= 0x04, //!< Control Font Italic Style
	DCF_STRIKEOUT	= 0x08, //!< Control Font Strikeout Style
	DCF_UNDERLINE	= 0x10, //!< Control Font Underline Style
	DCF_DEFAULT		= 0x20	//!< Control Font Default Style
};

// --------------------------------------------------
// XPopup stuff
// --------------------------------------------------
#define XPOPUPMENUCLASS TEXT("XPopupMenu32") //!< XPopupMenu Window Class Name

// --------------------------------------------------
// Ultradock stuff
// --------------------------------------------------

enum class SwitchBarPos: UINT {
	SWB_NONE = 0,
	SWB_LEFT,
	SWB_RIGHT,
	SWB_TOP,
	SWB_BOTTOM
};

// Dialog info structure
//struct MYDCXWINDOW {
//	RECT rc;
//	DWORD old_styles;
//	DWORD old_exstyles;
//
//	MYDCXWINDOW() : rc{}, old_styles(0), old_exstyles(0) {}
//};
//using LPMYDCXWINDOW = MYDCXWINDOW *;

// --------------------------------------------------
// DLL stuff
// --------------------------------------------------
// mIRC Function Alias
#define mIRC(x) _INTEL_DLL_ int WINAPI x(HWND mWnd, HWND aWnd, TCHAR * data, TCHAR * parms, BOOL, BOOL)

// Return String DLL Alias (data is limited to MIRC_BUFFER_SIZE_CCH)
#define ret(x) { if (ts_strcpyn(data, (x), MIRC_BUFFER_SIZE_CCH) == nullptr) data[0] = 0; return 3; }

#define PACKVERSION(major,minor) MAKELONG(minor,major)

// mIRC Signal structure
struct SIGNALSWITCH
{
	bool xdock;
	bool xstatusbar;
	bool xtray;

	SIGNALSWITCH()
		: xdock(false)
		, xstatusbar(true)
		, xtray(true)
	{}
};
using LPSIGNALSWITCH = SIGNALSWITCH *;

using VectorOfInts = std::vector<int>; //<! Vector of int

// UNICODE/ANSI wrappers
#define dcx_atoi(x) ts_atoi(x)
#define dcx_atoi64(x) ts_atoi64(x)
#define dcx_atof(x) ts_atof(x)
#define dcx_fopen(x,y) _wfopen(x,y)
#define dcx_strstr(x,y) ts_strstr((x),(y))
#define dcx_strncmp(x,y,z) ts_strncmp((x),(y),(z))
#define dcx_itoa(x,y,z) _itow((x), (y), (z))

//#define dcx_strcpyn(x, y, z) { if (lstrcpyn((x), (y), gsl::narrow_cast<int>((z))) == nullptr) (x)[0] = 0; }

template <typename T>
inline void dcx_strcpyn(TCHAR *const sDest, const TCHAR *sSrc, const T &iSize) { if (ts_strcpyn(sDest, sSrc, iSize) == nullptr) sDest[0] = 0; }

constexpr const TCHAR *const dcx_truefalse(const bool &x) noexcept { return (x) ? &(TEXT("$true"))[0] : &(TEXT("$false")[0]); }

#define dcx_Con(x,y) dcx_strcpyn((y), dcx_truefalse((x)), MIRC_BUFFER_SIZE_CCH);

#define dcx_ConChar(x,y) { \
if ((x)) (y)[0] = TEXT('1'); \
	else (y)[0] = TEXT('0'); \
	(y)[1] = 0; \
}
//#define dcx_testflag(x,y) (((x) & (y)) == (y))
template <typename T, typename M>
constexpr bool dcx_testflag(T x, M y) noexcept { return ((x & gsl::narrow_cast<T>(y)) == gsl::narrow_cast<T>(y)); }

#define dcxlParam(x,y) const auto y = reinterpret_cast<x>(lParam)
#define dcxwParam(x,y) auto y = reinterpret_cast<x>(wParam)

// --------------------------------------------------
// DLL routines
// --------------------------------------------------

int dcx_round(const float x);

bool ParseCommandToLogfont(const TString& cmd, LPLOGFONT lf);
TString ParseLogfontToCommand(const LPLOGFONT lf);
UINT parseFontFlags(const TString &flags);
UINT parseFontCharSet(const TString &charset);

auto readFile(const TString &filename)->std::unique_ptr<BYTE[]>;
TString readTextFile(const TString &tFile);
bool SaveDataToFile(const TString &tsFile, const TString &tsData);
TString FileDialog(const TString & data, const TString &method, const HWND pWnd);

int CALLBACK BrowseFolderCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);
gsl::owner<LPITEMIDLIST> GetFolderFromCSIDL(const int nCsidl);

HWND GetHwndFromString(const TString &str);
HWND FindOwner(const TString & data, const gsl::not_null<HWND> &defaultWnd);
std::optional<HWND> FindOwner(const TString & data);
bool CopyToClipboard(const HWND owner, const TString & str);

HBITMAP dcxLoadBitmap(HBITMAP dest, TString &filename);
HICON dcxLoadIcon(const int index, TString &filename, const bool large, const TString &flags);
HICON CreateGrayscaleIcon(HICON hIcon);

HRGN BitmapRegion(HBITMAP hBitmap, const COLORREF cTransparentColor, const bool bIsTransparent);

void ChangeHwndIcon(const HWND hwnd, const TString &flags, const int index, TString &filename);
bool AddFileIcons(HIMAGELIST himl, TString &filename, const bool bLarge, const int iIndex, const int iStart = 0, const int iEnd = -1);
int dcxPickIconDlg(const gsl::not_null<HWND> &hwnd, gsl::not_null<LPWSTR> pszIconPath, const UINT &cchIconPath, gsl::not_null<int *> piIconIndex);

BOOL dcxGetWindowRect(const gsl::not_null<HWND> &hWnd, const gsl::not_null<LPRECT> &lpRect);
bool GetWindowRectParent(const gsl::not_null<HWND> &hwnd, gsl::not_null<RECT *> rcWin);

SYSTEMTIME MircTimeToSystemTime(const long mircTime);
long SystemTimeToMircTime(const LPSYSTEMTIME pst);

void AddToolTipToolInfo(const HWND tiphwnd, const HWND ctrl);
#ifdef DCX_USE_GDIPLUS
constexpr const TCHAR *GetLastStatusStr(Gdiplus::Status status);
#endif

bool IsFile(TString &filename);

void dcxDrawShadowText(HDC hdc, LPCWSTR pszText, UINT cch, RECT *pRect, DWORD dwFlags, COLORREF crText, COLORREF crShadow, int ixOffset, int iyOffset);
//void calcStrippedRect(HDC hdc, const TString &txt, const UINT style, LPRECT rc, const bool ignoreleft);
void mIRC_DrawText(HDC hdc, const TString &txt, LPRECT rc, const UINT style, const bool shadow);
void DrawRotatedText(const TString &strDraw, const gsl::not_null<LPRECT> &rc, const gsl::not_null<HDC> &hDC, const int nAngleLine = 0, const bool bEnableAngleChar = false, const int nAngleChar = 0);

gsl::owner<HDC *> CreateHDCBuffer(gsl::not_null<HDC> hdc, const LPRECT rc);
void DeleteHDCBuffer(gsl::owner<HDC *> hBuffer);

int TGetWindowText(HWND hwnd, TString &txt);
void FreeOSCompatibility(void);
bool isRegexMatch(const TCHAR *matchtext, const TCHAR *pattern);

const char *queryAttribute(gsl::not_null<const TiXmlElement *> element, gsl::not_null<const char *> attribute, gsl::not_null<const char *> defaultValue = "");
int queryIntAttribute(gsl::not_null<const TiXmlElement *> element, gsl::not_null<const char *> attribute, const int defaultValue = 0);

void getmIRCPalette();
void getmIRCPalette(COLORREF *const Palette, const UINT PaletteItems);
void getmIRCPaletteMask(COLORREF *const Palette, const UINT PaletteItems, uint16_t uMask);
int unfoldColor(const WCHAR *color);

// UltraDock
void RemStyles(const gsl::not_null<HWND> &hwnd,int parm,long RemStyles);
void AddStyles(const gsl::not_null<HWND> &hwnd,int parm,long AddStyles);

// DirectX
HRESULT GetDXVersion( DWORD* pdwDirectXVersion, TCHAR* strDirectXVersion, int cchDirectXVersion );

TString MakeTextmIRCSafe(const TString &tsStr);
TString MakeTextmIRCSafe(const TCHAR *const tString);

extern SIGNALSWITCH dcxSignal;
extern COLORREF staticPalette[mIRC_PALETTE_SIZE];

#include "Dcx.h"

#endif // _DEFINES_H_
