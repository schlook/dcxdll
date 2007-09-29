/*!
 * \file tstring.h
 * \brief String and Token Management Class
 *
 * This experimental library was built to reproduce easy to use string containers managing effectively 
 * comparisons and token manipulations as done in the mIRC scripting language.
 *
 * \author David Legault ( clickhere at scriptsdb dot org )
 * \version 1.8
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
 *
 * � ScriptsDB.org - 2005
 */

#ifndef _TSTRING_H_
#define _TSTRING_H_

// Required for VS 2005
#define _CRT_SECURE_NO_DEPRECATE 1
// end VS2005

#include <stdio.h>
#include <stdlib.h>
//#include <string.h>

/*!
 * \brief String and Token Management Class
 */
class TString {

private:

	void deleteString( );
	void deleteWString( );
	int _replace(const char *subString, const char *rString); // Ook

public:

	char * m_pString; //!< String buffer
	WCHAR * m_pWString;
	static const char *m_cSpace;
	static const char *m_cComma;
	static const char *m_cTab;
#define SPACE m_cSpace
#define COMMA m_cComma
#define TAB m_cTab

	TString( );
	TString( const char * cString );
	TString( const WCHAR * cString );
	TString( const char chr );
	TString( const WCHAR chr );
	TString( const TString & tString );
	TString( const char *pStart, const char *pEnd );
	TString( const unsigned int tsSize);

	//! Destructor
	~TString( );

	/*! \fn char * to_chr( )
			\brief Returns the string as a C char string
	*/
	char * to_chr( ) const { return this->m_pString; };

	// Operator Overloads
	void operator =( const TString & tString );
	void operator =( const char * cString );
	void operator =( const WCHAR * cString );
	void operator =( const char chr );
	void operator =( const WCHAR chr );

	TString operator +( const char * cString );
	TString operator +( const char chr );
	TString operator +( const TString & tString );

	void operator +=( const char * cString );
	void operator +=( const char chr );
	void operator +=( const TString & tString );
	void operator +=( const WCHAR chr );

	bool operator ==( const int iNull ) const;
	bool operator ==( const char * cString ) const;
	bool operator ==( const char chr ) const;
	bool operator ==( const TString & tString ) const;

	bool operator !=( const int iNull ) const;
	bool operator !=( const char * cString ) const;
	bool operator !=( const char chr ) const;
	bool operator !=( const TString & tString ) const;

	bool operator >( const char * cString ) const;
	bool operator >( const char chr ) const;
	bool operator >( const TString & tString ) const;

	bool operator >=( const char * cString ) const;
	bool operator >=( const char chr ) const;
	bool operator >=( const TString & tString ) const;

	bool operator <( const char * cString ) const;
	bool operator <( const char chr ) const;
	bool operator <( const TString & tString ) const;

	bool operator <=( const char * cString ) const;
	bool operator <=( const char chr ) const;
	bool operator <=( const TString & tString ) const;

	TString operator *( const int N );
	void operator *=( const int N );

	char & operator []( long int N ) const;

	// General String Lib
	size_t len( ) const;
	size_t wlen( ) const;

	int find( const char * substring, const int N ) const;
	int find( const char chr, const int N ) const;

	TString sub( int N, int M ) const;
	TString wsub( int N, int M ) const;

	void trim( );

	int replace( const char * subString, const char * rString );
	int replace( const char * subString, const char rchr );
	int replace( const char chr, const char * rString );
	int replace( const char chr, const char rchr );

	// Token Lib
	void addtok( const char * cToken, const char * sepChars = SPACE );
	void deltok( const int N, const char * sepChars = SPACE );
	int findtok( const char * cToken, const int N, const char * sepChars = SPACE ) const;
	TString gettok( int N, const char * sepChars = SPACE ) const;
	TString gettok( int N, int M, const char * sepChars = SPACE ) const;
	void instok( const char * cToken, const int N, const char * sepChars = SPACE );
	bool istok( const char * cToken, const char * sepChars = SPACE ) const;
	TString matchtok( char * mString, int N, const char * sepChars = SPACE ) const;
	int numtok( const char * sepChars = SPACE ) const;
	void puttok( const char * cToken, int N, const char * sepChars = SPACE );
	void remtok( const char * cToken, int N, const char * sepChars = SPACE );
	void reptok( const char * cToken, const char * newToken, int N, const char * sepChars = SPACE );
	void sorttok( char * sortOptions, const char * sepChars = SPACE );
	// added by Ook
	TString wildtok( char * wildString, int N, const char * sepChars = SPACE ) const;
	int nwildtok( char * wildString, const char * sepChars = SPACE ) const;
	TString mid(int pos, int n) const;
	TString left(int n) const;
	TString right(int n) const;
	int sprintf(const char *fmt, ...);
	bool iswm(char *a) const;
	bool iswmcs(char *a) const;
	int to_int() const { return atoi(this->m_pString); };
	__int64 to_num( ) const { return _atoi64(this->m_pString); };
	double to_float() const { return atof(this->m_pString); };
	WCHAR *to_wchr(void);
	//
};

#define TSTAB TString::TAB
#define TSSPACE TString::SPACE
#define TSCOMMA TString::COMMA

#endif // TSTRING_H_
