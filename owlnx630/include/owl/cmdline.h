//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:12 $
//
// Command line parsing class
//----------------------------------------------------------------------------

#if !defined(OWL_CMDLINE_H)
#define OWL_CMDLINE_H

#if !defined(OWL_DEFS_H)
# include <owl/defs.h>
#endif
#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif


__OWL_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>

//
// class TCmdLine
// ~~~~~ ~~~~~~~~
// Command line argument processing class, processes in the form:
//
//  <Name> | {-/}<Option>[{:=}<Value>] ...
//
class _OWLCLASS TCmdLine {
  public:
    enum TKind {
      Start,   // No tokens have been parsed yet
      Name,    // Name type token, has no leading / or -
      Option,  // Option type token. Leading / or - skipped by Token
      Value,   // Value for name or option. Leading : or = skipped by Token
      Done     // No more tokens
    };
    TCmdLine(LPCTSTR cmdLine);
   ~TCmdLine();

    TKind   NextToken(bool removeCurrent=false);
    LPCTSTR GetLine() const {return Buffer;}
    void     Reset();

    TKind   Kind;       // Kind of current token
    LPTSTR   Token;      // Ptr to current token. (Not 0-terminated, use TokenLen)
    int     TokenLen;   // Length of current token

  private:
    LPTSTR Buffer;     // Command line buffer
    LPTSTR TokenStart; // Actual start of current token
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

__OWL_END_NAMESPACE


#endif  // OWL_CMDLINE_H
