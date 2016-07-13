/*!
 * \file dcxcontrol.cpp
 * \brief blah
 *
 * blah
 *
 * \author David Legault ( clickhere at scriptsdb dot org )
 * \version 1.0
 *
 * \b Revisions
 *
 * � ScriptsDB.org - 2006
 */

#include "defines.h"
#include "Dcx.h"

#include "Classes/dcxcontrol.h"
#include "Classes/dcxdialog.h"
#include "Classes/dcxprogressbar.h"
#include "Classes/dcxtrackbar.h"
#include "Classes/dcxtrackbar.h"
#include "Classes/dcxcomboex.h"
#include "Classes/dcxcolorcombo.h"
#include "Classes/dcxstatusbar.h"
#include "Classes/dcxtreeview.h"
#include "Classes/dcxtoolbar.h"
#include "Classes/dcxlistview.h"
#include "Classes/dcxrebar.h"
#include "Classes/dcxbutton.h"
#include "Classes/dcxrichedit.h"
#include "Classes/dcxupdown.h"
#include "Classes/dcxipaddress.h"
#include "Classes/dcxwebctrl.h"
#include "Classes/dcxcalendar.h"
#include "Classes/dcxdatetime.h"
#include "Classes/dcxpager.h"
#include "Classes/dcxdivider.h"
#include "Classes/dcxpanel.h"
#include "Classes/dcxtab.h"
#include "Classes/mirc/dcxbox.h"
#include "Classes/mirc/dcxline.h"
#include "Classes/mirc/dcxline.h"
#include "Classes/mirc/dcxradio.h"
#include "Classes/mirc/dcxcheck.h"
#include "Classes/mirc/dcxtext.h"
#include "Classes/mirc/dcxedit.h"
#include "Classes/mirc/dcxscroll.h"
#include "Classes/mirc/dcxlist.h"
#include "Classes/mirc/dcxlink.h"
#include "Classes/mirc/dcximage.h"
#include "Classes/dcxmwindow.h"
#include "Classes/dcxmdialog.h"
#include "Classes/dcxstacker.h"

#ifdef DCX_USE_DXSDK
#include "Classes/dcxdirectshow.h"
#endif // DCX_USE_DXSDK


/*!
 * \brief Constructor
 *
 * \param mID Control ID
 * \param p_Dialog Parent DcxDialog Object
 */

DcxControl::DcxControl( const UINT mID, DcxDialog *const p_Dialog )
: DcxWindow( mID )
, m_pParentDialog( p_Dialog )
, m_hFont(nullptr)
//, m_tsMark()
, m_clrText(CLR_INVALID)
, m_clrBackText(CLR_INVALID)
, m_hBackBrush(nullptr)
, m_clrBackground(CLR_INVALID)
, m_iRefCount(0U)
, m_UserID(mID - mIRC_ID_OFFSET)
, m_hCursor(nullptr)
//m_hCursor(m_pParentDialog->getCursor())	// inherit the parent dialogs custom cursor...
, m_bCursorFromFile(false)
, m_bAlphaBlend(false)
//, m_pParentCtrl(nullptr)
, m_pParentHWND(nullptr)
, m_bitmapBg(nullptr)
, m_bShadowText(false)
, m_bCtrlCodeText(true)
, m_bInPrint(false)
, m_hBorderBrush(nullptr)
, m_iAlphaLevel(0x7f)
, m_bNoTheme(false)
, m_bGradientFill(false)
, m_clrStartGradient(CLR_INVALID)
, m_clrEndGradient(CLR_INVALID)
, m_bGradientVertical(false)
, m_ToolTipHWND(nullptr)
, m_colTransparentBg(CLR_INVALID)
, m_DefaultWindowProc(nullptr)
, m_dEventMask(p_Dialog->getEventMask())	// inherit the parent dialogs event mask
{
	//m_dEventMask = p_Dialog->getEventMask();
	//if (p_Dialog == nullptr)
	//	throw Dcx::dcxException("DcxControl()");
}

/*!
 * \brief Destructor
 *
 * Control Interface Destructor.
 *
 * Frees global control resources.
 */

DcxControl::~DcxControl( ) {

	RemoveProp( m_Hwnd, TEXT("dcx_cthis") );

	// Reverse to old font
	setControlFont(nullptr, FALSE);

	// Delete background brush
	if (m_hBackBrush != nullptr) {

		DeleteBrush( m_hBackBrush );
		m_hBackBrush = nullptr;
	}
	// Delete border brush
	if (m_hBorderBrush != nullptr) {

		DeleteBrush( m_hBorderBrush );
		m_hBorderBrush = nullptr;
	}

	// check if we need to destroy the cursor (do not destroy if same cursor as parent dialog, parent will destroy this for us)
	if (m_bCursorFromFile && m_hCursor != nullptr && m_hCursor != m_pParentDialog->getCursor()) {

		DestroyCursor( m_hCursor );
		m_hCursor = nullptr;
	}

	m_pParentDialog->deleteControl( this );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxControl::parseGeneralControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme )
{
	*Styles |= WS_CLIPCHILDREN | WS_VISIBLE;

#if DCX_USE_HASHING
	for (const auto &tsStyle : styles)
	{
		switch (dcx_hash(tsStyle.to_chr()))
		{
			case L"notheme"_hash:
				*bNoTheme = TRUE;
				break;
			case L"tabstop"_hash:
				*Styles |= WS_TABSTOP;
				break;
			case L"group"_hash:
				*Styles |= WS_GROUP;
				break;
			case L"disabled"_hash:
				*Styles |= WS_DISABLED;
				break;
			case L"transparent"_hash:
				*ExStyles |= WS_EX_TRANSPARENT;
				break;
			case L"hidden"_hash:
				*Styles &= ~WS_VISIBLE;
				break;
			case L"alpha"_hash:
				m_bAlphaBlend = true;
				break;
			case L"shadow"_hash:
				m_bShadowText = true;
				break;
			case L"noformat"_hash:
				m_bCtrlCodeText = false;
				break;
			case L"hgradient"_hash:
				m_bGradientFill = true;
				break;
			case L"vgradient"_hash:
				{
					m_bGradientFill = true;
					m_bGradientVertical = true;
				}
				break;
			default:
				break;
		}
	}
#else
	for (const auto &tsStyle: styles)
	{
		if ( tsStyle == TEXT("notheme") )
			*bNoTheme = TRUE;
		else if ( tsStyle == TEXT("tabstop") )
			*Styles |= WS_TABSTOP;
		else if ( tsStyle == TEXT("group") )
			*Styles |= WS_GROUP;
		else if ( tsStyle == TEXT("disabled") )
			*Styles |= WS_DISABLED;
		else if ( tsStyle == TEXT("transparent") )
			*ExStyles |= WS_EX_TRANSPARENT;
		else if ( tsStyle == TEXT("hidden") )
			*Styles &= ~WS_VISIBLE;
		else if ( tsStyle == TEXT("alpha") )
			m_bAlphaBlend = true;
		else if ( tsStyle == TEXT("shadow") )
			m_bShadowText = true;
		else if ( tsStyle == TEXT("noformat") )
			m_bCtrlCodeText = false;
		else if ( tsStyle == TEXT("hgradient") )
			m_bGradientFill = true;
		else if ( tsStyle == TEXT("vgradient") ) {
			m_bGradientFill = true;
			m_bGradientVertical = true;
		}
	}
#endif
}

/*!
 * \brief blah
 *
 * blah
 */

const UINT &DcxControl::getUserID( ) const noexcept {

	return m_UserID;
}

/*!
 * \brief blah
 *
 * NB: Possible buffer overwrite condition when returned data is longer than allocated szReturn
 */

bool DcxControl::evalAliasEx( TCHAR *const szReturn, const int maxlen, const TCHAR *const szFormat, ... ) {

	TString parms;
	va_list args;

	va_start( args, szFormat );
	parms.tvprintf(szFormat, args );
	va_end( args );

	return m_pParentDialog->evalAlias(szReturn, maxlen, parms.to_chr());
}

bool DcxControl::execAliasEx( const TCHAR *const szFormat, ... ) {

	TString parms;
	va_list args;

	va_start( args, szFormat );
	parms.tvprintf(szFormat, args);
	va_end( args );

	return m_pParentDialog->execAlias(parms.to_chr());
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxControl::parseGlobalCommandRequest( const TString & input, const XSwitchFlags & flags ) {
	const auto numtok = input.numtok();

	// xdid -f [NAME] [ID] [SWITCH] [+FLAGS] [CHARSET] [SIZE] [FONTNAME]
	if ( flags[TEXT('f')] && numtok > 3 ) {
		LOGFONT lf;

		if (ParseCommandToLogfont(input.gettok(4, -1), &lf)) {
			auto hFont = CreateFontIndirect(&lf);
			setControlFont(hFont, FALSE);
		}

		redrawWindow( );
	}
	// xdid -p [NAME] [ID] [SWITCH] [X] [Y] [W] [H]
	else if ( flags[TEXT('p')] && numtok > 6 ) {

		const auto x = input.getfirsttok(4).to_int();
		const auto y = input.getnexttok().to_int();	// tok 5
		const auto w = input.getnexttok().to_int();	// tok 6
		const auto h = input.getnexttok().to_int();	// tok 7

		MoveWindow( m_Hwnd, x, y, w, h, TRUE );
		//this->InvalidateParentRect( m_Hwnd);
		//InvalidateRect( GetParent( m_Hwnd ), nullptr, TRUE );
		//this->redrawWindow( );
		//this->m_pParentDialog->redrawBufferedWindow();
		//SendMessage( m_Hwnd, WM_NCPAINT, (WPARAM) 1, (LPARAM) 0 );
	}
	// xdid -x [NAME] [ID] [SWITCH] [+FLAGS]
	else if ( flags[TEXT('x')] && numtok > 3 ) {

		this->removeStyle( WS_BORDER|WS_DLGFRAME );
		this->removeExStyle( WS_EX_CLIENTEDGE|WS_EX_DLGMODALFRAME|WS_EX_STATICEDGE|WS_EX_WINDOWEDGE );

		LONG Styles = 0, ExStyles = 0;
		this->parseBorderStyles( input.gettok( 4 ), &Styles, &ExStyles );

		this->addStyle( Styles );
		this->addExStyle( ExStyles );

		SetWindowPos(m_Hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
		InvalidateRect(m_Hwnd, nullptr, TRUE);
		SendMessage( m_Hwnd, WM_NCPAINT, (WPARAM) 1, (LPARAM) 0 );
	}
	// xdid -C [NAME] [ID] [SWITCH] [+FLAGS] [COLOR]
	else if ( flags[TEXT('C')] && numtok > 4 ) {
		const auto iFlags = this->parseColorFlags(input.getfirsttok(4));
		const auto clrColor = input.getnexttok().to_<COLORREF>();	// tok 5

		if (dcx_testflag(iFlags, DCC_BKGCOLOR)) {
			if (this->m_hBackBrush != nullptr) {
				DeleteBrush( this->m_hBackBrush );
				this->m_hBackBrush = nullptr;
				this->m_clrBackground = CLR_INVALID;
			}

			if ( clrColor != CLR_INVALID ) {
				this->m_hBackBrush = CreateSolidBrush( clrColor );
				this->m_clrBackground = clrColor;
			}
		}

		if (dcx_testflag(iFlags, DCC_TEXTCOLOR))
			this->m_clrText = clrColor;

		if (dcx_testflag(iFlags, DCC_TEXTBKGCOLOR))
			this->m_clrBackText = clrColor;

		if (dcx_testflag(iFlags, DCC_BORDERCOLOR)) {
			if (this->m_hBorderBrush != nullptr) {
				DeleteBrush( this->m_hBorderBrush );
				this->m_hBorderBrush = nullptr;
			}

			if ( clrColor != CLR_INVALID )
				this->m_hBorderBrush = CreateSolidBrush( clrColor );
		}

		if (dcx_testflag(iFlags, DCC_GRADSTARTCOLOR))
			this->m_clrStartGradient = clrColor;

		if (dcx_testflag(iFlags, DCC_GRADENDCOLOR))
			this->m_clrEndGradient = clrColor;

		// force a control redraw
		this->redrawWindow( );
	}
	// xdid -F [NAME] [ID] [SWITCH]
	else if (flags[TEXT('F')]) {
		SetFocus(m_Hwnd);
	}
	// xdid -J [NAME] [ID] [SWITCH] [+FLAGS] [CURSOR|FILENAME]
	// xdid -J DNAME ID [+FLAGS] [CURSOR|FILENAME]
	else if ( flags[TEXT('J')] && numtok > 4 ) {
		const auto iFlags = this->parseCursorFlags( input.getfirsttok( 4 ) );
		auto filename(input.getlasttoks());
		const auto CursorType = this->parseCursorType(filename);

		// if previous cursor was the dialogs cursor, just set as blank
		if (m_hCursor == m_pParentDialog->getCursor())
			m_hCursor = nullptr;

		this->m_hCursor = Dcx::dcxLoadCursor(iFlags, CursorType, this->m_bCursorFromFile, this->m_hCursor, filename);
	}
	// xdid -M [NAME] [ID] [SWITCH] [MARK INFO]
	else if (flags[TEXT('M')]) {
		TString info;

		if (numtok > 3)
			info = input.gettok(4, -1).trim();

		this->m_tsMark = info;
	}
	// xdid -Z [NAME] [ID] [SWITCH] [%]
	else if ( flags[TEXT('Z')] && numtok > 3 ) {

		const auto perc = (BYTE)(input.getfirsttok(4).to_int() & 0xFF);

		if (perc > 100)
			throw Dcx::dcxException("Invalid Percentage");

		int min, max;
		if (!GetScrollRange(m_Hwnd, SB_VERT, &min, &max))
			throw Dcx::dcxException("Unable to get scrollbar info");

		//scrollbar is defined and has visible range
		if (min != 0 || max != 0) {

			const auto pos = dcx_round((float)(max - min) * (float)perc / (float) 100.0);

			SCROLLINFO si;
			ZeroMemory(&si, sizeof (SCROLLINFO));
			si.cbSize = sizeof(SCROLLINFO);
			if (!GetScrollInfo(m_Hwnd, SB_VERT, &si))
				throw Dcx::dcxException("Unable to get scroll info");

			si.nPos = pos;
			SetScrollInfo(m_Hwnd, SB_VERT, &si, TRUE);
			SendMessage(m_Hwnd, WM_VSCROLL, MAKEWPARAM(SB_THUMBPOSITION, si.nPos), NULL);
		}
	}

	// xdid -b [NAME] [ID]
	else if ( flags[TEXT('b')] ) {
		EnableWindow( m_Hwnd, FALSE );
	}
	// xdid -e [NAME] [ID]
	else if ( flags[TEXT('e')] ) {
		EnableWindow( m_Hwnd, TRUE );
	}
	// xdid -h [NAME] [ID] [SWITCH] (+FLAGS) (DURATION)
	else if (flags[TEXT('h')]) {
		if (numtok > 4)
			AnimateWindow(m_Hwnd,
				input.gettok(5).to_dword(),
				(AW_HIDE | DcxDialog::getAnimateStyles(input.gettok(4))) & ~AW_ACTIVATE);
		else
			ShowWindow(m_Hwnd, SW_HIDE);

		// shouldnt need to update layout on a hidden window....
		//RECT rc;
		//if (GetClientRect(this->m_pParentDialog->getHwnd(), &rc))
		//{
		//	if (this->m_pParentDialog->updateLayout(rc))
		//		this->m_pParentDialog->redrawWindow(); // why do we need the redraw?
		//}
	}
	// xdid -s [NAME] [ID] [SWITCH] (+FLAGS) (DURATION)
	else if ( flags[TEXT('s')] ) {
		if (numtok > 4) {
			AnimateWindow(m_Hwnd,
				input.gettok(5).to_dword(),
				(AW_ACTIVATE | DcxDialog::getAnimateStyles(input.gettok( 4 ))) & ~AW_HIDE);
		}
		else
			ShowWindow(m_Hwnd, SW_SHOW);

		RECT rc;
		if (GetClientRect(this->m_pParentDialog->getHwnd(), &rc))
		{
			if (this->m_pParentDialog->updateLayout(rc))
				this->m_pParentDialog->redrawWindow(); // why do we need the redraw?
		}
	}
	// xdid -U [NAME] [ID]
	else if (flags[TEXT('U')]) {
		// Box Double click Bug: the GetNextDlgtabItem() function never returns & seems to just loop forever.
		// from functions doc:
		//	If the search for the next control with the WS_TABSTOP
		//	style encounters a window with the WS_EX_CONTROLPARENT style,
		//	the system recursively searches the window's children.
		//
		//HWND hNextCtrl = GetNextDlgTabItem(this->m_pParentDialog->getHwnd(), m_Hwnd, FALSE);
        //
		//if (hNextCtrl && (hNextCtrl != m_Hwnd))
		//	SendMessage(this->m_pParentDialog->getHwnd(), WM_NEXTDLGCTL, (WPARAM) hNextCtrl, TRUE);
		////::PostMessage(this->m_pParentDialog->getHwnd(), WM_NEXTDLGCTL, NULL, FALSE);
		//else
		//	SetFocus(nullptr);

		// Just remove focus from all controls
		SetFocus(nullptr);
	}
	// xdid -T [NAME] [ID] [SWITCH] (ToolTipText)
	else if (flags[TEXT('T')] && numtok > 2) {
		this->m_tsToolTip = (numtok > 3 ? input.gettok(4, -1).trim() : TEXT(""));
	}
	// xdid -R [NAME] [ID] [SWITCH] [FLAG] [ARGS]
	else if (flags[TEXT('R')] && numtok > 3) {
		const XSwitchFlags xflags(input.getfirsttok( 4 ));

		if (!xflags[TEXT('+')])
			throw Dcx::dcxException("Invalid Flag");

		RECT rc = { 0 };
		HRGN m_Region = nullptr;
		auto RegionMode = 0;
		auto noRegion = false;
		if (!GetWindowRect(m_Hwnd, &rc))
			throw Dcx::dcxException("Unable to get window rect!");

		if (xflags[TEXT('o')])
			RegionMode = RGN_OR;
		else if (xflags[TEXT('a')])
			RegionMode = RGN_AND;
		else if (xflags[TEXT('i')])
			RegionMode = RGN_DIFF;
		else if (xflags[TEXT('x')])
			RegionMode = RGN_XOR;

		if (xflags[TEXT('f')]) // image file - [COLOR] [FILE]
		{
			if (numtok < 6)
				throw Dcx::dcxException("Invalid Arguments");

			const auto tCol = input.getnexttok().to_<COLORREF>();		// tok 5
			auto filename(input.getlasttoks());							// tok 6, -1

			auto bitmapRgn = dcxLoadBitmap(nullptr, filename);

			if (bitmapRgn == nullptr)
				throw Dcx::dcxException("Unable To Load Image file.");
			Auto(DeleteBitmap(bitmapRgn));

			if (xflags[TEXT('R')]) // now resize image to match control.
				bitmapRgn = resizeBitmap(bitmapRgn, &rc);
			m_Region = BitmapRegion(bitmapRgn, tCol, ((tCol != -1) ? TRUE : FALSE));
		}
		else if (xflags[TEXT('r')]) // rounded rect - radius args (optional)
		{
			int radius = 20;

			if (numtok > 4)
				radius = input.getnexttok( ).to_int();	// tok 5

			m_Region = CreateRoundRectRgn(0,0,rc.right - rc.left,rc.bottom - rc.top, radius, radius);
		}
		else if (xflags[TEXT('c')]) // circle - radius arg (optional)
		{
			if (numtok > 4) {
				auto radius = input.getnexttok( ).to_int();	// tok 5
				if (radius < 1)
					radius = 100; // handle cases where arg isnt a number or is a negative.
				const auto cx = ((rc.right - rc.left) / 2);
				const auto cy = ((rc.bottom - rc.top) / 2);
				m_Region = CreateEllipticRgn(cx-radius,cy-radius,cx+radius,cy+radius);
			}
			else
				m_Region = CreateEllipticRgn(0,0,rc.right - rc.left,rc.bottom - rc.top);
		}
		else if (xflags[TEXT('p')]) // polygon
		{
			// u need at least 3 points for a shape
			if (numtok < 7)
				throw Dcx::dcxException("Invalid Arguments");

			const auto strPoints(input.getlasttoks());	// tok 5, -1
			const auto tPoints = strPoints.numtok();

			if (tPoints < 3)
				throw Dcx::dcxException("Invalid Points: At least 3 points required.");

			auto pnts = std::make_unique<POINT[]>(tPoints);
			UINT cnt = 0;

			for (const auto &strPoint: strPoints)
			{
				pnts[cnt].x = strPoint.getfirsttok(1, TSCOMMACHAR).to_<LONG>();
				pnts[cnt].y = strPoint.getnexttok(TSCOMMACHAR).to_<LONG>();	// tok 2
				++cnt;
			}

			m_Region = CreatePolygonRgn(pnts.get(), static_cast<int>(tPoints), WINDING);
		}
		else if (xflags[TEXT('b')]) { // alpha [1|0] [level]
			noRegion = true;
			if (numtok != 6)
				throw Dcx::dcxException("Invalid Args");

			m_bAlphaBlend = (input.getnexttok().to_int() > 0);	// tok 5

			const auto alpha = (BYTE)(input.getnexttok().to_int() & 0xFF);	// tok 6

			if (alpha == 255)
				m_bAlphaBlend = false;
			m_iAlphaLevel = alpha;
		}
		else {
			if (!xflags[TEXT('n')]) // none, no args
				throw Dcx::dcxException("Invalid Flag");
			
			noRegion = true;
			SetWindowRgn(m_Hwnd, nullptr, TRUE);
		}

		if (!noRegion) {
			if (m_Region == nullptr)
				throw Dcx::dcxException("Unable to create region.");

			if (RegionMode != 0) {
				auto wrgn = CreateRectRgn(0, 0, 0, 0);
				if (wrgn != nullptr) {
					Auto(DeleteObject(wrgn));
					if (GetWindowRgn(m_Hwnd,wrgn) != ERROR)
						CombineRgn(m_Region,m_Region,wrgn,RegionMode);
				}
			}
			SetWindowRgn(m_Hwnd,m_Region,TRUE);
		}
		redrawWindow();
	}
	// invalid command
	else
		throw Dcx::dcxException("Invalid Command");
}

HBITMAP DcxControl::resizeBitmap(HBITMAP srcBM, const RECT *const rc)
{
#if DCX_USE_WRAPPERS && 0

	if ((srcBM == nullptr) || (rc == nullptr))
		return nullptr;

	// set result to original bitmap incase resize fails at some point.
	auto hRes = srcBM;

	// get source bitmap info.
	BITMAP bm;
	if (GetObject(srcBM, sizeof(bm), &bm) == 0)
		return hRes;

	Dcx::dcxHDCBitmapResource srcDC(nullptr, srcBM);

	auto w = (rc->right - rc->left), h = (rc->bottom - rc->top);

	// create dest bitmap.
	Dcx::dcxHDCBitmap2Resource destDC(srcDC.get(), w, h);

	// resize bitmap, render into dest hdc/bitmap
	if (StretchBlt(destDC, 0, 0, w, h, srcDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY)) {
		GdiFlush();
		// set the return bitmap if resize worked.
		DeleteBitmap(hRes);
		hRes = destDC;
	}
	return hRes;
#else
	// set result to original bitmap incase resize fails at some point.
	auto hRes = srcBM;

	if ((srcBM == nullptr) || (rc == nullptr))
		return nullptr;

	// get source bitmap info.
	BITMAP bm;
	if (GetObject(srcBM, sizeof(bm), &bm) == 0)
		return hRes;

	// create temp hdc to render from
	auto srcDC = CreateCompatibleDC(nullptr);
	if (srcDC == nullptr)
		return hRes;

	// delete source hdc
	Auto(DeleteDC(srcDC));

	// select bitmap into new hdc
	auto oldSrcBm = SelectBitmap(srcDC, srcBM);

	// set source hdc back to orig bitmap.
	Auto(SelectBitmap(srcDC, oldSrcBm));

	// create temp dest hdc to render into.
	auto destDC = CreateCompatibleDC(srcDC);

	if (destDC == nullptr)
		return hRes;

	// delete dest hdc
	Auto(DeleteDC(destDC));

	auto w = (rc->right - rc->left), h = (rc->bottom - rc->top);

	// create dest bitmap.
	auto newBM = CreateCompatibleBitmap(srcDC, w, h);

	if (newBM == nullptr)
		return hRes;

	Auto(DeleteBitmap(newBM));

	// select dest bitmap into dest hdc
	auto oldDestBm = SelectBitmap(destDC, newBM);

	// set dest hdc back to orig bitmap.
	Auto(SelectBitmap(destDC, oldDestBm));

	// resize bitmap, render into dest hdc/bitmap
	if (StretchBlt(destDC, 0, 0, w, h, srcDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY)) {
		GdiFlush();
		// set the return bitmap if resize worked.
		std::swap(hRes, newBM);
	}
	return hRes;
#endif
}

DcxControlTypes DcxControl::TSTypeToControlType(const TString & t)
{
	static std::map<TString, DcxControlTypes> dcxTypesMap;
	//static std::unordered_map<TString, DcxControlTypes> dcxTypesMap;
	if (dcxTypesMap.empty()) {
		dcxTypesMap[TEXT("box")] = DcxControlTypes::BOX;
		dcxTypesMap[TEXT("check")] = DcxControlTypes::CHECK;
		dcxTypesMap[TEXT("edit")] = DcxControlTypes::EDIT;
		dcxTypesMap[TEXT("image")] = DcxControlTypes::IMAGE;
		dcxTypesMap[TEXT("line")] = DcxControlTypes::LINE;
		dcxTypesMap[TEXT("link")] = DcxControlTypes::LINK;
		dcxTypesMap[TEXT("list")] = DcxControlTypes::LIST;
		dcxTypesMap[TEXT("radio")] = DcxControlTypes::RADIO;
		dcxTypesMap[TEXT("scroll")] = DcxControlTypes::SCROLL;
		dcxTypesMap[TEXT("text")] = DcxControlTypes::TEXT;
		dcxTypesMap[TEXT("button")] = DcxControlTypes::BUTTON;
		dcxTypesMap[TEXT("calendar")] = DcxControlTypes::CALENDAR;
		dcxTypesMap[TEXT("colorcombo")] = DcxControlTypes::COLORCOMBO;
		dcxTypesMap[TEXT("comboex")] = DcxControlTypes::COMBOEX;
		dcxTypesMap[TEXT("datetime")] = DcxControlTypes::DATETIME;
		dcxTypesMap[TEXT("directshow")] = DcxControlTypes::DIRECTSHOW;
		dcxTypesMap[TEXT("divider")] = DcxControlTypes::DIVIDER;
		dcxTypesMap[TEXT("ipaddress")] = DcxControlTypes::IPADDRESS;
		dcxTypesMap[TEXT("listview")] = DcxControlTypes::LISTVIEW;
		dcxTypesMap[TEXT("dialog")] = DcxControlTypes::DIALOG;
		dcxTypesMap[TEXT("window")] = DcxControlTypes::WINDOW;
		dcxTypesMap[TEXT("pager")] = DcxControlTypes::PAGER;
		dcxTypesMap[TEXT("panel")] = DcxControlTypes::PANEL;
		dcxTypesMap[TEXT("pbar")] = DcxControlTypes::PROGRESSBAR;
		dcxTypesMap[TEXT("rebar")] = DcxControlTypes::REBAR;
		dcxTypesMap[TEXT("richedit")] = DcxControlTypes::RICHEDIT;
		dcxTypesMap[TEXT("stacker")] = DcxControlTypes::STACKER;
		dcxTypesMap[TEXT("statusbar")] = DcxControlTypes::STATUSBAR;
		dcxTypesMap[TEXT("tab")] = DcxControlTypes::TABB;
		dcxTypesMap[TEXT("toolbar")] = DcxControlTypes::TOOLBAR;
		dcxTypesMap[TEXT("trackbar")] = DcxControlTypes::TRACKBAR;
		dcxTypesMap[TEXT("treeview")] = DcxControlTypes::TREEVIEW;
		dcxTypesMap[TEXT("updown")] = DcxControlTypes::UPDOWN;
		dcxTypesMap[TEXT("webctrl")] = DcxControlTypes::WEBCTRL;
	}

	auto got = dcxTypesMap.find(t);

	if (got != dcxTypesMap.end())
		return got->second;
	return DcxControlTypes::UNKNOWN;
}

/*!
 * \brief blah
 *
 * blah
 */

const UINT DcxControl::parseColorFlags( const TString & flags ) {

	const XSwitchFlags xflags(flags);
	UINT iFlags = 0;

	// no +sign, missing params
	if ( !xflags[TEXT('+')] )
		return iFlags;

	if ( xflags[TEXT('b')] )
		iFlags |= DCC_BKGCOLOR;
	if ( xflags[TEXT('k')] )
		iFlags |= DCC_TEXTBKGCOLOR;
	if ( xflags[TEXT('t')] )
		iFlags |= DCC_TEXTCOLOR;
	if ( xflags[TEXT('r')] )
		iFlags |= DCC_BORDERCOLOR;
	if ( xflags[TEXT('g')] )
		iFlags |= DCC_GRADSTARTCOLOR;
	if ( xflags[TEXT('G')] )
		iFlags |= DCC_GRADENDCOLOR;

	return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxControl::parseBorderStyles(const TString & flags, LONG *const Styles, LONG *const ExStyles) {

	const XSwitchFlags xflags(flags);

	// no +sign, missing params
	if ( !xflags[TEXT('+')] ) 
		return;

	if ( xflags[TEXT('b')] )
		*Styles |= WS_BORDER;
	if ( xflags[TEXT('c')] )
		*ExStyles |= WS_EX_CLIENTEDGE;
	if ( xflags[TEXT('d')] )
		*Styles |= WS_DLGFRAME ;
	if ( xflags[TEXT('f')] )
		*ExStyles |= WS_EX_DLGMODALFRAME;
	if ( xflags[TEXT('s')] )
		*ExStyles |= WS_EX_STATICEDGE;
	if ( xflags[TEXT('w')] )
		*ExStyles |= WS_EX_WINDOWEDGE;

}

/*!
 * \brief blah
 *
 * blah
 */
//bool DcxControl::parseGlobalInfoRequest(const TString & input, TCHAR *const szReturnValue) const
//{
//	const auto prop(input.getfirsttok(3));
//
//	if (prop == TEXT("hwnd")) {
//		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%lu"), (DWORD)m_Hwnd);	// can't use %p as this gives a hex result.
//		return true;
//	}
//	else if (prop == TEXT("visible")) {
//		dcx_ConRetState((IsWindowVisible(m_Hwnd) != FALSE), szReturnValue);
//	}
//	else if (prop == TEXT("enabled")) {
//		dcx_ConRetState((IsWindowEnabled(m_Hwnd) != FALSE), szReturnValue);
//	}
//	else if (prop == TEXT("pos")) {
//		const auto rc = getWindowPosition();
//		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d %d %d"), rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
//		return true;
//	}
//	else if (prop == TEXT("dpos")) {
//		RECT rc;
//		if (GetWindowRect(m_Hwnd, &rc))
//		{
//			MapWindowPoints(nullptr, m_pParentDialog->getHwnd(), (LPPOINT)&rc, 2);
//
//			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d %d %d"), rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
//			return true;
//		}
//	}
//	else if (prop == TEXT("mark")) {
//		if (lstrcpyn(szReturnValue, m_tsMark.to_chr(), MIRC_BUFFER_SIZE_CCH) != nullptr)
//			return true;
//	}
//	else if (prop == TEXT("mouse")) {
//		POINT pt;
//		if (GetCursorPos(&pt))
//		{
//			MapWindowPoints(nullptr, m_Hwnd, &pt, 1);
//			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d"), pt.x, pt.y);
//			return true;
//		}
//	}
//	else if (prop == TEXT("pid")) {
//		//TCHAR classname[257] = { 0 };
//		//auto hParent = GetParent(m_Hwnd);
//		//GetClassName( hParent, classname, Dcx::countof(classname) );
//		//
//		//if (lstrcmp(classname, TEXT("#32770")) == 0) {
//		//	szReturnValue[0] = TEXT('0');
//		//	szReturnValue[1] = TEXT('\0');
//		//}
//		//else
//		//	wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"),  this->m_pParentDialog->getControlByHWND( hParent )->getUserID( ) );
//
//		stString<257> sClassname;
//
//		auto hParent = GetParent(m_Hwnd);
//		GetClassName(hParent, sClassname, sClassname.size());
//
//		if (sClassname == TEXT("#32770")) {
//			szReturnValue[0] = TEXT('0');
//			szReturnValue[1] = TEXT('\0');
//		}
//		else
//			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), m_pParentDialog->getControlByHWND(hParent)->getUserID());
//
//		return true;
//	}
//	else if (prop == TEXT("type")) {
//
//		if (lstrcpyn(szReturnValue, getType().to_chr(), MIRC_BUFFER_SIZE_CCH) != nullptr)
//			return true;
//	}
//	else if (prop == TEXT("styles")) {
//		if (lstrcpyn(szReturnValue, getStyles().to_chr(), MIRC_BUFFER_SIZE_CCH) != nullptr)
//			return true;
//	}
//	else if (prop == TEXT("font")) {
//		auto hFontControl = this->m_hFont;
//
//		//		if (!hFontControl)
//		//			hFontControl = (HFONT) this->getFont();
//		if (!hFontControl)
//			hFontControl = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
//
//		if (hFontControl) {
//			LOGFONT lfCurrent = { 0 };
//
//			//ZeroMemory(&lfCurrent, sizeof(LOGFONT));
//			if (GetObject(hFontControl, sizeof(LOGFONT), &lfCurrent) != 0)
//			{
//				if (lstrcpyn(szReturnValue, ParseLogfontToCommand(&lfCurrent).to_chr(), MIRC_BUFFER_SIZE_CCH) != nullptr)
//					return true;
//			}
//		}
//	}
//	// [NAME] [ID] [PROP]
//	else if (prop == TEXT("tooltipbgcolor")) {
//		DWORD cref = 0;
//
//		if (m_ToolTipHWND != nullptr)
//			cref = (DWORD)SendMessage(m_ToolTipHWND, TTM_GETTIPBKCOLOR, NULL, NULL);
//
//		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%lu"), cref);
//		return true;
//	}
//	// [NAME] [ID] [PROP]
//	else if (prop == TEXT("tooltiptextcolor")) {
//		DWORD cref = 0;
//
//		if (m_ToolTipHWND != nullptr)
//			cref = (DWORD)SendMessage(this->m_ToolTipHWND, TTM_GETTIPTEXTCOLOR, NULL, NULL);
//
//		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%lu"), cref);
//		return true;
//	}
//	// [NAME] [ID] [PROP]
//	else if (prop == TEXT("alpha")) {
//		dcx_ConRetState(m_bAlphaBlend, szReturnValue);
//	}
//	// [NAME] [ID] [PROP]
//	else if (prop == TEXT("textcolor")) {
//		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), m_clrText);
//		return true;
//	}
//	// [NAME] [ID] [PROP]
//	else if (prop == TEXT("textbgcolor")) {
//		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), m_clrBackText);
//		return true;
//	}
//	// [NAME] [ID] [PROP]
//	else if (prop == TEXT("bgcolor")) {
//		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), m_clrBackground);
//		return true;
//	}
//	else
//		throw Dcx::dcxException("Invalid property or number of arguments");
//
//	return false;
//}

bool DcxControl::parseGlobalInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const
{
	const auto prop(input.getfirsttok( 3 ));

	if ( prop == TEXT("hwnd") ) {
		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%lu"), (DWORD)m_Hwnd );	// can't use %p as this gives a hex result.
		return true;
	}
	else if ( prop == TEXT("visible") ) {
		szReturnValue = dcx_truefalse((IsWindowVisible(m_Hwnd) != FALSE));
		return true;
	}
	else if ( prop == TEXT("enabled") ) {
		szReturnValue = dcx_truefalse((IsWindowEnabled(m_Hwnd) != FALSE));
		return true;
	}
	else if ( prop == TEXT("pos") ) {
		const auto rc = getWindowPosition();
		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d %d %d"), rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top );
		return true;
	}
	else if ( prop == TEXT("dpos") ) {
		RECT rc;
		if (GetWindowRect(m_Hwnd, &rc))
		{
			MapWindowPoints(nullptr, m_pParentDialog->getHwnd(), (LPPOINT)&rc, 2);

			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d %d %d"), rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
			return true;
		}
	}
	else if ( prop == TEXT("mark") ) {
		szReturnValue = m_tsMark.to_chr();
		return true;
	}
	else if ( prop == TEXT("mouse") ) {
		POINT pt;
		if (GetCursorPos(&pt))
		{
			MapWindowPoints(nullptr, m_Hwnd, &pt, 1);
			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d"), pt.x, pt.y);
			return true;
		}
	}
	else if ( prop == TEXT("pid") ) {
		//TCHAR classname[257] = { 0 };
		//auto hParent = GetParent(m_Hwnd);
		//GetClassName( hParent, classname, Dcx::countof(classname) );
		//
		//if (lstrcmp(classname, TEXT("#32770")) == 0) {
		//	szReturnValue[0] = TEXT('0');
		//	szReturnValue[1] = TEXT('\0');
		//}
		//else
		//	wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"),  this->m_pParentDialog->getControlByHWND( hParent )->getUserID( ) );

		stString<257> sClassname;

		auto hParent = GetParent(m_Hwnd);
		GetClassName(hParent, sClassname, sClassname.size());

		if (sClassname == TEXT("#32770"))
			szReturnValue = TEXT('0');
		else
			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), m_pParentDialog->getControlByHWND(hParent)->getUserID());

		return true;
	}
	else if ( prop == TEXT("type") ) {

		szReturnValue = getType().to_chr();
		return true;
	}
	else if ( prop == TEXT("styles") ) {
		szReturnValue = getStyles().to_chr();
		return true;
	}
	else if ( prop == TEXT("font")) {
		auto hFontControl = m_hFont;

//		if (!hFontControl)
//			hFontControl = (HFONT) this->getFont();
		if (!hFontControl)
			hFontControl = (HFONT) GetStockObject(DEFAULT_GUI_FONT);

		if (hFontControl) {
			LOGFONT lfCurrent = { 0 };

			//ZeroMemory(&lfCurrent, sizeof(LOGFONT));
			if (GetObject(hFontControl, sizeof(LOGFONT), &lfCurrent) != 0)
			{
				szReturnValue = ParseLogfontToCommand(&lfCurrent).to_chr();
				return true;
			}
		}
	}
	// [NAME] [ID] [PROP]
	else if ( prop == TEXT("tooltipbgcolor")) {
		DWORD cref = 0;

		if (m_ToolTipHWND != nullptr)
			cref = (DWORD) SendMessage(m_ToolTipHWND,TTM_GETTIPBKCOLOR, NULL, NULL);

		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%lu"), cref);
		return true;
	}
	// [NAME] [ID] [PROP]
	else if (prop == TEXT("tooltiptextcolor")) {
		DWORD cref = 0;

		if (m_ToolTipHWND != nullptr)
			cref = (DWORD) SendMessage(m_ToolTipHWND, TTM_GETTIPTEXTCOLOR, NULL, NULL);

		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%lu"), cref);
		return true;
	}
	// [NAME] [ID] [PROP]
	else if (prop == TEXT("alpha")) {
		szReturnValue = dcx_truefalse(m_bAlphaBlend);
		return true;
	}
	// [NAME] [ID] [PROP]
	else if (prop == TEXT("textcolor")) {
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), m_clrText);
		return true;
	}
	// [NAME] [ID] [PROP]
	else if (prop == TEXT("textbgcolor")) {
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), m_clrBackText);
		return true;
	}
	// [NAME] [ID] [PROP]
	else if (prop == TEXT("bgcolor")) {
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), m_clrBackground);
		return true;
	}
	else
		throw Dcx::dcxException("Invalid property or number of arguments");

	return false;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxControl::registreDefaultWindowProc( ) {
	//this->removeStyle( WS_BORDER|WS_DLGFRAME );
	//this->removeExStyle( WS_EX_CLIENTEDGE|WS_EX_DLGMODALFRAME|WS_EX_STATICEDGE|WS_EX_WINDOWEDGE );

	this->m_DefaultWindowProc = SubclassWindow( m_Hwnd, DcxControl::WindowProc );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxControl::unregistreDefaultWindowProc( ) {
	if (this->m_DefaultWindowProc != nullptr) {
		// implies this has alrdy been called.
		if ((WNDPROC)GetWindowLongPtr(m_Hwnd, GWLP_WNDPROC) == DcxControl::WindowProc)
			SubclassWindow(m_Hwnd, this->m_DefaultWindowProc);
	}
	this->m_DefaultWindowProc = nullptr;
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT CALLBACK DcxControl::WindowProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	auto pthis = static_cast<DcxControl*>(GetProp(mHwnd, TEXT("dcx_cthis")));

	// sanity check, see that prop exists.
	if (pthis == nullptr)
		return DefWindowProc(mHwnd, uMsg, wParam, lParam);

	if (uMsg == WM_PAINT && pthis->m_pParentDialog->IsVistaStyle()) {
		ValidateRect(mHwnd, nullptr);
		RECT rcUpdate;
		if (GetWindowRect(mHwnd, &rcUpdate))
		{
			//RECT rcParent;
			//GetClientRect(GetParent(mHwnd), &rcParent);
			//MapWindowRect(GetParent(mHwnd), nullptr, &rcParent);
			//rcUpdate.left = max(rcUpdate.left, rcParent.left);
			//rcUpdate.top = max(rcUpdate.top, rcParent.top);
			//rcUpdate.right = min(rcUpdate.right, rcParent.right);
			//rcUpdate.bottom = min(rcUpdate.bottom, rcParent.bottom);
			pthis->m_pParentDialog->UpdateVistaStyle(&rcUpdate);
		}
		return 0L;
	}

	const auto fBlocked = (InSendMessageEx(nullptr) & (ISMEX_REPLIED | ISMEX_SEND)) == ISMEX_SEND;

	if (!fBlocked) {
		// If Message is blocking just call old win proc
		BOOL bParsed = FALSE;

		if ((uMsg != WM_DESTROY) && (uMsg != WM_NCDESTROY))
			pthis->incRef( );

		const auto lrRes = pthis->PostMessage(uMsg, wParam, lParam, bParsed);

		if ((uMsg != WM_DESTROY) && (uMsg != WM_NCDESTROY))
			pthis->decRef();

		if (bParsed)
			return lrRes;
	}

	if (pthis->m_DefaultWindowProc != nullptr)
		return CallWindowProc(pthis->m_DefaultWindowProc, mHwnd, uMsg, wParam, lParam);

	return DefWindowProc(mHwnd, uMsg, wParam, lParam);
}

/*!
 * \brief blah
 *
 * blah
 *
 * Input [NAME] [SWITCH] [ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)
 */

DcxControl * DcxControl::controlFactory(DcxDialog *const p_Dialog, const UINT mID, const TString & tsInput, const UINT offset, const UINT64 mask, HWND hParent) {

	const auto type(tsInput.getfirsttok(offset));

	RECT rc;

	rc.left = tsInput.getnexttok().to_<LONG>();
	rc.top = tsInput.getnexttok().to_<LONG>();
	rc.right = rc.left + tsInput.getnexttok().to_<LONG>();
	rc.bottom = rc.top + tsInput.getnexttok().to_<LONG>();

	const auto styles(tsInput.getlasttoks());

	if (hParent == nullptr)
		hParent = p_Dialog->getHwnd();

//	const auto dct = DcxControl::TSTypeToControlType(type);
//
//	if ((dct == DcxControlTypes::PROGRESSBAR) && (dcx_testflag(mask, CTLF_ALLOW_PBAR)))
//		return new DcxProgressBar(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::TRACKBAR) && (dcx_testflag(mask, CTLF_ALLOW_TRACKBAR)))
//		return new DcxTrackBar(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::COMBOEX) && (dcx_testflag(mask, CTLF_ALLOW_COMBOEX)))
//		return new DcxComboEx(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::COLORCOMBO) && (dcx_testflag(mask, CTLF_ALLOW_COLORCOMBO)))
//		return new DcxColorCombo(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::STATUSBAR) && (dcx_testflag(mask, CTLF_ALLOW_STATUSBAR)))
//		return new DcxStatusBar(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::TOOLBAR) && (dcx_testflag(mask, CTLF_ALLOW_TOOLBAR)))
//		return new DcxToolBar(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::TREEVIEW) && (dcx_testflag(mask, CTLF_ALLOW_TREEVIEW)))
//		return new DcxTreeView(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::LISTVIEW) && (dcx_testflag(mask, CTLF_ALLOW_LISTVIEW)))
//		return new DcxListView(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::REBAR) && (dcx_testflag(mask, CTLF_ALLOW_REBAR)))
//		return new DcxReBar(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::BUTTON) && (dcx_testflag(mask, CTLF_ALLOW_BUTTON)))
//		return new DcxButton(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::RICHEDIT) && (dcx_testflag(mask, CTLF_ALLOW_RICHEDIT)))
//		return new DcxRichEdit(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::UPDOWN) && (dcx_testflag(mask, CTLF_ALLOW_UPDOWN)))
//		return new DcxUpDown(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::IPADDRESS) && (dcx_testflag(mask, CTLF_ALLOW_IPADDRESS)))
//		return new DcxIpAddress(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::WEBCTRL) && (dcx_testflag(mask, CTLF_ALLOW_WEBCTRL)))
//		return new DcxWebControl(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::CALENDAR) && (dcx_testflag(mask, CTLF_ALLOW_CALANDER)))
//		return new DcxCalendar(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::DATETIME) && (dcx_testflag(mask, CTLF_ALLOW_CALANDER)))
//		return new DcxDateTime(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::DIVIDER) && (dcx_testflag(mask, CTLF_ALLOW_DIVIDER)))
//		return new DcxDivider(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::PANEL) && (dcx_testflag(mask, CTLF_ALLOW_PANEL)))
//		return new DcxPanel(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::TABB) && (dcx_testflag(mask, CTLF_ALLOW_TAB)))
//		return new DcxTab(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::LINE) && (dcx_testflag(mask, CTLF_ALLOW_LINE)))
//		return new DcxLine(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::BOX) && (dcx_testflag(mask, CTLF_ALLOW_BOX)))
//		return new DcxBox(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::RADIO) && (dcx_testflag(mask, CTLF_ALLOW_RADIO)))
//		return new DcxRadio(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::CHECK) && (dcx_testflag(mask, CTLF_ALLOW_CHECK)))
//		return new DcxCheck(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::TEXT) && (dcx_testflag(mask, CTLF_ALLOW_TEXT)))
//		return new DcxText(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::EDIT) && (dcx_testflag(mask, CTLF_ALLOW_EDIT)))
//		return new DcxEdit(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::SCROLL) && (dcx_testflag(mask, CTLF_ALLOW_SCROLL)))
//		return new DcxScroll(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::LIST) && (dcx_testflag(mask, CTLF_ALLOW_LIST)))
//		return new DcxList(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::LINK) && (dcx_testflag(mask, CTLF_ALLOW_LINK)))
//		return new DcxLink(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::IMAGE) && (dcx_testflag(mask, CTLF_ALLOW_IMAGE)))
//		return new DcxImage(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::PAGER) && (dcx_testflag(mask, CTLF_ALLOW_PAGER)))
//		return new DcxPager(mID, p_Dialog, hParent, &rc, styles);
//	else if ((dct == DcxControlTypes::STACKER) && (dcx_testflag(mask, CTLF_ALLOW_STACKER)))
//		return new DcxStacker(mID, p_Dialog, hParent, &rc, styles);
//#ifdef DCX_USE_DXSDK
//
//	else if ((dct == DcxControlTypes::DIRECTSHOW) && (dcx_testflag(mask, CTLF_ALLOW_DIRECTSHOW)))
//		return new DcxDirectshow(mID, p_Dialog, hParent, &rc, styles);
//
//#endif // DCX_USE_DXSDK
//
//	else if ((dct == DcxControlTypes::WINDOW) && (dcx_testflag(mask, CTLF_ALLOW_DOCK))) {
//		if (styles.empty())
//			throw Dcx::dcxException("No window name");
//
//		const auto tsWin(styles.getfirsttok(1));
//
//		// this helps stop '@' being passed as $window(@).hwnd == $window(-2).hwnd & usually causes a crash.
//		if (tsWin.len() < 2)
//			throw Dcx::dcxException(TEXT("No such window: %s"), tsWin);
//
//		auto winHwnd = (HWND)tsWin.to_num();
//		if (!IsWindow(winHwnd)) {
//			TCHAR windowHwnd[30];
//
//			mIRCLinker::evalex(windowHwnd, 30, TEXT("$window(%s).hwnd"), tsWin.to_chr());
//
//			winHwnd = (HWND)dcx_atoi(windowHwnd);
//		}
//
//		if (!IsWindow(winHwnd))
//			throw Dcx::dcxException(TEXT("No such window: %s"), tsWin);
//
//		if (p_Dialog->getControlByHWND(winHwnd) != nullptr)
//			throw Dcx::dcxException("Window already a DCX Control");
//
//		return new DcxMWindow(winHwnd, hParent, mID, p_Dialog, &rc, styles);
//	}
//	else if ((dct == DcxControlTypes::DIALOG) && (dcx_testflag(mask, CTLF_ALLOW_DOCK))) {
//		if (styles.empty())
//			throw Dcx::dcxException("No dialog name");
//
//		const auto tsDname(styles.getfirsttok(1));
//		auto winHwnd = GetHwndFromString(tsDname);
//
//		if (IsWindow(winHwnd))
//			throw Dcx::dcxException(TEXT("No such dialog: %"), tsDname);
//
//		if (p_Dialog->getControlByHWND(winHwnd) != nullptr)
//			Dcx::dcxException(TEXT("Control already exists : %"), tsDname);
//
//		auto newDialog = new DcxMDialog(winHwnd, hParent, mID, p_Dialog, &rc, styles);
//		auto dlg = Dcx::Dialogs.getDialogByHandle(winHwnd);
//
//		// if its a dcx marked dialog, mark the parent name
//		if (dlg != nullptr)
//			dlg->setParentName(p_Dialog->getName());
//
//		return newDialog;
//	}
	
	switch (DcxControl::TSTypeToControlType(type))
	{
	case DcxControlTypes::PROGRESSBAR:
		if (dcx_testflag(mask, CTLF_ALLOW_PBAR))
			return new DcxProgressBar(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::TRACKBAR:
		if (dcx_testflag(mask, CTLF_ALLOW_TRACKBAR))
			return new DcxTrackBar(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::COMBOEX:
		if (dcx_testflag(mask, CTLF_ALLOW_COMBOEX))
			return new DcxComboEx(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::COLORCOMBO:
		if (dcx_testflag(mask, CTLF_ALLOW_COLORCOMBO))
			return new DcxColorCombo(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::STATUSBAR:
		if (dcx_testflag(mask, CTLF_ALLOW_STATUSBAR))
			return new DcxStatusBar(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::TOOLBAR:
		if (dcx_testflag(mask, CTLF_ALLOW_TOOLBAR))
			return new DcxToolBar(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::TREEVIEW:
		if (dcx_testflag(mask, CTLF_ALLOW_TREEVIEW))
			return new DcxTreeView(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::LISTVIEW:
		if (dcx_testflag(mask, CTLF_ALLOW_LISTVIEW))
			return new DcxListView(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::REBAR:
		if (dcx_testflag(mask, CTLF_ALLOW_REBAR))
			return new DcxReBar(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::BUTTON:
		if (dcx_testflag(mask, CTLF_ALLOW_BUTTON))
			return new DcxButton(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::RICHEDIT:
		if (dcx_testflag(mask, CTLF_ALLOW_RICHEDIT))
			return new DcxRichEdit(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::UPDOWN:
		if (dcx_testflag(mask, CTLF_ALLOW_UPDOWN))
			return new DcxUpDown(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::IPADDRESS:
		if (dcx_testflag(mask, CTLF_ALLOW_IPADDRESS))
			return new DcxIpAddress(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::WEBCTRL:
		if (dcx_testflag(mask, CTLF_ALLOW_WEBCTRL))
			return new DcxWebControl(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::CALENDAR:
		if (dcx_testflag(mask, CTLF_ALLOW_CALANDER))
			return new DcxCalendar(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::DATETIME:
		if (dcx_testflag(mask, CTLF_ALLOW_CALANDER))
			return new DcxDateTime(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::DIVIDER:
		if (dcx_testflag(mask, CTLF_ALLOW_DIVIDER))
			return new DcxDivider(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::PANEL:
		if (dcx_testflag(mask, CTLF_ALLOW_PANEL))
			return new DcxPanel(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::TABB:
		if (dcx_testflag(mask, CTLF_ALLOW_TAB))
			return new DcxTab(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::LINE:
		if (dcx_testflag(mask, CTLF_ALLOW_LINE))
			return new DcxLine(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::BOX:
		if (dcx_testflag(mask, CTLF_ALLOW_BOX))
			return new DcxBox(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::RADIO:
		if (dcx_testflag(mask, CTLF_ALLOW_RADIO))
			return new DcxRadio(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::CHECK:
		if (dcx_testflag(mask, CTLF_ALLOW_CHECK))
			return new DcxCheck(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::TEXT:
		if (dcx_testflag(mask, CTLF_ALLOW_TEXT))
			return new DcxText(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::EDIT:
		if (dcx_testflag(mask, CTLF_ALLOW_EDIT))
			return new DcxEdit(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::SCROLL:
		if (dcx_testflag(mask, CTLF_ALLOW_SCROLL))
			return new DcxScroll(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::LIST:
		if (dcx_testflag(mask, CTLF_ALLOW_LIST))
			return new DcxList(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::LINK:
		if (dcx_testflag(mask, CTLF_ALLOW_LINK))
			return new DcxLink(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::IMAGE:
		if (dcx_testflag(mask, CTLF_ALLOW_IMAGE))
			return new DcxImage(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::PAGER:
		if (dcx_testflag(mask, CTLF_ALLOW_PAGER))
			return new DcxPager(mID, p_Dialog, hParent, &rc, styles);
		break;
	case DcxControlTypes::STACKER:
		if (dcx_testflag(mask, CTLF_ALLOW_STACKER))
			return new DcxStacker(mID, p_Dialog, hParent, &rc, styles);
		break;
#ifdef DCX_USE_DXSDK

	case DcxControlTypes::DIRECTSHOW:
		if (dcx_testflag(mask, CTLF_ALLOW_DIRECTSHOW))
			return new DcxDirectshow(mID, p_Dialog, hParent, &rc, styles);
		break;

#endif // DCX_USE_DXSDK

	case DcxControlTypes::WINDOW:
		if (dcx_testflag(mask, CTLF_ALLOW_DOCK)) {
			if (styles.empty())
				throw Dcx::dcxException("No window name");

			const auto tsWin(styles.getfirsttok(1));

			// this helps stop '@' being passed as $window(@).hwnd == $window(-2).hwnd & usually causes a crash.
			if (tsWin.len() < 2)
				throw Dcx::dcxException(TEXT("No such window: %s"), tsWin);

			auto winHwnd = (HWND)tsWin.to_num();
			if (!IsWindow(winHwnd)) {
				TCHAR windowHwnd[30];

				mIRCLinker::evalex(windowHwnd, Dcx::countof(windowHwnd), TEXT("$window(%s).hwnd"), tsWin.to_chr());

				winHwnd = (HWND)dcx_atoi(windowHwnd);
			}

			if (!IsWindow(winHwnd))
				throw Dcx::dcxException(TEXT("No such window: %s"), tsWin);

			if (p_Dialog->getControlByHWND(winHwnd) != nullptr)
				throw Dcx::dcxException("Window already a DCX Control");

			return new DcxMWindow(winHwnd, hParent, mID, p_Dialog, &rc, styles);
		}
		break;
	case DcxControlTypes::DIALOG:
		if (dcx_testflag(mask, CTLF_ALLOW_DOCK)) {
			if (styles.empty())
				throw Dcx::dcxException("No dialog name");

			const auto tsDname(styles.getfirsttok(1));
			auto winHwnd = GetHwndFromString(tsDname);

			if (IsWindow(winHwnd))
				throw Dcx::dcxException(TEXT("No such dialog: %"), tsDname);

			if (p_Dialog->getControlByHWND(winHwnd) != nullptr)
				Dcx::dcxException(TEXT("Control already exists : %"), tsDname);

			auto newDialog = new DcxMDialog(winHwnd, hParent, mID, p_Dialog, &rc, styles);
			auto dlg = Dcx::Dialogs.getDialogByHandle(winHwnd);

			// if its a dcx marked dialog, mark the parent name
			if (dlg != nullptr)
				dlg->setParentName(p_Dialog->getName());

			return newDialog;
		}
		break;
	case DcxControlTypes::UNKNOWN:
		throw Dcx::dcxException(TEXT("Unknown Control Type: %"), type);
		break;
	}
	throw Dcx::dcxException(TEXT("Control Type not supported: %"), type);
	return nullptr;	// never gets executed, but compilers complain about its absense...
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxControl::setFont( const HFONT hFont, const BOOL fRedraw ) {
  return SendMessage( m_Hwnd, WM_SETFONT, (WPARAM) hFont, (LPARAM) MAKELPARAM(fRedraw, 0) );
}

/*!
 * \brief blah
 *
 * blah
 */

HFONT DcxControl::getFont( ) const noexcept {
  return GetWindowFont( m_Hwnd );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxControl::setRedraw( const BOOL fView ) {
  return SendMessage( m_Hwnd, WM_SETREDRAW, (WPARAM) fView, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxControl::setControlFont( const HFONT hFont, const BOOL fRedraw ) {

	auto hControlFont = this->getFont();

	if (hControlFont != GetStockObject(DEFAULT_GUI_FONT)) {

		if (hControlFont != nullptr) {
			DeleteFont(hControlFont);
			this->m_hFont = nullptr;
		}
		else if (this->m_hFont != nullptr) {
			DeleteFont(this->m_hFont);
			this->m_hFont = nullptr;
		}
	}

	this->setFont(hFont, fRedraw);
	this->m_hFont = hFont;
}

/*!
 * \brief blah
 *
 * blah
 */

const HBRUSH &DcxControl::getBackClrBrush( ) const noexcept {

  return this->m_hBackBrush;
}

/*!
 * \brief blah
 *
 * blah
 */

const COLORREF &DcxControl::getBackColor( ) const noexcept {
	return this->m_clrBackText;
}

/*!
 * \brief blah
 *
 * blah
 */

const COLORREF &DcxControl::getTextColor( ) const noexcept {
	return this->m_clrText;
}

const RECT DcxControl::getWindowPosition(void) const {
	RECT rc;
	SetRectEmpty(&rc);
	if (GetWindowRect( m_Hwnd, &rc ))
		MapWindowRect(nullptr, GetParent( m_Hwnd ), &rc);
	return rc;
}

void DcxControl::updateParentCtrl(void)
{
	// find the host control, if any.
	this->m_pParentHWND = GetParent(m_Hwnd);
}

void DcxControl::DrawCtrlBackground(const HDC hdc, const DcxControl *const p_this, const LPRECT rwnd, HTHEME hTheme, const int iPartId, const int iStateId)
{
	if ((hdc == nullptr) || (p_this == nullptr))
		return;

	// fill background.
	if (!p_this->isExStyle(WS_EX_TRANSPARENT)) {
		RECT rc = { 0 };
		if (rwnd == nullptr)
		{
			if (!GetClientRect(p_this->getHwnd(), &rc))
				return;
		}
		else
			CopyRect(&rc, rwnd);

		if (!IsWindowEnabled(p_this->m_Hwnd)) {// use disabled colouring when windows disabled.
			if (hTheme != nullptr && !p_this->m_bNoTheme && Dcx::UXModule.dcxIsThemeActive()) {
				if (Dcx::UXModule.dcxIsThemeBackgroundPartiallyTransparent(hTheme, iPartId, iStateId))
					Dcx::UXModule.dcxDrawThemeParentBackground(p_this->m_Hwnd, hdc, &rc);
				Dcx::UXModule.dcxDrawThemeBackground(hTheme, hdc, iPartId, iStateId, &rc, nullptr);
			}
			else
				FillRect( hdc, &rc, GetSysColorBrush(COLOR_3DFACE) );
		}
		else if (p_this->m_bGradientFill) {
			auto clrStart = p_this->m_clrStartGradient;
			auto clrEnd = p_this->m_clrEndGradient;

			if (clrStart == CLR_INVALID)
				clrStart = GetSysColor(COLOR_3DFACE);
			if (clrEnd == CLR_INVALID)
				clrEnd = GetSysColor(COLOR_GRADIENTACTIVECAPTION);

			XPopupMenuItem::DrawGradient( hdc, &rc, clrStart, clrEnd, p_this->m_bGradientVertical);
		}
		else {
			auto hBrush = p_this->getBackClrBrush();
			if (hBrush == nullptr) {
				if (hTheme && !p_this->m_bNoTheme && Dcx::UXModule.dcxIsThemeActive()) {
					if (Dcx::UXModule.dcxIsThemeBackgroundPartiallyTransparent(hTheme, iPartId, iStateId))
						Dcx::UXModule.dcxDrawThemeParentBackground(p_this->m_Hwnd, hdc, &rc);
					Dcx::UXModule.dcxDrawThemeBackground(hTheme, hdc, iPartId, iStateId, &rc, nullptr);
					return;
				}
				else
					hBrush = GetSysColorBrush(COLOR_3DFACE);
			}
			if (hBrush != nullptr)
				FillRect( hdc, &rc, hBrush );
		}
	}
}

void DcxControl::DrawControl(HDC hDC, HWND hwnd)
{
#if DCX_USE_WRAPPERS
	// if window matches this one, don't draw (loop condition)
	if (hwnd == m_Hwnd)
		return;

	// if window isn't visible, don't draw.
	if (!IsWindowVisible(hwnd))
		return;

	// if window is within a background paint of it's own, don't draw. (loop condition)
	{
		auto p_ctrl = this->m_pParentDialog->getControlByHWND(hwnd);
		if (p_ctrl != nullptr && p_ctrl->m_bInPrint)
			return;
	}

	RECT rc;
	if (!GetWindowRect(hwnd, &rc))
		return;

	MapWindowRect(nullptr, GetParent(hwnd), &rc);

	// if window isn't within the client area of the control who's background we are drawing, don't draw.
	if (!RectVisible(hDC, &rc))
		return;

	auto w = (rc.right - rc.left), h = (rc.bottom - rc.top);

	Dcx::dcxBitmapResource hBitmap(hDC, w, h);

	Dcx::dcxHDCBitmapResource hdcMemory(hDC, hBitmap);

	::SendMessage(hwnd, WM_ERASEBKGND, (WPARAM)(HDC)hdcMemory, 1L); // HACK: using 1L instead of NULL as a workaround for stacker.
	::SendMessage(hwnd, WM_PRINT, (WPARAM)(HDC)hdcMemory, (LPARAM)PRF_NONCLIENT | PRF_CLIENT | PRF_CHILDREN | PRF_CHECKVISIBLE /*| PRF_ERASEBKGND*/);

	BitBlt(hDC, rc.left, rc.top, w, h, hdcMemory, 0, 0, SRCCOPY);
#else
	// if window matches this one, don't draw (loop condition)
	if (hwnd == m_Hwnd)
		return;
	
	// if window isnTEXT('t visible, don')t draw.
	if (!IsWindowVisible(hwnd))
		return;
	
	// if window is within a background paint of it's own, don't draw. (loop condition)
	{
		auto p_ctrl = this->m_pParentDialog->getControlByHWND(hwnd);
		if (p_ctrl != nullptr && p_ctrl->m_bInPrint)
			return;
	}
	
	RECT rc;
	if (!GetWindowRect(hwnd, &rc))
		return;
	
	MapWindowRect(nullptr,GetParent(hwnd),&rc);
	
	// if window isn't within the client area of the control who's background we are drawing, don't draw.
	if (!RectVisible(hDC, &rc))
		return;
	
	auto hdcMemory = ::CreateCompatibleDC(hDC);
	
	if (hdcMemory == nullptr)
		return;
	
	Auto(DeleteDC(hdcMemory));
	
	auto w = (rc.right - rc.left), h = (rc.bottom - rc.top);
	auto hBitmap = ::CreateCompatibleBitmap(hDC, w, h);
	
	if (hBitmap == nullptr)
		return;
	
	Auto(DeleteBitmap(hBitmap));
	
	auto hbmpOld = SelectBitmap(hdcMemory, hBitmap);
	
	Auto(SelectBitmap(hdcMemory, hbmpOld));
	
	::SendMessage(hwnd, WM_ERASEBKGND, (WPARAM)hdcMemory, 1L); // HACK: using 1L instead of NULL as a workaround for stacker.
	::SendMessage( hwnd, WM_PRINT, (WPARAM)hdcMemory, (LPARAM)PRF_NONCLIENT | PRF_CLIENT | PRF_CHILDREN | PRF_CHECKVISIBLE /*| PRF_ERASEBKGND*/);
	
	BitBlt( hDC, rc.left, rc.top, w, h, hdcMemory, 0, 0, SRCCOPY);
#endif

}

void DcxControl::DrawParentsBackground(const HDC hdc, const RECT *const rcBounds, const HWND dHwnd)
{
	// fill in parent bg
	RECT rcClient = { 0 };
	auto hwnd = m_Hwnd;

	if (dHwnd != nullptr)
		hwnd = dHwnd;

	if (rcBounds == nullptr)
	{
		if (!GetClientRect(hwnd, &rcClient)) // get controls client area
			return;
	}
	else
		rcClient = *rcBounds;

	// if themes are active use them.
//	if (dcxIsThemeActive() && !this->m_bNoTheme) {
//#ifdef DCX_USE_WINSDK
//		if (DrawThemeParentBackgroundExUx != nullptr)
//			DrawThemeParentBackgroundExUx(hwnd, hdc, 0, &rcClient); // Vista only, does basicly the same as below.
//		else
//#endif
//			DrawThemeParentBackgroundUx(hwnd, hdc, &rcClient); // XP+
//		return;
//	}

	/*
		The following code draws the parents background & client area,
		followed by all child controls covered by this one.
	*/
	updateParentCtrl(); // find the host control, if any.

	//If in Vista mode
	if (m_pParentDialog->IsVistaStyle())
	{
		// Check if the hdc to render too is the main hdc, if so bkg is already drawn so just return
		if (hdc == m_pParentDialog->GetVistaHDC())
			return;
		// Check if parent is dialog.
		if (m_pParentHWND == m_pParentDialog->getHwnd())
		{
			// When in vista mode dialog has already been drawn
			// So just grab image from windows DC.
			auto hdcParent = m_pParentDialog->GetVistaHDC();
			auto rcWin = rcClient;
			m_pParentDialog->MapVistaRect(hwnd, &rcWin);
			BitBlt( hdc, rcClient.left, rcClient.top, (rcClient.right - rcClient.left), (rcClient.bottom - rcClient.top),
				hdcParent, rcWin.left, rcWin.top, SRCCOPY);
			return;
		}
	}
	if (m_pParentDialog->isExStyle(WS_EX_COMPOSITED))
	{
		// When in composited mode underling controls have already been drawn
		// So just grab image from windows DC.

		auto hdcParent = GetDC(m_pParentHWND);
		if (hdcParent != nullptr)
		{
			Auto(ReleaseDC(m_pParentHWND, hdcParent));
		
			auto rcWin = rcClient;
			MapWindowRect(hwnd, m_pParentHWND, &rcWin);
		
			BitBlt(hdc, rcClient.left, rcClient.top, (rcClient.right - rcClient.left), (rcClient.bottom - rcClient.top),
				hdcParent, rcWin.left, rcWin.top, SRCCOPY);
		}
		return;
	}

	//// make a new HDC for background rendering
	//HDC hdcbkg = CreateCompatibleDC( hdc );
	//if (hdcbkg != nullptr) {
	//	//RECT rcParentWin, rcParentClient, rcWin;
	//	RECT rcParentWin, rcWin;
	//	// get parent windows client area.
	//	GetClientRect(this->m_pParentHWND,&rcParentWin);
	//	// make a bitmap for rendering to.
	//	HBITMAP memBM = CreateCompatibleBitmap ( hdc, rcParentWin.right - rcParentWin.left, rcParentWin.bottom - rcParentWin.top );
	//	if (memBM != nullptr) {
	//		// get this controls x & y pos within its parent.
	//		rcWin = rcClient;
	//		MapWindowPoints(hwnd,this->m_pParentHWND, (LPPOINT)&rcWin, 2);
	//		// associate bitmap with HDC
	//		HBITMAP oldBM = (HBITMAP)SelectObject ( hdcbkg, memBM );
	//		HRGN clipRgn = CreateRectRgnIndirect(&rcWin); // clip parents drawing to this controls rect.
	//		if (clipRgn != nullptr) {
	//			SelectClipRgn(hdcbkg, clipRgn);
	//			DeleteObject(clipRgn);
	//		}
	//		// Sending WM_ERASEBKGND followed by WM_PRINTCLIENT emulates the method used by DrawThemeParentBackgroundEx() on vista.
	//		this->m_bInPrint = true; // this helps prevent long drawing loops
	//		// fill in the parents image
	//		::SendMessage(this->m_pParentHWND, WM_ERASEBKGND, (WPARAM)hdcbkg,1L); // HACK: using 1L instead of nullptr as a workaround for stacker.
	//		::SendMessage(this->m_pParentHWND, WM_PRINTCLIENT, (WPARAM)hdcbkg,PRF_CLIENT);
	//		// now draw all child controls within area of this control.
	//		// NB: AVOID EnumChildWindows()
	//		HWND child = GetWindow(m_Hwnd, GW_HWNDPREV);
	//		while (child != nullptr) {
	//			this->DrawControl(hdcbkg, child);
	//			child = GetWindow(child, GW_HWNDPREV);
	//		}
	//		this->m_bInPrint = false;
	//		// draw background to main hdc
	//		BitBlt( hdc, rcClient.left, rcClient.top, (rcClient.right - rcClient.left), (rcClient.bottom - rcClient.top),
	//			hdcbkg, rcWin.left, rcWin.top, SRCCOPY);
	//		DeleteObject(SelectObject( hdcbkg, oldBM ));
	//	}
	//	DeleteDC( hdcbkg );
	//}
	// make a new HDC for background rendering

	RECT rcParentWin = { 0 }, rcWin = { 0 };
	if (!GetClientRect(m_pParentHWND, &rcParentWin))
		return;

#if DCX_USE_WRAPPERS
	Dcx::dcxHDCBuffer hdcbkg(hdc, rcParentWin);

	// get this controls x & y pos within its parent.
	rcWin = rcClient;
	MapWindowRect(hwnd, m_pParentHWND, &rcWin);
	auto clipRgn = CreateRectRgnIndirect(&rcWin); // clip parents drawing to this controls rect.
	if (clipRgn != nullptr) {
		SelectClipRgn(hdcbkg, clipRgn);
		DeleteRgn(clipRgn);
	}

	// Sending WM_ERASEBKGND followed by WM_PRINTCLIENT emulates the method used by DrawThemeParentBackgroundEx() on vista.
	m_bInPrint = true; // this helps prevent long drawing loops
	// fill in the parents image
	::SendMessage(m_pParentHWND, WM_ERASEBKGND, (WPARAM)(HDC)hdcbkg, 1L); // HACK: using 1L instead of NULL as a workaround for stacker.
	::SendMessage(m_pParentHWND, WM_PRINTCLIENT, (WPARAM)(HDC)hdcbkg, PRF_CLIENT);

	for (auto hChild = GetWindow(m_Hwnd, GW_HWNDPREV); hChild != nullptr; hChild = GetWindow(hChild, GW_HWNDPREV))
		DrawControl(hdcbkg, hChild);

	m_bInPrint = false;
	// draw background to main hdc
	BitBlt(hdc, rcClient.left, rcClient.top, (rcClient.right - rcClient.left), (rcClient.bottom - rcClient.top),
		hdcbkg, rcWin.left, rcWin.top, SRCCOPY);
#else
	auto hdcbkg = CreateHDCBuffer(hdc, &rcParentWin);
	if (hdcbkg == nullptr)
		return;
	Auto(DeleteHDCBuffer(hdcbkg));

	// get this controls x & y pos within its parent.
	rcWin = rcClient;
	MapWindowRect(hwnd, this->m_pParentHWND, &rcWin);
	auto clipRgn = CreateRectRgnIndirect(&rcWin); // clip parents drawing to this controls rect.
	if (clipRgn != nullptr) {
		SelectClipRgn(*hdcbkg, clipRgn);
		DeleteRgn(clipRgn);
	}

	// Sending WM_ERASEBKGND followed by WM_PRINTCLIENT emulates the method used by DrawThemeParentBackgroundEx() on vista.
	this->m_bInPrint = true; // this helps prevent long drawing loops
							 // fill in the parents image
	::SendMessage(this->m_pParentHWND, WM_ERASEBKGND, (WPARAM)*hdcbkg, 1L); // HACK: using 1L instead of NULL as a workaround for stacker.
	::SendMessage(this->m_pParentHWND, WM_PRINTCLIENT, (WPARAM)*hdcbkg, PRF_CLIENT);

	//::SendMessage(this->m_pParentHWND, WM_PRINT, (WPARAM)*hdcbkg,PRF_CLIENT|PRF_ERASEBKGND);
	// now draw all child controls within area of this control.
	// NB: AVOID EnumChildWindows()
	//HWND child = GetWindow(m_Hwnd, GW_HWNDPREV);
	//while (child != nullptr) {
	//	this->DrawControl(*hdcbkg, child);
	//	child = GetWindow(child, GW_HWNDPREV);
	//}

	for (auto hChild = GetWindow(m_Hwnd, GW_HWNDPREV); hChild != nullptr; hChild = GetWindow(hChild, GW_HWNDPREV))
		this->DrawControl(*hdcbkg, hChild);

	this->m_bInPrint = false;
	// draw background to main hdc
	BitBlt(hdc, rcClient.left, rcClient.top, (rcClient.right - rcClient.left), (rcClient.bottom - rcClient.top),
		*hdcbkg, rcWin.left, rcWin.top, SRCCOPY);
#endif
}

LPALPHAINFO DcxControl::SetupAlphaBlend(HDC *hdc, const bool DoubleBuffer)
{
	if ((hdc == nullptr) || (*hdc == nullptr) || (!this->m_bAlphaBlend && !DoubleBuffer))
		return nullptr;

	//auto ai = new ALPHAINFO;
	auto ai = std::make_unique<ALPHAINFO>();

	ZeroMemory(ai.get(),sizeof(ALPHAINFO));
	/*
		1: draw parents bg to temp hdc
		2: copy bg to temp hdcbkg for later alpha
		3: draw button to temp hdc, over parents bg
		4: alpha blend temp hdc to hdc
	*/
	// get windows client rect
	if (!GetClientRect(m_Hwnd, &ai->ai_rcClient))
	{
		//delete ai;
		return nullptr;
	}
	// get window rect
	if (!GetWindowRect(m_Hwnd, &ai->ai_rcWin))
	{
		//delete ai;
		return nullptr;
	}
	if (Dcx::UXModule.IsBufferedPaintSupported()) {
		BP_PAINTPARAMS paintParams = { 0 };
		paintParams.cbSize = sizeof(paintParams);
		paintParams.dwFlags = BPPF_ERASE;
		//paintParams.dwFlags = BPPF_NONCLIENT;
		ai->ai_bf.AlphaFormat = AC_SRC_OVER;
		ai->ai_bf.SourceConstantAlpha = this->m_iAlphaLevel; // 0x7f half of 0xff = 50% transparency
		if (this->m_bAlphaBlend)
			paintParams.pBlendFunction = &ai->ai_bf;

		ai->ai_Buffer = Dcx::UXModule.dcxBeginBufferedPaint(*hdc, &ai->ai_rcClient, BPBF_COMPATIBLEBITMAP, &paintParams, &ai->ai_hdc);
		if (ai->ai_Buffer != nullptr) {
			this->DrawParentsBackground(ai->ai_hdc, &ai->ai_rcClient);
			BitBlt(*hdc, ai->ai_rcClient.left, ai->ai_rcClient.top, ai->ai_rcClient.right - ai->ai_rcClient.left, ai->ai_rcClient.bottom - ai->ai_rcClient.top, ai->ai_hdc, ai->ai_rcClient.left, ai->ai_rcClient.top, SRCCOPY);
			*hdc = ai->ai_hdc;
			return ai.release();
		}
	}
	// if vista method failed, fall through to our own method.

	//{
	//	// create a new HDC for alpha blending.
	//	ai->ai_hdc = CreateCompatibleDC( *hdc );
	//	ai->ai_bkg = nullptr;
	//	if (ai->ai_hdc != nullptr) {
	//		//GetClientRect(m_Hwnd,&ai->ai_rcWin);
	//		GetWindowRect(m_Hwnd,&ai->ai_rcWin);
	//		// create a bitmap to render to
	//		ai->ai_bitmap = CreateCompatibleBitmap ( *hdc, ai->ai_rcWin.right - ai->ai_rcWin.left, ai->ai_rcWin.bottom - ai->ai_rcWin.top );
	//		if (ai->ai_bitmap != nullptr) {
	//			GetClientRect(m_Hwnd,&ai->ai_rcClient);
	//			// associate bitmap with hdc
	//			ai->ai_oldBM = (HBITMAP)SelectObject ( ai->ai_hdc, ai->ai_bitmap );
	//			// fill in parent bg
	//			this->DrawParentsBackground(ai->ai_hdc, &ai->ai_rcClient);
	//			// If alpha blending, make a background bitmap & fill it.
	//			if (this->m_bAlphaBlend) {
	//				// avoid doing the whole background rendering again by simply copying the one we just did.
	//				HDC hdcbkg = CreateCompatibleDC( *hdc );
	//				if (hdcbkg != nullptr) {
	//					ai->ai_bkg = CreateCompatibleBitmap ( *hdc, ai->ai_rcWin.right - ai->ai_rcWin.left, ai->ai_rcWin.bottom - ai->ai_rcWin.top );
	//					if (ai->ai_bkg != nullptr) {
	//						HBITMAP oldBM = (HBITMAP)SelectObject ( hdcbkg, ai->ai_bkg );
	//
	//						BitBlt( hdcbkg, ai->ai_rcClient.left, ai->ai_rcClient.top, ai->ai_rcClient.right - ai->ai_rcClient.left, ai->ai_rcClient.bottom - ai->ai_rcClient.top, ai->ai_hdc, ai->ai_rcClient.left, ai->ai_rcClient.top, SRCCOPY);
	//
	//						SelectObject(hdcbkg, oldBM);
	//					}
	//					DeleteDC(hdcbkg);
	//				}
	//			}
	//			ai->ai_Oldhdc = *hdc;
	//			*hdc = ai->ai_hdc;
	//		}
	//	}
	//}
	// create a new HDC Buffer for alpha blending.

	ai->ai_hdcBuffer = CreateHDCBuffer(*hdc, &ai->ai_rcWin);
	ai->ai_bkg = nullptr;
	if (ai->ai_hdcBuffer != nullptr) {
		// assign hdc for easy refrence & compat with previous code.
		ai->ai_hdc = *ai->ai_hdcBuffer;
		// fill in parent bg
		DrawParentsBackground(ai->ai_hdc, &ai->ai_rcClient);
		// If alpha blending, make a background bitmap & fill it.
		if (m_bAlphaBlend) {
			// avoid doing the whole background rendering again by simply copying the one we just did.
			auto hdcbkg = CreateCompatibleDC(*hdc);
			if (hdcbkg != nullptr) {
				Auto(DeleteDC(hdcbkg));

				ai->ai_bkg = CreateCompatibleBitmap(*hdc, ai->ai_rcWin.right - ai->ai_rcWin.left, ai->ai_rcWin.bottom - ai->ai_rcWin.top);
				if (ai->ai_bkg != nullptr) {
					auto oldBM = SelectBitmap(hdcbkg, ai->ai_bkg);
					Auto(SelectBitmap(hdcbkg, oldBM));

					BitBlt(hdcbkg, ai->ai_rcClient.left, ai->ai_rcClient.top, ai->ai_rcClient.right - ai->ai_rcClient.left, ai->ai_rcClient.bottom - ai->ai_rcClient.top, ai->ai_hdc, ai->ai_rcClient.left, ai->ai_rcClient.top, SRCCOPY);
				}
			}

			//Dcx::dcxHDCBitmap2Resource hdcbkg(*hdc, (ai->ai_rcWin.right - ai->ai_rcWin.left),(ai->ai_rcWin.bottom - ai->ai_rcWin.top));
			//BitBlt(hdcbkg, ai->ai_rcClient.left, ai->ai_rcClient.top, ai->ai_rcClient.right - ai->ai_rcClient.left, ai->ai_rcClient.bottom - ai->ai_rcClient.top, ai->ai_hdc, ai->ai_rcClient.left, ai->ai_rcClient.top, SRCCOPY);
		}
		ai->ai_Oldhdc = *hdc;
		*hdc = ai->ai_hdc;
	}
	return ai.release();
}

void DcxControl::FinishAlphaBlend(LPALPHAINFO ai)
{
	if (ai == nullptr)
		return;
	Auto(delete ai);

	if (ai->ai_Buffer != nullptr) {
		Dcx::UXModule.dcxEndBufferedPaint(ai->ai_Buffer, TRUE);
		return;
	}
	//// if we can't do Vista method, try do our own
	//if (ai->ai_hdc != nullptr) {
	//	if (ai->ai_bitmap != nullptr) {
	//		int w = (ai->ai_rcClient.right - ai->ai_rcClient.left), h = (ai->ai_rcClient.bottom - ai->ai_rcClient.top);
	//		if (this->m_bAlphaBlend) {
	//			if (ai->ai_bkg != nullptr) {
	//				// create a new HDC for alpha blending. (doing things this way avoids any flicker)
	//				HDC hdcbkg = CreateCompatibleDC( ai->ai_Oldhdc);
	//				if (hdcbkg != nullptr) {
	//					// associate bitmap with hdc
	//					HBITMAP oldBM = (HBITMAP)SelectObject ( hdcbkg, ai->ai_bkg );
	//					// alpha blend finished button with parents background
	//					BLENDFUNCTION bf = { AC_SRC_OVER, 0, (BYTE)this->m_iAlphaLevel, 0 }; // 0x7f half of 0xff = 50% transparency
	//					AlphaBlend(hdcbkg,ai->ai_rcClient.left,ai->ai_rcClient.top,w,h,ai->ai_hdc, ai->ai_rcClient.left, ai->ai_rcClient.top, w, h,bf);
	//					// draw final image to windows hdc.
	//					BitBlt(ai->ai_Oldhdc,ai->ai_rcClient.left,ai->ai_rcClient.top,w,h,hdcbkg,ai->ai_rcClient.left, ai->ai_rcClient.top, SRCCOPY);
	//
	//					SelectObject( hdcbkg, oldBM);
	//					DeleteDC(hdcbkg);
	//				}
	//				DeleteBitmap(ai->ai_bkg);
	//			}
	//		}
	//		else
	//			BitBlt(ai->ai_Oldhdc,ai->ai_rcClient.left,ai->ai_rcClient.top,w,h,ai->ai_hdc,ai->ai_rcClient.left, ai->ai_rcClient.top, SRCCOPY);
	//
	//		SelectObject (ai->ai_hdc, ai->ai_oldBM );
	//		DeleteObject(ai->ai_bitmap);
	//	}
	//	DeleteDC( ai->ai_hdc );
	//}
	//delete ai;

	// if we can't do Vista method, try do our own
	if (ai->ai_hdcBuffer != nullptr) {
		Auto(DeleteHDCBuffer(ai->ai_hdcBuffer));

		auto w = (ai->ai_rcClient.right - ai->ai_rcClient.left), h = (ai->ai_rcClient.bottom - ai->ai_rcClient.top);
		if (this->m_bAlphaBlend) {
			if (ai->ai_bkg != nullptr) {
				Auto(DeleteBitmap(ai->ai_bkg));

				// create a new HDC for alpha blending. (doing things this way avoids any flicker)
				auto hdcbkg = CreateCompatibleDC(ai->ai_Oldhdc);
				if (hdcbkg != nullptr) {
					Auto(DeleteDC(hdcbkg));

					// associate bitmap with hdc
					auto oldBM = SelectBitmap(hdcbkg, ai->ai_bkg);
					Auto(SelectBitmap(hdcbkg, oldBM));

					// alpha blend finished button with parents background
					ai->ai_bf.AlphaFormat = AC_SRC_OVER;
					ai->ai_bf.SourceConstantAlpha = this->m_iAlphaLevel; // 0x7f half of 0xff = 50% transparency
					AlphaBlend(hdcbkg,ai->ai_rcClient.left,ai->ai_rcClient.top,w,h,ai->ai_hdc, ai->ai_rcClient.left, ai->ai_rcClient.top, w, h, ai->ai_bf);
					// draw final image to windows hdc.
					BitBlt(ai->ai_Oldhdc,ai->ai_rcClient.left,ai->ai_rcClient.top,w,h,hdcbkg,ai->ai_rcClient.left, ai->ai_rcClient.top, SRCCOPY);
				}
			}
		}
		else
			BitBlt(ai->ai_Oldhdc,ai->ai_rcClient.left,ai->ai_rcClient.top,w,h,ai->ai_hdc,ai->ai_rcClient.left, ai->ai_rcClient.top, SRCCOPY);
	}
}

void DcxControl::showError(const TCHAR *const prop, const TCHAR *const cmd, const TCHAR *const err) const
{
	if (m_pParentDialog->IsVerbose()) {
		//TString res;
		//if (prop != nullptr)
		//	res.tsprintf(TEXT("D_IERROR %s(%s, %u).%s: %s"), getType().to_chr(), m_pParentDialog->getName().to_chr(), getUserID(), prop, err);
		//else
		//	res.tsprintf(TEXT("D_CERROR (%s) xdid %s %s %u: %s"), getType().to_chr(), cmd, m_pParentDialog->getName().to_chr(), getUserID(), err);		
		//Dcx::error(cmd, res.to_chr());

		TString res;
		if (prop != nullptr)
			_ts_sprintf(res, TEXT("D_IERROR %(%, %).%: %"), getType(), m_pParentDialog->getName(), getUserID(), prop, err);
		else
			_ts_sprintf(res, TEXT("D_CERROR (%) xdid % % %: %"), getType(), cmd, m_pParentDialog->getName(), getUserID(), err);
		Dcx::error(cmd, res.to_chr());
	}

	if (!m_pParentDialog->getAliasName().empty())
		const_cast<DcxControl *>(this)->execAliasEx(TEXT("error,%u,%s,%s,%s,%s"), getUserID(), getType().to_chr(), (prop != nullptr ? prop : TEXT("none")), (cmd != nullptr ? cmd : TEXT("none")), err);
}

//void DcxControl::showErrorEx(const TCHAR *const prop, const TCHAR *const cmd, const TCHAR *const fmt, ...) const
//{
//	va_list args;
//	TString txt;
//
//	va_start( args, fmt );
//	txt.tvprintf(fmt, args );
//	va_end( args );
//
//	showError(prop, cmd, txt.to_chr());
//}

LRESULT DcxControl::CommonMessage( const UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed )
{
	LRESULT lRes = 0L;
	switch( uMsg ) {

		case WM_HELP:
			{
				if (dcx_testflag(m_pParentDialog->getEventMask(), DCX_EVENT_HELP))
					execAliasEx(TEXT("help,%u"), getUserID( ) );
				bParsed = TRUE;
				lRes = TRUE;
			}
			break;

		case WM_SETCURSOR:
			{
				if ( (LOWORD( lParam ) == HTCLIENT) && ((HWND) wParam == m_Hwnd) && (m_hCursor != nullptr) ) {
					if (GetCursor() != m_hCursor)
						SetCursor( m_hCursor );
					bParsed = TRUE;
					return TRUE;
				}
			}
			break;

		case WM_MOUSEMOVE:
			{
				m_pParentDialog->setMouseControl( getUserID( ) );
			}
			break;

		case WM_SETFOCUS:
			{
				m_pParentDialog->setFocusControl( getUserID( ) );
			}
			break;

		case WM_CTLCOLORDLG:
			{
				bParsed = TRUE;
				return (INT_PTR) getBackClrBrush( );
			}
			break;

		case WM_CTLCOLORBTN:
		case WM_CTLCOLORLISTBOX:
		case WM_CTLCOLORSCROLLBAR:
		case WM_CTLCOLORSTATIC:
		case WM_CTLCOLOREDIT:
			{

				auto p_Control = this->m_pParentDialog->getControlByHWND((HWND)lParam);

				if ( p_Control != nullptr ) {

					const auto clrText = p_Control->getTextColor();
					const auto clrBackText = p_Control->getBackColor();
					auto hBackBrush = p_Control->getBackClrBrush();

					bParsed = TRUE;
					lRes = CallWindowProc(this->m_DefaultWindowProc, m_Hwnd, uMsg, wParam, lParam);

					if ( clrText != CLR_INVALID )
						SetTextColor( (HDC) wParam, clrText );

					if ( clrBackText != CLR_INVALID )
						SetBkColor( (HDC) wParam, clrBackText );

					if (p_Control->isExStyle(WS_EX_TRANSPARENT)) {
						// when transparent set as no bkg brush & default transparent drawing.
						SetBkMode((HDC) wParam, TRANSPARENT);
						hBackBrush = (HBRUSH)GetStockObject(HOLLOW_BRUSH);
					}

					if ( hBackBrush != nullptr )
						lRes = (LRESULT) hBackBrush;
				}
			}
			break;
		case WM_LBUTTONDOWN:
			{
				if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
					execAliasEx(TEXT("lbdown,%u"), getUserID( ) );
			}
			break;

		case WM_LBUTTONUP:
			{
				if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
				{
					execAliasEx(TEXT("lbup,%u"), getUserID( ) );
					execAliasEx(TEXT("sclick,%u"), getUserID( ) );
				}
			}
			break;

		case WM_LBUTTONDBLCLK:
			{
				if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK)) {
					execAliasEx(TEXT("dclick,%u"), getUserID( ) );
					execAliasEx(TEXT("lbdblclk,%u"), getUserID( ) );
				}
			}
			break;

		case WM_RBUTTONDOWN:
			{
				if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
					execAliasEx(TEXT("rbdown,%u"), getUserID( ) );
			}
			break;

		case WM_RBUTTONUP:
			{
				if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
					execAliasEx(TEXT("rbup,%u"), getUserID( ) );
			}
			break;

		case WM_RBUTTONDBLCLK:
			{
				if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
					execAliasEx(TEXT("rdclick,%u"), getUserID( ) );
			}
			break;

		case WM_CONTEXTMENU:
			{
				if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_CLICK))
					execAliasEx(TEXT("rclick,%u"), getUserID( ) );
				bParsed = TRUE; // stops event being passed down to parent controls
			}
			break;
		case WM_DROPFILES:
		{
			dcxwParam(HDROP, files);

			//TCHAR filename[500];
			//const auto count = DragQueryFile(files, 0xFFFFFFFF, filename, Dcx::countof(filename));
			//
			//if (count > 0) {
			//	if (dcx_testflag(m_pParentDialog->getEventMask(), DCX_EVENT_DRAG)) {
			//		TCHAR ret[20];
			//
			//		evalAliasEx(ret, Dcx::countof(ret), TEXT("dragbegin,%u,%u"), getUserID(), count);
			//
			//		// cancel drag drop event
			//		if (lstrcmpi(ret, TEXT("cancel")) == 0) {
			//			DragFinish(files);
			//			return 0L;
			//		}
			//
			//		// for each file, send callback message
			//		for (auto i = decltype(count){0}; i < count; i++) {
			//			if (DragQueryFile(files, i, filename, Dcx::countof(filename)))
			//				execAliasEx(TEXT("dragfile,%u,%s"), getUserID(), filename);
			//		}
			//
			//		execAliasEx(TEXT("dragfinish,%u"), getUserID());
			//	}
			//}
			//
			//DragFinish(files);

			stString<500> sFilename;
			const auto count = DragQueryFile(files, 0xFFFFFFFF, sFilename, sFilename.size());

			if (count > 0) {
				if (dcx_testflag(m_pParentDialog->getEventMask(), DCX_EVENT_DRAG)) {
					stString<20> sRet;

					evalAliasEx(sRet, sRet.size(), TEXT("dragbegin,%u,%u"), getUserID(), count);

					// cancel drag drop event
					if (sRet == TEXT("cancel")) {
						DragFinish(files);
						return 0L;
					}

					// for each file, send callback message
					for (auto i = decltype(count){0}; i < count; i++) {
						if (DragQueryFile(files, i, sFilename, sFilename.size()))
							execAliasEx(TEXT("dragfile,%u,%s"), getUserID(), sFilename);
					}

					execAliasEx(TEXT("dragfinish,%u"), getUserID());
				}
			}

			DragFinish(files);
			break;
		}
		case WM_NOTIFY:
			{
				dcxlParam(LPNMHDR, hdr);

				if (hdr == nullptr)
					break;

				switch( hdr->code ) {
					case TTN_GETDISPINFO:
						{
							if (!this->m_tsToolTip.empty()) {
								dcxlParam(LPNMTTDISPINFO, di);

								di->lpszText = this->m_tsToolTip.to_chr();
								di->hinst = nullptr;
								bParsed = TRUE;
							}
						}
						break;
					case TTN_LINKCLICK:
						{
							bParsed = TRUE;
							execAliasEx(TEXT("tooltiplink,%u"), getUserID( ) );
						}
						break;
					}
			}
			break;
			// Default WM_PRINTCLIENT method that handles alpha for us.
			// This Message is required for AnimateWindow() to work (also used by new transparency/alpha code)
		case WM_PRINTCLIENT:
			{
				dcxwParam(HDC, hdc);

				bParsed = TRUE;

				// Setup alpha blend if any.
				auto ai = this->SetupAlphaBlend(&hdc);

				lRes = CallWindowProc( this->m_DefaultWindowProc, m_Hwnd, uMsg, (WPARAM) hdc, lParam );

				this->FinishAlphaBlend(ai);
			}
			break;
		case WM_PRINT:
			{
				if (this->m_bInPrint) // avoid a drawing loop.
					bParsed = TRUE;
			}
			break;

		// redraw the control if the theme has changed
		case WM_THEMECHANGED:
			this->redrawWindow();
			break;

		case WM_CLOSE:
			{
				if (GetKeyState(VK_ESCAPE) != 0) // don't allow the window to close if escape is pressed. Needs looking into for a better method.
					bParsed = TRUE;
			}
			break;
		case WM_KEYDOWN:
		{
			if (dcx_testflag(m_pParentDialog->getEventMask(), DCX_EVENT_EDIT)) {
				if (wParam == VK_RETURN)
					execAliasEx(TEXT("return,%u"), getUserID());

				//if ((this->m_bIgnoreRepeat) && (lParam & 0x40000000)) // ignore repeats
				// break;

				execAliasEx(TEXT("keydown,%u,%d"), getUserID(), wParam);
			}
			break;
		}
		case WM_KEYUP:
		{
			if (dcx_testflag(m_pParentDialog->getEventMask(), DCX_EVENT_EDIT))
				execAliasEx(TEXT("keyup,%u,%d"), getUserID(), wParam);
			break;
		}

		//case WM_WINDOWPOSCHANGING:
		//{
		//	if (dcx_testflag(this->m_pParentDialog->getEventMask(), DCX_EVENT_MOVE)) {
		//		WINDOWPOS *wp = (WINDOWPOS *) lParam;
//
		//		// break if nomove & nosize specified, since thats all we care about.
		//		if ((dcx_testflag(wp->flags, SWP_NOMOVE)) && (dcx_testflag(wp->flags, SWP_NOSIZE)))
		//			break;
//
		//		TCHAR ret[256];
//
		//		this->callAliasEx(ret, "changing,%d,%d,%d,%d,%d,%d", this->getUserID(), (dcx_testflag(wp->flags, 3)),wp->x, wp->y, wp->cx, wp->cy);
//
		//		if (wp != nullptr) {
		//			if (lstrcmp(TEXT("nosize"), ret) == 0)
		//				wp->flags |= SWP_NOSIZE;
		//			else if (lstrcmp(TEXT("nomove"), ret) == 0)
		//				wp->flags |= SWP_NOMOVE;
		//			else if (lstrcmp(TEXT("nochange"), ret) == 0)
		//				wp->flags |= SWP_NOSIZE | SWP_NOMOVE;
		//		}
		//	}
		//	break;
		//}
	}
	return lRes;
}

// Invalidate controls area in parent.
void DcxControl::InvalidateParentRect(HWND hwnd)
{
	RECT rc = { 0 };
	auto parent = GetParent(hwnd);
	if (!GetWindowRect(hwnd, &rc))
		return;

	MapWindowRect(nullptr, parent, &rc);
	InvalidateRect(parent, &rc, TRUE);
}

void DcxControl::calcTextRect(HDC hdc, const TString &txt, LPRECT rc, const UINT style)
{
	auto t(txt);
	if (this->m_bCtrlCodeText)
		t.strip();
	if (this->m_bShadowText)
		dcxDrawShadowText(hdc, t.to_chr(), t.len(), rc, style | DT_CALCRECT, this->m_clrText, 0,5,5);
	else
		DrawText(hdc, t.to_chr(), (int)t.len(), rc, style | DT_CALCRECT);
}

void DcxControl::ctrlDrawText(HDC hdc, const TString &txt, const LPRECT rc, const UINT style)
{
	if (!this->m_bCtrlCodeText) {
		const auto oldBkgMode = SetBkMode(hdc, TRANSPARENT);
		Auto(SetBkMode(hdc, oldBkgMode));

		if (this->m_bShadowText)
			dcxDrawShadowText(hdc, txt.to_chr(), txt.len(), rc, style, this->m_clrText, 0, 5, 5);
		else
			DrawText(hdc, txt.to_chr(), (int)txt.len(), rc, style);
	}
	else
		mIRC_DrawText(hdc, txt, rc, style, this->m_bShadowText);
}

const TString DcxControl::getStyles(void) const
{
	TString result;
	const auto exStyles = GetWindowExStyle(m_Hwnd);
	const auto Styles = GetWindowStyle(m_Hwnd);

	if (Dcx::UXModule.dcxGetWindowTheme(m_Hwnd) == nullptr)
		result = TEXT("notheme");
	if ( dcx_testflag(Styles,WS_TABSTOP ) )
		result.addtok(TEXT("tabstop"));
	if ( dcx_testflag(Styles,WS_GROUP ) )
		result.addtok(TEXT("group"));
	if ( dcx_testflag(Styles,WS_DISABLED ) )
		result.addtok(TEXT("disabled"));
	if ( dcx_testflag(exStyles,WS_EX_TRANSPARENT ))
		result.addtok(TEXT("transparent"));
	if ( !dcx_testflag(Styles,WS_VISIBLE ))
		result.addtok(TEXT("hidden"));
	if ( this->m_bAlphaBlend )
		result.addtok(TEXT("alpha"));
	if ( this->m_bShadowText )
		result.addtok(TEXT("shadow"));
	if ( !this->m_bCtrlCodeText ) 
		result.addtok(TEXT("noformat"));
	if ( this->m_bGradientFill ) {
		if ( this->m_bGradientVertical )
			result.addtok(TEXT("vgradient"));
		else 
			result.addtok(TEXT("hgradient"));
	}
	result.addtok(TEXT("utf8"));
	return result;
}

const TString DcxControl::getBorderStyles(void) const
{
	TString bstyles;
	const auto exStyles = GetWindowExStyle(m_Hwnd);
	const auto Styles = GetWindowStyle(m_Hwnd);

	if (dcx_testflag(Styles,WS_BORDER))
		bstyles += TEXT('b');
	if (dcx_testflag(exStyles,WS_EX_CLIENTEDGE))
		bstyles += TEXT('c');
	if (dcx_testflag(Styles,WS_DLGFRAME))
		bstyles += TEXT('d');
	if (dcx_testflag(exStyles,WS_EX_DLGMODALFRAME))
		bstyles += TEXT('f');
	if (dcx_testflag(exStyles,WS_EX_STATICEDGE))
		bstyles += TEXT('s');
	if (dcx_testflag(exStyles,WS_EX_WINDOWEDGE))
		bstyles += TEXT('w');
	return bstyles;
}

void DcxControl::toXml(TiXmlElement *const xml) const
{
	const auto styles(getStyles());

	xml->SetAttribute("id", static_cast<int>(getUserID()));
	xml->SetAttribute("type", getType().c_str());
	if (!styles.empty())
		xml->SetAttribute("styles", styles.c_str());
}

TiXmlElement * DcxControl::toXml(void) const
{
	auto result = new TiXmlElement("control");
	toXml(result);
	return result;
}
