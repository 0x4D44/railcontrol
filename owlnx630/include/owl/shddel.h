//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1991, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//
//----------------------------------------------------------------------------

#if !defined(OWL_SHDDEL_H)
#define OWL_SHDDEL_H

#if !defined( OWL_PRIVATE_DEFS_H )
# include <owl/private/defs.h>
#endif
#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif


__OWL_BEGIN_NAMESPACE


//
// class TShouldDelete
// ~~~~~ ~~~~~~~~~~~~~
class TShouldDelete
{
  public:
    enum TDelete { NoDelete, DefDelete, Delete };
    typedef TDelete DeleteType;  // For compatibility

    TShouldDelete(TDelete dt = Delete);

    bool OwnsElements();
    void OwnsElements(bool del);
    bool DelObj(TDelete dt);

  private:
    TDelete ShouldDelete;  // Is either Delete or NoDelete
};

//----------------------------------------------------------------------------
// Inline implementation
//

inline TShouldDelete::TShouldDelete(TDelete dt)
{
  OwnsElements(dt != NoDelete);
}

inline bool TShouldDelete::OwnsElements()
{
  return ShouldDelete == Delete;
}

inline void TShouldDelete::OwnsElements(bool del)
{
  ShouldDelete = del ? Delete : NoDelete;
}

inline bool TShouldDelete::DelObj(TDelete dt)
{
  return dt == Delete || (dt==DefDelete && ShouldDelete==Delete);
}


__OWL_END_NAMESPACE


#endif  // OWL_SHDDEL_H
