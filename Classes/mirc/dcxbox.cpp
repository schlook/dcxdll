/*!
 * \file dcxbox.cpp
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
#include "dcxbox.h"
#include "Dcx.h"
#include "Classes/dcxdialog.h"

#include "Classes/layout/layoutcellfixed.h"
#include "Classes/layout/layoutcellfill.h"
#include "Classes/layout/layoutcellpane.h"



/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param mParentHwnd Parent Window Handle
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxBox::DcxBox( const UINT ID, DcxDialog * p_Dialog, const HWND mParentHwnd, const RECT * rc, const TString & styles ) 
: DcxControl( ID, p_Dialog )
, m_TitleButton(NULL)
, _hTheme(NULL)
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;

	this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

	this->m_Hwnd = CreateWindowEx(
		ExStyles | WS_EX_CONTROLPARENT,
		DCX_BOXCLASS,
		NULL,
		Styles | WS_CHILD,
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		(HMENU) ID,
		GetModuleHandle(NULL),
		NULL);

	if (!IsWindow(this->m_Hwnd))
		throw TEXT("Unable To Create Window");

	// remove all borders
	this->removeStyle( WS_BORDER|WS_DLGFRAME );
	this->removeExStyle( WS_EX_CLIENTEDGE|WS_EX_DLGMODALFRAME|WS_EX_STATICEDGE|WS_EX_WINDOWEDGE );

	if ( bNoTheme )
		Dcx::UXModule.dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );

	this->m_pLayoutManager = new LayoutManager( this->m_Hwnd );

	this->setControlFont( GetStockFont( DEFAULT_GUI_FONT ), FALSE );
	this->registreDefaultWindowProc( );
	SetProp( this->m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this );

	if (this->m_iBoxStyles & BOXS_CHECK) {
		this->m_TitleButton = CreateWindowEx(
			ExStyles,
			TEXT("BUTTON"),
			NULL,
			WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_AUTOCHECKBOX,
			CW_USEDEFAULT,CW_USEDEFAULT,11,10,
			this->m_Hwnd,
			(HMENU) ID,
			GetModuleHandle(NULL), 
			NULL);
	}
	else if (this->m_iBoxStyles & BOXS_RADIO) {
		this->m_TitleButton = CreateWindowEx(
			ExStyles,
			TEXT("BUTTON"),
			NULL,
			WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_AUTORADIOBUTTON,
			CW_USEDEFAULT,CW_USEDEFAULT,11,10,
			this->m_Hwnd,
			(HMENU) ID,
			GetModuleHandle(NULL),
			NULL);
	}
	if (IsWindow(this->m_TitleButton)) {
		if ( bNoTheme )
			Dcx::UXModule.dcxSetWindowTheme( this->m_TitleButton , L" ", L" " );
		if (!(Styles & WS_DISABLED))
			SendMessage(this->m_TitleButton,BM_SETCHECK,BST_CHECKED,0L);
	}
	if (Dcx::UXModule.isUseable())
		this->_hTheme = Dcx::UXModule.dcxOpenThemeData(this->m_Hwnd,L"BUTTON");
}

/*!
* \brief blah
*
* blah
*/

DcxBox::~DcxBox( ) {

	if ( this->m_pLayoutManager != NULL )
		delete this->m_pLayoutManager;
	if (this->_hTheme)
		Dcx::UXModule.dcxCloseThemeData(this->_hTheme);
	this->unregistreDefaultWindowProc( );
}

/*!
* \brief blah
*
* blah
*/

void DcxBox::parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme )
{
	this->m_iBoxStyles = 0;

	//const UINT numtok = styles.numtok( );
	//styles.getfirsttok( 0 );
	//for (UINT i = 1; i <= numtok; i++ )
	//{
	//	const TString tsStyle(styles.getnexttok( ));	// tok i

	//	if (tsStyle == TEXT("right"))
	//		this->m_iBoxStyles |= BOXS_RIGHT;
	//	else if (tsStyle == TEXT("center"))
	//		this->m_iBoxStyles |= BOXS_CENTER;
	//	else if (tsStyle == TEXT("bottom"))
	//		this->m_iBoxStyles |= BOXS_BOTTOM;
	//	else if (tsStyle == TEXT("none"))
	//		this->m_iBoxStyles |= BOXS_NONE;
	//	else if (tsStyle == TEXT("rounded"))
	//		this->m_iBoxStyles |= BOXS_ROUNDED;
	//	else if (tsStyle == TEXT("check")) {
	//		this->m_iBoxStyles &= ~BOXS_RADIO;
	//		this->m_iBoxStyles |= BOXS_CHECK;
	//	}
	//	else if (tsStyle == TEXT("radio")) {
	//		this->m_iBoxStyles &= ~BOXS_CHECK;
	//		this->m_iBoxStyles |= BOXS_RADIO;
	//	}
	//	else if (tsStyle == TEXT("transparent"))
	//		*ExStyles |= WS_EX_TRANSPARENT;
	//}

	for (TString tsStyle(styles.getfirsttok( 1 )); tsStyle != TEXT(""); tsStyle = styles.getnexttok( ))
	{
		if (tsStyle == TEXT("right"))
			this->m_iBoxStyles |= BOXS_RIGHT;
		else if (tsStyle == TEXT("center"))
			this->m_iBoxStyles |= BOXS_CENTER;
		else if (tsStyle == TEXT("bottom"))
			this->m_iBoxStyles |= BOXS_BOTTOM;
		else if (tsStyle == TEXT("none"))
			this->m_iBoxStyles |= BOXS_NONE;
		else if (tsStyle == TEXT("rounded"))
			this->m_iBoxStyles |= BOXS_ROUNDED;
		else if (tsStyle == TEXT("check")) {
			this->m_iBoxStyles &= ~BOXS_RADIO;
			this->m_iBoxStyles |= BOXS_CHECK;
		}
		else if (tsStyle == TEXT("radio")) {
			this->m_iBoxStyles &= ~BOXS_CHECK;
			this->m_iBoxStyles |= BOXS_RADIO;
		}
		else if (tsStyle == TEXT("transparent"))
			*ExStyles |= WS_EX_TRANSPARENT;
	}

	this->parseGeneralControlStyles( styles, Styles, ExStyles, bNoTheme );
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */

void DcxBox::parseInfoRequest( const TString & input, PTCHAR szReturnValue ) const
{
	//  unsigned int numtok = input.numtok( );

	const TString prop(input.getfirsttok( 3 ));

	// [NAME] [ID] [PROP]
	if ( prop == TEXT("text") ) {

		GetWindowText( this->m_Hwnd, szReturnValue, MIRC_BUFFER_SIZE_CCH );
		return;
	}
	else if ( prop == TEXT("inbox") ) {

		RECT rc;
		GetClientRect( this->m_Hwnd, &rc );

		InflateRect( &rc, -2, -2 );
		if ( GetWindowTextLength( this->m_Hwnd ) > 0 ) {

			HDC hdc = GetDC( this->m_Hwnd );
			HFONT oldFont = NULL;
			RECT rcText = rc;

			if (this->m_hFont != NULL)
				oldFont = SelectFont(hdc, this->m_hFont);

			TString text;
			TGetWindowText( this->m_Hwnd, text );
			DrawText( hdc, text.to_chr(), text.len(), &rcText, DT_CALCRECT );

			if (oldFont != NULL)
				SelectFont(hdc, oldFont);

			ReleaseDC( this->m_Hwnd, hdc );

			//const int w = rcText.right - rcText.left;
			const int h = rcText.bottom - rcText.top;

			if ( this->m_iBoxStyles & BOXS_BOTTOM )
				rc.bottom -= (h + 2);
			else
				rc.top += (h - 2);
		}

		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d %d %d"), rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top );
		return;
	}
	else if ( this->parseGlobalInfoRequest( input, szReturnValue ) )
		return;

	szReturnValue[0] = 0;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxBox::parseCommandRequest( const TString & input ) {

	const XSwitchFlags flags(input.getfirsttok(3));
	const unsigned int numtok = input.numtok( );

	// xdid -c [NAME] [ID] [SWITCH] [ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)
	if ( flags[TEXT('c')] && numtok > 8 ) {

		const UINT ID = mIRC_ID_OFFSET + (UINT)input.getnexttok( ).to_int( );	// tok 4

		if ( (ID > mIRC_ID_OFFSET - 1) && 
			!IsWindow( GetDlgItem( this->m_pParentDialog->getHwnd( ), ID ) ) && 
			this->m_pParentDialog->getControlByID( ID ) == NULL ) 
		{
			try {
				DcxControl * p_Control = DcxControl::controlFactory(this->m_pParentDialog,ID,input,5,CTLF_ALLOW_ALL,this->m_Hwnd);

				if ( p_Control != NULL ) {
					this->m_pParentDialog->addControl( p_Control );
					this->redrawWindow( );
				}
			}
			catch ( TCHAR *err ) {
				this->showErrorEx(NULL, TEXT("-c"), TEXT("Unable To Create Control %d (%s)"), ID - mIRC_ID_OFFSET, err);
			}
		}
		else
			this->showErrorEx(NULL,TEXT("-c"), TEXT("Control with ID \"%d\" already exists"), ID - mIRC_ID_OFFSET );
	}
	// xdid -d [NAME] [ID] [SWITCH] [ID]
	else if ( flags[TEXT('d')] && numtok > 3 ) {

		const UINT ID = mIRC_ID_OFFSET + input.getnexttok( ).to_int( );	// tok 4
		DcxControl * p_Control;

		if ( IsWindow( GetDlgItem( this->m_Hwnd, ID ) ) && 
			(ID > mIRC_ID_OFFSET - 1) &&
			( p_Control = this->m_pParentDialog->getControlByID( ID ) ) != NULL ) 
		{
			HWND cHwnd = p_Control->getHwnd( );
			if ( p_Control->getType( ) == TEXT("dialog") || p_Control->getType( ) == TEXT("window") )
				delete p_Control;
			else if ( p_Control->getRefCount( ) == 0 ) {
				this->m_pParentDialog->deleteControl( p_Control ); // remove from internal list!
				DestroyWindow( cHwnd );
			}
			else
				this->showErrorEx(NULL,TEXT("-d"), TEXT("Can't delete control with ID \"%d\" when it is inside it's own event (dialog %s)"), p_Control->getUserID( ), this->m_pParentDialog->getName( ).to_chr( ) );
		}
		else
			this->showErrorEx(NULL,TEXT("-d"), TEXT("Unknown control with ID \"%d\" (dialog %s)"), ID - mIRC_ID_OFFSET, this->m_pParentDialog->getName( ).to_chr( ) );
	}
	/*
	//xdid -l [NAME] [ID] [SWITCH] [OPTIONS]

	[OPTIONS] :

	root [TAB]+flpiw [ID] [WEIGHT] [W] [H]
	add PATH[TAB]+flpiw [ID] [WEIGHT] [W] [H]
	space PATH[TAB]+ [L] [T] [R] [B]
	*/
	else if ( flags[TEXT('l')] && numtok > 3 ) {

		const TString tsCmd(input.getnexttok( ));	// tok 4

		if ( tsCmd == TEXT("update") ) {
			if ( this->m_pLayoutManager != NULL ) {
				RECT rc;
				GetClientRect( this->m_Hwnd, &rc );
				this->m_pLayoutManager->updateLayout( rc );
				this->redrawWindow();
			}
		}
		else if (tsCmd == TEXT("clear")) {
			if (this->m_pLayoutManager != NULL)
				delete this->m_pLayoutManager;
			this->m_pLayoutManager = new LayoutManager(this->m_Hwnd);
			//this->redrawWindow(); // dont redraw here, leave that for an `update` cmd
		}
		else if ( numtok > 8 ) {

			const TString tsInput(input.getfirsttok(1, TSTAB));
			const TString p2(input.getnexttok( TSTAB ).trim());	// tok 2

			const TString com(tsInput.gettok(4).trim());
			const TString path(tsInput.gettok(5, -1).trim());

			const UINT lflags = this->parseLayoutFlags( p2.getfirsttok( 1 ) );
			const UINT ID = p2.getnexttok( ).to_int( );		// tok 2
			const UINT WGT = p2.getnexttok( ).to_int( );	// tok 3
			const UINT W = p2.getnexttok( ).to_int( );		// tok 4
			const UINT H = p2.getnexttok( ).to_int( );		// tok 5

			if ( com ==  TEXT("root") || com == TEXT("cell") ) {

				HWND cHwnd = GetDlgItem( this->m_Hwnd, mIRC_ID_OFFSET + ID );

				LayoutCell * p_Cell = NULL;

				// LayoutCellPane
				if ( lflags & LAYOUTPANE ) {

					if ( lflags & LAYOUTHORZ )
						p_Cell = new LayoutCellPane( LayoutCellPane::HORZ );
					else
						p_Cell = new LayoutCellPane( LayoutCellPane::VERT );
				} // if ( lflags & LAYOUTPANE )
				// LayoutFill Cell
				else if ( lflags & LAYOUTFILL ) {
					if ( lflags & LAYOUTID ) {
						if ( cHwnd != NULL && IsWindow( cHwnd ) )
							p_Cell = new LayoutCellFill( cHwnd );
						else {
							this->showErrorEx(NULL,TEXT("-l"), TEXT("Cell Fill -> Invalid ID : %d"), ID );
							return;
						}
					}
					else {
						p_Cell = new LayoutCellFill( );
					}
				} // else if ( lflags & LAYOUTFILL )
				// LayoutCellFixed
				else if ( lflags & LAYOUTFIXED ) {

					LayoutCellFixed::FixedType type;

					if ( lflags & LAYOUTVERT && lflags & LAYOUTHORZ )
						type = LayoutCellFixed::BOTH;
					else if ( lflags & LAYOUTVERT )
						type = LayoutCellFixed::HEIGHT;
					else
						type = LayoutCellFixed::WIDTH;

					// Defined Rectangle
					if ( lflags & LAYOUTDIM ) {

						RECT rc;
						SetRect( &rc, 0, 0, W, H );

						if ( lflags & LAYOUTID ) {

							if ( cHwnd != NULL && IsWindow( cHwnd ) )
								p_Cell = new LayoutCellFixed( cHwnd, rc, type );
							else {
								this->showErrorEx(NULL,TEXT("-l"), TEXT("Cell Fixed -> Invalid ID : %d"), ID );
								return;
							}
						}
						else
							p_Cell = new LayoutCellFixed( rc, type );

					}
					// No defined Rectangle
					else {

						if ( lflags & LAYOUTID ) {

							if ( cHwnd != NULL && IsWindow( cHwnd ) )
								p_Cell = new LayoutCellFixed( cHwnd, type );
							else {
								this->showErrorEx(NULL,TEXT("-l"), TEXT("Cell Fixed -> Invalid ID : %d"), ID );
								return;
							}
						}
					} //else
				} // else if ( lflags & LAYOUTFIXED )
				else {
					this->showError(NULL,TEXT("-l"), TEXT("Unknown Cell Type"));
					return;
				}

				if ( com == TEXT("root") ) {

					if ( p_Cell != NULL )
						this->m_pLayoutManager->setRoot( p_Cell );

				} // if ( com == TEXT("root") )
				else if ( com == TEXT("cell") ) {

					if ( p_Cell != NULL ) {

						LayoutCell * p_GetCell;

						if ( path == TEXT("root") )
							p_GetCell = this->m_pLayoutManager->getRoot( );
						else
							p_GetCell = this->m_pLayoutManager->getCell( path );

						if ( p_GetCell == NULL ) {
							this->showErrorEx(NULL,TEXT("-l"), TEXT("Invalid item path: %s"), path.to_chr( ) );
							delete p_Cell;
							return;
						}

						if ( p_GetCell->getType( ) == LayoutCell::PANE ) {

							LayoutCellPane * p_PaneCell = (LayoutCellPane *) p_GetCell;
							p_PaneCell->addChild( p_Cell, WGT );
						}
					}
				} // else if ( com == TEXT("cell") )
			} // if ( com ==  TEXT("root") || com == TEXT("cell") )
			else if ( com ==  TEXT("space") ) {

				LayoutCell * p_GetCell;

				if ( path == TEXT("root") )
					p_GetCell = this->m_pLayoutManager->getRoot( );
				else
					p_GetCell = this->m_pLayoutManager->getCell( path );

				if ( p_GetCell != NULL ) {

					RECT rc;
					SetRect( &rc, ID, WGT, W, H );
					p_GetCell->setBorder( rc );
				}
			} // else if ( com == TEXT("space") )
		} // if ( numtok > 7 )
	}
	//xdid -t [NAME] [ID] [SWITCH]
	else if ( flags[TEXT('t')] ) {
		SetWindowText(this->m_Hwnd, input.gettok(4, -1).trim().to_chr());
		this->redrawWindow( );
	}
	else
		this->parseGlobalCommandRequest( input, flags );
}

/*!
 * \brief blah
 *
 * blah
 */

UINT DcxBox::parseLayoutFlags( const TString & flags ) {

	const XSwitchFlags xflags(flags);
	UINT iFlags = 0;

	// no +sign, missing params
	if ( !xflags[TEXT('+')] ) 
		return iFlags;

	if ( xflags[TEXT('f')] )
		iFlags |= LAYOUTFIXED;
	if ( xflags[TEXT('h')] )
		iFlags |= LAYOUTHORZ;
	if ( xflags[TEXT('i')] )
		iFlags |= LAYOUTID;
	if ( xflags[TEXT('l')] )
		iFlags |= LAYOUTFILL ;
	if ( xflags[TEXT('p')] )
		iFlags |= LAYOUTPANE;
	if ( xflags[TEXT('v')] )
		iFlags |= LAYOUTVERT;
	if ( xflags[TEXT('w')] )
		iFlags |= LAYOUTDIM;

	return iFlags;
}
BOOL CALLBACK DcxBox::EnumBoxChildren(HWND hwnd,LPDCXENUM de)
{
	//LPDCXENUM de = (LPDCXENUM)lParam;
	if ((de->mChildHwnd != hwnd) && (GetParent(hwnd) == de->mBox))
		EnableWindow(hwnd,de->mState);

	return TRUE;
}

void DcxBox::toXml(TiXmlElement * xml) const {
	TString wtext;
	TGetWindowText(this->m_Hwnd, wtext);
	__super::toXml(xml);
	xml->SetAttribute("caption", wtext.c_str());
	this->m_pLayoutManager->getRoot()->toXml(xml);
}

TString DcxBox::getStyles(void) const {
	TString result(__super::getStyles());
	if (this->m_iBoxStyles & BOXS_RIGHT)
		result.addtok(TEXT("right"));
	if (this->m_iBoxStyles & BOXS_CENTER)
		result.addtok(TEXT("center"));
	if (this->m_iBoxStyles & BOXS_BOTTOM)
		result.addtok(TEXT("bottom"));
	if (this->m_iBoxStyles & BOXS_NONE)
		result.addtok(TEXT("none"));
	if (this->m_iBoxStyles & BOXS_ROUNDED)
		result.addtok(TEXT("rounded"));
	if (this->m_iBoxStyles & BOXS_CHECK)
		result.addtok(TEXT("check"));
	else if (this->m_iBoxStyles & BOXS_RADIO)
		result.addtok(TEXT("radio"));

	return result;
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxBox::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
	return 0L;
}

LRESULT DcxBox::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

	LRESULT lRes = 0L;
	switch( uMsg ) {

	case WM_NOTIFY:
		{
			LPNMHDR hdr = (LPNMHDR) lParam;

			if (!hdr)
				break;

			if (IsWindow(hdr->hwndFrom)) {
				DcxControl *c_this = (DcxControl *) GetProp(hdr->hwndFrom,TEXT("dcx_cthis"));
				if (c_this != NULL)
					lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
			}
		}
		break;

    case WM_COMMAND:
			{
				if (((HWND)lParam != NULL) && ((HWND)lParam == this->m_TitleButton) ) {
					switch ( HIWORD( wParam ) )
					{
						case BN_CLICKED:
						{
							const BOOL state = (SendMessage(this->m_TitleButton,BM_GETCHECK,0,0) == BST_CHECKED);
							if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK) {
								TCHAR ret[10];

								this->evalAliasEx(ret, 9, TEXT("%s,%d,%d"), TEXT("checkchange"), this->getUserID(), state);

								if (lstrcmp(TEXT("nochange"), ret) == 0)
									return 0L;
							}

							DCXENUM de;
							de.mChildHwnd = this->m_TitleButton;
							de.mBox = this->m_Hwnd;
							de.mState = state;

							EnumChildWindows(this->m_Hwnd,(WNDENUMPROC)DcxBox::EnumBoxChildren,(LPARAM)&de);
							break;
						}
					} // end switch
					break;
				}
			}
		case WM_HSCROLL:
		case WM_VSCROLL:
			{
				if (lParam == NULL)
					break;

				if (IsWindow((HWND) lParam)) {
					DcxControl *c_this = (DcxControl *) GetProp((HWND) lParam,TEXT("dcx_cthis"));
					if (c_this != NULL)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
			break;

		case WM_COMPAREITEM:
			{
				LPCOMPAREITEMSTRUCT idata = (LPCOMPAREITEMSTRUCT)lParam;
				if ((idata != NULL) && (IsWindow(idata->hwndItem))) {
					DcxControl *c_this = (DcxControl *) GetProp(idata->hwndItem,TEXT("dcx_cthis"));
					if (c_this != NULL)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
			break;

		case WM_DELETEITEM:
			{
				DELETEITEMSTRUCT *idata = (DELETEITEMSTRUCT *)lParam;
				if ((idata != NULL) && (IsWindow(idata->hwndItem))) {
					DcxControl *c_this = (DcxControl *) GetProp(idata->hwndItem,TEXT("dcx_cthis"));
					if (c_this != NULL)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
			break;

		case WM_MEASUREITEM:
			{
				HWND cHwnd = GetDlgItem(this->m_Hwnd, wParam);
				if (IsWindow(cHwnd)) {
					DcxControl *c_this = (DcxControl *) GetProp(cHwnd,TEXT("dcx_cthis"));
					if (c_this != NULL)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
			break;

		case WM_DRAWITEM:
			{
				DRAWITEMSTRUCT *idata = (DRAWITEMSTRUCT *)lParam;
				if ((idata != NULL) && (IsWindow(idata->hwndItem))) {
					DcxControl *c_this = (DcxControl *) GetProp(idata->hwndItem,TEXT("dcx_cthis"));
					if (c_this != NULL)
						lRes = c_this->ParentMessage(uMsg, wParam, lParam, bParsed);
				}
			}
			break;

		case WM_SIZE:
			{

				HWND bars = NULL;

				while ( ( bars = FindWindowEx( this->m_Hwnd, bars, DCX_REBARCTRLCLASS, NULL ) ) != NULL ) {

					SendMessage( bars, WM_SIZE, (WPARAM) 0, (LPARAM) 0 );
				}

				while ( ( bars = FindWindowEx( this->m_Hwnd, bars, DCX_STATUSBARCLASS, NULL ) ) != NULL ) {

					SendMessage( bars, WM_SIZE, (WPARAM) 0, (LPARAM) 0 );
				}

				while ( ( bars = FindWindowEx( this->m_Hwnd, bars, DCX_TOOLBARCLASS, NULL ) ) != NULL ) {

					SendMessage( bars, WM_SIZE, (WPARAM) 0, (LPARAM) 0 );
				}

				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_SIZE)
					this->execAliasEx(TEXT("%s,%d"), TEXT("sizing"), this->getUserID( ) );

				//if (this->m_pLayoutManager != NULL) {
				//	RECT rc;
				//	SetRect( &rc, 0, 0, LOWORD( lParam ), HIWORD( lParam ) );
				//	if (this->m_pLayoutManager->updateLayout( rc ))
				//		this->redrawWindow( );
				//}
			}
			break;
		case WM_WINDOWPOSCHANGING:
			{
				if (lParam != NULL) {
					WINDOWPOS * wp = (WINDOWPOS *) lParam;
					if (this->m_pLayoutManager != NULL) {
						RECT rc;
						SetRect( &rc, 0, 0, wp->cx, wp->cy );
						this->m_pLayoutManager->updateLayout( rc );
					}
				}
			}
			break;

		case WM_ENABLE: {
			this->redrawWindow();
			break;
		}


		case WM_ERASEBKGND:
		{
			this->EraseBackground((HDC)wParam);
			bParsed = TRUE;
			return TRUE;
		}
		break;

		case WM_PRINTCLIENT:
			this->DrawClientArea((HDC)wParam);
			bParsed = TRUE;
			break;

		case WM_PAINT:
			{
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(this->m_Hwnd, &ps);

				this->DrawClientArea(hdc);

				EndPaint(this->m_Hwnd, &ps);

				bParsed = TRUE;
				return 0L;
			}
			break;

		case WM_THEMECHANGED:
			{
				if (this->_hTheme != NULL) {
					Dcx::UXModule.dcxCloseThemeData(this->_hTheme);
					this->_hTheme = Dcx::UXModule.dcxOpenThemeData(this->m_Hwnd,L"BUTTON");
				}
			}
			break;
		case WM_DESTROY:
			{
				delete this;
				bParsed = TRUE;
			}
			break;

		default:
			lRes = this->CommonMessage( uMsg, wParam, lParam, bParsed);
			break;
	}

	return lRes;
}
void DcxBox::EraseBackground(HDC hdc)
{
	RECT rc;
	GetClientRect( this->m_Hwnd, &rc );
	// fill background.
	if (this->isExStyle(WS_EX_TRANSPARENT) || this->m_pParentDialog->isExStyle(WS_EX_COMPOSITED))
		this->DrawParentsBackground(hdc, &rc);
	else // normal bkg
		DcxControl::DrawCtrlBackground(hdc, this, &rc);
	// Update CLA if any.
	if (this->m_pLayoutManager != NULL)
		this->m_pLayoutManager->updateLayout( rc );
}

void DcxBox::DrawClientArea(HDC hdc)
{
	RECT rc, rc2, rcText, rcText2;
	TString wtext;
	const int n = TGetWindowText(this->m_Hwnd, wtext);

	GetClientRect(this->m_Hwnd, &rc);

	// Setup alpha blend if any.
	LPALPHAINFO ai = this->SetupAlphaBlend(&hdc,true);

	// if no border, dont bother
	if (this->m_iBoxStyles & BOXS_NONE) {
		DcxControl::DrawCtrlBackground(hdc, this, &rc);
		this->FinishAlphaBlend(ai);
		return;
	}

	CopyRect(&rc2, &rc);

	SetBkMode(hdc, TRANSPARENT);

	// no text, no box!
	if (n == 0) {
		if (this->m_iBoxStyles & BOXS_ROUNDED) {
			HRGN m_Region = CreateRoundRectRgn(rc2.left, rc2.top, rc2.right, rc2.bottom, 10, 10);
			if (m_Region) {
				SelectClipRgn(hdc,m_Region);
				DcxControl::DrawCtrlBackground(hdc, this, &rc2);
				SelectClipRgn(hdc,NULL);

				HBRUSH hBorderBrush = this->m_hBorderBrush;
				if (hBorderBrush == NULL)
					hBorderBrush = GetStockBrush(BLACK_BRUSH);

				FrameRgn(hdc,m_Region,hBorderBrush,1,1);
				DeleteRgn(m_Region);
			}
		}
		else {
			DcxControl::DrawCtrlBackground(hdc, this, &rc2);
			DrawEdge(hdc, &rc2, EDGE_ETCHED, BF_RECT);
		}
		if (IsWindow(this->m_TitleButton))
			SetWindowPos(this->m_TitleButton,NULL,rc2.left,rc2.top,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
	}
	// draw text
	else {
		// prepare for appearance
		if (this->m_hFont != NULL)
			SelectFont(hdc, this->m_hFont);

		if (this->m_clrText != CLR_INVALID)
			SetTextColor(hdc, this->m_clrText);
		else
			SetTextColor(hdc, GetSysColor(IsWindowEnabled(this->m_Hwnd) ? COLOR_WINDOWTEXT : COLOR_GRAYTEXT));

		CopyRect(&rcText, &rc); // MUST initialize rect first!.

		this->calcTextRect(hdc, wtext, &rcText, DT_LEFT | DT_END_ELLIPSIS |DT_SINGLELINE);
		if (this->m_bShadowText) {
			rcText.bottom = min((rcText.bottom +6), rc.bottom);
			rcText.right = min((rcText.right +6), rc.right);
		}

		const int w = rcText.right - rcText.left;
		const int h = rcText.bottom - rcText.top;

		// shift border and text locations
		// text at bottom?
		if (this->m_iBoxStyles & BOXS_BOTTOM) {
			rcText.top = rc2.bottom - h;
			rc2.bottom -= h/2;
		}
		// text at top
		else {
			rcText.top = rc2.top;
			rc2.top += h/2;
		}

		// text location
		rcText.bottom = rcText.top + h;

		// align text horizontally
		const int bw = rc.right - rc.left - (2 * DCX_BOXTEXTSPACING);

		if (w > bw) {
			rcText.left = rc.left + DCX_BOXTEXTSPACING;
			rcText.right = rc.right - DCX_BOXTEXTSPACING;
		}
		else {
			if (this->m_iBoxStyles & BOXS_RIGHT)
				rcText.left = rc.right - DCX_BOXTEXTSPACING - w;
			else if (this->m_iBoxStyles & BOXS_CENTER)
				rcText.left = (rc.left + rc.right - w) /2;
			else
				rcText.left = rc.left + DCX_BOXTEXTSPACING;

			rcText.right = rcText.left + w;
		}

		// clear some space for the text
		CopyRect(&rcText2, &rcText);
		InflateRect(&rcText2, 3, 0);

		if (IsWindow(this->m_TitleButton))
		{
			RECT bSZ;
			GetWindowRect(this->m_TitleButton,&bSZ);
			bSZ.bottom = (bSZ.right - bSZ.left);
			SetWindowPos(this->m_TitleButton,NULL,rcText2.left,rcText2.top,bSZ.bottom,(rcText2.bottom - rcText2.top),SWP_NOZORDER|SWP_NOACTIVATE);
			rcText.left += bSZ.bottom;
			rcText.right += bSZ.bottom;
			rcText2.left += bSZ.bottom;
			rcText2.right += bSZ.bottom;
		}

		// draw the border
		if (this->m_iBoxStyles & BOXS_ROUNDED) {
			HRGN m_Region = CreateRoundRectRgn(rc2.left, rc2.top, rc2.right, rc2.bottom, 10, 10);
			if (m_Region) {
				SelectClipRgn(hdc,m_Region);
				DcxControl::DrawCtrlBackground(hdc, this, &rc2);
				SelectClipRgn(hdc,NULL);
				ExcludeClipRect(hdc, rcText2.left, rcText2.top, rcText2.right, rcText2.bottom);

				HBRUSH hBorderBrush = this->m_hBorderBrush;
				if (hBorderBrush == NULL)
					hBorderBrush = GetStockBrush(BLACK_BRUSH);

				FrameRgn(hdc,m_Region,hBorderBrush,1,1);
				DeleteRgn(m_Region);
			}
		}
		else {
			DcxControl::DrawCtrlBackground(hdc, this, &rc2);
			ExcludeClipRect(hdc, rcText2.left, rcText2.top, rcText2.right, rcText2.bottom);
			DrawEdge(hdc, &rc2, EDGE_ETCHED, BF_RECT);
		}
		SelectClipRgn(hdc,NULL);

		// draw the text
		this->ctrlDrawText(hdc, wtext, &rcText, DT_LEFT | DT_END_ELLIPSIS |DT_SINGLELINE);
	}

	this->FinishAlphaBlend(ai);
}
