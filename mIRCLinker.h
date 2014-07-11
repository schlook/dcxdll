#pragma once
//#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
//#pragma warning( push )
//#pragma warning( disable : 2292 ) //warning #2292: destructor is declared but copy constructor and assignment operator are not
//#endif

namespace mIRCLinker
{
	extern HANDLE		m_hFileMap;		//!< Handle to the mIRC DLL File Map
	extern PTCHAR		m_pData;		//!< Pointer to a character buffer of size MIRC_BUFFER_SIZE_CCH to send mIRC custom commands
	extern HWND			m_mIRCHWND;		//!< mIRC Window Handle
	extern DWORD		m_dwVersion;
	extern int			m_iMapCnt;		//!< MapFile counter.
	extern bool			m_bDebug;		//!< is mIRC is using /debug upon DCX LoadDLL().
	extern HWND			m_hSwitchbar;	//!< The Switchbars HWND
	extern HWND			m_hToolbar;		//!< The Toolbars HWND
	extern HWND			m_hMDI;			//!< The MDIClients HWND
	extern HWND			m_hTreebar;		//!< The Treebars HWND
	extern HWND			m_hTreeview;	//!< The TreeView control child of the Treebar.
	extern HFONT		m_hTreeFont;	//!< The Treebars original font.
	extern HIMAGELIST	m_hTreeImages;	//!< The Treebars original image list.
	extern TString		m_sLastError;
	extern WNDPROC		m_wpmIRCDefaultWndProc;
	extern bool			m_bUnicodemIRC;
	extern bool			m_bSendMessageDisabled;

	void initMapFile();

	HWND &getSwitchbar();
	HWND &getToolbar();
	HWND &getTreebar();
	HWND &getTreeview();
	HIMAGELIST &getTreeImages();
	HFONT &getTreeFont();
	HWND &getMDIClient();
	HWND &getHWND();
	DWORD &getVersion();
	WORD getMainVersion();
	WORD getSubVersion();
	const TString & getLastError();
	bool setTreeFont(HFONT newFont);
	bool isOrNewerVersion(const WORD main, const WORD sub);
	bool isVersion(const WORD main, const WORD sub);
	//bool isAero();
	//bool isVista();
	bool &isDebug();
	__inline bool isUnicode() { return m_bUnicodemIRC; };

	//bool isDXInstalled9();
	bool isAlias(const TCHAR * aliasName);

	void load(LOADINFO * lInfo);
	void unload(void);

	void hookWindowProc(WNDPROC newProc);
	void resetWindowProc(void);
	LRESULT callDefaultWindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

	bool mIRC_SndMsg(const UINT uMsg);
	bool eval(TCHAR *res, const int maxlen, const TCHAR *data);
	bool tsEval(TString &res, const TCHAR *data);
	bool iEval(__int64 *res, const TCHAR *data);
	bool evalex(TCHAR *res, const int maxlen, const TCHAR *szFormat, ...);
	bool tsEvalex(TString &res, const TCHAR *szFormat, ...);
	bool exec(const TCHAR *data);
	bool execex(const TCHAR *szFormat, ...);
	void signal(const TCHAR *msg);
	void signalex(const bool allow, const TCHAR *szFormat, ...);
	void echo(const TCHAR *data);
#if DCX_DEBUG_OUTPUT
	void debug(const TCHAR *cmd, const TCHAR *msg);
#endif
};
//#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
//#pragma warning( pop )
//#endif
