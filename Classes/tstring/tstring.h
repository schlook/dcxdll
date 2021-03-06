/*!
 * \file tstring.h
 * \brief String and Token Management Class
 *
 * This experimental library was built to reproduce easy to use string containers managing effectively 
 * comparisons and token manipulations as done in the mIRC scripting language.
 *
 * \author David Legault ( clickhere at scriptsdb dot org )
 * \version 1.12
 *
 * \b Revisions
 *	1.1
 *		Fixed some mem leaks. Ook
 *	1.2
 *		Added left() mid() right() functions. Ook
 *		Added wildtok() nwildtok() functions. Ook
 *	1.3
 *		Changed += operators to only need one lstrcpy(). Ook
 *	1.4
 *		Changed *= operator to only need one new call. Ook
 *	1.5
 *		Added replace() functions. Ook
 *		Added _replace() function for use by the replace() functions. Ook
 *		Changed token functions to only call lstrlen( sepChars ) once. Ook
 *		Changed find() to only call lstrlen( substring ) once. Ook
 *		Added istok() function. Ook
 *	1.6
 *		Added addtok() function. Ook
 *		Added Visual Studio 2005 specific defines. Ook
 *	1.7
 *		Added sprintf() function. Ook
 *	1.8
 *		Added iswm() & iswmcs(). Ook
 *	1.9
 *		More changes & shit than i can remember. Ook
 *	1.10
 *		Fixed a bug in charToWchar() that caused the conversion to always fail.
 *	1.11
 *		Added strip() function.
 *	1.12
 *		changed gettok() to not copy the contents to token first.
 *		changed findtok() to take an unsigned int arg.
 *		changed deltok() to take an unsigned int arg.
 *		changed instok() to take an unsigned arg.
 *		and loads more...
 *
 * � ScriptsDB.org - 2013
 */
#if _MSC_VER > 1000
#pragma once
#endif
#ifndef _TSTRING_H_
#define _TSTRING_H_

// Required for VS 2005
//#define _CRT_SECURE_NO_DEPRECATE 1
// end VS2005

#include <stdio.h>
#include <stdlib.h>
//#include <string.h>
#define SPACE m_cSpace
#define COMMA m_cComma
#define TAB m_cTab

#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( push )
#pragma warning( disable : 2304 )
#pragma warning( disable : 2287 )
#endif

#ifdef UNICODE
#define ts_upr(x,y) _wcsupr_s((x),(y))
#define ts_strstr(x,y) wcsstr((x),(y))
#define ts_strcat_s(x,y,z) wcscat_s((x),(z),(y))
#define ts_strcat(x,y) lstrcat((x),(y))
#define ts_strncat(x,y,z) wcsncat((x),(y),(z))
#define ts_toupper(c) ((((c) >= TEXT('a')) && ((c) <= TEXT('z'))) ? _toupper((c)) : (c) )
#define ts_strlen(x) lstrlen((x))
#define ts_strcpyn(dest,src,len) lstrcpyn((dest),(src),(len))
#define ts_strcpy(dest,src) lstrcpy((dest),(src))
#define ts_strcmp(x,y) lstrcmp((x),(y))
#define ts_strncmp(x,y,z) wcsncmp((x),(y),(z))
#define ts_vscprintf(fmt, args) _vscwprintf((fmt), (args))
#define ts_vsprintf(txt, cnt, fmt, args ) vswprintf((txt), (cnt), (fmt), (args))
#else
#define ts_upr(x,y) _strupr_s((x),(y))
#define ts_strstr(x,y) strstr((x),(y))
#define ts_strcat_s(x,y,z) strcat_s((x),(z),(y))
#define ts_strcat(x,y) lstrcat((x),(y))
#define ts_strncat(x,y,z) strncat((x),(y),(z))
//#define ts_toupper(c) ( (((c) >= TEXT('a')) && ((c) <= TEXT('z'))) ? _toupper((c)) : (c) )
#define ts_toupper(c) rfc_toupper((c))
#define ts_tolower(c) rfc_tolower((c))
#define ts_strlen(x) lstrlen((x))
#define ts_strcpyn(dest,src,len) lstrcpyn((dest),(src),(len))
#define ts_strcpy(dest,src) lstrcpy((dest),(src))
#define ts_strcmp(x,y) lstrcmp((x),(y))
#define ts_strncmp(x,y,z) strncmp((x),(y),(z))
#define ts_vscprintf(fmt, args) _vscprintf((fmt), (args))
#define ts_vsprintf(txt, cnt, fmt, args ) vsprintf_s((txt), (cnt), (fmt), (args))
#endif
#define TS_getmemsize(x) ((x) + (16 - ((x) % 16)))
#define TS_wgetmemsize(x) (unsigned long)((((x)*sizeof(WCHAR)) + (16 - (((x)*sizeof(WCHAR)) % 16)))/sizeof(WCHAR))

/*!
 * \brief String and Token Management Class
 */
class TString {

private:

	void deleteString( const bool bKeepBufferSize = false );
	void deleteWString(const bool bKeepBufferSize = false);
	int i_replace(const TCHAR *subString, const TCHAR *rString); // Ook
	int i_remove(const TCHAR *subString);
	static int match (register TCHAR *m, register TCHAR *n, const bool cs /* case sensitive */);

	size_t set_buffersize(const size_t size) { m_buffersize = TS_getmemsize(size); return m_buffersize; };	// make buffersize a multiple of 16
	//allocate a buffer thats sized to multiples of 16 bytes, (%byte + (16 - (%byte % 16)))
	TCHAR *allocstr_bytes(const size_t size) { return (TCHAR *)(new BYTE[set_buffersize(size)]); };
	// allocate a buffer thats size characters long & its a multiple of 16bytes.
	TCHAR *allocstr_cch(const size_t size) { return allocstr_bytes(size*sizeof(TCHAR)); };
	//TCHAR *allocstr_cch(const size_t size) { return (TCHAR *)(new BYTE[set_buffersize(size*sizeof(TCHAR))]); };

#if !UNICODE
	static unsigned char tolowertab[];
	static unsigned char touppertab[];
#endif
	static WCHAR *charToWchar(const char *cString, size_t *buffer_size = NULL);
	static char *WcharTochar(const WCHAR *wString, size_t *buffer_size = NULL);

	mutable TCHAR			*m_savedpos;
	mutable unsigned int	m_savedtotaltoks;
	mutable unsigned int	m_savedcurrenttok;
	size_t					m_buffersize;	// size of string buffer in use. (size in bytes)

public:

	TCHAR * m_pString; //!< String buffer
#ifdef UNICODE
	mutable char * m_pTempString;
#else
	mutable WCHAR * m_pTempString;
#endif
	static const TCHAR *m_cSpace;
	static const TCHAR *m_cComma;
	static const TCHAR *m_cTab;

	TString( );
	TString( const WCHAR * cString ); // we don't want these 3 as explicits
	TString( const char * cString );
	TString( const TString & tString);
	TString( const TCHAR *pStart, const TCHAR *pEnd);
	explicit TString( const WCHAR chr);
	explicit TString( const char chr );
	explicit TString( const unsigned int tsSize);

	//! Destructor
	~TString( );

	// Operator Overloads
	TString & operator =( const TString & tString );
	TString & operator =( const WCHAR * cString );
	TString & operator =( const WCHAR chr );
	TString & operator =( const char * cString );
	TString & operator =( const char chr );

	TString operator +( const TCHAR * cString );
	TString operator +( const TCHAR chr );
	TString operator +( const TString & tString );

	TString operator -( const TCHAR * cString );
	TString operator -( const TCHAR chr );
	TString operator -( const TString & tString );

	TString & operator +=( const TCHAR * cString );
	TString & operator +=( const TCHAR chr );
	TString & operator +=( const TString & tString );

	TString & operator -=( const TCHAR * cString );
	TString & operator -=( const TCHAR chr );
	TString & operator -=( const TString & tString );

	bool operator ==( const int iNull ) const;
	bool operator ==( const TCHAR * cString ) const;
	bool operator ==( const TCHAR chr ) const;
	bool operator ==( const TString & tString ) const;

	bool operator !=( const int iNull ) const;
	bool operator !=( const TCHAR * cString ) const;
	bool operator !=( const TCHAR chr ) const;
	bool operator !=( const TString & tString ) const;

	bool operator >( const TCHAR * cString ) const;
	bool operator >( const TCHAR chr ) const;
	bool operator >( const TString & tString ) const;

	bool operator >=( const TCHAR * cString ) const;
	bool operator >=( const TCHAR chr ) const;
	bool operator >=( const TString & tString ) const;

	bool operator <( const TCHAR * cString ) const;
	bool operator <( const TCHAR chr ) const;
	bool operator <( const TString & tString ) const;

	bool operator <=( const TCHAR * cString ) const;
	bool operator <=( const TCHAR chr ) const;
	bool operator <=( const TString & tString ) const;

	TString operator *( const int &N );
	TString & operator *=( const int &N );

	TCHAR & operator []( long int N ) const;
	operator int() { return this->to_int(); }
	operator __int64() { return this->to_num(); }
	operator double() { return this->to_float(); }

	// General String Lib
	size_t len( ) const;
	size_t size() const { return m_buffersize; };

	int find( const TCHAR * substring, const int N ) const;
	int find( const TCHAR chr, const int N ) const;

	TString sub( int N, int M ) const;

	TString &trim();	// removes spaces at start & end of text.
	TString &strip();	// removes spaces at start & end of text & all ctrl codes in text.

	int replace( const TCHAR * subString, const TCHAR * rString );
	int replace( const TCHAR * subString, const TCHAR rchr );
	int replace( const TCHAR chr, const TCHAR * rString );
	int replace( const TCHAR chr, const TCHAR rchr );
	int mreplace(const TCHAR chr, const TCHAR *rchr);

	// Token Lib
	void addtok( const TCHAR * cToken, const TCHAR * sepChars = SPACE );
	void addtok( const __int64 nToken, const TCHAR * sepChars = SPACE );
	//void addtok( const TString &cToken, const TCHAR * sepChars = SPACE );
	void deltok( const unsigned int N, const TCHAR * sepChars = SPACE );
	size_t findtok( const TCHAR * cToken, const unsigned int N, const TCHAR * sepChars = SPACE ) const;
	TString gettok( int N, const TCHAR * sepChars = SPACE ) const;
	TString gettok( int N, int M, const TCHAR * sepChars = SPACE ) const;
	TString getfirsttok( const unsigned int N, const TCHAR * sepChars = SPACE ) const;	// must becalled before the first getnexttok()
	TString getnexttok( const TCHAR * sepChars = SPACE ) const;							// gets subsequent tokens after a getfirsttok() call.
	void instok( const TCHAR * cToken, const unsigned int N, const TCHAR * sepChars = SPACE );
	bool istok( const TCHAR * cToken, const TCHAR * sepChars = SPACE ) const;
	TString matchtok( TCHAR * mString, int N, const TCHAR * sepChars = SPACE ) const;
	unsigned int numtok( const TCHAR * sepChars = SPACE ) const;
	void puttok( const TCHAR * cToken, int N, const TCHAR * sepChars = SPACE );
	void remtok( const TCHAR * cToken, int N, const TCHAR * sepChars = SPACE );
	void reptok( const TCHAR * cToken, const TCHAR * newToken, int N, const TCHAR * sepChars = SPACE );
	void sorttok( TCHAR * sortOptions, const TCHAR * sepChars = SPACE );
	// added by Ook
	TString wildtok( TCHAR * wildString, int N, const TCHAR * sepChars = SPACE ) const;
	int nwildtok( TCHAR * wildString, const TCHAR * sepChars = SPACE ) const;

	// extract left/right/mid
	TString mid(const int pos, int n) const;
	TString left(int n) const;
	TString right(int n) const;

	int tsprintf(const TCHAR *fmt, ...);
	int tvprintf(const TCHAR *fmt, va_list * args);
	bool iswm(TCHAR *a) const;
	bool iswmcs(TCHAR *a) const;
	TCHAR * to_chr() const { return this->m_pString; };	// returns the string in the projects current format.
#ifdef UNICODE
	WCHAR *to_wchr(bool tryutf8 = false) const { return this->m_pString; };	// returns the string in wide format
	char * c_str(void) const;	// returns the string as a char *
	int to_int() const { return _wtoi(this->m_pString); };
	__int64 to_num( ) const { return _wtoi64(this->m_pString); };
	double to_float() const { return _wtof(this->m_pString); };
#else
	WCHAR *to_wchr(bool tryutf8 = false);	// returns the string in wide format
	char * c_str( ) const { return this->m_pString; };	// returns the string as a char *
	int to_int() const { return atoi(this->m_pString); };
	__int64 to_num( ) const { return _atoi64(this->m_pString); };
	double to_float() const { return atof(this->m_pString); };
#endif
	static inline int rfc_tolower(const int c);
	static inline int rfc_toupper(const int c);
	//
};
#define TSTAB TString::TAB
#define TSSPACE TString::SPACE
#define TSCOMMA TString::COMMA
#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( pop )
#endif

#endif // TSTRING_H_
