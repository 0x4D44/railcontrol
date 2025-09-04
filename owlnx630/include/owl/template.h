//------------------------------------------------------------------------------88
// ObjectWindows
// Copyright (c) 1998 by Yura Bidus
//
//  $Revision: 1.31.2.2 $
//  $Author: jogybl $
//  $Date: 2010-03-24 11:54:27 $
//
// Definition of container classes used and made available by OWL
//------------------------------------------------------------------------------

#if !defined(OWL_TEMPLATE_H)
#define OWL_TEMPLATE_H

#if !defined(OWL_DEFS_H)
# include <owl/defs.h>
#endif
#if defined(BI_STD_RTL) || (__GNUC__ >= 3)
# include <new>
#else
# include <new.h>
#endif
#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif
//__OWL_BEGIN_NAMESPACE


// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>

#if !defined(BI_NO_DEFAULT_TEMPLATES)
#define OWL_TDFLT(d) = d 
#else
#define OWL_TDFLT(d) 
#endif

//-------------------------------------------------------------------------
// class TStandardAllocator
// ~~~~~ ~~~~~~~~~~~~~~~~~~
// Provides class-specific operator new and operator delete that simply call
// the global operator new and operator delete.  That is, TStandardAllocator
// does not provide any specialized behavior. It is used in the non-managed
// versions of the parametrized containers.
//
class TStandardAllocator {
  public:
    friend void * operator new(size_t sz, const TStandardAllocator&);

    //
    // debug support under MS VC++ 5/6
    //
#if defined(_DEBUG) && !defined(BI_OWL_NO_DEBUG_CRT)
    // for file name/line number tracking using DEBUG_NEW
    void* operator new(size_t nSize);
    void* operator new(size_t nSize, LPCSTR lpszFileName, int nLine);

    void operator delete(void* p);
#if _MSC_VER >= 1200
    void PASCAL operator delete(void *p, LPCSTR /*lpszFileName*/, int /*nLine*/);
#endif

    void * Alloc(size_t sz);
    void _OWLENTRY  Free(void *ptr);
    void * ReAlloc(void *ptr, size_t sz);

#if !defined(BI_NO_ARRAYNEW)
    friend void * operator new [] (size_t sz, const TStandardAllocator&);
    friend void * operator new [] (unsigned, void *ptr);
    void _OWLENTRY operator delete [] (void *ptr);
    void * operator new[](size_t sz);
#endif

#else
    void _OWLENTRY operator delete(void *ptr);
    void * operator new(size_t sz);

#if !defined(BI_NO_ARRAYNEW)
    friend void * operator new [] (size_t sz, const TStandardAllocator&);
#ifdef BI_COMP_FORTE
                friend void * operator new [] (unsigned, void *ptr) throw();
#elif (__GNUC__ >= 3)
    friend void * operator new [] (unsigned, void *ptr) throw();
#else
    friend void * operator new [] (unsigned, void *ptr);
#endif //else not BI_COMP_FORTE

    void _OWLENTRY operator delete [] (void  *ptr);
    void * operator new[](size_t sz);
#endif

    void * Alloc(size_t sz);
    void _OWLENTRY  Free(void *ptr);
    void * ReAlloc(void *ptr, size_t sz);

#endif

};

//
// inlines
//
#if defined(_DEBUG) && !defined(BI_OWL_NO_DEBUG_CRT)

inline void * operator new(size_t sz, const TStandardAllocator&)
  { return ::operator new(sz, _CLIENT_BLOCK, __FILE__, __LINE__); }

# if !defined(BI_NO_ARRAYNEW)

inline void * operator new [] (size_t sz, const TStandardAllocator&)
  { return ::operator new [] (sz, _CLIENT_BLOCK, __FILE__, __LINE__);}

#   if !defined(BI_COMP_GNUC) && !defined(BI_ARRAY_NEW_DEFINED)
inline void * operator new [] (unsigned, void *ptr)
  { return ptr; }
#   endif

# endif

inline void* TStandardAllocator::operator new(size_t nSize){  
  return ::operator new(nSize, _CLIENT_BLOCK, __FILE__, __LINE__); 
}
inline void* TStandardAllocator::operator new(size_t nSize, LPCSTR lpszFileName, int nLine){  
  return ::operator new(nSize, _CLIENT_BLOCK, lpszFileName, nLine); 
}
inline void TStandardAllocator::operator delete(void* p){ 
  _free_dbg(p, _CLIENT_BLOCK); 
}

# if _MSC_VER >= 1200

inline void PASCAL TStandardAllocator::operator delete(void *p, LPCSTR /*lpszFileName*/, int /*nLine*/){  
  _free_dbg(p, _CLIENT_BLOCK); 
}

# endif

inline void * TStandardAllocator::Alloc(size_t sz){ 
  return _malloc_dbg(sz,_CLIENT_BLOCK,__FILE__, __LINE__);
}
inline void _OWLENTRY TStandardAllocator::Free(void *ptr){ 
  _free_dbg(ptr,_CLIENT_BLOCK);                        
}
inline void * TStandardAllocator::ReAlloc(void *ptr, size_t sz){ 
  return _realloc_dbg(ptr,sz,_CLIENT_BLOCK,__FILE__, __LINE__);
}

# if !defined(BI_NO_ARRAYNEW)

inline void _OWLENTRY TStandardAllocator::operator delete [] (void *ptr){ 
  ::operator delete [] (ptr); 
}
inline void * TStandardAllocator::operator new[](size_t sz){ 
  return ::operator new[](sz);
}

# endif

#else

inline void * operator new(size_t sz, const TStandardAllocator&){
  return ::operator new(sz);  
}

#if !defined(BI_NO_ARRAYNEW)

inline void * operator new [] (size_t sz, const TStandardAllocator&){ 
  return ::operator new [] (sz);
}
#  if !defined(BI_COMP_GNUC) && !defined(BI_ARRAY_NEW_DEFINED) && !defined(BI_COMP_FORTE) && !defined(BI_COMP_ACC)
inline void * operator new [] (unsigned, void *ptr){ 
  return ptr;                 
}
#  endif //if not gnuc or Solaris forte compiler

#endif //if array new overloading allowed

inline void _OWLENTRY TStandardAllocator::operator delete(void *ptr){ 
  ::operator delete(ptr);     
}
inline void * TStandardAllocator::operator new(size_t sz){ 
  return ::operator new(sz);  
}

# if !defined(BI_NO_ARRAYNEW)

inline void _OWLENTRY TStandardAllocator::operator delete [] (void *ptr){ 
  ::operator delete [] (ptr); 
}
inline void * TStandardAllocator::operator new[](size_t sz){ 
  return ::operator new[](sz);
}

# endif

inline void * TStandardAllocator::Alloc(size_t sz){ 
  return ::malloc(sz);        
}
inline void _OWLENTRY TStandardAllocator::Free(void *ptr){ 
  ::free(ptr);                
}
inline void * TStandardAllocator::ReAlloc(void *ptr, size_t sz){ 
  return ::realloc(ptr,sz);   
}

#endif


//----------------------------------------------------------------------------
// class TSharedAllocator
// ~~~~~ ~~~~~~~~~~~~~~~~
// Provides class-specific operator new and operator delete that allocate from
// shared memory.
//
class TSharedAllocator {
  public:
    friend void * operator new(size_t sz, const TSharedAllocator&);

#if !defined(BI_NO_ARRAYNEW)
    friend void * operator new [](size_t sz, const TSharedAllocator&);
    void * operator new[](size_t sz);
    void operator delete [](void* ptr);
#endif

    void * operator new(size_t sz);
    void operator delete(void* ptr);

    void *   Alloc(size_t sz);
    void _OWLENTRY  Free(void *ptr);
    void *   ReAlloc(void *ptr, size_t sz);
};

//----------------------------------------------------------------------------
// TSharedAllocator::operator new
// TSharedAllocator::operator delete
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// When compiling for WINDOWS, allocates memory as GMEM_SHARE.
// When compiling for DOS, uses the global operator new and operator delete.                                                      */
//
#if !defined(OWL_PRIVATE_WSYSINC_H)
# include <owl/private/wsysinc.h>
#endif

inline void * operator new(size_t sz, const TSharedAllocator&){
  return ::operator new(sz);
}
inline void * TSharedAllocator::operator new(size_t sz){
  return ::operator new(sz);
}
inline void TSharedAllocator::operator delete(void * ptr){
  ::operator delete(ptr);
}
#if !defined(BI_NO_ARRAYNEW)
inline void * operator new [](size_t sz, const TSharedAllocator&){
  return ::operator new [](sz);
}
inline void * TSharedAllocator::operator new[](size_t sz){
  return ::operator new[](sz);
}
inline void TSharedAllocator::operator delete [](void * ptr){
  ::operator delete [](ptr);
}
#endif  //#if !defined(BI_NO_ARRAYNEW)
inline void * TSharedAllocator::Alloc(size_t sz){
  return malloc(sz);
}
inline void _OWLENTRY TSharedAllocator::Free(void *ptr){
  free(ptr);
}
inline void * TSharedAllocator::ReAlloc(void *ptr, size_t sz){
  return realloc(ptr,sz);
}



//----------------------------------------------------------------------------
// class TLocalAllocator
// ~~~~~ ~~~~~~~~~~~~~~~~
// Provides class-specific operator new and operator delete that allocate from
// local memory.
//
class TLocalAllocator {
  public:
    friend void * operator new(size_t sz, const TLocalAllocator&);

    void * operator new(size_t sz);
    void operator delete(void* ptr);

#if !defined(BI_NO_ARRAYNEW)
    friend void * operator new [](size_t sz, const TLocalAllocator&);
    void * operator new [](size_t sz);
    void operator delete [](void* ptr);
#endif

    void *   Alloc(size_t sz);
    void _OWLENTRY  Free(void *ptr);
    void *   ReAlloc(void *ptr, size_t sz);
};


//----------------------------------------------------------------------------
// TLocalAllocator::operator new
// TLocalAllocator::operator delete
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Uses LocalAlloc.
inline void * operator new(size_t sz, const TLocalAllocator&){
  return (void*)::LocalAlloc(LPTR, sz);
}
inline void * TLocalAllocator::operator new(size_t sz){
  return (void*)::LocalAlloc(LPTR, sz);
}
inline void TLocalAllocator::operator delete(void * ptr){
  ::LocalFree((HLOCAL)ptr);
}
#if !defined(BI_NO_ARRAYNEW)
inline void * operator new [](size_t sz, const TLocalAllocator&){
  return (void*)::LocalAlloc(LPTR, sz);
}
inline void * TLocalAllocator::operator new [](size_t sz){
  return (void*)::LocalAlloc(LPTR, sz);
}
inline void TLocalAllocator::operator delete [](void * ptr){
  ::LocalFree((HLOCAL)ptr);
}
#endif
inline void * TLocalAllocator::Alloc(size_t sz){
  return (void*)::LocalAlloc(LPTR, sz);
}
inline void _OWLENTRY TLocalAllocator::Free(void *ptr){
  ::LocalFree((HLOCAL)ptr);
}
inline void * TLocalAllocator::ReAlloc(void *ptr, size_t sz){
  return (void*)::LocalReAlloc((HLOCAL)ptr,sz,LPTR);
}

//----------------------------------------------------------------------------
// class TGlobalAllocator
// ~~~~~ ~~~~~~~~~~~~~~~~
// Provides class-specific operator new and operator delete that allocate from
// global memory.
//
class TGlobalAllocator {
  public:
    friend void * operator new(size_t sz, const TGlobalAllocator&);

    void * operator new(size_t sz);
    void operator delete(void* ptr);

#if !defined(BI_NO_ARRAYNEW)
    friend void * operator new [](size_t sz, const TGlobalAllocator&);
    void * operator new [](size_t sz);
    void operator delete [](void* ptr);
#endif

    void *   Alloc(size_t sz);
    void _OWLENTRY  Free(void *ptr);
    void *   ReAlloc(void *ptr, size_t sz);
};


//----------------------------------------------------------------------------
// TGlobalAllocator::operator new
// TGlobalAllocator::operator delete
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Uses GlobalAlloc.
inline void * operator new(size_t sz, const TGlobalAllocator&){
  return (void*)::GlobalLock(::GlobalAlloc(GPTR, sz));
}
inline void * TGlobalAllocator::operator new(size_t sz){
  return (void*)::GlobalLock(::GlobalAlloc(GPTR, sz));
}
inline void TGlobalAllocator::operator delete(void * ptr){
  HGLOBAL hMem = (HGLOBAL)GlobalHandle(ptr);
  if (GlobalUnlock(hMem) == 0)
    GlobalFree(hMem);
}
#if !defined(BI_NO_ARRAYNEW)
inline void * operator new [](size_t sz, const TGlobalAllocator&){
  return (void*)::GlobalLock(::GlobalAlloc(GPTR, sz));
}
inline void * TGlobalAllocator::operator new [](size_t sz){
  return (void*)::GlobalLock(::GlobalAlloc(GPTR, sz));
}
inline void TGlobalAllocator::operator delete [](void * ptr){
  HGLOBAL hMem = (HGLOBAL)GlobalHandle(ptr);
  if (GlobalUnlock(hMem) == 0)
    GlobalFree(hMem);
}
#endif
inline void * TGlobalAllocator::Alloc(size_t sz){
  return (void*)::GlobalLock(::GlobalAlloc(GPTR, sz));
}
inline void _OWLENTRY TGlobalAllocator::Free(void *ptr){
  HGLOBAL hMem = (HGLOBAL)GlobalHandle(ptr);
  if (GlobalUnlock(hMem) == 0)
    GlobalFree(hMem);
}
inline void * TGlobalAllocator::ReAlloc(void *ptr, size_t sz){
  HGLOBAL hMem = (HGLOBAL)GlobalHandle(ptr);
  if(hMem && GlobalUnlock(hMem) == 0)
    return (void*)::GlobalLock(GlobalReAlloc(hMem, sz, GPTR));
  return 0;
}
//--------------------------------------------------------
// class TOleAllocator 
// ~~~~~ ~~~~~~~~~~~~~
//
/*
class TOleAllocator {
  public:
    TOleAllocator();  // Gets allocator, does not initialize OLE
   ~TOleAllocator();
    void*  Alloc(unsigned long size);
    void          Free(void * pv);
    void * Realloc(void *ptr, size_t sz);
    IMalloc*  Mem;
};
*/
//

__OWL_BEGIN_NAMESPACE

//==========================================================================
// Memory Manager
// ~~~~~~ ~~~~~~~
//--------------------------------------------------------------------------
//  template <class Alloc> class TMBlockList                              
//  Used internally.                                                      
template <class A> class TMBaseMemBlocks;

template <class A> class TMBlockList {
  public:
    TMBlockList( TMBlockList * );
    ~TMBlockList(){}

  private:
    void *operator new( size_t, size_t, const A& );
    void operator delete( void * );
#if _MSC_VER >= 1200
    void operator delete(void *p, size_t, const A& );
#endif

    TMBlockList*  Next;
    friend class TMBaseMemBlocks<A>;
};

template <class A> inline 
TMBlockList<A>::TMBlockList(TMBlockList<A>* next) : Next(next)
{
}
template <class A> inline void* 
TMBlockList<A>::operator new (size_t sz, size_t extra, const A& a)
{
  return new(a)char[sz+extra];
}
template <class A> inline void 
TMBlockList<A>::operator delete ( void *ptr )
{
#if !defined(BI_NO_ARRAYNEW)
  A::operator delete [] (ptr);
#else
  A::operator delete (ptr);
#endif
}
#if _MSC_VER >= 1200
template <class A> inline void 
TMBlockList<A>::operator delete(void *p, size_t, const A& )
{
#if !defined(BI_NO_ARRAYNEW)
  A::operator delete [] (p);
#else
  A::operator delete (p);
#endif
}
#endif
//--------------------------------------------------------------------------
//  class TBlockList                                                      
//  ~~~~~ ~~~~~~~~~~ 
//  Used internally.                                                      

#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif

class TBlockList : public TMBlockList<TStandardAllocator> {
  public:
    TBlockList( TBlockList *blk) : TMBlockList<TStandardAllocator>(blk){}
};
//--------------------------------------------------------------------------
//  template <class A> class TMBaseMemBlocks                          
//  Used internally.                                                      
template <class A> class TMBaseMemBlocks :  public A {
  public:
    TMBaseMemBlocks( size_t = 8192 );
    ~TMBaseMemBlocks();

    char*     Block() const { return REINTERPRET_CAST(char*, CurBlock); }
    unsigned  Count() const { return BlockCount; }
    int       AllocBlock( size_t );
    void      FreeTo( unsigned );
    size_t    GetBlockSize() const { return BlockSize; }

  private:
    TMBlockList<A>* CurBlock;
    const size_t    BlockSize;
    unsigned        BlockCount;
};

template <class A> inline 
TMBaseMemBlocks<A>::TMBaseMemBlocks( size_t sz )
: CurBlock(0), BlockSize(sz), BlockCount(0)
{
  CHECK( sz != 0 );
}
template <class A> inline 
TMBaseMemBlocks<A>::~TMBaseMemBlocks()
{
#if !defined( BI_WINDOWS_WEP_BUG )
  FreeTo( 0 );
#endif
}
template <class A> int 
TMBaseMemBlocks<A>::AllocBlock( size_t sz )
{
  TMBlockList<A>* temp = new( __max(sz,BlockSize), *this ) TMBlockList<A>( CurBlock );
  CurBlock = temp+1;
  BlockCount++;
  return 1;
}

template <class A> void 
TMBaseMemBlocks<A>::FreeTo( unsigned term )
{
  PRECONDITION( BlockCount >= term );
  while( BlockCount > term ){
    TMBlockList<A> *temp = CurBlock-1;
    CurBlock = temp->Next;

    delete temp;
    BlockCount--;
  }
}

//--------------------------------------------------------------------------
//  class TBaseMemBlocks                                                  
//  Used internally.                                                      
class TBaseMemBlocks : public TMBaseMemBlocks<TStandardAllocator> {
  public:
    TBaseMemBlocks( size_t sz = 8192 ) :
        TMBaseMemBlocks<TStandardAllocator>(sz) {}
};
//--------------------------------------------------------------------------
//  template <class A> class TMMemStack                               
//                                                                        
//  Managed memory stack.  Implements mark and release style memory       
//  management, using the allocator A.                                
template <class A> class TMMarker;

template <class A> class TMMemStack{
  public:
    friend class TMMarker<A>;

    TMMemStack( size_t = 8192 );
    void* Allocate( size_t );

  private:
    TMBaseMemBlocks<A> Data;
    size_t CurLoc;
};

__OWL_END_NAMESPACE

template <class A> inline void* operator new( size_t sz, __OWL TMMemStack<A>& m )
{
  return m.Allocate( sz );
}

#if !defined(BI_NO_ARRAYNEW)
template <class A> inline void *operator new [] ( size_t sz, __OWL TMMemStack<A>& m )
{
  return m.Allocate( sz );
}
#endif

__OWL_BEGIN_NAMESPACE

template <class A> inline TMMemStack<A>::TMMemStack( size_t sz ) 
: Data( sz ),CurLoc(sz)
{
  CHECK( sz != 0 );
}

template <class A> void* TMMemStack<A>::Allocate( size_t sz )
{
  sz = __max( 1U, sz );
  if( sz > Data.GetBlockSize() - CurLoc )
  {
    if( Data.AllocBlock( sz ) == 0 )
      return 0;
    else
      CurLoc = 0;
  }
  void* temp = Data.Block() + CurLoc;
  CurLoc += sz;
  return temp;
}

//--------------------------------------------------------------------------
//  template <class A> class TMMarker                                 
//  Provides the mark for TMMemStack.                                     
template <class A> class TMMarker{
  public:
    TMMarker(TMMemStack<A>& ms):
        Memstk(ms),Blk(ms.Data.Count()),CurLoc(ms.CurLoc)
      {}

    ~TMMarker()
      {
        PRECONDITION( Blk < Memstk.Data.Count() || 
               (Blk == Memstk.Data.Count() && CurLoc <= Memstk.CurLoc ));
        Memstk.Data.FreeTo( Blk );
        Memstk.CurLoc = CurLoc;
      }
  private:
    TMMemStack<A>&  Memstk;
    const unsigned  Blk;
    const size_t    CurLoc;
};

//--------------------------------------------------------------------------
//  class TMemStack                                                       
//  ~~~~~ ~~~~~~~~~
//  Implements mark and release style memory management using the         
//  standard allocator.                                                   
class TMemStack : public TMMemStack<TStandardAllocator> {
  public:
    TMemStack( size_t sz = 8192 ) : TMMemStack<TStandardAllocator>(sz) {}
};

//--------------------------------------------------------------------------
//  template <class A> class TMarker                                  
//  Provides the mark for TMemStack.                                      
class TMarker : public TMMarker<TStandardAllocator>{
  public:
    TMarker( TMMemStack<TStandardAllocator>& ms ):
        TMMarker<TStandardAllocator>(ms) {}
};

//--------------------------------------------------------------------------
//  template <class A> class TMMemBlocks
//  ~~~~~~~~ ~~~~~~~~~ ~~~~~ ~~~~~~~~~~~
//  Managed single-size block allocator.  Allocates blocks
//  of the size specified in the constructor, using the memory
//  manager specified by A.
template <class A> class TMMemBlocks{
  public:
    TMMemBlocks( size_t sz, unsigned count = 100 );
    void* Allocate( size_t sz);
    void  Free( void * block);

  private:
    void*         FreeList;
    TMMemStack<A> Mem;
    size_t        Size;
};

template <class A> inline 
TMMemBlocks<A>::TMMemBlocks( size_t sz, unsigned int count ) 
: FreeList(0), Mem( sz*count ), Size( __max(sz,sizeof(void*)))
{
  CHECK( sz != 0 && count != 0 );
}

template <class A> inline void*
#if (__DEBUG > 0)
TMMemBlocks<A>::Allocate( size_t sz )
#else
TMMemBlocks<A>::Allocate( size_t )
#endif
{
  PRECONDITION( Size == __max(sz,sizeof(void *)) );
  if( FreeList == 0 )
    return Mem.Allocate( Size );
  else{
    void *temp = FreeList;
    FreeList = *(void**)temp;
    return temp;
  }
}

template <class A> inline void 
TMMemBlocks<A>::Free( void* block )
{
    *(void **)block = FreeList;
    FreeList = block;
}

//--------------------------------------------------------------------------
//  class TMemBlocks                                                      
//                                                                        
//  Single-size block allocator.  Allocates blocks of the size            
//  specified in the constructor, using the global operator new           
//  and operator delete.                                                  
class TMemBlocks : public TMMemBlocks<TStandardAllocator>{
  public:
    TMemBlocks( size_t sz, unsigned n = 100 ) :
        TMMemBlocks<TStandardAllocator>( sz, n ) {}

};

typedef TMMemBlocks<TStandardAllocator> TStandardBlocks;

// support template functions
template <class T> inline T* __owl_construct(void* P)
{
  return new(P)T;
}
template <class T> inline T* __owl_construct(void* P, const T& t)
{
  return new(P)T(t);
}
template <class T> inline void __owl_destruct(T* t)
{
  t->~T();
}
// specializations
// chars
inline char* __owl_construct(void* p, char c)                { *((char*)p) = c; return (char*)p;}
inline void __owl_destruct(char* /*t*/){}
// integers
inline int* __owl_construct(void* p, int c)                  { *((int*)p) = c; return (int*)p;}
inline unsigned int* __owl_construct(void* p, unsigned int c){  *((unsigned int*)p) = c; return (unsigned int*)p;}
inline void __owl_destruct(int* /*t*/){}
// long
inline long* __owl_construct(void* p, long c)                 { *((long*)p) = c; return (long*)p;}
inline void __owl_destruct(long* /*t*/){}

//--------------------------------------------------------------------------
// Container classes
//

/////////////
//
//class Iterator for TObjectContainer
//
template<class T, class T1> class TObjArrayIterator{
  public:
    TObjArrayIterator(T1& array):Vect(array),Cur(0){}
    operator bool() const         { return Cur < (int)Vect.Size();  }
    const T& operator ++(int)     { Cur++; return *Vect.Data[Cur-1];}
    const T* operator ++()        { return Cur<(int)Vect.Size()?Vect.Data[++Cur]:0; }   //??????????????
    const T& operator *() const   { return *Vect.Data[Cur];         }
    T& operator *()               { return *Vect.Data[Cur];         }
    const T& Current() const      { return *Vect.Data[Cur];         }
    T& Current()                  { return *Vect.Data[Cur];         }
    void  Restart()               { Cur = 0;                        }

  protected:
    T1&   Vect;
    int   Cur;
};

//
// Iterator for Pointer Container
//
template<class T, class T1> class TPtrArrayIterator{
  public:
    TPtrArrayIterator(T1& array):Vect(array),Cur(0){}
    operator bool() const        { return Cur < (int)Vect.Size();       }
    const T  operator ++(int)    { Cur++; return Vect.Data[Cur-1];      }
    const T* operator ++()       { return Cur<(int)Vect.Size()?&Vect.Data[++Cur]:0;} //?????????????
    const T  operator *() const  { return Vect.Data[Cur];               }
          T  operator *()        { return Vect.Data[Cur];               }
    const T  Current() const     { return Vect.Data[Cur];               }
          T  Current()           { return Vect.Data[Cur];               }
    void     Restart()           { Cur = 0;                             }

  protected:
    T1& Vect;
    int Cur;
};

//
// TMapNode
//
// Usage:
//  typedef TMapNode<string,string>                         TStringMapNode;
//  typedef TSortedObjectArray< TStringMapNode >            TStringMap;
//  typedef TSortedObjectArray< TStringMapNode >::Iterator  TStringMapIterator;
//
template<class T1, class T2> class TMapNode {
  public:
    TMapNode(T1& name, T2& val):Name(name),Value(val){}
    TMapNode(T1& name):Name(name){}
    TMapNode(const TMapNode<T1,T2>& node):Name(node.Name),Value(node.Value){}
    TMapNode(){}
    ~TMapNode(){}

    bool operator <(const TMapNode& ms) const   { return  Name < ms.Name; }
    bool operator ==(const TMapNode& ms) const  { return  Name == ms.Name; }
    TMapNode& operator =(const TMapNode& other)
      {
        Name  = other.Name;
        Value = other.Value;
        return *this;
      }

    T1 Name;
    T2 Value;
};

//------------------------------------------------------------------------------
// base array class -> reduse code size
class TArrayBase {
  public:
    TArrayBase():ItemCnt(0),Reserved(0){}

    uint    GetItemsInContainer() const { return Size();        }
    int     LowerBound() const    { return 0;                   }
    int     UpperBound() const    { return Size()-1;            }
    uint    ArraySize()  const    { return Reserved;            }
    bool    IsFull() const        { return ItemCnt>=Reserved-1; }
    bool    IsEmpty() const       { return Empty();             }
    uint    Size()  const         { return ItemCnt;             }
    uint    Count() const         { return ItemCnt;             }
    bool    Empty() const         { return ItemCnt == 0;        }
    // support STL name convention
    uint    size()  const         { return ItemCnt;             }
    bool    empty() const         { return Empty();             }

  protected:
    uint  ItemCnt;     
    uint  Reserved;    
};

#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif



//------------------------------------------------------------------------------
//TMObjectArray - holded aray pointers to strings
//------------------------------------------------------------------------------
// ISO/IEC C++ standard says:
//   11.4 par.2: elaborated-type-specifier shall be used in a friend
//     declaration for a class. elaborated-type-specifier must have class/struct
//     specifier.
//   7.1.5.3 par.2
//     The identifier in elaborated-type-specifier cannot resolve to typedef-name
//     or template type-parameter.
// This is the reason we duplicate the iterator definitions in friend declarations.
//------------------------------------------------------------------------------
#if !defined(BI_NO_TMPL_ALLOCATOR)
template<class T, class A OWL_TDFLT(TStandardAllocator) > class TMObjectArray : public A,  public TArrayBase {
#else
template<class T> class TObjectArray: public TArrayBase {
#endif
  public:
    typedef void (*IterFunc)(T&, void*);
    typedef bool (*CondFunc)(const T&, void*);

#if !defined(BI_NO_TMPL_ALLOCATOR)
    typedef TObjArrayIterator<T, TMObjectArray<T, A> > Iterator;
    friend class TObjArrayIterator<T, TMObjectArray<T, A> >;
#else
    typedef TObjArrayIterator<T, TObjectArray<T> > Iterator;
    friend class TObjArrayIterator<T, TObjectArray<T> >;
#endif
    typedef Iterator                                   iterator;

#if !defined(BI_NO_TMPL_ALLOCATOR)
    _OWLCTOR TMObjectArray(int upper, int lower=0, int delta=0)
             : Mem(sizeof(T),upper-lower+1),Data(0){ Resize(upper);}
    _OWLCTOR TMObjectArray() : Mem(sizeof(T),10), Data(0){}
    _OWLCTOR ~TMObjectArray();
    _OWLCTOR TMObjectArray(const TMObjectArray<T,A>& array);
    TMObjectArray<T,A>& operator=(const TMObjectArray<T,A>& array);
    TMMemBlocks<A>& GetAllocator() { return Mem; }
#else
    _OWLCTOR TObjectArray(int upper, int /*lower*/=0, int delta=0)
             : Data(0){ Resize(upper);}
    _OWLCTOR TObjectArray() : Data(0){}
    _OWLCTOR ~TObjectArray();
    _OWLCTOR TObjectArray(const TObjectArray<T>& array);
    TObjectArray<T>&  operator=(const TObjectArray<T>& array);
#endif

    void  RemoveEntry( int loc )       { Remove(loc);         }
    void  SqueezeEntry( unsigned loc ) { Remove(loc);         }
    void  Grow(int index);

    int   Add(const T& t);
    void  AddAt(const T& t, uint index);
#if defined(BI_NO_TMPL_ALLOCATOR)
    bool  DetachItem(const T& t);
    bool  Detach(int loc);
#endif
    bool  DestroyItem(const T& t);
    bool  Destroy(int loc);
    bool  HasMember(const T& t) const;
    int   Find(const T& t) const;
    
          T& operator [](int loc);
    const T& operator [](int loc) const;

    void  ForEach(IterFunc iter, void* args);
    T*    FirstThat(CondFunc cond, void* args) const;
    T*    LastThat(CondFunc cond, void* args) const;
    
    void Flush();

  protected:
    void Resize(int delta);
    void Remove(int index);
#if !defined(BI_NO_TMPL_ALLOCATOR)
    bool  DetachItem(const T& t);
    bool  Detach(int loc);
#endif

  protected:
    T**                 Data;        
#if !defined(BI_NO_TMPL_ALLOCATOR)
    TMMemBlocks<A>  Mem;
#endif
};
//------------------------------------------------------------------------------
#if !defined(BI_NO_TMPL_ALLOCATOR)
template<class T> class TObjectArray: public TMObjectArray<T,TStandardAllocator> {
  public:
    typedef void (*IterFunc)(T&, void*);
    typedef bool (*CondFunc)(const T&, void*);

    typedef TObjArrayIterator<T, TObjectArray<T> > Iterator;
    typedef Iterator                               iterator;
		friend Iterator;

    _OWLCTOR TObjectArray(int upper, int lower=0, int delta=0)
      : TMObjectArray<T,TStandardAllocator>(upper,lower,delta){}
    _OWLCTOR TObjectArray(){}
    _OWLCTOR ~TObjectArray(){}
    _OWLCTOR TObjectArray(const TObjectArray<T>& array):TMObjectArray<T,TStandardAllocator>(array){}
    TObjectArray<T>& operator=(const TObjectArray<T>& array)
      { 
        TMObjectArray<T,TStandardAllocator>::operator=(array); 
        return *this;
      }
};
#endif // BI_NO_TMPL_ALLOCATOR

//------------------------------------------------------------------------------
#if !defined(BI_NO_TMPL_ALLOCATOR)
template<class T, class A OWL_TDFLT(TStandardAllocator) > class TMSortedObjectArray : public TMObjectArray<T,A> {
#else
template<class T> class TSortedObjectArray : public TObjectArray<T> {
#endif
  public:
#if !defined(BI_NO_TMPL_ALLOCATOR)
    typedef TObjArrayIterator<T, TMSortedObjectArray<T, A> > Iterator;
    friend class TObjArrayIterator<T, TMSortedObjectArray<T, A> >;
#else
    typedef TObjArrayIterator<T, TSortedObjectArray<T> > Iterator;
    friend class  TObjArrayIterator<T, TSortedObjectArray<T> >;
#endif
    typedef Iterator  iterator;

#if !defined(BI_NO_TMPL_ALLOCATOR)
    _OWLCTOR TMSortedObjectArray(int upper, int lower=0, int delta=0)
                       : TMObjectArray<T,A>(upper,lower,delta){}
    _OWLCTOR TMSortedObjectArray() : TMObjectArray<T,A>(){}
    _OWLCTOR ~TMSortedObjectArray(){}
    _OWLCTOR TMSortedObjectArray(const TMSortedObjectArray<T,A>& array);
    TMSortedObjectArray<T,A>& operator=(const TMSortedObjectArray<T,A>& array);
#else
    _OWLCTOR TSortedObjectArray(int upper, int lower=0, int delta=0)
                       : TObjectArray<T>(upper,lower,delta){}
    _OWLCTOR TSortedObjectArray() : TObjectArray<T>(){}
    _OWLCTOR ~TSortedObjectArray(){}
    _OWLCTOR TSortedObjectArray(const TSortedObjectArray<T>& array);
    TSortedObjectArray<T>& operator=(const TSortedObjectArray<T>& array);
#endif
    int   Add(const T& t);
#if defined(BI_NO_TMPL_ALLOCATOR)
    bool  DetachItem(const T& t);
#endif
    bool  DestroyItem(const T& t);
//    bool  Destroy(int loc);
    bool  HasMember(const T& t) const;
    int   Find(const T& t) const;

  protected:
    int   AddAt(const T& t, int index){ return 0;} // hide
#if !defined(BI_NO_TMPL_ALLOCATOR)
    bool  DetachItem(const T& t);
#endif
};

//------------------------------------------------------------------------------
#if !defined(BI_NO_TMPL_ALLOCATOR)
template<class T> class TSortedObjectArray: public TMSortedObjectArray<T,TStandardAllocator> {
  public:
    typedef void (*IterFunc)(T&, void*);
    typedef bool (*CondFunc)(const T&, void*);

    typedef TObjArrayIterator<T, TSortedObjectArray<T> > Iterator;
    typedef Iterator  iterator;
    friend Iterator;

    _OWLCTOR TSortedObjectArray(int upper, int lower=0, int delta=0)
      : TMSortedObjectArray<T,TStandardAllocator>(upper,lower,delta){}
    _OWLCTOR TSortedObjectArray(){}
    _OWLCTOR ~TSortedObjectArray(){}
    _OWLCTOR TSortedObjectArray(const TSortedObjectArray<T>& array):TMSortedObjectArray<T,TStandardAllocator>(array){}
    TSortedObjectArray<T>& operator=(const TSortedObjectArray<T>& array)
      {
        TMSortedObjectArray<T,TStandardAllocator>::operator=(array);
        return *this;
      }
};
#endif // BI_NO_TMPL_ALLOCATOR

//------------------------------------------------------------------------------
// store simple types/objects, normally used to store pointers - Jogy.
template<class T, class R, class A OWL_TDFLT(TStandardAllocator) > class TTypedArray : public A, public TArrayBase {
  public:
    typedef void (*IterFunc)(R, void*);
    typedef bool (*CondFunc)(R, void*);

    typedef TPtrArrayIterator<R, TTypedArray<T, R, A> > Iterator;
    friend class TPtrArrayIterator<R, TTypedArray<T, R, A> >;
    typedef Iterator  iterator;

    _OWLCTOR TTypedArray(int upper, int /*lower*/=0, int /*delta*/=0)
                       :Data(0) {Resize(upper);}
    _OWLCTOR TTypedArray():Data(0) {}
    _OWLCTOR ~TTypedArray() { if(Data) Free(Data);  }
    _OWLCTOR TTypedArray(const TTypedArray<T,R,A>& array);
    TTypedArray<T,R,A>& operator=(const TTypedArray<T,R,A>& array);

    void  RemoveEntry( int loc ){ Remove(loc);                }
    void  SqueezeEntry( unsigned loc ) { Remove(loc);         }
    void  Grow(int index);

    int   Add(R t);
    void  AddAt(R t, uint index);
    bool  DetachItem(R t);
    bool  Detach(int loc);
    bool  DestroyItem(R t);
    bool  Destroy(int loc);
    bool  HasMember(R t) const;
    int   Find(R t) const;

    T&    operator [](int loc);
    T&    operator [](int loc) const;

    void  ForEach(IterFunc iter, void* args);
    T*    FirstThat(CondFunc cond, void* args) const;
    T*    LastThat(CondFunc cond, void* args) const;

    void  Flush();

  protected:
    void  Resize(int delta);
    void  Remove(int index);

  protected:
    T*    Data;
};
//------------------------------------------------------------------------------
template<class T, class R, class A OWL_TDFLT(TStandardAllocator) > class TSTypedArray : public TTypedArray<T,R,A> {
  public:
    typedef TPtrArrayIterator<R, TSTypedArray<T,R,A> > Iterator;
    friend class TPtrArrayIterator<R, TSTypedArray<T,R,A> >;
    typedef Iterator  iterator;

    _OWLCTOR TSTypedArray(int upper, int lower=0, int delta=0)
                       : TTypedArray<T,R,A>(upper,lower,delta){}
    _OWLCTOR TSTypedArray() : TTypedArray<T,R,A>(){}
    _OWLCTOR ~TSTypedArray(){}
    _OWLCTOR TSTypedArray(const TSTypedArray<T,R,A>& array);
    TSTypedArray<T,R,A>& operator=(const TSTypedArray<T,R,A>& array);

    int   Add(R t);
    bool  DetachItem(R t);
    bool  DestroyItem(R t);
    bool  Destroy(int loc);
    bool  HasMember(R t) const;
    int   Find(R t) const;

  private:
    int AddAt(R t, int index); // hide
};
//------------------------------------------------------------------------------
//@class TPtrArray - array of pointers of simple types
template<class T> class TPtrArray : public TTypedArray<T,T,TStandardAllocator>{
  public:
    _OWLCTOR TPtrArray(int upper, int lower=0, int delta=0)
                      :TTypedArray<T,T,TStandardAllocator>(upper,lower,delta){}
    _OWLCTOR TPtrArray():TTypedArray<T,T,TStandardAllocator>(){}
    _OWLCTOR TPtrArray(const TPtrArray<T>& array):TTypedArray<T,T,TStandardAllocator>(array){}
};
//------------------------------------------------------------------------------
//@class TPtrArray - array of pointers of simple types
template<class T> class TSortedPtrArray : public TSTypedArray<T,T,TStandardAllocator>{
  public:
    _OWLCTOR TSortedPtrArray(int upper, int lower=0, int delta=0)
                      :TSTypedArray<T,T,TStandardAllocator>(upper,lower,delta){}
    _OWLCTOR TSortedPtrArray():TSTypedArray<T,T,TStandardAllocator>(){}
    _OWLCTOR TSortedPtrArray(const TSortedPtrArray<T>& array):TSTypedArray<T,T,TStandardAllocator>(array){}
};
//------------------------------------------------------------------------------
//
template<class T, class R, class A OWL_TDFLT(TStandardAllocator) > class TMIPtrArray: public TTypedArray<T,R,A> {
  public:
    typedef TPtrArrayIterator<R, TMIPtrArray<T,R,A> > Iterator;
    friend class TPtrArrayIterator<R, TMIPtrArray<T,R,A> >;
    typedef Iterator  iterator;

    _OWLCTOR TMIPtrArray(int upper, int lower=0, int delta=0);
    _OWLCTOR TMIPtrArray();
    _OWLCTOR ~TMIPtrArray();
    _OWLCTOR TMIPtrArray(const TMIPtrArray<T,R,A>& array);
    TMIPtrArray<T,R,A>& operator=(const TMIPtrArray<T,R,A>& array);

    bool  DestroyItem(R t);
    bool  Destroy(int loc);
    void  Flush(bool del = true);
};
//------------------------------------------------------------------------------
//
template<class T> class TIPtrArray: public TPtrArray<T> {
  public:
    typedef TPtrArrayIterator<T, TIPtrArray<T> > Iterator;
    friend class TPtrArrayIterator<T, TIPtrArray<T> >;
    typedef Iterator  iterator;

    _OWLCTOR TIPtrArray(int upper, int lower=0, int delta=0);
    _OWLCTOR TIPtrArray();
    _OWLCTOR ~TIPtrArray();
    _OWLCTOR TIPtrArray(const TIPtrArray<T>& array);
    TIPtrArray<T>& operator=(const TIPtrArray<T>& array);

    bool  DestroyItem(T t);
    bool  Destroy(int loc);
    void  Flush(bool del = true);
};
//
//
//
template<class T> class TISortedPtrArray: public TSortedPtrArray<T> {
  public:
    typedef TPtrArrayIterator<T, TISortedPtrArray<T> > Iterator;
    friend class TPtrArrayIterator<T, TISortedPtrArray<T> >;
    typedef Iterator  iterator;

    _OWLCTOR TISortedPtrArray(int upper, int lower=0, int delta=0);
    _OWLCTOR TISortedPtrArray();
    _OWLCTOR TISortedPtrArray(const TISortedPtrArray<T>& array);
    _OWLCTOR ~TISortedPtrArray();
    TISortedPtrArray<T>& operator=(const TISortedPtrArray<T>& array);

    bool  DestroyItem(T t);
    bool  Destroy(int loc);
    void  Flush(bool del = true);
};
//------------------------------------------------------------------------------
template<class T> class TBaseNode {
  public:
    TBaseNode(const T& data):Next(0),Prev(0),Data(data){}

    TBaseNode*  Next;
    TBaseNode*  Prev;
    T           Data;
};

// forward declaration
template<class T> class TBaseList;

//@class Iterator
template<class T> class TBaseListIterator{
  public:
    TBaseListIterator(TBaseList<T>& list) : List(list),Cur(list.First){}
    operator bool() const         { return Cur != 0;                }
    const T& operator ++(int)     { TBaseNode<T>* tmp = Cur; Cur = Cur->Next; return tmp->Data; }
    const T* operator ++()        { Cur = Cur->Next; return Cur ? &Cur->Data : 0;  } //?????
    const T& operator --(int)     { TBaseNode<T>* tmp = Cur; Cur = Cur->Prev; return tmp->Data; }
    const T* operator --()        { Cur = Cur->Prev; return Cur ? &Cur->Data : 0;  } //?????
    const T& operator *() const   { return Cur->Data;               }
    const T& Current() const      { return Cur->Data;               }
    void  Restart()               { Cur = List.First;               }


    TBaseList<T>&   List;
    TBaseNode<T>*   Cur;
};

//------------------------------------------------------------------------------
//@class TBaseList ->
template<class T> class TBaseList {
  public:
    typedef void (*IterFunc)(T&, void*);
    typedef bool (*CondFunc)(const T&, void*);

    typedef TBaseListIterator<T> Iterator;
    friend class TBaseListIterator<T>;
    typedef Iterator  iterator;

  public:
    _OWLCTOR TBaseList();
    _OWLCTOR TBaseList(int /*upper*/, int lower=0, int delta=0);
    _OWLCTOR ~TBaseList();

    int     LowerBound() const    { return 0;                   }
    uint    Size()  const         { return ItemSize;            }
    int     UpperBound() const    { return Size()-1;            }
    uint    ArraySize()  const    { return Size();              }
    bool    IsFull() const        { return true;                }
    bool    Empty() const         { return First == 0;          }
    bool    IsEmpty() const       { return Empty();             }
    uint    GetItemsInContainer() const { return Size();        }


    //@cmember add new item at end of list
    void  Add(const T& data);
    //@cmember add new item after iterator, if 0 add before first
    void  Add(Iterator* iter, const T& data);
    // stack
    //@cmember add new item at begin of list
    void  Push(const T& data);
    //@cmember add new item at end of list
    void  PushBack(const T& data);
    //@cmember remove 1 item return data item
    T     Pop();
    //@cmember return top of list
    const T&  Top();
    //@cmember return bottom of array
    const T&  Bottom();

    //@cmember Flush all items
    void Flush();
    //@cmember DetachItem( detach item and delete it
    bool  DetachItem(const T& t);
    //@cmember Detach( but and delete it
    bool Detach(int loc);
    //@cmember check item in container
    bool HasMember(const T& t) const;
    //@cmember Find
    TBaseNode<T>* Find(const T& t) const;
    //@cmember iterates all strings
    void ForEach(IterFunc iter, void* args);
    //@cmember FirstThat( return first member
    T* FirstThat(CondFunc cond, void* args) const;
    //@cmember LastThat return last member
    T* LastThat(CondFunc cond, void* args) const;

  protected:
    //@cmember remove item from list not delete node
    bool  Remove(TBaseNode<T>* t);
    //@cmember add new item after item, if 0 add before first return index
    int   Add(TBaseNode<T>* node, const T& data);
    //@cmember add new item at end of list return index
    int   Add(TBaseNode<T>* node);
    //@cmember add new item after item, if 0 add before first
    int   Add(TBaseNode<T>* item, TBaseNode<T>* node);

  protected:
    TBaseNode<T>* First;
    TBaseNode<T>* Last;
    uint          ItemSize; // optimization for Size()
};

//------------------------------------------------------------------------------
template<class T, class T1> class TTernaryNode {
  public:
    TTernaryNode(T split,const T1& data):Data(data),Split(split),LoId(0),EqId(0),HiId(0){}

    T             Split;
    TTernaryNode* LoId;
    TTernaryNode* EqId;
    TTernaryNode* HiId;
    T1            Data;
};

//@class TBaseList ->
template<class T, class T1> class TTernaryTree {
  //@access Public Functions
  public:
    //typedef TTernaryTreeIterator<T> Iterator;
    //friend  Iterator;

  public:
    _OWLCTOR TTernaryTree():Root(0){}
    _OWLCTOR ~TTernaryTree(){}

    T1* Find(T* s)
        {
          return RSearch(Root, s);
        }
    void Insert(T* s, const T1& data)
        {
          Root = RInsert(Root, s, data);
        }
    void Flush()
        {
          Flush(Root);
        }
  protected:
    T1* RSearch(TTernaryNode<T,T1>* p, T* s)
        {
          if(!p)
            return 0;
          if(*s < p->Split)
            return RSearch(p->LoId, s);
          else if(*s > p->Split)
            return RSearch(p->HiId, s);
          else{
            if(*s == 0)
              return &p->Data;
            return RSearch(p->EqId, ++s);
          }
        }
    TTernaryNode<T,T1>* RInsert(TTernaryNode<T,T1>* p, T* s, const T1& data)
        {
          if(!p)
            p = new TTernaryNode<T,T1>(*s, data);
          if(*s < p->Split)
            p->LoId = RInsert(p->LoId, s, data);
          else if(*s == p->Split){
            if(*s != 0)
              p->EqId = RInsert(p->EqId, ++s, data);
          }
          else
            p->HiId = RInsert(p->HiId, s, data);
          ItemCnt++;
          return p;
        }
    void Flush(TTernaryNode<T,T1>* p)
        {
          if(p){
            Flush(p->LoId);
            Flush(p->EqId);
            Flush(p->HiId);
            delete p;
          }
        }
  protected:
    TTernaryNode<T,T1>* Root;
    uint  ItemCnt;        //@field item count
};

#include <owl/posclass.h>

//----------------------------------------------------------------------------

//
// inlines
//
// TMObjectArray
// ~~~~~~~~~~~~~
//
#if !defined(BI_NO_TMPL_ALLOCATOR)
template<class T, class A> _OWLCTOR TMObjectArray<T,A>::TMObjectArray(const TMObjectArray& array)
:Mem(sizeof(T),__max(10,array.Reserved/10)),Data(0) 
#else
template<class T> _OWLCTOR TObjectArray<T>::TObjectArray(const TObjectArray& array):Data(0)
#endif
{
  if(array.Reserved)
    Resize(array.Reserved);
  for(int i = 0; i < (int)array.ItemCnt; i++){
#if !defined(BI_NO_TMPL_ALLOCATOR)
    Data[i] = __owl_construct<T>(Mem.Allocate(sizeof(T)),*array.Data[i]);
#else
    Data[i] = new T(*array.Data[i]);
#endif
  }
  ItemCnt = array.ItemCnt;
}

//
#if !defined(BI_NO_TMPL_ALLOCATOR)
template<class T, class A> inline _OWLCTOR TMObjectArray<T,A>::~TMObjectArray()
#else
template<class T> inline _OWLCTOR TObjectArray<T>::~TObjectArray()
#endif
{
  Flush();
  if(Data)
#if !defined(BI_NO_TMPL_ALLOCATOR)
    Free(Data);
#else
    free(Data);
#endif
}

//
#if !defined(BI_NO_TMPL_ALLOCATOR)
template<class T, class A> TMObjectArray<T,A>& TMObjectArray<T,A>::operator=(const TMObjectArray<T,A>& array)
#else
template<class T> TObjectArray<T>& TObjectArray<T>::operator=(const TObjectArray<T>& array)
#endif
{
  if(this == &array)
    return *this;

  Flush();
  if(array.ItemCnt >= Reserved)
    Resize(array.ItemCnt - Reserved);
  for(int i = 0; i < (int)array.ItemCnt; i++){
#if !defined(BI_NO_TMPL_ALLOCATOR)
    Data[i] = __owl_construct<T>(Mem.Allocate(sizeof(T)),*array.Data[i]);
#else
    Data[i] = new T(*array.Data[i]);
#endif
  } 
  ItemCnt = array.ItemCnt;

  return *this;
}

//
#if !defined(BI_NO_TMPL_ALLOCATOR)
template<class T, class A> inline int TMObjectArray<T,A>::Add(const T& t)
#else
template<class T> inline int TObjectArray<T>::Add(const T& t)
#endif
{
  if(ItemCnt>=Reserved)
    Resize(ItemCnt+1);
#if !defined(BI_NO_TMPL_ALLOCATOR)
  Data[ItemCnt] = __owl_construct<T>(Mem.Allocate(sizeof(T)),t);
#else
  Data[ItemCnt] = new T(t);
#endif
  return ItemCnt++; // if fail -> throw xalloc
}
//
#if !defined(BI_NO_TMPL_ALLOCATOR)
template<class T, class A> void TMObjectArray<T,A>::AddAt(const T& t, uint index)
#else
template<class T> inline void TObjectArray<T>::AddAt(const T& t, uint index)
#endif
{
  if(index == Size()){
    Add(t);
    return;
  }
  if(index >=Reserved)
    Resize(index+1);    // on error -> throw xalloc
  if(ItemCnt>=Reserved)
    Resize(ItemCnt+1);  // on error -> throw xalloc
  if(index < ItemCnt && ItemCnt)
    memmove(&Data[index+1], &Data[index], sizeof(T*)*(ItemCnt-index));
#if !defined(BI_NO_TMPL_ALLOCATOR)
  Data[index] = __owl_construct<T>(Mem.Allocate(sizeof(T)),t);
#else
  Data[index] = new T(t);
#endif
  ItemCnt++;
}
//
#if defined(BI_NO_TMPL_ALLOCATOR)
template<class T> inline bool TObjectArray<T>::DetachItem(const T& t)
{
  int index = Find(t);
  if(index != (int)NPOS){
    Remove(index);
    return true;
  }
  return false;
}

//
template<class T> inline bool TObjectArray<T>::Detach(int loc)
{
  if(loc < (int)Size()){
    Remove(loc);
    return true;
  }
  return false;
}
#endif

//
#if !defined(BI_NO_TMPL_ALLOCATOR)
template<class T, class A> inline bool TMObjectArray<T,A>::DestroyItem(const T& t)
#else
template<class T> inline bool TObjectArray<T>::DestroyItem(const T& t)
#endif
{
  int index = Find(t);
  if(index != (int)NPOS){
    T* tmp = Data[index];
    Remove(index);
#if !defined(BI_NO_TMPL_ALLOCATOR)
    __owl_destruct(tmp);// call destructor
    Mem.Free(tmp);
#else
    delete tmp;
#endif
    return true;
  }
  return false;
}
//
#if !defined(BI_NO_TMPL_ALLOCATOR)
template<class T, class A> inline bool TMObjectArray<T,A>::Destroy(int loc)
#else
template<class T> inline bool TObjectArray<T>::Destroy(int loc)
#endif
{
  if(loc < (int)Size()){
    T* tmp = Data[loc];
    Remove(loc);
#if !defined(BI_NO_TMPL_ALLOCATOR)
    __owl_destruct(tmp);// call destructor
    Mem.Free(tmp);
#else
    delete tmp;
#endif
    return true;
  }
  return false;
}

//
#if !defined(BI_NO_TMPL_ALLOCATOR)
template<class T, class A> inline bool TMObjectArray<T,A>::HasMember(const T& t) const
#else
template<class T> inline bool TObjectArray<T>::HasMember(const T& t) const
#endif
{
  return Find(t) != (int)NPOS;
}
//
#if !defined(BI_NO_TMPL_ALLOCATOR)
template<class T, class A> int TMObjectArray<T,A>::Find(const T& t) const
#else
template<class T> int TObjectArray<T>::Find(const T& t) const
#endif
{
  for(int i = 0; i < (int)Size(); i++)
    if(*Data[i] == t)
      return i;
  return (int)NPOS;
}
//
#if !defined(BI_NO_TMPL_ALLOCATOR)
template<class T, class A> inline T& TMObjectArray<T,A>::operator [](int loc)
#else
template<class T> inline T& TObjectArray<T>::operator [](int loc)
#endif
{
  PRECONDITION(loc < (int)Size());
  return *Data[loc];
}

//
#if !defined(BI_NO_TMPL_ALLOCATOR)
template<class T, class A> inline const T& TMObjectArray<T,A>::operator [](int loc) const
#else
template<class T> inline const T& TObjectArray<T>::operator [](int loc) const
#endif
{
  PRECONDITION(loc < (int)Size());
  return *Data[loc];
}

//
#if !defined(BI_NO_TMPL_ALLOCATOR)
template<class T, class A> void TMObjectArray<T,A>::ForEach(IterFunc iter, void* args)
#else
template<class T> void TObjectArray<T>::ForEach(IterFunc iter, void* args)
#endif
{
  for(int i = 0; i < (int)Size(); i++)
    (iter)(*Data[i],args);
}

//
#if !defined(BI_NO_TMPL_ALLOCATOR)
template<class T, class A> T* TMObjectArray<T,A>::FirstThat(CondFunc cond, void* args) const
#else
template<class T> T* TObjectArray<T>::FirstThat(CondFunc cond, void* args) const
#endif
{
  for(int i = 0; i < (int)Size(); i++)
    if((cond)(*Data[i],args))
      return Data[i];
  return 0;
}

//
#if !defined(BI_NO_TMPL_ALLOCATOR)
template<class T, class A> T* TMObjectArray<T,A>::LastThat(CondFunc cond, void* args) const
#else
template<class T> T* TObjectArray<T>::LastThat(CondFunc cond, void* args) const
#endif
{
  for(int i = (int)Size()-1; i >=0; i--)
    if((cond)(*Data[i],args))
      return Data[i];
  return 0;
}

//
#if !defined(BI_NO_TMPL_ALLOCATOR)
template<class T, class A> void TMObjectArray<T,A>::Flush()
#else
template<class T> void TObjectArray<T>::Flush()
#endif
{
  for(int i = 0; i < (int)Size(); i++){
#if !defined(BI_NO_TMPL_ALLOCATOR)
    __owl_destruct(Data[i]);// call destructor
    Mem.Free(Data[i]);
#else
    delete Data[i]; 
#endif
  }
  ItemCnt = 0;
}

//
#if !defined(BI_NO_TMPL_ALLOCATOR)
template<class T, class A> inline void TMObjectArray<T,A>::Grow(int index)
#else
template<class T> inline void TObjectArray<T>::Grow(int index)
#endif
{
  if((int)Reserved > index)
    return;
  Resize(index - Reserved);
}

//
#if !defined(BI_NO_TMPL_ALLOCATOR)
template<class T, class A> void TMObjectArray<T,A>::Resize(int delta)
{
  if(Data){
    T** data = (T**)ReAlloc((void*)Data,(Reserved+delta)*sizeof(T**));
    if(!data){
      data = (T**)Alloc((Reserved+delta)*sizeof(T**));
      memcpy(data,Data,Reserved*sizeof(T**));
      Free(Data);
    }
    Data = data;
  }
  else
    Data = (T**)Alloc((Reserved+delta)*sizeof(T**));
#else
template<class T> void TObjectArray<T>::Resize(int delta)
{
  Data = (T**)realloc((void*)Data,(Reserved+delta)*sizeof(T**));
#endif

  memset((uint8*)Data+Reserved*sizeof(T**),0,delta*sizeof(T**));
  Reserved += delta;
}

//
#if !defined(BI_NO_TMPL_ALLOCATOR)
template<class T, class A> inline void TMObjectArray<T,A>::Remove(int index)
#else
template<class T> inline void TObjectArray<T>::Remove(int index)
#endif
{
  memmove(&Data[index],&Data[index+1], sizeof(T*)*(ItemCnt-index-1));
  ItemCnt--;
}

//
// TMSortedObjectArray/TSortedObjectArray
// ~~~~~~~~~~~~~~~~~~

//
#if !defined(BI_NO_TMPL_ALLOCATOR)
template<class T, class A> inline _OWLCTOR 
TMSortedObjectArray<T,A>::TMSortedObjectArray(const TMSortedObjectArray<T,A>& array)
:
  TMObjectArray<T,A>(array)
{
}
#else
template<class T> inline _OWLCTOR 
TSortedObjectArray<T>::TSortedObjectArray(const TSortedObjectArray<T>& array)
:
  TObjectArray<T>(array)
{
}
#endif

//
#if !defined(BI_NO_TMPL_ALLOCATOR)
template<class T, class A> inline TMSortedObjectArray<T,A>&
TMSortedObjectArray<T,A>::operator=(const TMSortedObjectArray<T,A>& array)
{
  if(this == &array)
    return *this;
  TMObjectArray<T,A>::operator=(array);
  return *this;
}
#else
template<class T> inline TSortedObjectArray<T>&
TSortedObjectArray<T>::operator=(const TSortedObjectArray<T>& array)
{
  if(this == &array)
    return *this;
  TObjectArray<T>::operator=(array);
  return *this;
}
#endif

//
#if !defined(BI_NO_TMPL_ALLOCATOR)
template<class T, class A> int TMSortedObjectArray<T,A>::Add(const T& t)
#else
template<class T> int TSortedObjectArray<T>::Add(const T& t)
#endif
{
  if(this->ItemCnt>=this->Reserved)
    Resize(this->ItemCnt+1); // on error -> throw xalloc
  unsigned loc = this->ItemCnt++;
  while( loc > 0 && t < *(this->Data[loc-1])) {
    this->Data[loc] = this->Data[loc-1];
    loc--;
  }
#if !defined(BI_NO_TMPL_ALLOCATOR)
  this->Data[loc] = __owl_construct<T>(Mem.Allocate(sizeof(T)),t);
#else
  this->Data[loc] = new T(t); // if fail -> throw xalloc
#endif
  return loc;
}

//
#if defined(BI_NO_TMPL_ALLOCATOR)
template<class T> inline bool TSortedObjectArray<T>::DetachItem(const T& t)
{
  int index = Find(t);
  if(index != (int)NPOS){
    this->Detach(index);
    return true;
  }
  return false;
}
#endif

//
#if !defined(BI_NO_TMPL_ALLOCATOR)
template<class T, class A> inline bool TMSortedObjectArray<T,A>::DestroyItem(const T& t)
#else
template<class T> inline bool TSortedObjectArray<T>::DestroyItem(const T& t)
#endif
{
  int index = Find(t);
  if(index != (int)NPOS){
    T* tmp = this->Data[index];
    this->Remove(index);
#if !defined(BI_NO_TMPL_ALLOCATOR)
    __owl_destruct(tmp);// call destructor
    Mem.Free(tmp);
#else
    delete tmp;
#endif
    return true;
  }
  return false;
}

//
#if !defined(BI_NO_TMPL_ALLOCATOR)
template<class T, class A> inline bool TMSortedObjectArray<T,A>::HasMember(const T& t) const
#else
template<class T> inline bool TSortedObjectArray<T>::HasMember(const T& t) const
#endif
{
  return Find(t) != (int)NPOS;
}

//
#if !defined(BI_NO_TMPL_ALLOCATOR)
template<class T, class A> int TMSortedObjectArray<T,A>::Find(const T& t) const
#else
template<class T> int TSortedObjectArray<T>::Find(const T& t) const
#endif
{
  if( this->Size() == 0 )
    return (int)NPOS;
  unsigned lower = 0;
  unsigned upper = this->Size()-1;

  while( lower < upper && upper != NPOS ){
    unsigned middle = (lower+upper)/2;
    if(*(this->Data[middle]) == t)
      return middle;
    if(*(this->Data[middle]) < t)
      lower = middle+1;
    else
      upper = middle-1;
  }

  if( lower == upper && *(this->Data[lower]) == t)
    return (int)lower;
  return (int)NPOS;
}

//
// class TTypedArray<T> 
// ~~~~~ ~~~~~~~~~~~~~~
//
template<class T, class R, class A> _OWLCTOR
TTypedArray<T,R,A>::TTypedArray(const TTypedArray<T,R,A>& array)
:
  Data(0)
{
  if(array.Reserved)
    Resize(array.Reserved);
  for(int i = 0; i < (int)array.ItemCnt; i++)
    Data[i] = array.Data[i];
  ItemCnt = array.ItemCnt;
}
//
template<class T, class R, class A> TTypedArray<T,R,A>&
TTypedArray<T,R,A>::operator=(const TTypedArray<T,R,A>& array)
{
  if(this == &array)
    return *this;

  Flush();
  if(array.ItemCnt >= Reserved)
    Resize(array.ItemCnt - Reserved);
  for(int i = 0; i < (int)array.ItemCnt; i++)
    Data[i] = array.Data[i];
  ItemCnt = array.ItemCnt;

  return *this;
}

//
template<class T, class R, class A> inline int
TTypedArray<T,R,A>::Add(const R t)
{
  if(ItemCnt>=Reserved)
    Resize(ItemCnt+1);
  Data[ItemCnt++] = t;
  return ItemCnt-1; // if fail -> throw xalloc
}

//
template<class T, class R, class A> void
TTypedArray<T,R,A>::AddAt(const R t, uint index)
{
  if(index == Size()){
    Add(t);
    return;
  }
  if(index >=Reserved)
    Resize(index+1); // on error -> throw xalloc
  if(ItemCnt>=Reserved)
    Resize(ItemCnt+1); // on error -> throw xalloc
  if(index < ItemCnt && ItemCnt)
    memmove(&Data[index+1], &Data[index], sizeof(T*)*(ItemCnt-index));
  Data[index] = t;
  ItemCnt++;
}

//
template<class T, class R, class A> bool
TTypedArray<T,R,A>::DetachItem(const R t)
{
  int index = Find(t);
  if(index != (int)NPOS){
    Remove(index);
    return true;
  }
  return false;
}

//
template<class T, class R, class A> bool
TTypedArray<T,R,A>::Detach(int loc)
{
  if(loc < (int)Size()){
    Remove(loc);
    return true;
  }
  return false;
}

//
template<class T, class R, class A> inline bool
TTypedArray<T,R,A>::DestroyItem(R t)
{
  return DetachItem(t);
}

//
template<class T, class R, class A> inline bool
TTypedArray<T,R,A>::Destroy(int loc)
{
  return Detach(loc);
}

//
template<class T, class R, class A> inline bool
TTypedArray<T,R,A>::HasMember(R t) const
{
  return Find(t) != (int)NPOS;
}

//
template<class T, class R, class A> int
TTypedArray<T,R,A>::Find(R t) const
{
  for(int i = 0; i < (int)Size(); i++)
    if(Data[i] == t)
      return i;
  return (int)NPOS;
}

//
template<class T, class R, class A> inline T&
TTypedArray<T,R,A>::operator [](int loc)
{
  PRECONDITION(loc < (int)Size());
  return Data[loc];
}

//
template<class T, class R, class A> inline T&
TTypedArray<T,R,A>::operator [](int loc) const
{
  PRECONDITION(loc < (int)Size());
  return Data[loc];
}

//
template<class T, class R, class A> void
TTypedArray<T,R,A>::ForEach(IterFunc iter, void* args)
{
  for(int i = 0; i < (int)Size(); i++)
    (iter)(Data[i], args);
}

//
template<class T, class R, class A> T*
TTypedArray<T,R,A>::FirstThat(CondFunc cond, void* args) const
{
  for(int i = 0; i < (int)Size(); i++)
    if((cond)(Data[i],args))
      return &Data[i];
  return 0;
}

//
template<class T, class R, class A> T*
TTypedArray<T,R,A>::LastThat(CondFunc cond, void* args) const
{
  for(int i = (int)Size()-1; i >=0; i--)
    if((cond)(Data[i],args))
      return &Data[i];
  return 0;
}

//
template<class T, class R, class A> inline void
TTypedArray<T,R,A>::Flush()
{
  ItemCnt = 0;
}

//
template<class T, class R, class A> inline void
TTypedArray<T,R,A>::Grow(int index)
{
  if(Reserved > (uint)index)
    return;
  Resize(index - Reserved);
}

//
template<class T, class R, class A> void
TTypedArray<T,R,A>::Resize(int delta)
{
  if (Data)
  {
    T* data = (T*)this->ReAlloc((void*)Data,(Reserved+delta)*sizeof(T));
    if (!data) //no ReAlloc, try Alloc instead
    {
      data = (T*)this->Alloc((Reserved+delta)*sizeof(T));
      if (data) 
      {
        memcpy(data,Data,Reserved*sizeof(T));
        Free(Data);
        Data = data;
      }
      else
#if defined(_DEBUG)
        return; //or throw bad realloc?
#else
        return; //silently return
#endif
    }
    else
      Data = data;
  }
  else // if (Data)
    Data = (T*)this->Alloc((Reserved+delta)*sizeof(T));

  memset((uint8*)Data+Reserved*sizeof(T*),0,delta*sizeof(T));
  Reserved += delta;
}

//
template<class T, class R, class A> inline void
TTypedArray<T,R,A>::Remove(int index)
{
  memmove(&Data[index],&Data[index+1], sizeof(T)*(ItemCnt-index-1));
  ItemCnt--;
}
//
// TSTypedArray
// ~~~~~~~~~~~~
//
template<class T, class R, class A> inline _OWLCTOR
TSTypedArray<T,R,A>::TSTypedArray(const TSTypedArray<T,R,A>& array)
:
  TTypedArray<T,R,A>(array)
{
}

//
template<class T, class R, class A> inline TSTypedArray<T,R,A>&
TSTypedArray<T,R,A>::operator=(const TSTypedArray<T,R,A>& array)
{
  if(this == &array)
    return *this;
  TTypedArray<T,R,A>::operator=(array);
  return *this;
}

//
template<class T, class R, class A> int
TSTypedArray<T,R,A>::Add(R t)
{
  if(this->ItemCnt>=this->Reserved)
    Resize(this->ItemCnt+1); // on error -> throw xalloc
  unsigned loc = this->ItemCnt++;
  while( loc > 0 && t < this->Data[loc-1]) {
    this->Data[loc] = this->Data[loc-1];
    loc--;
  }
  this->Data[loc] = t;
  return loc;
}

//
template<class T, class R, class A> bool
TSTypedArray<T,R,A>::DetachItem(R t)
{
  int index = Find(t);
  if(index != (int)NPOS){
    this->Remove(index);
    return true;
  }
  return false;
}

//
template<class T, class R, class A> inline bool
TSTypedArray<T,R,A>::DestroyItem(R t)
{
  return DetachItem(t);
}

//
template<class T, class R, class A> inline bool
TSTypedArray<T,R,A>::HasMember(R t) const
{
  return Find(t) != (int)NPOS;
}

//
template<class T, class R, class A> int
TSTypedArray<T,R,A>::Find(R t) const
{
  if( this->Size() == 0 )
    return (int)NPOS;
  unsigned lower = 0;
  unsigned upper = this->Size()-1;

  while( lower < upper && upper != NPOS ){
    unsigned middle = (lower+upper)/2;
    if(this->Data[middle] == t)
      return middle;
    if(this->Data[middle] < t)
      lower = middle+1;
    else
      upper = middle-1;
  }

  if( lower == upper && this->Data[lower] == t)
    return (int)lower;
  return (int)NPOS;
}

//
// TMIPtrArray
// ~~~~~~~~~~~
//
template<class T, class R, class A> inline _OWLCTOR
TMIPtrArray<T,R,A>::TMIPtrArray(int upper, int lower, int delta)
:
  TTypedArray<T,R,A>(upper,lower,delta)
{
}
//
template<class T, class R, class A> inline _OWLCTOR
TMIPtrArray<T,R,A>::TMIPtrArray(const TMIPtrArray<T,R,A>& array)
:
  TTypedArray<T,R,A>(array)
{
}
//
template<class T, class R, class A> inline _OWLCTOR
TMIPtrArray<T,R,A>::TMIPtrArray()
{
}
//
template<class T, class R, class A> inline _OWLCTOR 
TMIPtrArray<T,R,A>::~TMIPtrArray()
{
  Flush(true);
}
//
template<class T, class R, class A> inline TMIPtrArray<T,R,A>&
TMIPtrArray<T,R,A>::operator=(const TMIPtrArray<T,R,A>& array)
{
  if(this == &array)
    return *this;
  Flush(true);
  TTypedArray<T,R,A>::operator=(array);
  return *this;
}
//
template<class T, class R, class A> inline bool 
TMIPtrArray<T,R,A>::DestroyItem(R t)
{
  int index = Find(t);
  if(index != (int)NPOS){
    R tmp = this->Data[index];
    this->Remove(index);
    delete tmp;// using global delete because we didn't allocate it
    return true;
  }
  return false;
}
//
template<class T, class R, class A> inline bool 
TMIPtrArray<T,R,A>::Destroy(int loc)
{
  if(loc < (int)this->Size()){
    R tmp = this->Data[loc];
    this->Remove(loc);
    delete tmp;// using global delete because we didn't allocate it
    return true;
  }
  return false;
}
//
template<class T, class R, class A> void
TMIPtrArray<T,R,A>::Flush(bool del)
{
  if(del){
    for(int i = 0; i < (int)this->Size(); i++)
      delete this->Data[i];// using global delete because we didn't allocate it
  }
  TTypedArray<T,R,A>::Flush();
}
/////////////////////////////////////////////////////////////////
//
// class TIPtrArray
// ~~~~~ ~~~~~~~~~~
//
template<class T> inline _OWLCTOR
TIPtrArray<T>::TIPtrArray(int upper, int lower, int delta)
:
  TPtrArray<T>(upper,lower,delta)
{
}
//
template<class T> inline _OWLCTOR
TIPtrArray<T>::TIPtrArray(const TIPtrArray<T>& array)
:
  TPtrArray<T>(array)
{
}
//
template<class T> inline TIPtrArray<T>&
TIPtrArray<T>::operator=(const TIPtrArray<T>& array)
{
  if(this == &array)
    return *this;
  Flush(true);
  TPtrArray<T>::operator=(array);
  return *this;
}

//
template<class T> inline _OWLCTOR
TIPtrArray<T>::TIPtrArray()
{
}

//
template<class T> inline _OWLCTOR
TIPtrArray<T>::~TIPtrArray()
{
  Flush(true);
}

//
template<class T> void
TIPtrArray<T>::Flush(bool del)
{
  if(del){
    for(int i = 0; i < (int)this->Size(); i++)
      delete this->Data[i];// using global delete because we didn't allocate it
  }
  TPtrArray<T>::Flush();
}

//
template<class T> bool
TIPtrArray<T>::DestroyItem(T t)
{
  int index = Find(t);
  if(index != (int)NPOS){
    T tmp = this->Data[index];
    this->Remove(index);
    delete tmp;// using global delete because we didn't allocate it
    return true;
  }
  return false;
}

//
template<class T> bool
TIPtrArray<T>::Destroy(int loc)
{
  if(loc < (int)this->Size()){
    T tmp = this->Data[loc];
    this->Remove(loc);
    delete tmp;// using global delete because we didn't allocate it
    return true;
  }
  return false;
}

//
// TISortedPtrArray
// ~~~~~~~~~

//
template<class T> inline _OWLCTOR
TISortedPtrArray<T>::TISortedPtrArray(int upper, int lower, int delta)
:
  TSortedPtrArray<T>(upper,lower,delta)
{
}

//
template<class T> inline _OWLCTOR
TISortedPtrArray<T>::TISortedPtrArray()
{
}

//
template<class T> inline _OWLCTOR
TISortedPtrArray<T>::~TISortedPtrArray()
{
  Flush(true);
}

//
template<class T> inline _OWLCTOR
TISortedPtrArray<T>::TISortedPtrArray(const TISortedPtrArray<T>& array)
:
  TSortedPtrArray<T>(array)
{
}

//
template<class T> inline TISortedPtrArray<T>&
TISortedPtrArray<T>::operator=(const TISortedPtrArray<T>& array)
{
  if(this == &array)
    return *this;

  Flush(true);
  TSortedPtrArray<T>::operator=(array);
  return *this;
}

//
template<class T> void
TISortedPtrArray<T>::Flush(bool del)
{
  if(del){
    for(int i = 0; i < (int)this->Size(); i++)
      delete this->Data[i];// using global delete because we didn't allocate it
  }
  TSortedPtrArray<T>::Flush();
}

//
template<class T> inline bool
TISortedPtrArray<T>::DestroyItem(T t)
{
  int index = Find(t);
  if(index != (int)NPOS){
    T tmp = this->Data[index];
    this->Remove(index);
    delete tmp;// using global delete because we didn't allocate it
    return true;
  }
  return false;
}

//
template<class T> inline bool
TISortedPtrArray<T>::Destroy(int loc)
{
  if(loc < (int)this->Size()){
    T tmp = this->Data[loc];
    this->Remove(loc);
    delete tmp;// using global delete because we didn't allocate it
    return true;
  }
  return false;
}


//
// TBaseList
// ~~~~~~~~~

//
template<class T> inline _OWLCTOR
TBaseList<T>::TBaseList()
:
  First(0),Last(0),ItemSize(0)
{
}

//
template<class T> inline _OWLCTOR
TBaseList<T>::TBaseList(int /*upper*/, int /*lower*/, int /*delta*/)
:
  First(0),Last(0),ItemSize(0)
{
}

//
template<class T> inline _OWLCTOR
TBaseList<T>::~TBaseList()
{
  Flush();
}

//
template<class T> inline void
TBaseList<T>::Add(const T& data)
{
  TBaseNode<T>* node = new TBaseNode<T>(data);
  Add(node);
}

//
template<class T> inline void
TBaseList<T>::Add(
#ifdef _MSC_VER
  typename
#endif
  TBaseList<T>::Iterator* iter, const T& data)
{
  Add( iter ? iter->Cur : 0, new TBaseNode<T>(data));
}

//
template<class T> inline void
TBaseList<T>::Push(const T& data)
{
  Add((TBaseNode<T>*)0, data);
}

//
template<class T> inline void
TBaseList<T>::PushBack(const T& data)
{
  Add(Last, data);
}

//
template<class T> inline T
TBaseList<T>::Pop()
{
  TBaseNode<T>* tmp = First;
  Remove(tmp);
  T data = tmp->Data; delete tmp;
  return data;
}

//
template<class T> inline const T&
TBaseList<T>::Top()
{
  return First->Data;
}

//
template<class T> inline const T&
TBaseList<T>::Bottom()
{
  return Last->Data;
}

//
template<class T> void
TBaseList<T>::Flush()
{
  for(TBaseNode<T>* node = First; node;){
    TBaseNode<T>* tmp = node;
    node = node->Next;
    delete tmp;
  }
  First = Last = 0;
  ItemSize = 0;
}

//
template<class T> bool
TBaseList<T>::DetachItem(const T& t)
{
  TBaseNode<T>* item = Find(t);
  if(item && Remove(item)){
    delete item;
    return true;
  }
  return false;
}

//
template<class T> bool
TBaseList<T>::Detach(int loc)
{
  if(loc < (int)Size()){
    int counter = 0;
    for(TBaseNode<T>* i = First; i; i = i->Next, counter++){
      if(counter == loc){
        Remove(i);
        return true;
      }
    }
  }
  return false;
}

//
template<class T> inline bool
TBaseList<T>::HasMember(const T& t) const
{
  return Find(t) != 0;
}

//
template<class T> TBaseNode<T>*
TBaseList<T>::Find(const T& t) const
{
  for(TBaseNode<T>* i = First; i; i = i->Next)
    if(i->Data == t)
      return i;
  return 0;
}

//
template<class T> void
TBaseList<T>::ForEach(IterFunc iter, void* args)
{
  for(TBaseNode<T>* i = First; i; i = i->Next)
    (iter)(i->Data, args);
}

//
template<class T> T*
TBaseList<T>::FirstThat(CondFunc cond, void* args) const
{
  for(TBaseNode<T>* i = First; i; i = i->Next)
    if((cond)(i->Data,args))
      return &i->Data;
  return 0;
}

//
template<class T> T*
TBaseList<T>::LastThat(CondFunc cond, void* args) const
{
  for(TBaseNode<T>* i = Last; i; i = i->Prev)
    if((cond)(i->Data,args))
      return &i->Data;
  return 0;
}

//
template<class T> bool
TBaseList<T>::Remove(TBaseNode<T>* node)
{
  if(!node)
    return false;
  if(node == First){
    First = node->Next;
    node->Next = 0;
    if(First)
      First->Prev = 0;
  }
  else if(node == Last){
    Last = node->Prev;
    node->Prev = 0;
    if(Last)
      Last->Next = 0;
  }
  else{
    node->Prev->Next = node->Next;
    node->Next->Prev = node->Prev;
    node->Prev = 0;
    node->Next = 0;
  }

  ItemSize--;
  return true;
}

//
template<class T> inline int
TBaseList<T>::Add(TBaseNode<T>* node, const T& data)
{
  return Add(node, new TBaseNode<T>(data));
}

//
template<class T> int
TBaseList<T>::Add(TBaseNode<T>* node)
{
  if(!node)
    return (int)NPOS;
  if(!First){
    First = node;
    Last = node;
  }
  else{
    node->Prev = Last;
    Last->Next = node;
    Last = node;
  }
  return ++ItemSize;
}

//
template<class T> int
TBaseList<T>::Add(TBaseNode<T>* item, TBaseNode<T>* node)
{
  if(!node)
    return (int)NPOS;
  if(!First){
    return Add(node);
  }
  if(!item){
    node->Next  = First;
    First->Prev = node;
    First = node;
  }
  else{
    if(item == Last)
      Last = node;
    node->Prev = item;
    node->Next = item->Next;
    item->Next = node;
    if(node->Next)
      node->Next->Prev = node;
  }
  return ++ItemSize;
}

__OWL_END_NAMESPACE

#endif  // OWL_TEMPLATE_H
