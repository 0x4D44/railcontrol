//------------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1998 by Yura Bidus
//
//  $Revision: 1.27 $
//  $Author: jogybl $
//  $Date: 2009-08-05 10:32:03 $
//
// This source file implements TConfigFile,TIniConfigFile,TRegConfigFile and 
// TConfigFileSection.
//------------------------------------------------------------------------------
#include <owl/pch.h>
//#pragma hdrstop

#include <owl/configfl.h>
#include <owl/registry.h>
#include <owl/file.h>
#include <owl/date.h>
#include <owl/time.h>
#include <owl/template.h>
#include <owl/pointer.h>

#if !defined(OWL_PRIVATE_STRMDEFS_H)
#include <owl/private/strmdefs.h>
#endif

#if defined WINELIB
//#include <wine/msvcrt/stdlib.h> //for _ltoa, _itoa
#endif

__OWL_USING_STD_NAMESPACE

__OWL_BEGIN_NAMESPACE

static const _TCHAR szNO[]    = _T("no");
static const _TCHAR szOFF[]   = _T("off");
static const _TCHAR szZERO[]  = _T("0");
static const _TCHAR szFALSE[] = _T("false");
static const _TCHAR szYES[]   = _T("yes");
static const _TCHAR szON[]    = _T("on");
static const _TCHAR szONE[]   = _T("1");
static const _TCHAR szTRUE[]  = _T("true");
//static const _TCHAR szDef[]   = _T("!)@(#*$&%^");
static const _TCHAR szSep[]   = _T("\\");

int  TConfigFile::DefaultPrecision = -1;

////////////////////////////////////////////////////////////////////////////
//  This static method is used for parsing strings.
//
static LPTSTR skip_ws(LPTSTR sz)
{
  while (*sz == _T(' ') || *sz == _T('\t'))
    ++sz;

  return sz;
}

////////////////////////////////////////////////////////////////////////////
//  This static method is used for parsing strings.
//
static LPTSTR skipPrefix (LPTSTR sz, _TCHAR ch)
{
  sz = skip_ws(sz);

  if (_totupper(*sz) == ch){
    ++sz;
    sz = skip_ws(sz);
    if (*sz == _T('='))
      sz = skip_ws(sz+1);
  }
  return sz;
}

//
//  This static method converts a string to a bool, accepting most common
//  boolean keywords: yes, on, true and 1.
//
bool TConfigFile::StringToBool(LPCTSTR str, bool *pVal)
{
  if ((_tcsicmp(str,szNO)==0)    || 
      (_tcsicmp(str,szOFF)==0)  || 
      (_tcsicmp(str,szZERO)==0) || 
      (_tcsicmp(str,szFALSE)==0)
    ){
    *pVal = false;
    return true;
  }
  if ((_tcsicmp(str,szYES)==0)  || 
      (_tcsicmp(str,szON)==0)    || 
      (_tcsicmp(str,szONE)==0)  || 
      (_tcsicmp(str,szTRUE)==0)
    ){
    *pVal = true;
    return true;
  }
  return false;
}

////////////////////////////////////////////////////////////////////////////
//  This static method returns yes or no given a boolean value.
//
owl_string TConfigFile::BoolToString(bool b)
{
  return owl_string(b ? szYES : szNO);
}
//
// where string is integer????
bool is_digit(_TCHAR c)
{
  return _istdigit(c)||(_T('a')<=c && c<=_T('f'))||(_T('A')<=c && c<=_T('F'));
}
//
// check if all string is digits
//
bool is_integer(LPTSTR text)
{
  _TCHAR* p = text;
  _TCHAR* ep;

  // skip leading wa=hite spaces
  p = skip_ws(p);
  ep= p + ::_tcslen(p) - 1;
  // skip trailing whitespaces
  while(*ep && (*ep == _T('\t') || *ep == _T(' ')))
    *ep-- = _T('\0');
  
  while(*p && *p != _T('\n')){
    if(!is_digit(*p))
      return false;
    p++;
  }
  return true;
}
//
static LPTSTR next_name(LPTSTR p)
{
  while(*p != _T('\0'))
    p++;
  return ++p;
}
//
void TConfigFile::LoadValues(const TConfigFile& file)
{
  _TCHAR  text[MAX_PATH];

  TAPointer<_TCHAR> _ClnObj1,_ClnObj2;
  _TCHAR *sections, *entries; 

  int sect_size = 4096;
  int size;
  do{
    _ClnObj1 = sections = new _TCHAR[sect_size];
    size = file.ReadSections(sections, sect_size);
    sect_size += 1024;
  }
  while(size >= (sect_size-2));

  // while sections not '\0'
  while(*sections){
    sect_size = 4096;
    do{
      _ClnObj2 = entries = new _TCHAR[sect_size];
      size = file.ReadSection(sections, entries, sect_size);
      sect_size += 1024;
    }
    while(size >= (sect_size-2));
    
    //2. Read all values
    while(*entries){
      //?? where difference between int and text entries????
      int size = file.ReadString(sections,entries,text,MAX_PATH,_T(""));
      // if size ==0 or strlen == 0 -> error
      if(!size || !::_tcslen(text) || is_integer(text)){
        size = file.ReadInteger(sections,entries, INT_MAX);
        if(size < INT_MAX)
          WriteInteger(sections,entries, size);
      }
      else
        WriteString(sections,entries, text);

      //get next entry name
      entries = next_name(entries);
    }
    //get next section
    sections  = next_name(sections);
  }
}
//
//  This method reads binary data.
//
bool TConfigFile::ReadData(LPCTSTR section, LPCTSTR entry, void* buffer, uint bufSize) const
{
  TAPointer<_TCHAR> __ClnObj = new _TCHAR[bufSize * 3 + 1];
  LPTSTR text = __ClnObj;

  if (!ReadString(section, entry, text, bufSize * 3 + 1, _T("")))
    return false;

  if(!::_tcslen(text))
    return false;

  uint8* data = (uint8*)buffer;
  for (uint i = 0; i < bufSize; i++) {
    int n; // string in form 'XX '
    if(_stscanf(text, _T("%2X"), &n) == EOF)
      return true;
    *data++ = (uint8)n;

    text += 3;
  }
  return true;
}
//
//  This method writes binary data.
//
bool TConfigFile::WriteData(LPCTSTR section, LPCTSTR entry, void* buffer, uint bufSize)
{
  TAPointer<_TCHAR> __ClnObj = new _TCHAR[bufSize * 3 + 1];
  
  LPTSTR text = __ClnObj;
  uint8* data = (uint8*)buffer;

  for (uint i = 0; i < bufSize; i++) {
    _stprintf(text, _T("%02X "), (int)*data++);
    text += 3;
  }
  return WriteString(section, entry, __ClnObj);
}
//
//  This method reads an integer.
//
int TConfigFile::ReadInteger(LPCTSTR section, LPCTSTR entry, int defval) const
{
  PRECONDITION(VALIDPTR(entry));

  _TCHAR text [20];
  LPTSTR end;

  if (!ReadString(section, entry, text, COUNTOF(text), _T("")))
    return defval;

  int retval = (int)_tcstol(text, &end, 10);

  if (*end)
    return retval;

  return retval;
}

//
//  This method writes an integer.  We format it as a string.
//
bool TConfigFile::WriteInteger(LPCTSTR section, LPCTSTR entry, int value)
{
  _TCHAR  text [20];

#if defined __GNUC__
  #if defined UNICODE
  snwprintf(text, 20, _T("%d"), value);
  #else
  snprintf(text, 20, "%d", value);
  #endif
#else
  _ltot (value, text, 10);
#endif



  return WriteString(section, entry, text);
}

//
//
//
int TConfigFile::ReadSection(const owl_string& section, owl_string& buffer) const
{
  TAPointer<_TCHAR> _ClnObj1;
  _TCHAR *sections; 
  int sect_size = 4096;
  int size;

  do{
    _ClnObj1 = sections = new _TCHAR[sect_size];
    size = ReadSection(section.c_str(),sections, sect_size);
    sect_size += 1024;
  }
  while(size >= (sect_size-2));
  buffer = sections;
  return size;
}
//
//
//
int TConfigFile::ReadSections(owl_string& buffer) const
{
  TAPointer<_TCHAR> _ClnObj1;
  _TCHAR *sections; 
  int sect_size = 4096;
  int size;

  do{
    _ClnObj1 = sections = new _TCHAR[sect_size];
    size = ReadSections(sections, sect_size);
    sect_size += 1024;
  }
  while(size >= (sect_size-2));
  buffer = sections;
  return size;
}
//
//
//
int TConfigFile::ReadString(const owl_string& section, const owl_string& entry, owl_string& buffer, LPCTSTR defstr) const
{
  TTmpBuffer<_TCHAR>  text(MAX_PATH);
  ReadString(section.c_str(), entry.c_str(), (_TCHAR*)text, MAX_PATH, _T(""));
  
  buffer = _T("");
  if(!::_tcslen(text)){
    if(defstr && defstr[0])
      buffer = defstr;
    return false;
  }
  buffer = owl_string(text);
  return buffer.length();
}

//
//
//
double TConfigFile::ReadDouble(LPCTSTR section, LPCTSTR  entry, double defval) const
{
  double retval = defval;
  _TCHAR* end;
  _TCHAR  text[80];
  
  ReadString(section, entry, text, COUNTOF(text), _T(""));

  if (::_tcslen(text)){
    retval = _tcstod(text, &end);

    if(*end)
      retval = defval;
  }
  return retval;
}

//
//  This method writes an integer.
//
bool TConfigFile::WriteDouble(LPCTSTR section, LPCTSTR entry, double val) 
{
  _TCHAR text [80];

  //
  //  We request a formatted number with a LOT of significant digits:
  //
  if(DefaultPrecision < 0)
    _stprintf(text, _T("%1.20Lg"), (long double)val);
  else{
    if(DefaultPrecision==0)
      _stprintf(text, _T("%Lg"), (long double)val);
    else{
      owl_string format = _T("%1.");
#if defined __GNUC__
  #if defined UNICODE
      snwprintf(text, 10, _T("%d"), DefaultPrecision);
  #else
      snprintf(text, 10, "%d", DefaultPrecision);
  #endif
      format += text;
#else
      format += _itot(DefaultPrecision, text, 10);
#endif
      format += _T("Lg");
      _stprintf(text, format.c_str(), (long double)val);
    }
  }
  return WriteString(section, entry, text);
}
//
//  This method reads a boolean value.
//
bool TConfigFile::ReadBool(LPCTSTR section, LPCTSTR entry, bool defval) const
{
  _TCHAR  text[20];
  ReadString(section, entry, text, COUNTOF(text), _T(""));

  bool bVal;

  if (::_tcslen(text)){
    bool b = StringToBool(text, &bVal);

    if (b)
      return bVal;
  }

  return defval;
}
//
//  This method writes a bool.
//
bool TConfigFile::WriteBool (LPCTSTR section, LPCTSTR entry, bool val) 
{
  return WriteString(section, entry, BoolToString(val).c_str());
}
//
//
//
bool TConfigFile::ReadDate(LPCTSTR section, LPCTSTR entry, TDate& value) const
{
  _TCHAR  text[80];
  ReadString(section, entry, text, COUNTOF(text), _T(""));
  if(::_tcslen(text)){
#if defined(BI_STD_NEWSSTREAM)
    _tistrstream _strtemp(text);
    // Y.B. 22/06/2002 workaround ?????
    _tistream& strtemp = *(_tistream*)&_strtemp;
#else
    _tistrstream strtemp(text, COUNTOF(text));
#endif
    value = TDate((const _tistream&)strtemp);
    return true;
  }
  return false;
}
//
//
//
bool TConfigFile::WriteDate(LPCTSTR section, LPCTSTR entry, const TDate& val)
{
#if defined(BI_STD_NEWSSTREAM)
  _tostrstream strtemp;
#else
  _TCHAR text[80];
  _tostrstream strtemp(text, COUNTOF(text));
#endif  
  TDate::HowToPrint oldOpt = TDate::SetPrintOption(TDate::Numbers);
  strtemp << val.AsString().c_str() << ends;
  TDate::SetPrintOption(oldOpt);

#if defined(BI_STD_NEWSSTREAM)
  return WriteString(section, entry, strtemp.str().c_str());
#else
  return WriteString(section, entry, text);
#endif
}
//
//
//
bool TConfigFile::ReadSystemTime(LPCTSTR section, LPCTSTR entry, TSystemTime& val) const
{
  _TCHAR  text[80];
  ReadString(section, entry, text, COUNTOF(text), _T(""));
  if(::_tcslen(text)){
    SYSTEMTIME dt;
    _stscanf(text,_T("%hu,%hu,%hu,%hu,%hu,%hu,%hu,%hu"),&dt.wYear,&dt.wMonth,&dt.wDayOfWeek,
            &dt.wDay,&dt.wHour,&dt.wMinute,&dt.wSecond,&dt.wMilliseconds);
    val = TSystemTime(dt);
    return true;
  }
  return false;
}
//
//
//
bool TConfigFile::WriteSystemTime(LPCTSTR section, LPCTSTR entry, const TSystemTime& dt)
{
  _TCHAR text[80]; 
  wsprintf(text,_T("%hu,%hu,%hu,%hu,%hu,%hu,%hu,%hu"),dt.wYear,dt.wMonth,dt.wDayOfWeek,
            dt.wDay,dt.wHour,dt.wMinute,dt.wSecond,dt.wMilliseconds);
  return WriteString(section, entry, text);
}
//
//
//    
bool TConfigFile::ReadTime(LPCTSTR section, LPCTSTR entry, TTime& val) const
{
  _TCHAR  text[80];
  ReadString(section, entry, text, COUNTOF(text), _T(""));
  if(::_tcslen(text)){
    int h,m,s;
    _stscanf(text,_T("h=%d m=%d s=%d"), &h, &m, &s);
    val = TTime(h,m,s);
    return true;
  }
  return false;
}
//
//
//
bool TConfigFile::WriteTime(LPCTSTR section, LPCTSTR entry, TTime& val)
{
  _TCHAR text[80];
  wsprintf(text,_T("h=%d m=%d s=%d"),val.Hour(),val.Minute(),val.Second());
  return WriteString(section, entry, text);
}
//
//
//  This method will take a profile string of the form:
//
//                   +---------------- lfHeight
//                   |  +------------- lfWidth
//                   |  |             /lfWeight
//                   |  |   +------- / lfItalic
//                   |  |   |        \ lfUnderline
//                   |  |   |         \lfStrikeOut
//                   |  |   |   +----- lfEscapement
//                   |  |   |   |  +-- lfOrientation
//                   |  |   |   |  |
//                   v  v   v   v  v
//                  10, 8, BIU, 0, 0, 0, 0, 0, 0, 0, Times New Roman
//                                    ^  ^  ^  ^  ^  ^
//                                    |  |  |  |  |  |
//                         lfCharSet -+  |  |  |  |  |
//                    lfOutPrecision ----+  |  |  |  |
//                   lfClipPrecision -------+  |  |  |
//                         lfQuality ----------+  |  |
//                  lfPitchAndFamily -------------+  |
//                        lfFaceName ----------------+
//
//  and fills in a logfont structure.
//
bool TConfigFile::ReadFont(LPCTSTR section, LPCTSTR entry, LOGFONT& pFont) const
{
  _TCHAR  text[MAX_PATH];
  ReadString(section, entry, text, MAX_PATH, _T(""));
  
  if(!::_tcslen(text))
    return false;

  LPCTSTR szSep = _T(", \t");
  bool    b;
  _TCHAR* sz;
  LOGFONT lf;
  int     i, n;

  _tcscat(text,_T("\n"));
  sz = _tcstok(text, szSep);
  n = 0;
  while (sz){
    i = _ttoi(sz);   // not needed all the time, but oh wel...

    switch (n++){
      // int   lfHeight;
      case 0:
        lf.lfHeight = i;
        break;

      // int   lfWidth;
      case 1:
        lf.lfWidth = i;
        break;

      // int   lfWeight;
      // BYTE  lfItalic;
      // BYTE  lfUnderline;
      // BYTE  lfStrikeOut;
      case 2:
        lf.lfWeight      = FW_NORMAL;
        lf.lfItalic      = FALSE;
        lf.lfUnderline  = FALSE;
        lf.lfStrikeOut  = FALSE;

        if (_tcschr(sz, _T('B')))   
          lf.lfWeight = FW_BOLD;
        if (_tcschr(sz, _T('I')))   
          lf.lfItalic = TRUE;
        if (_tcschr(sz, _T('U')))   
          lf.lfUnderline = TRUE;
        if (_tcschr(sz, _T('S')))   
          lf.lfStrikeOut = TRUE;
        break;

      // int   lfEscapement;
      case 3:
        lf.lfEscapement = i;
        break;

      // int   lfOrientation;
      case 4:
        lf.lfOrientation = i;
        break;

      // BYTE  lfCharSet;
      case 5:
        lf.lfCharSet = uint8(i);
        break;

      // BYTE  lfOutPrecision;
      case 6:
        lf.lfOutPrecision = uint8(i);
        break;

      // BYTE  lfClipPrecision;
      case 7:
        lf.lfClipPrecision = uint8(i);
        break;

      // BYTE  lfQuality;
      case 8:
        lf.lfQuality = uint8(i);
        break;

      // BYTE  lfPitchAndFamily;
      case 9:
        lf.lfPitchAndFamily = uint8(i);
        break;

      // BYTE  lfFaceName[LF_FACESIZE];
      case 10:
        ::_tcscpy(lf.lfFaceName, sz);
        break;
    }

    if (n==10)
      sz = _tcstok(0, _T("\n"));
    else
      sz = _tcstok(0, szSep);
  }
  b = (n == 11);

  if(b)
    pFont = lf;

  return b;
}

//
//
//  This method writes a font description in the following format:
//
//                   +---------------- lfHeight
//                   |  +------------- lfWidth
//                   |  |             /lfWeight
//                   |  |   +------- / lfItalic
//                   |  |   |        \ lfUnderline
//                   |  |   |         \lfStrikeOut
//                   |  |   |   +----- lfEscapement
//                   |  |   |   |  +-- lfOrientation
//                   |  |   |   |  |
//                   v  v   v   v  v
//                  10, 8, BIU, 0, 0, 0, 0, 0, 0, 0, Times New Roman
//                                    ^  ^  ^  ^  ^  ^
//                                    |  |  |  |  |  |
//                         lfCharSet -+  |  |  |  |  |
//                    lfOutPrecision ----+  |  |  |  |
//                   lfClipPrecision -------+  |  |  |
//                         lfQuality ----------+  |  |
//                  lfPitchAndFamily -------------+  |
//                        lfFaceName ----------------+
//
bool TConfigFile::WriteFont (LPCTSTR section, LPCTSTR entry, const LOGFONT& logFont)
{
  owl_string  s;
  _TCHAR      ach [40];
  int          i;
  _TCHAR      comma[] = _T(",");
  
#if defined __GNUC__
  _tprintf(ach, _T("%d"), logFont.lfHeight);
  s = ach;
#else
  s = _itot(logFont.lfHeight, ach, 10);
#endif 
  s += comma;
#if defined __GNUC__
  _tprintf(ach, _T("%d"), logFont.lfWidth);
  s += ach;
#else
  s += _itot(logFont.lfWidth, ach, 10);
#endif 
  s += comma;

  i = 0;
  ach[i++] = (logFont.lfWeight < FW_BOLD) ? _T('N') : _T('B');
  if (logFont.lfItalic)    
    ach[i++] = _T('I');
  if (logFont.lfUnderline) 
    ach[i++] = _T('U');
  if (logFont.lfStrikeOut) 
    ach[i++] = _T('S');
  ach[i] = 0;
  s += ach;
  s += comma;

#if defined __GNUC__
  _tprintf(ach, _T("%d"), logFont.lfEscapement);
  s += ach;
#else
  s += _itot(logFont.lfEscapement, ach, 10);
#endif
  s += comma;
#if defined __GNUC__
  _tprintf(ach, _T("%d"), logFont.lfOrientation);
  s += ach;
#else
  s += _itot(logFont.lfOrientation, ach, 10);
#endif
  s += comma;
#if defined __GNUC__
  _tprintf(ach, _T("%d"), logFont.lfCharSet);
  s += ach;
#else
  s += _itot(logFont.lfCharSet, ach, 10);
#endif
  s += comma;
#if defined __GNUC__
  _tprintf(ach, _T("%d"), logFont.lfOutPrecision);
  s += ach;
#else
  s += _itot(logFont.lfOutPrecision, ach, 10);
#endif
  s += comma;
#if defined __GNUC__
  _tprintf(ach, _T("%d"), logFont.lfClipPrecision);
  s += ach;
#else
  s += _itot(logFont.lfClipPrecision, ach, 10);
#endif
  s += comma;
#if defined __GNUC__
  _tprintf(ach, _T("%d"), logFont.lfQuality);
  s += ach;
#else
  s += _itot(logFont.lfQuality, ach, 10);
#endif
  s += comma;
#if defined __GNUC__
  _tprintf(ach, _T("%d"), logFont.lfPitchAndFamily);
  s += ach;
#else
  s += _itot(logFont.lfPitchAndFamily, ach, 10);
#endif
  s += comma;
  s += logFont.lfFaceName;

  return WriteString(section, entry, s.c_str());
}
//
//  This method reads an RGB color.
//
bool TConfigFile::ReadColor(LPCTSTR section, LPCTSTR entry, TColor& color) const
{
  _TCHAR  text[40];
  ReadString(section, entry, text, COUNTOF(text), _T(""));
  
  if(!::_tcslen(text))
    return false;

  TColor  clr;
  _TCHAR*  sz[3];
  bool    b;

  b = false;

  sz[0] = text;
  sz[1] = _tcschr(sz[0], _T(','));
  if(sz[1]){
    *(sz[1]++) = 0;
    sz[2] = _tcschr(sz[1], _T(','));
    if (sz[2]){
      *(sz[2]++) = 0;

      int R = _ttoi(skipPrefix(sz[0], _T('R')));
      int G = _ttoi(skipPrefix(sz[1], _T('G')));
      int B = _ttoi(skipPrefix(sz[2], _T('B')));

      if (R >= 0 && R < 256 &&
          G >= 0 && G < 256 &&
          B >= 0 && B < 256){
        clr = RGB(R,G,B);
        b = true;
      }
    }
  }
  if(b)
    color = clr;
  return b;
}
//
//  This method writes an RGB color
//
bool TConfigFile::WriteColor(LPCTSTR section, LPCTSTR entry, const TColor& clr) 
{
  _TCHAR ach[40];

  wsprintf (ach, _T("R=%d,G=%d,B=%d"), clr.Red(), clr.Green(), clr.Blue());
  
  return WriteString(section, entry, ach);
}
//
//  This method reads an X,Y point.
//
bool TConfigFile::ReadPoint (LPCTSTR section, LPCTSTR entry, TPoint& point) const
{
  _TCHAR  text[40];
  ReadString(section, entry, text, COUNTOF(text), _T(""));
  
  if(!::_tcslen(text))
    return false;

  LPCTSTR  sz;
  bool    b = false;
  int     array[2], i;

  sz = text;

  for ( i = 0; i < 2; i++){
    sz = _tcschr (sz, _T('='));
    if(! sz)
      break;
    sz++;
    if(!_istdigit(*sz))
      break;

    array[i] = _ttoi(sz);
  }

  if (i == 2){
    b = true;
    point.x  = array[0];
    point.y = array[1];
  }
  return b;
}
//
//  This method writes a TPoint.
//
bool TConfigFile::WritePoint (LPCTSTR section, LPCTSTR entry, const TPoint& point)
{
  _TCHAR buf[20];

  wsprintf(buf, _T("X=%d,Y=%d"), point.x, point.y);

  return WriteString(section, entry, buf);
}
//
//  This method reads a rectangle.
//
bool TConfigFile::ReadRect (LPCTSTR section, LPCTSTR entry, TRect& rect) const
{
  _TCHAR  text[40];
  ReadString(section, entry, text, COUNTOF(text), _T(""));

  if(!::_tcslen(text))
    return false;

  LPCTSTR  sz;
  bool    b = false;
  int     nArray[4], i;

  sz = text;

  for(i = 0; i < 4; i++){
    sz = _tcschr(sz, _T('='));
    if (!sz)
      break;

    sz++;
    if(!_istdigit(*sz))
      break;

    nArray[i] = _ttoi(sz);
  }

  if(i == 4){
    b = true;
    rect.left    = nArray[0];
    rect.top     = nArray[1];
    rect.right   = nArray[0] + nArray[2];
    rect.bottom  = nArray[1] + nArray[3];
  }
  return b;
}
//
//  This method writes a rectangle.
//
bool TConfigFile::WriteRect (LPCTSTR section, LPCTSTR entry, const TRect& r)
{
  _TCHAR buf [40];

  wsprintf (buf, _T("L=%d,T=%d,W=%d,H=%d"), r.left, r.top, r.Width(), r.Height());

  return WriteString(section, entry, buf);
}
//
//  This method writes a TSize.
//
bool TConfigFile::WriteSize (LPCTSTR section, LPCTSTR entry, const TSize& size)
{
  _TCHAR buf [80];

  wsprintf(buf, _T("W=%d,H=%d"), size.cx, size.cy);

  return WriteString(section, entry, buf);
}
//
bool TConfigFile::SectionExists(LPCTSTR section) const
{
  _TCHAR buff[3];
  return ReadSection(section, buff, 3) != 0;
}
///////////////////////////////////////////////////////////////////////////////////////
//
// class TIniConfigFile 
// ~~~~~ ~~~~~~~~~~~~~~
//
int TIniConfigFile::ReadSection(LPCTSTR section, LPTSTR buffer, uint bufSize) const
{
  return GetPrivateProfileString(section, 0, _T(""), buffer, bufSize, FileName.c_str());
}
//
int TIniConfigFile::ReadSections(LPTSTR buffer, uint bufSize) const
{
  return GetPrivateProfileString(0, 0, _T(""), buffer, bufSize,FileName.c_str());
}
//
//  This method deletes the specified section.
//
bool TIniConfigFile::EraseSection (LPCTSTR section)
{
  return ::WritePrivateProfileString(section, 0, 0, FileName.c_str()) != 0;
}
//
//
//
//  This method deletes the specified entry.
//
bool TIniConfigFile::EraseEntry(LPCTSTR section, LPCTSTR entry)
{
  return ::WritePrivateProfileString (section, entry, 0, FileName.c_str()) != 0;
}
//
//
//
void TIniConfigFile::UpdateFile()
{
  ::WritePrivateProfileString(0, 0, 0, FileName.c_str());
}
//
//  This method is the base method for reading strings.
//
int TIniConfigFile::ReadString (LPCTSTR section, LPCTSTR entry, 
                                LPTSTR buffer,  uint bufSize, LPCTSTR def) const
{
  return ::GetPrivateProfileString (section, entry, def?def:_T(""), buffer, bufSize, FileName.c_str());
}
//
//  This method is the low-level method for writing strings.
//
bool TIniConfigFile::WriteString (LPCTSTR section, LPCTSTR entry, LPCTSTR value)
{
  return ::WritePrivateProfileString (section, entry, value, FileName.c_str()) != 0;
}
//
//  This method reads binary data.
//
bool TIniConfigFile::ReadData(LPCTSTR section, LPCTSTR entry, void* buffer, uint bufSize) const
{
  return ::GetPrivateProfileStruct(section, entry, buffer, bufSize, FileName.c_str()) != 0;
}
//
//  This method writes binary data.
//
bool TIniConfigFile::WriteData(LPCTSTR section, LPCTSTR entry, void* buffer, uint bufSize)
{
  return ::WritePrivateProfileStruct(section, entry, buffer, bufSize, FileName.c_str()) != 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
//
// Container support
//
class TStringMapNode: public TMapNode<owl_string,owl_string> {
public:
    TStringMapNode(owl_string& str, owl_string& val)
      : TMapNode<owl_string,owl_string>(str,val){}
    TStringMapNode(LPCTSTR str, LPCTSTR val)
      : TMapNode<owl_string,owl_string>(){Name = str; Value = val;}
    TStringMapNode(LPCTSTR str)
      : TMapNode<owl_string,owl_string>(){Name = str;}
};
//
typedef TSortedObjectArray<TStringMapNode>  TStringMap;
//
class TStringNode: public TMapNode<owl_string,TStringMap> {
  public:
    TStringNode(LPCTSTR str)
      : TMapNode<owl_string,TStringMap>(){Name = str;}
};
//
class TMemConfigFileSections: public TIPtrArray<TStringNode*>  {
  public:
    TMemConfigFileSections(){}
    ~TMemConfigFileSections(){}

    bool  DestroyItem(TStringNode* item);
    bool  Destroy(int loc);
    int   Add(TStringNode* item);
    bool  HasMember(TStringNode* item) const;
    int   Find(TStringNode* item) const;
};
//
bool TMemConfigFileSections::HasMember(TStringNode* item) const
{
  return Find(item) != (int)NPOS;
}
//
bool TMemConfigFileSections::DestroyItem(TStringNode* item)
{
  int index = Find(item);
  if(index != (int)NPOS){
    TStringNode* item = Data[index];
    Remove(index);
    delete item;// using global delete because we didn't allocate it
    return true;
  }
  return false;
}
//
bool TMemConfigFileSections::Destroy(int loc)
{
  if(loc < (int)Size()){
    TStringNode* item = Data[loc];
    Remove(loc);
    delete item;// using global delete because we didn't allocate it
    return true;
  }
  return false;
}
//
int TMemConfigFileSections::Add(TStringNode* item)
{
   if(ItemCnt>=Reserved)
     Resize(ItemCnt+1); // on error -> throw xalloc
  unsigned loc = ItemCnt++;
  while( loc > 0 && *item < *Data[loc-1]) {
    Data[loc] = Data[loc-1];
    loc--;
   }
  Data[loc] = item;
  return loc;
}
//
int TMemConfigFileSections::Find(TStringNode* item) const
{
  if( Size() == 0 )
    return (int)NPOS;
  unsigned lower = 0;
  unsigned upper = Size()-1;

  while( lower < upper && upper != NPOS ){
    unsigned middle = (lower+upper)/2;
    if(*Data[middle] == *item)
      return middle;
    if(*Data[middle] < *item)
      lower = middle+1;
    else
      upper = middle-1;
  }

  if( lower == upper && *Data[lower] == *item)
    return (int)lower;
  return (int)NPOS;
}
///////////////////////////////////////////////////////////////////////
//
// class TMemConfigFile
// ~~~~~ ~~~~~~~~~~~~~~ 
TMemConfigFile::TMemConfigFile(LPCTSTR  fileName)
:
  TConfigFile(fileName)
{
  Sections = new TMemConfigFileSections();
}
//
TMemConfigFile::~TMemConfigFile()
{
  Flush();
  delete Sections;
}
//
void TMemConfigFile::Flush()
{
  Sections->Flush();
}
//
void TMemConfigFile::LoadValues(const TConfigFile& file)
{
  Flush();
  TConfigFile::LoadValues(file);
}
//
int TMemConfigFile::AddSection(LPCTSTR  section)
{
  return Sections->Add(new TStringNode(section));
}
//
bool TMemConfigFile::EraseEntry(LPCTSTR section, LPCTSTR entry)
{
  TStringNode sn(section);
  int index = Sections->Find(&sn);
  if(index != (int)NPOS){
    return (*Sections)[index]->Value.DestroyItem(entry);
  }
  return false;
}
//
bool TMemConfigFile::EraseSection(LPCTSTR section)
{
  TStringNode sn(section);
  return Sections->DestroyItem(&sn);
}
//
// Copies all value names to specified buffer
//
int  TMemConfigFile::ReadSection(LPCTSTR section, LPTSTR buffer, uint bufSize) const
{
  TStringNode sn(section);
  int index = Sections->Find(&sn);
  size_t count = 0; 
  _TCHAR* p = buffer;
  if(index != (int)NPOS){
    const TStringMap& map = (*Sections)[index]->Value;
    for(uint i = 0; i < map.Size(); i++){
      size_t size = ::_tcslen(map[i].Name.c_str());
      if(bufSize > size){
        ::_tcscpy(p,map[i].Name.c_str());
        bufSize -= size - 1;
        count    += size + 1;
        p += size;
        *p++ = _T('\0');
      }
      else
        return bufSize -2;
    }
  }
  // last trailing null
  *p = _T('\0');
  return ++count;
}
//
// Copies all section names to specified buffer
//
int TMemConfigFile::ReadSections(LPTSTR buffer, uint bufSize) const
{
  size_t count = 0; 
  _TCHAR* p = buffer;
  for(uint i = 0; i < Sections->Size(); i++){
    size_t size = ::_tcslen((*Sections)[i]->Name.c_str());
    if(bufSize > size){
      ::_tcscpy(p, (*Sections)[i]->Name.c_str());
      bufSize -= size - 1;
      count    += size + 1;
      p += size;
      *p++ = _T('\0');
    }
    else
      return bufSize -2;
  }
  // last trailing null
  *p = _T('\0');
  return ++count;
}
//
int TMemConfigFile::ReadString(LPCTSTR section, LPCTSTR entry, LPTSTR buffer,  uint bufSize, LPCTSTR defstr) const
{
  TStringNode sn(section);
  int index = Sections->Find(&sn);
  if(index != (int)NPOS){
    const TStringMap& map = (*Sections)[index]->Value;
    index = map.Find(TStringMapNode(entry));
    if(index != (int)NPOS){
      size_t count = ::_tcslen(map[index].Value.c_str());
      if(count < bufSize)
        ::_tcscpy(buffer,map[index].Value.c_str());
      else
        _tcsncpy(buffer,map[index].Value.c_str(), bufSize);
      buffer[bufSize-1] = 0;
      return count;
    }
  }
  ::_tcscpy(buffer,defstr?defstr:_T(""));
  return 0;
}
//
bool TMemConfigFile::WriteString(LPCTSTR section, LPCTSTR entry, LPCTSTR value)
{
  TStringNode sn(section);
  int index = Sections->Find(&sn);
  if(index == (int)NPOS)
    index = AddSection(section);
  TStringMap& map = (*Sections)[index]->Value;
  index = map.Find(TStringMapNode(entry));
  if(index != (int)NPOS)
    map[index].Value = owl_string(value);
  else
    index =  map.Add(TStringMapNode(entry,value));
  return index != (int)NPOS;
}
//
void TMemConfigFile::UpdateFile()
{
  TTextFile file(FileName.c_str(),TTextFile::WriteOnly|TTextFile::CreateAlways);
  if(!file.IsOpen())
    return;

  TAPointer<_TCHAR> buf = new _TCHAR[MAX_PATH];
  for(uint section = 0; section < Sections->Size(); section++){
    // Section name
    file << _T("[") << (*Sections)[section]->Name.c_str() << _T("]") << _T("\r\n");

    TStringMap& map = (*Sections)[section]->Value;
    for(uint value = 0; value < map.Size(); value++)
      file << map[value].Name.c_str() << _T("=") << map[value].Value.c_str() << _T("\r\n");
  }
}
//
//
//
static LPTSTR getName(owl_string& name,LPTSTR p)
{
  while(!_istalpha(*p))
    p++;
  _TCHAR* p1 = p++;
  while(_istalpha(*p))
    p++;
  
  _TCHAR c = *p;
  *p = _T('\0');

  name = p1;
  *p = c;
  return p;
}
//
static void getValue(owl_string& name,LPTSTR p)
{
  while(*p && (*p==_T(' ') || *p ==_T('\t')) && *p != _T('='))
    p++;
  name = ++p;
}
//
//
//
void TMemConfigFile::LoadFromFile()
{
  TTextFile file(FileName.c_str());
  if(!file.IsOpen())
    return;

  Sections->Flush();
  TFileLineIterator fitr(file);
  int sec_index = -1;
  owl_string tmp_name1,tmp_name2;
  
  while(fitr){
    LPTSTR p = skip_ws((LPTSTR)(LPCTSTR)fitr);
    if(::_tcslen(p) && *p != _T(';')){
      // read section
      if(*p == _T('[')){
        getName(tmp_name1,p);
        sec_index = AddSection(tmp_name1.c_str());
      }
      // else read value
      else if(sec_index >= 0){
        p = getName(tmp_name1,p);
        getValue(tmp_name2,p);
        (*Sections)[sec_index]->Value.Add(TStringMapNode(tmp_name1,tmp_name2));
      }
    }
    ++fitr;
  }
}
//

//
// class TRegConfigFile
// ~~~~~ ~~~~~~~~~~~~~~ 
//  This source file implements TRegConfigFile.  This class is derived from
//  TConfigFile and overrides the low-level storage methods to vector them
//  to the registry.
//
//  TRegConfigFile is not intended to handle all of an app's registry needs,
//  but rather to be used for a convenient wrapper for app preference
//  settings.  All data is stored in HKEY_CURRENT_USER, using FileName like
//  so:
//
//      HKEY_CURRENT_USER\Software\"FileName"\...
//
//  FileName should contain the company name and product name to adhere to
//  standard registry practices.  Eg:
//
//      FileName = "My Company\\Product"
//      HKEY_CURRENT_USER\Software\My Company\Product\...
//
//  A version number can also be used to allow multiple versions of the
//  same product to live together:
//
//      mFile = "My Company\\Product\\V1.0"
//      HKEY_CURRENT_USER\Software\My Company\Product\V1.0\...
//
//  NOTE: FileName must not end with a backslash!
//
//  All sections are created under the subtree as described above.  Eg,
//
//      Section = "Options"
//      HKEY_CURRENT_USER\Software\My Company\Product\V1.0\Options
//
//  Entries are stored under sections as REG_SZ values (for numbers it is REG_DWORD).  Eg,
//
//      entry = "Tile"
//      HKEY_CURRENT_USER\Software\My Company\Product\V1.0\Options\Tile
//
//
TRegConfigFile::TRegConfigFile(LPCTSTR name, HKEY root)
:  
  TConfigFile(_T("")),hRoot(root)
{
  FileName = _T("Software\\");
  FileName += name;
  TRegKey testKey(hRoot);
  if(testKey==HKEY(0)){
    hRoot = 0;
    TXRegistry::Check(1, name);
  }
}
//
TRegConfigFile::~TRegConfigFile()
{
}
//
bool TRegConfigFile::SectionExists(LPCTSTR section) const
{
  owl_string value_name = FileName;
  value_name += szSep;
  value_name += section;
  TRegKey key(hRoot,value_name.c_str(), KEY_QUERY_VALUE, TRegKey::NoCreate);
  return key != HKEY(0);
}
//
int TRegConfigFile::ReadSection(LPCTSTR section, LPTSTR buffer, uint bufSize) const
{
  size_t count = 0; 
  _TCHAR* p = buffer;

  owl_string value_name = FileName;
  value_name += szSep;
  value_name += section;

  TRegKey root(hRoot,value_name.c_str());
  TRegValueIterator iter(root);
  while(iter){
    TRegValue cur_value(iter);
    size_t size = ::_tcslen(cur_value.GetName());
    if(bufSize > size){
      ::_tcscpy(p, cur_value.GetName());
      bufSize -= size - 1;
      count    += size + 1;
      p += size;
      *p++ = _T('\0');
    }
    ++iter;
  }
  // last trailing null
  *p = _T('\0');
  return ++count;
}
//
//
//
int TRegConfigFile::ReadSections(LPTSTR buffer, uint bufSize) const
{
  TRegKey root(hRoot,FileName.c_str());
  size_t count = 0; 
  _TCHAR* p = buffer;

  TRegKeyIterator iter(root);
  while(iter){
    TRegKey cur_key(iter);
    size_t size = ::_tcslen(cur_key.GetName());
    if(bufSize > size){
      ::_tcscpy(p, cur_key.GetName());
      bufSize -= size - 1;
      count    += size + 1;
      p += size;
      *p++ = _T('\0');
    }
    ++iter;
  }
  // last trailing null
  *p = _T('\0');
  return ++count;
}
//
bool TRegConfigFile::EraseSection(LPCTSTR section)
{
  return TRegKey(hRoot,FileName.c_str()).NukeKey(section) == ERROR_SUCCESS; 
}
//
bool TRegConfigFile::EraseEntry(LPCTSTR section, LPCTSTR entry)
{
  owl_string value_name = FileName;
  value_name += szSep;
  value_name += section;
  return TRegKey(hRoot,value_name.c_str()).DeleteValue(entry) == ERROR_SUCCESS;
}
//
void TRegConfigFile::UpdateFile()
{
  // don't need this currently
}
//
int TRegConfigFile::ReadInteger(LPCTSTR section, LPCTSTR entry,int defint) const
{
  uint32 value  = defint;
  uint32 size    = sizeof(value);
  uint32 type;

  owl_string value_name = FileName;
  value_name += szSep;
  value_name += section;
  
  if(TRegKey(hRoot,value_name.c_str()).QueryValue(entry,&type,(uint8*)&value,&size)!=ERROR_SUCCESS)
    return defint;
  
  return (int)value;
}
//
bool TRegConfigFile::WriteInteger(LPCTSTR section, LPCTSTR entry, int value)
{
  owl_string value_name = FileName;
  value_name += szSep;
  value_name += section;
  return TRegKey(hRoot,value_name.c_str()).SetValue(entry, value) == ERROR_SUCCESS;
}
//
int TRegConfigFile::ReadString(LPCTSTR section, LPCTSTR entry, LPTSTR buffer, uint bufSize, LPCTSTR defstr) const
{
  uint32 size  = bufSize;
  uint32 type;

  owl_string value_name = FileName;
  value_name += szSep;
  value_name += section;
  
  if(TRegKey(hRoot,value_name.c_str()).QueryValue(entry,&type,(uint8*)buffer,&size)!=ERROR_SUCCESS){
    ::_tcscpy(buffer,defstr?defstr:_T(""));
    return ::_tcslen(buffer);
  }
  return (int)size;
}
//
bool TRegConfigFile::WriteString(LPCTSTR section, LPCTSTR entry, LPCTSTR value)
{
  owl_string value_name = FileName;
  value_name += szSep;
  value_name += section;

  //Jogy For REG_SZ types, the trailing zero must be included in the buffer 
  uint32 size = (::_tcslen(value) + 1)*sizeof(_TCHAR);

  return TRegKey(hRoot,value_name.c_str()).SetValue(entry,REG_SZ, (uint8*)value, size) == ERROR_SUCCESS;
}
//
//  This method reads binary data.
//
bool TRegConfigFile::ReadData(LPCTSTR section, LPCTSTR entry, void* buffer, uint bufSize) const
{
  uint32 size = bufSize;
  uint32 type;
  owl_string vname = FileName;
  vname += szSep;
  vname += section;

  return TRegKey(hRoot,vname.c_str()).QueryValue(entry,&type,
                 (uint8*)buffer,&size) == ERROR_SUCCESS;
}
//
//  This method writes binary data.
//
bool TRegConfigFile::WriteData(LPCTSTR section, LPCTSTR entry, void* buffer, uint bufSize)
{
  owl_string vname = FileName;
  vname += szSep;
  vname += section;

  return TRegKey(hRoot,vname.c_str()).SetValue(entry,REG_BINARY, 
                 (uint8*)buffer, bufSize) == ERROR_SUCCESS;
}
//
__OWL_END_NAMESPACE
//============================================================================================
