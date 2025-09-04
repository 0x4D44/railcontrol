//------------------------------------------------------------------------
// ObjectWindows
//
//                                                           
//  CSTRING.CPP                                                
//                                                                        
//$Revision: 1.28 $
//$Author: jogybl $
//$Date: 2009-08-05 10:48:52 $
//
//Definitions for core string functions
//                                                                       
//                                                                       
//------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_DEFS_H)
#include <owl/defs.h>
#endif

#if !defined(BI_STD_EXCEPTIONS)
extern _OWLDATA(XALLOC) __xalloc;
#endif

///////////////////////////////////////
// Here UNICODE wstrstreambuf, wistrstream, wostrstream, wstrstream, 
//
#if defined(BI_STD_IO) && defined(_UNICODE)
#include <owl/private/wstrstream>

#if defined(BI_COMP_MSC)

_STD_BEGIN

wistrstream::~wistrstream()
  {  // destruct an istrstream
  }

wostrstream::wostrstream(wchar_t *s, streamsize n, openmode mode)
  : wostream(&_Sb),
  _Sb(s, n, s == 0 || (mode & app) == 0 ? s : s + _tcslen(s))
  {  // write at terminating null (if there)
  }

wostrstream::~wostrstream()
  {  // destruct an ostrstream
  }

wstrstream::wstrstream(wchar_t *s, streamsize n, openmode mode)
  : wiostream(&_Sb),
  _Sb(s, n, s == 0 || (mode & app) == 0 ? s : s + _tcslen(s))
  {  // write at terminating null (if there)
  }

wstrstream::~wstrstream()
  {  // destruct a strstream
  }

wstrstreambuf::~wstrstreambuf()
  {  // destruct a strstreambuf
  _Tidy();
  }

void wstrstreambuf::freeze(bool freezeit)
  {  // freeze a dynamic string
  if (freezeit && !(_Strmode & _Frozen))
    {  // disable writing
    _Strmode |= _Frozen;
    _Pendsave = epptr();
    setp(pbase(), pptr(), eback());
    }
  else if (!freezeit && _Strmode & _Frozen)
    {  // re-enable writing
    _Strmode &= ~_Frozen;
    setp(pbase(), pptr(), _Pendsave);
    }
  }

wstrstreambuf::int_type wstrstreambuf::overflow(int_type ch)
  {  // try to extend write area
  if (ch == EOF)
    return (0);
  else if (pptr() != 0 && pptr() < epptr())
    return ((wchar_t)(*_Pninc() = (wchar_t)ch));
  else if (!(_Strmode & _Dynamic)
    || _Strmode & (_Constant | _Frozen))
    return (EOF);
  else
    {  // okay to extend
    int osize = gptr() == 0 ? 0 : epptr() - eback();
    int nsize = osize + _Alsize;
    wchar_t *p = _Palloc != 0 ? (wchar_t *)(*_Palloc)(nsize)
      : new wchar_t[nsize];
    if (p == 0)
      return (EOF);
    if (0 < osize)
      memcpy(p, eback(), osize);
    else if (_ALSIZE < _Alsize)
      _Alsize = _ALSIZE;
    if (!(_Strmode & _Allocated))
      ;
    else if (_Pfree != 0)
      (*_Pfree)(eback());
    else
      delete[] eback();
    _Strmode |= _Allocated;
    if (osize == 0)
      {  // setup new buffer
      _Seekhigh = p;
      setp(p, p + nsize);
      setg(p, p, p);
      }
    else
      {  // revise old pointers
      _Seekhigh = _Seekhigh - eback() + p;
      setp(pbase() - eback() + p, pptr() - eback() + p,
        p + nsize);
      setg(p, gptr() - eback() + p, pptr() + 1);
      }
    return ((wchar_t)(*_Pninc() = (wchar_t)ch));
    }
  }

wstrstreambuf::int_type wstrstreambuf::pbackfail(int_type ch)
  {  // try to putback a character
  if (gptr() == 0 || gptr() <= eback()
    || ch != EOF
      && (wchar_t)ch != (wchar_t)gptr()[-1]
      && _Strmode & _Constant)
    return (EOF);
  else
    {  // safe to back up
    gbump(-1);
    return (ch == EOF ? 0 : (wchar_t)(*gptr() = (wchar_t)ch));
    }
  }

wstrstreambuf::int_type wstrstreambuf::underflow()
  {  // read only if read position available
  if (gptr() == 0)
    return (EOF);
  else if (gptr() < egptr())
    return ((wchar_t)*gptr());
  else if (pptr() == 0
    || pptr() <= gptr() && _Seekhigh <= gptr())
    return (EOF);
  else
    {  // update _Seekhigh and expand read region
    if (_Seekhigh < pptr())
      _Seekhigh = pptr();
    setg(eback(), gptr(), _Seekhigh);
    return ((wchar_t)*gptr());
    }
  }

wstrstreambuf::pos_type wstrstreambuf::seekoff(off_type off, ios::seekdir way, ios::openmode which)
  {  // seek by specified offset
  if (pptr() != 0 && _Seekhigh < pptr())
    _Seekhigh = pptr();
  if (which & ios::in && gptr() != 0)
    {  // set input (and maybe output) pointer
    if (way == ios::end)
      off += _Seekhigh - eback();
    else if (way == ios::cur && !(which & ios::out))
      off += gptr() - eback();
    else if (way != ios::beg || off == _BADOFF)
      off = _BADOFF;
    if (0 <= off && off <= _Seekhigh - eback())
      {  // set one or two pointers
      gbump(eback() - gptr() + off);
      if (which & ios::out && pptr() != 0)
        setp(pbase(), gptr(), epptr());
      }
    else
      off = _BADOFF;
    }
  else if (which & ios::out && pptr() != 0)
    {  // set only output pointer
    if (way == ios::end)
      off += _Seekhigh - eback();
    else if (way == ios::cur)
      off += pptr() - eback();
    else if (way != ios::beg || off == _BADOFF)
      off = _BADOFF;
    if (0 <= off && off <= _Seekhigh - eback())
      pbump(eback() - pptr() + off);
    else
      off = _BADOFF;
    }
  else  // nothing to set
    off = _BADOFF;
  return (streampos(off));
  }

wstrstreambuf::pos_type wstrstreambuf::seekpos(pos_type sp, ios::openmode which)
  {  // seek to memorized position
  streamoff off = (streamoff)sp;
  if (pptr() != 0 && _Seekhigh < pptr())
    _Seekhigh = pptr();
  if (off == _BADOFF)
    ;
  else if (which & ios::in && gptr() != 0)
    {  // set input (and maybe output) pointer
    if (0 <= off && off <= _Seekhigh - eback())
      {  // set valid offset
      gbump(eback() - gptr() + off);
      if (which & ios::out && pptr() != 0)
        setp(pbase(), gptr(), epptr());
      }
    else
      off = _BADOFF;
    }
  else if (which & ios::out && pptr() != 0)
    {  // set output pointer
    if (0 <= off && off <= _Seekhigh - eback())
      pbump(eback() - pptr() + off);
    else
      off = _BADOFF;
    }
  else  // nothing to set
    off = _BADOFF;
  return (streampos(off));
  }

void wstrstreambuf::_Init(int n, wchar_t *gp, wchar_t *pp, _Strstate mode)
  {  // initialize with possibly static buffer
  wstreambuf::_Init();
  _Pendsave = 0;
  _Seekhigh = 0;
  _Palloc = 0;
  _Pfree = 0;
  _Strmode = mode;
  if (gp == 0)
    {  // make dynamic
    _Alsize = _MINSIZE <= n ? n : _ALSIZE;
    _Strmode |= _Dynamic;
    }
  else
    {  // make static
    int size = n < 0 ? INT_MAX : n == 0 ? _tcslen(gp) : n;
    _Alsize = 0;
    _Seekhigh = gp + size;
    if (pp == 0)
      setg(gp, gp, gp + size);
    else
      {  // make writable too
      if (pp < gp)
        pp = gp;
      else if (gp + size < pp)
        pp = gp + size;
      setp(pp, gp + size);
      setg(gp, gp, pp);
      }
    }
  }

void wstrstreambuf::_Tidy()
  {  // discard any allocated storage
  if ((_Strmode & (_Allocated | _Frozen)) != _Allocated)
    ;
  else if (_Pfree != 0)
    (*_Pfree)(eback());
  else
    delete[] eback();
  _Seekhigh = 0;
  _Strmode &= ~(_Allocated | _Frozen);
  }


_STD_END

#elif defined(BI_COMP_BORLANDC)
#ifndef _RWSTD_NO_NAMESPACE
namespace std {
#endif
  
  /* class wstrstreambuf : public basic_streambuf*/
  /* wstrstreambuf(streamsize)   */

  wstrstreambuf::wstrstreambuf(streamsize n)
#ifndef _RWSTD_NO_COMPLEX_DEFAULT_TEMPLATES
  : basic_streambuf<wchar_t>()
#else
  : basic_streambuf<wchar_t, char_traits<wchar_t> >()
#endif // _RWSTD_NO_COMPLEX_DEFAULT_TEMPLATES 
  , __strmode(dynamic)
  , __alsize(n)
  , __palloc(0)
  , __pfree(0)
  , __data(0)
  , __end_pos(0)
  {
  }

  /* wstrstreambuf(void *(*palloc)(size_t), void (*pfree)(void *)) */
  wstrstreambuf::wstrstreambuf(void *(*palloc)(size_t), void (*pfree)(void *))
#ifndef _RWSTD_NO_COMPLEX_DEFAULT_TEMPLATES
  : basic_streambuf<wchar_t>()
#else
  : basic_streambuf<wchar_t, char_traits<wchar_t> >()
#endif
  , __strmode(dynamic)
  , __alsize(0)
  , __palloc(palloc)
  , __pfree(pfree)
  , __data(0)
  , __end_pos(0)
  {
  }

  /* wstrstreambuf(char_type *, streamsize, char_type *)  */

  wstrstreambuf::wstrstreambuf(char_type *g, streamsize n, char_type *p)
#ifndef _RWSTD_NO_COMPLEX_DEFAULT_TEMPLATES
  : basic_streambuf<wchar_t>()
#else
  : basic_streambuf<wchar_t, char_traits<wchar_t> >()
#endif // _RWSTD_NO_COMPLEX_DEFAULT_TEMPLATES 
  , __strmode(0)
  , __alsize(0)
  , __palloc(0)
  , __pfree(0)
  , __data(0)
  {
    int  N; 
    if(n > 0)
      N = n;
    else {
      if(n == 0)
        N = char_traits<wchar_t>::length(g);
      else
        N = INT_MAX;
    }

    if(p)
    {
      setg(g, g, p);
      setp(g, g+N);
      pbump(p-g);
    }
    else
    {
      setg(g, g, g+N);
    }

    __end_pos = pptr() - pbase();
  }

  /* wstrstreambuf(const char_type *, streamsize) */

  wstrstreambuf::wstrstreambuf(const char_type *s, streamsize n)
#ifndef _RWSTD_NO_COMPLEX_DEFAULT_TEMPLATES
  : basic_streambuf<wchar_t>()
#else
  : basic_streambuf<wchar_t, char_traits<wchar_t> >()
#endif
  , __strmode(constant)
  , __alsize(0)
  , __palloc(0)
  , __pfree(0)
  , __data(0)
  {
    int N;

    if(n > 0)
      N = n;
    else {
      if(n == 0)
        N = char_traits<wchar_t>::length((wchar_t *)s);
      else
        N = INT_MAX;
    }

    setg((wchar_t *)s, (wchar_t *)s, (wchar_t *)s+N);
    __end_pos = pptr() - pbase();
  }

  /* virtual ~wstrstreambuf()  */

  wstrstreambuf::~wstrstreambuf()
  {
    if((__strmode & allocated) && (!(__strmode & frozen)) ) {
      if(__data)
        delete [] __data;
    }
  }

  /* void freeze(bool)   */

  void wstrstreambuf::freeze(bool f)
  {
    if(__strmode & dynamic)
    {
      if(f)
        __strmode |= frozen;     // set frozen
      else
        __strmode &= ~frozen;    // clear frozen
    }
  }

  /* char_type *str()   */

  wchar_t* wstrstreambuf::str()
  {
    freeze();
    return eback();
  }

  /* int pcount()   */

  int wstrstreambuf::pcount() const
  {
    if(pptr())
      return pptr() - pbase();
    return 0;
  }

  /* int_type overflow(int_type)   */
  wstrstreambuf::int_type  wstrstreambuf::overflow(int_type c)
  {
    wchar_t           *temp;
    size_t            old_numb_of_elements, new_numb_of_elements, old_read_pos;
    const size_t      increment=512;          
  
    if( traits::eq_int_type(c,traits::eof()) )
      return traits::not_eof(c);

    if(((__strmode & dynamic) == 0) || (__strmode & frozen))
      return traits::eof();

    old_numb_of_elements = pptr() - __data;
    old_read_pos = gptr()- __data;

    new_numb_of_elements = old_numb_of_elements;

    if(__alsize)
      new_numb_of_elements += __alsize;
    else
      new_numb_of_elements += increment;

    if(__palloc)
      temp = (wchar_t *)(*__palloc)(new_numb_of_elements);
    else
      temp = new wchar_t[new_numb_of_elements];

    memset(temp,0,new_numb_of_elements);
     

    if(!temp)
      return traits::eof();
   
    memcpy(temp,__data,old_numb_of_elements);

    setp(temp, temp+new_numb_of_elements);

    pbump(old_numb_of_elements);   
 
    setg(temp, temp+old_read_pos, pptr());

    if (__strmode & allocated){ 
      if(__pfree)
        (*__pfree)(__data);
      else
        delete [] __data;
    }

    __strmode |= allocated;
    __data = temp;
    return sputc(c);
  }

  /* int_type pbackfail(int_type)   */
  wstrstreambuf::int_type wstrstreambuf::pbackfail(int_type c)
  {

    if ( (c!= traits::eof()) && (gptr()>eback()) ) {
      if ( (int_type)(*(gptr()-1))==c ) {
        gbump(-1);
        return c;
      }
      else {
        if( !(__strmode & constant) )  {
          gbump(-1);
          *gptr()=(char_type)c;
          return c;
        }
      }
    }       

    if ( (c== traits::eof()) && (gptr()>eback()) ){ 
      gbump(-1);
      return traits::not_eof(c);
    }
    return traits::eof();
  }

  /* int_type underflow() */
  wstrstreambuf::int_type wstrstreambuf::underflow()
  {
    if(gptr() && (gptr()<egptr()) ) 
      return traits::not_eof(*gptr());

    if(((pptr() != 0) && (pptr() > egptr())) && (!gptr())){
      setg(pbase(), pbase(), pptr());
      return traits::to_int_type(*gptr());
    }

    if((pptr() != 0) && (pptr() > egptr())){
      setg(eback(), gptr(), pptr());
      return traits::to_int_type(*gptr());
    }
                                  
    return traits::eof();
  }

  /*
   * pos_type seekoff(off_type, ios_base::seekdir, ios_base::openmode)
   */

  streampos wstrstreambuf::seekoff(off_type off,
                                  ios_base::seekdir way,
                                  ios_base::openmode which)
  {
    streamsize       newoff;
  
#if defined(DINKUMWARE_STL)
	ios_base::openmode mode_ = which;
#endif  
  
    if((which & ios_base::in) && (which & ios_base::out)){
      if ( (way==ios_base::beg) || (way==ios_base::end) ){
        if ( seekoff(off,way,ios_base::out) == pos_type(-1) )
          return pos_type(-1);  
        return seekoff(off,way,ios_base::in);
      }
      else
        return pos_type(-1);  // invalid way
    }
  
    if((which & ios_base::in) && (gptr()!=0)){
      if(way == ios_base::beg)
        newoff = 0;
      if(way == ios_base::cur)
        newoff = gptr() - eback();
      if(way == ios_base::end)
        newoff = egptr() - eback();

      if ( ((eback()+long(newoff)+long(off))> egptr()) || ((newoff+off)< 0) )
        return pos_type(-1);

      setg(eback(), eback() + long(newoff) + long(off), egptr());

      return pos_type(newoff+off);
    }

    if((which & ios_base::in) && (gptr()==0) && (egptr()==0) && (eback()==0) ) 
      return pos_type(0);

    if((which & ios_base::out) && (pptr()!=0) && !(mode_ & ios_base::app) )
    {
      if(way == ios_base::beg)
        newoff = 0;
      if(way == ios_base::cur)
        newoff = pptr() - pbase();
      if(way == ios_base::end)
        newoff = __end_pos;

      if ( (pptr() - pbase()) > __end_pos ) __end_pos = pptr() - pbase();

      if ( ((newoff+off)<0) || ((pbase()+long(newoff)+long(off))> epptr()) )
        return pos_type(-1);  

      pbump( newoff+off-(pptr()-pbase())  );

      if ( egptr()>pptr() )
        if ( gptr() > pptr() )
          setg(eback(), pptr(), pptr());
        else
          setg(eback(),gptr(),pptr());
      return pos_type(newoff+off);
    }
    else 
    {
      if ( mode_ & ios_base::app )
        return pos_type(pptr()-pbase());

      if((which & ios_base::out) && (pptr()==0) && (epptr()==0) && (pbase()==0) )
        return pos_type(0);
    }
    return pos_type(-1);
  }

             
  /*
   * pos_type seekpos(pos_type, ios_base::openmode)
   */

  streampos wstrstreambuf::seekpos(pos_type sp, ios_base::openmode which)
  {
    streamsize    newoff = sp;

#if defined(DINKUMWARE_STL)
	ios_base::openmode mode_ = which;
#endif  

    if((which & ios_base::in) && (which & ios_base::out)){
      if ( seekpos(sp,ios_base::out) == pos_type(-1) )
        return pos_type(-1);  
      return seekpos(sp,ios_base::in);
    }
  
    if((which & ios_base::in) && (gptr()!=0)){
      if ( ((eback()+long(newoff))> egptr()) || (newoff< 0) )
        return pos_type(-1);

      setg(eback(), eback() + long(newoff), egptr());

      return pos_type(newoff);
    }
    if((which & ios_base::out) && (pptr()!=0) && !(mode_ & ios_base::app) ){
      if ( (newoff<0) || ((pbase()+long(newoff))> epptr()) )
        return pos_type(-1); 

      if ( (pptr() - pbase()) > __end_pos ) __end_pos = pptr() - pbase(); 
 
      pbump( newoff-(pptr()-pbase())  );
      if ( egptr()>pptr() )
        if ( gptr() > pptr() )
          setg(eback(), pptr(), pptr());
        else
          setg(eback(),gptr(),pptr());
      return pos_type(newoff);
        
    }
    else 
      if ( mode_ & ios_base::app )
        return pos_type(pptr()-pbase());

    return pos_type(-1);
  }

  /* wstreamsize setbuf(wchar_t*, streamsize)  */

  wstreambuf* wstrstreambuf::setbuf(wchar_t* s, streamsize n)
  {
    if(((__strmode & dynamic) == 0) || (__strmode & frozen)){
      return (wstreambuf*)(0);
    }

#if defined(DINKUMWARE_STL)
	ios_base::openmode mode_ = ios_base::out;  //Jogy ???
#endif  

    if((mode_ & ios_base::out) != 0){
      if ( n > ( pptr() - pbase() ) ){
        bool is_allocated = false;
        if ( s == 0 ) {
          if(__palloc)
            s = (wchar_t *)(*__palloc)(n);
          else
            s = new wchar_t[n];
          is_allocated = true;
        }

        if ( s != __data)
          memset(s,0,n);

        if ( s ){
          int   old_numb_of_elements;

          if (pptr()){
            old_numb_of_elements = pptr() - __data;
                
            if ( s!=__data )
              memcpy(s,__data,old_numb_of_elements);
               
            setp (s,s+n-1);

            pbump(old_numb_of_elements);  

            wchar_t *tmp=s+(gptr()-eback()); 
            if ( tmp == 0 ) 
              tmp = s; 
            setg(s, tmp, pptr()+1);

            if( s != __data && __strmode & allocated){
              if(__pfree)
                (*__pfree)(__data);
              else
                delete [] __data;
            }
            __data = s;
          }
          else{
            setp(s,s+n-1);
            if((mode_ & ios_base::in) != 0)
              setg(s, s , s);
            __data =s; 
          }
        }
        else
          return (wstreambuf*)(0);
        
        if (is_allocated)
          __strmode |= allocated;
        else
          __strmode &= ~allocated;
      }
      else
        return (wstreambuf*)(0);
    }

    return (wstreambuf*)(this);
  }

  int wstrstreambuf::doallocate()
  {
    return 0;
  }

  /*
   * streamsize xsputn(const char_type *, streamsize)
   */

  streamsize wstrstreambuf::xsputn(const wchar_t*s, streamsize n)
  {
    if ( !s || (n == 0) ) 
      return 0;

    if ( n > ( epptr()-pptr()+128 ) ){ 
      if ( setbuf(0, pptr()-pbase()+n+128)== 0){
        return 0;
      }

      char_traits<wchar_t>::copy(pptr(), s, n);
      pbump(n);
    }
    else{
      int  i=0;
      while((i < n) && ( !char_traits<wchar_t>::eq_int_type(sputc(*s++),traits::eof())))
        i++;
      return i;
    }
    return n;    
  }
  /* class wistrstream */

  /* wistrstream(const char_type *) */
  wistrstream::wistrstream(const char_type *s)
#ifndef _RWSTD_NO_COMPLEX_DEFAULT_TEMPLATES
  : basic_istream<wchar_t>()
#else
  : basic_istream<wchar_t, char_traits<wchar_t> >()
#endif // _RWSTD_NO_COMPLEX_DEFAULT_TEMPLATES 
  , __sb(s, 0)
  {
    init(&__sb);
  }

  /* istrstream(const char_type *, streamsize)   */
  wistrstream::wistrstream(const char_type *s, streamsize n)
#ifndef _RWSTD_NO_COMPLEX_DEFAULT_TEMPLATES
  : basic_istream<wchar_t>()
#else
  : basic_istream<wchar_t, char_traits<wchar_t> >()
#endif // _RWSTD_NO_COMPLEX_DEFAULT_TEMPLATES 
  , __sb(s, n)
  {
    init(&__sb);
  }

  /* wistrstream(char_type *)  */
  wistrstream::wistrstream(char_type *s)
#ifndef _RWSTD_NO_COMPLEX_DEFAULT_TEMPLATES
  : basic_istream<wchar_t>( )
#else
  : basic_istream<wchar_t, char_traits<wchar_t> >( )
#endif //_RWSTD_NO_COMPLEX_DEFAULT_TEMPLATES 
  , __sb((const char_type *)s, 0)
  {
    init(&__sb);
  }

  /* wistrstream(char_type *, streamsize) */

  wistrstream::wistrstream(char_type *s, streamsize n)
#ifndef _RWSTD_NO_COMPLEX_DEFAULT_TEMPLATES
  : basic_istream<wchar_t>( )
#else
  : basic_istream<wchar_t, char_traits<wchar_t> >( )
#endif // _RWSTD_NO_COMPLEX_DEFAULT_TEMPLATES
  , __sb((const char_type *)s, n)
  {
    init(&__sb);
  }

  /* ~istrstream() */
  wistrstream::~wistrstream()
  {
  }

  /* strstreambuf *rdbuf() const  */
  wstrstreambuf *wistrstream::rdbuf() const
  {
    return (wstrstreambuf*)&__sb;
  }

  /* char_type *str()  */
  wchar_t* wistrstream::str()
  {
    return __sb.str();
  }

  /* class wostrstream  */

  /* wostrstream() */

  wostrstream::wostrstream()
#ifndef _RWSTD_NO_COMPLEX_DEFAULT_TEMPLATES
  : basic_ostream<wchar_t>( )
#else
  : basic_ostream<wchar_t, char_traits<wchar_t> >( )
#endif // _RWSTD_NO_COMPLEX_DEFAULT_TEMPLATES
  , __sb()
  {
    init(&__sb);
  }

  /* ostrstream(char_type *, int, ios_base::openmode) */
  wostrstream::wostrstream(char_type *s, int n, ios_base::openmode mode)
#ifndef _RWSTD_NO_COMPLEX_DEFAULT_TEMPLATES
  : basic_ostream<wchar_t>( )
#else
  : basic_ostream<wchar_t, char_traits<wchar_t> >( )
#endif // _RWSTD_NO_COMPLEX_DEFAULT_TEMPLATES
  , __sb(s, n, (mode & (ios_base::app|ios_base::ate))? s + traits::length(s) : s)
  {
    init(&__sb);
//    s[n]=0;  // Codeguard reports access overrun, should be s[n-1]=0
    s[n-1]=0;
  }

  /* ~wostrstream()  */

  wostrstream::~wostrstream()
  {
  }

  /* wstrstreambuf *rdbuf() const */
  wstrstreambuf *wostrstream::rdbuf() const
  {
    return (wstrstreambuf *)&__sb;
  }

  /* void freeze(bool)  */

  void wostrstream::freeze(bool f)
  {
    __sb.freeze(f);
  }

  /* char_type *str() */

  wchar_t* wostrstream::str()
  {
    return __sb.str();
  }

  /* int pcount() const */

  int wostrstream::pcount() const
  {
    return __sb.pcount();
  }

  /* class wstrstream */

#if defined(DINKUMWARE_STL)
  wstrstream::wstrstream()
  : __sb()
#ifndef _RWSTD_NO_DEFAULT_TEMPLATES
  , basic_iostream<wchar_t>( &__sb )
#else
  , basic_iostream<wchar_t, char_traits<wchar_t> >( &__sb )
#endif // _RWSTD_NO_DEFAULT_TEMPLATES
  {
  }
#else //#if defined(DINKUMWARE_STL)
  wstrstream::wstrstream()
#ifndef _RWSTD_NO_DEFAULT_TEMPLATES
  : basic_iostream<wchar_t>( )
#else
  : basic_iostream<wchar_t, char_traits<wchar_t> >( )
#endif // _RWSTD_NO_DEFAULT_TEMPLATES
  , __sb()
  {
    init(&__sb);
  }
#endif //#if defined(DINKUMWARE_STL)

  /* wstrstream(char_type *, int, ios_base::openmode) */
#if defined(DINKUMWARE_STL)
  wstrstream::wstrstream(char_type *s, int n, ios_base::openmode mode)
  : __sb(s, n, (mode & (ios_base::app|ios_base::ate))? s + traits::length(s) : s)
#ifndef _RWSTD_NO_DEFAULT_TEMPLATES
  , basic_iostream<wchar_t>( &__sb )
#else
  , basic_iostream<wchar_t, char_traits<wchar_t> >( &__sb )
#endif // _RWSTD_NO_DEFAULT_TEMPLATES
  {
  }
#else //#if defined(DINKUMWARE_STL)
  wstrstream::wstrstream(char_type *s, int n, ios_base::openmode mode)
#ifndef _RWSTD_NO_DEFAULT_TEMPLATES
  : basic_iostream<wchar_t>( )
#else
  : basic_iostream<wchar_t, char_traits<wchar_t> >( )
#endif // _RWSTD_NO_DEFAULT_TEMPLATES
  , __sb(s, n, (mode & (ios_base::app|ios_base::ate))? s + traits::length(s) : s)
  {
    init(&__sb);
//    s[n]=0;
  }
#endif //#if defined(DINKUMWARE_STL)

  /* ~wstrstream() */
  wstrstream::~wstrstream()
  {
  }

  /* strstreambuf *rdbuf() const */
  wstrstreambuf *wstrstream::rdbuf() const
  {
    return (wstrstreambuf *)&__sb;
  }

  /* char_type *str()    */
  wchar_t *wstrstream::str()
  {
    return __sb.str();
  }

  /* void freeze(bool)  */
  void wstrstream::freeze(bool f)
  {
    __sb.freeze(f);
  }

  /* int pcount() const    */
  int wstrstream::pcount() const
  {
    return __sb.pcount();
  }

#ifndef _RWSTD_NO_NAMESPACE
}
#endif

#else
//#error Unknow compiler

#endif // defined(BI_COMP_MSC) 

/*
 * Copyright (c) 1998 by Yura Bidus.  ALL RIGHTS RESERVED. 
 */

#endif // defined(BI_STD_IO) 

//
// Borland includes this in run-time library
//
#if !defined(BI_COMP_BORLANDC)

#include <owl/private/regexp.h>

__OWL_BEGIN_NAMESPACE

/////////////////////////////////////
// TRegexp Class
typedef _TUCHAR PatternType;

int makepat(const _TCHAR _FAR* exp, PatternType _FAR* pat, size_t maxpattern);
const _TCHAR _FAR* matchs(const _TCHAR _FAR* str,
                          const PatternType _FAR* pat,
                          _TCHAR _FAR* _FAR* startpat);

const unsigned TRegexp::maxpat=128;

TRegexp::TRegexp(const _TCHAR _FAR* str)
{
  gen_pattern( str );
}

TRegexp::TRegexp(const TRegexp _FAR & r)
{
    copy_pattern( r );
}

TRegexp::~TRegexp()
{
    delete[] the_pattern;
}

void TRegexp::copy_pattern(const TRegexp _FAR & r)
{
    the_pattern = new PatternType[maxpat];
    memcpy( the_pattern, r.the_pattern, maxpat );
    stat = r.stat;
}

void TRegexp::gen_pattern(const _TCHAR _FAR* str)
{
    the_pattern = new PatternType[maxpat];
    stat = (StatVal)makepat( str, the_pattern, maxpat );
}

TRegexp _FAR & TRegexp::operator = ( const _TCHAR _FAR *cp )
{
    delete the_pattern;
    gen_pattern( cp );
    return *this;
}

TRegexp _FAR & TRegexp::operator = ( const TRegexp _FAR & r )
{
    if(this != &r)
        {
        delete the_pattern;
        copy_pattern( r );
        }
    return *this;
}

TRegexp::StatVal TRegexp::status() throw()
{
  StatVal temp = stat;
  stat = OK;
  return temp;
}

//typedef unsigned char PatternType;
typedef _TUCHAR PatternType;

int makepat(const _TCHAR _FAR *exp, PatternType  _FAR *pat, size_t maxpattern);
const _TCHAR _FAR * matchs( const _TCHAR _FAR *str,
                          const PatternType _FAR *pat,
                          _TCHAR _FAR * _FAR *startpat);

size_t TRegexp::find( const owl_string _FAR& str,
                      size_t _FAR *len,
                      size_t i ) const
{
  PRECONDITION( stat==OK );
  _TCHAR _FAR * startp;
  const _TCHAR _FAR * s = str.c_str();
  const _TCHAR _FAR * endp = NS_OWL::matchs( s+i, the_pattern, &startp );
  if( endp ){
    *len = (size_t)((_TCHAR _FAR *)endp - startp + 1);
    return (size_t)(startp - (_TCHAR _FAR *)s);
  }
  else{
  *len = 0;
    return static_cast<size_t>(-1); //JJH added static_cast
  }
}

/*
 *
 * Author:    Allen I. Holub
 *
 * (c) C Gazette. May be used freely as long as author and publication are
 * acknowledged
 *
 */

/*
 *
 * Modified by Borland International to compile without warnings as C++.
 *
 * Modified by Yura Bidus to support UNICODE.
 */

/* Metacharacters in the input:                                     */
#define BOL     _T('^')     /* start-of-line anchor                 */
#define EOL     _T('$')     /* end-of-line anchor                   */
#define ANY     _T('.')     /* matches any character                */
#define CCL     _T('[')     /* start a character class              */
#define CCLEND  _T(']')     /* end a character class                */
#define NCCL    _T('^')     /* negates character class if 1st char. */
#define CLOSURE _T('*')     /* Kleene closure (matches 0 or more)   */
#define PCLOSE  _T('+')     /* Positive closure (1 or more)         */
#define OPT     _T('?')     /* Optional closure (0 or 1)            */

//?????????????????
// 0x80 will not work in UNICODE !!!!!!!!!!!!
typedef enum action {    /* These are put in the pattern string  */
                         /* to represent metacharacters.         */
  M_BOL =    (0x80 | _T('^')),
  M_EOL =    (0x80 | _T('$')),
  M_ANY =    (0x80 | _T('.')),
  M_CCL =    (0x80 | _T('[')),
  M_OPT =    (0x80 | _T('?')),
  M_CLOSE =  (0x80 | _T('*')),
  M_PCLOSE = (0x80 | _T('+'))
} action;

//typedef unsigned char pattern;  /* pattern strings are unsigned char */
typedef _TUCHAR pattern;

#define IS_ACTION(x) ((x)&0x80) /* true => element of pat. string is an   */
                                /* action that represents a metacharacter */

/*----------------------------------------------------------------------*/
#define MAPSIZE 16      /* need this many bytes for character class bit map */

/*
 * Advance a pointer into the pattern template
 * to the next pattern element, this is a +1 for
 * all pattern elements but M_CCL, where you
 * to skip past both the M_CCL character and the
 * bitmap that follows that character
 */

#define ADVANCE(pat) (pat += (*pat == (pattern)M_CCL) ? (MAPSIZE+1) : 1)

/*
 * Bitmap functions. Set bit b in the map and
 * test bit b to see if it was set previously.
 */

#define SETBIT(b,map) ((map)[((b) & 0x7f) >>3] |= (1<< ((b) & 0x07)) )
#define TSTBIT(b,map) ((map)[((b) & 0x7f) >>3] &  (1<< ((b) & 0x07)) )
/*----------------------------------------------------------------------*/
#define E_NONE       0      /* Possible return values from pat_err. */
#define E_ILLEGAL    1      /* Set in makepat() to indicate prob-   */
#define E_NOMEM      2      /* lems that came up while making the   */
#define E_PAT        3      /* pattern template.                    */

/*----------------------------------------------------------------------*/

  static const _TCHAR _FAR *doccl(pattern _FAR *, const _TCHAR _FAR *);
  static int        hex2bin(int);
  extern int        makepat( const _TCHAR _FAR *, pattern _FAR *, size_t);
  extern const _TCHAR _FAR *matchs( const _TCHAR _FAR *, const pattern _FAR*, _TCHAR _FAR * _FAR *);
  static int        oct2bin(int);
  static int        omatch(const _TCHAR _FAR* _FAR*, const pattern _FAR *, const _TCHAR _FAR *);
  extern const _TCHAR _FAR *patcmp(const _TCHAR _FAR *, const pattern _FAR *, const _TCHAR _FAR *);
  extern int        esc( const _TCHAR _FAR * _FAR *);

/*----------------------------------------------------------------------*/
int
makepat( const _TCHAR _FAR *exp, pattern _FAR *pat, size_t maxpat)
//  char      _FAR *exp;      /* Regular expression */
//  pattern   _FAR *pat;      /* Assembled compiled pattern */
//  int             maxpat;   /* Length of pat */
{
  /*
   * Make a pattern template from the string pointed to by exp. Stop when
   * '\0' or '\n' is found in exp.  The pattern template is assembled
   * in pat whose length is given by maxpat.
   *
   * Return:
   * E_ILLEGAL       Illegal input pattern.
   * E_NOMEM         out of memory.
   * E_PAT           pattern too long.
   */

  pattern  _FAR *cur;       /* pointer to current pattern element  */
  pattern  _FAR *prev;      /* pointer to previous pattern element */
  int            Error = E_ILLEGAL;

  if (!*exp || *exp == _T('\n'))
    goto exit;

  if (*exp == CLOSURE || *exp == PCLOSE || *exp == OPT)
    goto exit;

  Error = E_NOMEM;
  if (!pat) 
    goto exit;      /* Check for bad pat */

  prev = cur = pat;
  Error = E_PAT;

  while (*exp && *exp != _T('\n')) {

    if (cur >= &pat[maxpat - 1]) 
      goto exit;

    switch (*exp) {
    case ANY:
      *cur = (pattern)M_ANY;
      prev = cur++;
      ++exp;
      break;

    case BOL:
      *cur = (cur == pat) ? (pattern)M_BOL : *exp;
      prev = cur++;
      ++exp;
      break;

    case EOL:
      *cur = (!exp[1] || exp[1] == _T('\n')) ? (pattern)M_EOL : *exp;
      prev = cur++;
      ++exp;
      break;

    case CCL:
      if (uint((cur - pat) + MAPSIZE) >= maxpat)
    goto exit;      /* not enough room for bit map */
      prev = cur;
      *cur++ = (pattern)M_CCL;
      exp = doccl(cur, exp);
      cur += MAPSIZE;
      break;

    case OPT:
    case CLOSURE:
    case PCLOSE:
      switch (*prev) {
      case M_BOL:
      case M_EOL:
      case M_OPT:
      case M_PCLOSE:
      case M_CLOSE:
    goto exit;
      }

      /* memmove( prev+1, prev, cur-prev ); */
      {
    pattern _FAR *p = cur;
    while (p > prev) {
      *p = *(p - 1);
      p--;
    }
      }
      *prev = (*exp == OPT) ? (pattern)M_OPT :
    (*exp == PCLOSE) ? (pattern)M_PCLOSE : (pattern)M_CLOSE;
      ++cur;
      ++exp;
      break;

    default:
      prev = cur;
      *cur++ = (pattern)esc(&exp);
      break;
    }
  }

  *cur = _T('\0');
  Error = E_NONE;

 exit:
  return Error;
}

/*----------------------------------------------------------------------*/
static const _TCHAR _FAR*
doccl( pattern _FAR *map, const _TCHAR _FAR* src)
{
  /*
   * Set bits in the map corresponding to characters specified in the src
   * character class.
   */

  int first, last, negative;
  const _TCHAR _FAR *start;

  ++src;            /* skip past the [          */
  negative = (*src == NCCL);
  if (negative)         /* check for negative ccl   */
    ++src;
  start = src;          /* start of characters in class */
  memset(map, 0, MAPSIZE);  /* bitmap initially empty       */

  while (*src && *src != CCLEND) {
    if (*src != _T('-')) {
      first = esc(&src);    /* Use temp. to avoid macro  */
      SETBIT(first, map);   /* side effects.             */
    }
    else if (src == start) {
      SETBIT(_T('-'), map);     /* literal dash at start or end */
      ++src;
    }
    else {
      ++src;            /* skip to end-of-sequence char */
      if (*src < src[-2]) {
        first = *src;
        last = src[-2];
      }
      else {
        first = src[-2];
        last = *src;
      }
      while (++first <= last)
        SETBIT(first, map);
      src++;
    }
  }

  if (*src == CCLEND)
    ++src;          /* Skip CCLEND */

  if (negative)
    for (first = MAPSIZE; --first >= 0;)
      *map++ ^= ~0;     /* invert all bits */

  return src;
}

/*----------------------------------------------------------------------*/
const _TCHAR _FAR *
matchs( const _TCHAR _FAR* str, const pattern _FAR* pat, _TCHAR _FAR* _FAR* startpat)
{
  const _TCHAR _FAR* endp = NULL;
  const _TCHAR _FAR* start;

  if (!pat)
    return NULL;

  if (*str == _T('\0')) {
    if ((*pat == (pattern)M_EOL) || (*pat == (pattern)M_BOL && (!pat[1] || pat[1] == (pattern)M_EOL)))
      endp = str;
  } 
  else {
    start = str;        /* Do a brute-force substring search,
                 * comparing a pattern against the input string */
    while (*str) {
      endp = patcmp(str, pat, start);
      if (endp)
    break;
      str++;
    }
  }
  *startpat = (_TCHAR*)str;
  return endp;
}

/*----------------------------------------------------------------------*/
const _TCHAR _FAR*
patcmp( const _TCHAR _FAR* str, const pattern _FAR* pat, const _TCHAR _FAR* start)
{
  /*
   * Like strcmp, but compares str against pat. Each element of str is
   * compared with the template until either a mis-match is found or the end
   * of the template is reached. In the former case a 0 is returned; in the
   * latter, a pointer into str (pointing to the last character in the
   * matched pattern) is returned. Strstart points at the first character in
   * the string, which might not be the same thing as line if the search
   * started in the middle of the string.
   */

  const _TCHAR _FAR* bocl;     /* beginning of closure string.         */
  const _TCHAR _FAR* end=0;    /* return value: end-of-string pointer. */

  if (!pat)         /* make sure pattern is valid   */
    return (NULL);

  while (*pat) {
    if (*pat == (pattern)M_OPT) {
      /*
       * Zero or one matches. It doesn't matter if omatch fails---it will
       * advance str past the character on success, though. Always advance
       * the pattern past both the M_OPT and the operand.
       */

      omatch(&str, ++pat, start);
      ADVANCE(pat);
    } 
    else if (!(*pat == (pattern)M_CLOSE || *pat == (pattern)M_PCLOSE)) {
      /*
       * Do a simple match. Note that omatch() fails if there's still
       * something in pat but we're at end of string.
       */

      if (!omatch(&str, pat, start))
        return NULL;

      ADVANCE(pat);

    } else {            /* Process a Kleene or positive closure */

      if (*pat++ == (pattern)M_PCLOSE)  /* one match required */
        if (!omatch(&str, pat, start))
          return NULL;

      /* Match as many as possible, zero is okay */

      bocl = str;
      while (*str && omatch(&str, pat, start)) { /* do nothing */ }

      /*
       * 'str' now points to the character that made made us fail. Try to
       * process the rest of the string. If the character following the
       * closure could have been in the closure (as in the pattern "[a-z]*t")
       * the final 't' will be sucked up in the while loop. So, if the match
       * fails, back up a notch and try to match the rest of the string
       * again, repeating this process recursively until we get back to the
       * beginning of the closure. The recursion goes, at most, one levels
       * deep.
       */

      if (*ADVANCE(pat)) {
        for (; bocl <= str; --str) {
          end = patcmp(str, pat, start);
          if (end) break;
        }
        return end;
      }
    break;
    }
  }

  /*
   * omatch() advances str to point at the next character to be matched. So
   * str points at the character following the last character matched when
   * you reach the end of the template. The exceptions are templates
   * containing only a BOLN or EOLN token. In these cases omatch doesn't
   * advance. Since we must return a pointer to the last matched character,
   * decrement str to make it point at the end of the matched string, making
   * sure that the decrement hasn't gone past the beginning of the string.
   *
   * Note that $ is a position, not a character, but in the case of a pattern
   * ^$, a pointer to the end of line character is returned. In ^xyz$, a
   * pointer to the z is returned.
   *
   * The --str is done outside the return statement because __max() is a macro
   * with side-effects.
   */

  --str;
  return (__max(start, str));
}

/*----------------------------------------------------------------------*/
static int
omatch( const _TCHAR _FAR* _FAR* strp,
        const pattern _FAR* pat,
        const _TCHAR _FAR* start )
{
  /*
   * Match one pattern element, pointed at by pat, against the character at
   * **strp. Return 0 on a failure, 1 on success. *strp is advanced to skip
   * over the matched character on a successful match. Closure is handled one
   * level up by patcmp().
   *
   * "start" points at the character at the left edge of the line. This might
   * not be the same thing as *strp if the search is starting in the middle
   * of the string. An end-of- line anchor matches '\n' or '\0'.
   */

  int advance = -1; /* amount to advance *strp, -1 == error  */

  switch (*pat) {
  case M_BOL:           /* First char in string? */
    if (*strp == start)     /* Only one star here.   */
      advance = 0;
    break;

  case M_ANY:           /* . = anything but newline */
    if (**strp != _T('\n'))
      advance = 1;
    break;

  case M_EOL:
    if (**strp == _T('\n') || **strp == _T('\0'))
      advance = 0;
    break;

  case M_CCL:
    if (TSTBIT(**strp, pat + 1))
      advance = 1;
    break;

  default:          /* literal match */
    if (**strp == *pat)
      advance = 1;
    break;
  }

  if (advance > 0)
    *strp += advance;

  return (advance + 1);
}

#define ISHEXDIGIT(x) (_istdigit(x)                             \
                            || (_T('a')<=(x) && (x)<=_T('f'))   \
                            || (_T('A')<=(x) && (x)<=_T('F'))   )

#define ISOCTDIGIT(x) (_T('0')<=(x) && (x)<=_T('7'))

static int hex2bin( int c )
{
     /* Convert the hex digit represented by 'c' to an int. 'c'
      * must be one of: 0123456789abcdefABCDEF
      */
     return (_istdigit(c) ? (c)-_T('0') : ((_toupper(c))-_T('A'))+10)  & 0xf;
}

static int oct2bin( int c )
{
     /* Convert the hex digit represented by 'c' to an int. 'c'
      * must be a digit in the range '0'-'7'.
      */
     return ( ((c)-_T('0'))  &  0x7 );
}

/*------------------------------------------------------------*/

int esc( const _TCHAR _FAR* _FAR* s)
{
     /* Map escape sequences into their equivalent symbols. Return
      * the equivalent ASCII character. *s is advanced past the
      * escape sequence. If no escape sequence is present, the
      * current character is returned and the string is advanced by
      * one. The following are recognized:
      *
      *  \b     backspace
      *  \f     formfeed
      *  \n     newline
      *  \r     carriage return
      *  \s     space
      *  \t     tab
      *  \e     ASCII ESC character ('\033')
      *  \DDD   number formed of 1-3 octal digits
      *  \xDDD  number formed of 1-3 hex digits
      *  \^C    C = any letter. Control code
      */

     int rval;

     if( **s != _T('\\') )
          rval = *( (*s)++ );
     else {
          ++(*s);                                 /* Skip the \ */
          switch( toupper(**s) ) {
            case _T('\0'):  rval = _T('\\');             break;
            case _T('B'):   rval = _T('\b') ;            break;
            case _T('F'):   rval = _T('\f') ;            break;
            case _T('N'):   rval = _T('\n') ;            break;
            case _T('R'):   rval = _T('\r') ;            break;
            case _T('S'):   rval = _T(' ')  ;            break;
            case _T('T'):   rval = _T('\t') ;            break;
            case _T('E'):   rval = _T('\033');           break;

            case _T('^'):
              rval = *++(*s) ;
              rval = _toupper(rval) - _T('@') ;
              break;

            case _T('X'):
              rval = 0;
              ++(*s);
              if( ISHEXDIGIT(**s) ) {
                   rval  = hex2bin( *(*s)++ );
              }
              if( ISHEXDIGIT(**s) ) {
                   rval <<= 4;
                   rval  |= hex2bin( *(*s)++ );
              }
              if( ISHEXDIGIT(**s) ) {
                   rval <<= 4;
                   rval  |= hex2bin( *(*s)++ );
              }
              --(*s);
              break;

            default:
              if( !ISOCTDIGIT(**s) )
                   rval = **s;
              else {
                   ++(*s);
                   rval = oct2bin( *(*s)++ );
                   if( ISOCTDIGIT(**s) ) {
                        rval <<= 3;
                        rval  |= oct2bin( *(*s)++ );
                   }
                   if( ISOCTDIGIT(**s) ) {
                        rval <<= 3;
                        rval  |= oct2bin( *(*s)++ );
                   }
                   --(*s);
              }
              break;
          }
          ++(*s);
     }
     return rval;
}

__OWL_END_NAMESPACE

#endif

//
//
//////////////////////////////////////////
#if defined(BI_STD_STRING)

# if _MSC_VER < 1600  
//Jogy - this does not link correctly with VC2010. In 6.32 and later the class TResString is removed, 
//       but here we keep it for backward binary compatibility for other compilers

#include <windows.h>
#include <string>

//__OWL_BEGIN_NAMESPACE

TResString::TResString(HINSTANCE instance, uint id, int len)
{
  _TCHAR *buf = NULL;
  try {
    buf = new _TCHAR[len];
    if (::LoadString(instance, id, buf, len)) {
      reserve(len);
      static_cast<owl_string &>(*this) = buf;
      delete [] buf;
    }
  }
  catch (...) {
    delete [] buf;
    throw;
  }
}
//__OWL_END_NAMESPACE

# endif // # if _MSC_VER < 1600  

#else

#include <owl/private/memory.h>

#if !defined(BI_COMP_BORLANDC)

//#include <iostream.h>
#if !defined(BI_STD_IO)
#  include <iomanip.h>
#else
#  include <iomanip>
#endif

//extern xalloc __xalloc;
__OWL_BEGIN_NAMESPACE

string::~string() THROW_NONE
{
    if( p->RemoveReference() == 0 )
        delete p;
}

void string::assert_element( size_t pos ) const THROW_OUTOFRANGE
{
    if( !valid_element(pos) )
        throw outofrange();
}

void string::assert_index( size_t pos ) const THROW_OUTOFRANGE
{
    if( !valid_index(pos) )
        throw outofrange();
}

//------------------------------------------------------------------------------
// TSubString
void TSubString::assert_element( size_t n ) const THROW_OUTOFRANGE
{
    if( !valid_element(n) )
        throw string::outofrange();
}

TStringRef::TStringRef( const char _FAR *str1,
                        size_t count1,
                        const char _FAR *str2,
                        size_t count2,
                        size_t extra ) : TReference(1), flags(0)
{
    nchars = count1+count2;
    capacity = round_capacity(nchars+extra);
    array = (_TCHAR _FAR *)malloc(capacity+1);
    if( array == 0 )
#if defined(BI_STD_EXCEPTIONS)
      throw XALLOC();
#else
      __xalloc.raise();
#endif
    if(count1)
      memcpy( array, str1, count1 );
    if(count2)
      memcpy( array+count1, str2, count2 );
    array[count1+count2] = _T('\0');
}

TStringRef::~TStringRef() THROW_NONE
{
    free(array);
}

void TStringRef::reserve( size_t ic ) THROW_XALLOC_OUTOFRANGE
{
  flags |= MemReserved;
  size_t newCapac = round_capacity(ic+1);
  if( capacity - newCapac > string::get_max_waste() ){
    array = (_TCHAR _FAR *)realloc( array, newCapac+1 );
    capacity = newCapac;
  }
}

void TStringRef::check_freeboard() THROW_NONE
{
    size_t newCapac = round_capacity(nchars);
    if( capacity - newCapac > string::get_max_waste() )
        {
        array = (char _FAR *)realloc( array, newCapac+1 );
        capacity = newCapac;
        }
}

void TStringRef::grow_to( size_t n )
    THROW_XALLOC_LENGTHERROR
{
    capacity = n;
    array = (char _FAR *)realloc(array, capacity+1); // NB: realloc() is used
    if( array == 0 )
#if defined(BI_STD_EXCEPTIONS)
      throw XALLOC();
#else
      __xalloc.raise();
#endif
}

size_t TStringRef::round_capacity( size_t nc ) THROW_NONE
{
    size_t ic = string::get_initial_capacity();
    size_t rs = string::get_resize_increment();
    return (nc - ic + rs - 1) / rs * rs + ic;
}

void TStringRef::splice( size_t start, size_t extent,
                         const char _FAR *dat, size_t n )
        THROW_XALLOC_LENGTHERROR
{
  char _FAR *destarray;         // Will point to final destination array

  // Final length:
  size_t tot = nchars + n - extent;
  // Final capacity:
  size_t newCapac = round_capacity(tot);

  // Resize if necessary:
  if( newCapac > capacity){
    grow_to(newCapac);  // Grew
    destarray = array;  // Record what will be the final
                        // destination array
  }
  else if( capacity-newCapac > string::get_max_waste() && (flags & MemReserved) == 0 ){
        // Shrunk.  destarray will point to brand new memory
    destarray = (char _FAR *)malloc(newCapac+1);
    if( array == 0 )
#if defined(BI_STD_EXCEPTIONS)
      throw XALLOC();
#else
      __xalloc.raise();
#endif
    if( start )
      memcpy( destarray, array, start ); // Copy beginning of string.
    capacity = newCapac;
  }
  else
    destarray = array;  // string capacity stayed the same.  Reuse old array.

  //
  // Copy the end of the string. This will be necessary if new memory is
  // involved, or if the size of the replacing substring does not match
  // the original extent.
  //
  if( destarray!=array || n!=extent )
    memmove(destarray+start+n, array+start+extent, nchars-start-extent);

  // Copy middle of string:
  if( n ){
    if( dat )
      memmove(destarray+start, dat, n);  /* NB: memmove() necessary */
    else
      memset(destarray+start, _T(' '), n);
  }

  nchars = tot;
  destarray[nchars] = _T('\0');

  if(destarray != array){
    free(array);
    array = destarray;
  }
}
//------------------------------------------------------------------------------
int       string::case_sensitive    = 1;
int       string::paranoid_check    = 0;
int       string::skip_white        = 1;
size_t    string::initial_capac     = 63;
size_t    string::resize_inc        = 64;
size_t    string::freeboard         = 63;

int string::set_case_sensitive( int tf )
{
    int ret = case_sensitive;
    case_sensitive = tf;
    return ret;
}

int string::set_paranoid_check( int ck )
{
    int ret = paranoid_check;
    paranoid_check = ck;
    return ret;
}

int string::skip_whitespace( int sk )
{
    int ret = skip_white;
    skip_white = sk;
    return ret;
}

size_t string::initial_capacity( size_t ic )
{
    size_t ret = initial_capac;
    initial_capac = ic;
    return ret;
}

size_t string::resize_increment( size_t ri )
{
    size_t ret = resize_inc;
    resize_inc = ri;
    if( resize_inc == 0 )
        resize_inc = 1;
    return ret;
}

size_t string::max_waste( size_t mw )
{
    size_t ret = freeboard;
    freeboard = mw;
    return ret;
}
//------------------------------------------------------------------------------
string _FAR & string::append( const string _FAR &s, size_t orig, size_t n )
    THROW_XALLOC_LENGTHERROR
{
    cow();
    size_t loc = __min(orig,s.length());
    size_t len = __min(s.length()-loc,n);
    p->splice( p->nchars, 0, s.c_str()+loc, len );
    return *this;
}

string _FAR & string::append( const char _FAR *cp )
    THROW_XALLOC_LENGTHERROR
{
    if( cp != 0 )
        {
        cow();
        p->splice( p->nchars, 0, cp, strlen(cp) );
        }
    return *this;
}

string _FAR & string::append( const char _FAR *cp, size_t orig, size_t n )
    THROW_XALLOC_LENGTHERROR
{
    if( cp != 0 )
        {
        cow();
        size_t slen = strlen(cp);
        size_t loc = __min(orig,slen);
        slen -= loc;
        size_t len = __min(slen,n);
        p->splice( p->nchars, 0, cp+loc, len );
        }
    return *this;
}

string _FAR & string::assign( const string _FAR & s, size_t orig, size_t n )
    THROW_XALLOC
{
    if( orig != 0 || n < s.length() )
        {
        size_t loc = __min(orig, s.length());
        size_t len = __min(s.length()-loc,n);
        TStringRef *temp = new TStringRef( s.c_str()+loc, len, 0, 0, 0 );
        if( p->RemoveReference() == 0 )
            delete p;
        p = temp;
        }
    else
        {
        s.p->AddReference();
        if( p->RemoveReference() == 0 )
            delete p;
        p = s.p;
        }
    return *this;
}

void string::clone()
{
    p->RemoveReference();
    p = new TStringRef( c_str(), length(), 0, 0, 0 );
}

string::string( HINSTANCE instance, UINT id, int len )
    THROW_XALLOC_LENGTHERROR
{
    p = new TStringRef( instance, id, len );
}

TStringRef::TStringRef( HINSTANCE instance, UINT id, int len )
    THROW_XALLOC_LENGTHERROR
:
  TReference(1), flags(0)
{
    capacity = round_capacity(len);
    array = (char _FAR *)malloc( capacity + 1 );
    if( array == 0 )
#if defined(BI_STD_EXCEPTIONS)
      throw XALLOC();
#else
      __xalloc.raise();
#endif
    nchars = LoadString( instance, id, array, len );
}

void string::to_lower() THROW_NONE
{
  cow();
  ::_tcslwr((LPTSTR)c_str());
}

void string::to_upper() THROW_NONE
{
  cow();
  ::_tcsupr((LPTSTR)c_str());
}

string _FARFUNC to_lower( const string _FAR &s ) THROW_NONE
{
    string temp = s.copy();
    temp.to_lower();
    return temp;
}

string _FARFUNC to_upper( const string _FAR &s ) THROW_NONE
{
    string temp = s.copy();
    temp.to_upper();
    return temp;
}

void TSubString::to_lower() THROW_NONE
{
    if( begin != NPOS )         // Ignore null substrings
        {
        s->cow();
        char _FAR *p = s->p->array + begin;
        unsigned n = extent;
        while( n-- )
            {
            *p = char(_tolower(*p));
            p++;
            }
        }
}

void TSubString::to_upper() THROW_NONE
{
    if( begin != NPOS )         // Ignore null substrings
        {
        s->cow();
        char _FAR *p = s->p->array + begin;
        unsigned n = extent;
        while( n-- )
            {
            *p = char(_toupper(*p));
            p++;
            }
        }
}

string string::substr( size_t pos, size_t n )
    const THROW_XALLOC_OUTOFRANGE
{
    assert_index(pos);
    return string( c_str(), pos, n );
}

string string::substr( size_t pos ) const THROW_XALLOC_OUTOFRANGE
{
    assert_index(pos);
    return string( c_str()+pos );
}

TSubString string::strip( string::StripType st, char c )
{
    size_t start = 0;                        // index of first character
    size_t end = length()-1;                 // index of last character
    const char _FAR *direct = p->array;

    if( st==Leading || st==Both )
        {
        for( ; start<=end; start++)
            {
            if( direct[start] != c )
                goto nonNull;
            }
        return TSubString( this, NPOS, 0 ); // Null substring
    }

nonNull:

    if(st==Trailing || st==Both)
        {
        for( ; end >= start; end--)
            if( direct[end] != c )
                break;
        }
    return TSubString(this, start, end-start+1);
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*  string I/O                                                            */
/*                                                                        */
/*------------------------------------------------------------------------*/


// Read to EOF, or null terminator, whichever comes first:
_OWLFUNC(istream _FAR &) string::read_file( istream _FAR & strm )
{
    cow();
    p->read_to_delim(strm, _T('\0'));
    return strm;
}

// Identical to above:
_OWLFUNC(istream _FAR &) string::read_string( istream _FAR & strm )
{
    cow();
    p->read_to_delim(strm, _T('\0'));
    return strm;
}

_OWLFUNC(istream _FAR &) string::read_line( istream _FAR & strm )
{
  char ch;

  // If the skipWhite flag has been set, then skip any leading whitespace
  if(get_skip_whitespace_flag()){
    do{
      strm.get(ch);
    } while(strm.good() && isspace(ch));
    strm.putback(ch);
  }
  cow();
  p->read_to_delim(strm, _T('\n'));
  return strm;
}

// Read to EOF or delimitor.
_OWLFUNC(istream _FAR &) string::read_to_delim( istream _FAR & strm, char delim )
{
  cow();
  p->read_to_delim(strm, delim);
  return strm;
}

_OWLFUNC(istream _FAR &) string::read_token( istream _FAR & strm )
{
  cow();
  p->read_token(strm);
  return strm;
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*  string global functions                                               */
/*                                                                        */
/*------------------------------------------------------------------------*/

_OWLFUNC(istream _FAR &)  operator>>( istream _FAR & strm,
                                           string _FAR & s )
{
  return s.read_token(strm);
}

_OWLFUNC(ostream _FAR &)  operator<<( ostream _FAR & os,
                                           const string _FAR & s )
{
  return os << s.c_str();
}

_OWLFUNC(istream _FAR &)  getline( istream _FAR & is, string _FAR & s )
{
  return s.read_line( is );
}

_OWLFUNC(istream _FAR &)  getline( istream _FAR & is, string _FAR &s, char c )
{
  return s.read_to_delim( is, c );
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*  TStringRef I/O                                                        */
/*                                                                        */
/*------------------------------------------------------------------------*/


/*------------------------------------------------------------------------*/
/*                                                                        */
/*  Read up to an EOF, or a delimiting character, whichever comes         */
/*  first.  The delimiter is not stored in the string,                    */
/*  but is removed from the input stream.                                 */
/*                                                                        */
/*  Because we don't know how big a string to expect, we first read       */
/*  as much as we can and then, if the EOF or null hasn't been            */
/*  encountered, do a resize and keep reading.                            */
/*                                                                        */
/*------------------------------------------------------------------------*/

void TStringRef::read_to_delim( istream _FAR & strm, char delim )
    THROW_XALLOC
{
  char ch;
  nchars = 0;

  while(1){

    // Read as many characters as we can, up to the delimitor:
    strm.get( array+nchars, capacity-nchars+1, delim );

    // This is the new string length:
    nchars += strlen( array+nchars );

    // What stopped us?  An EOF?
    if( !strm.good() )
      break;          // EOF encountered (or worse!)

    // Nope.  Was it the delimiter?
    strm.get(ch);
    if(ch==delim)
      break;  // Yup. We're done.  Don't put it back on the stream.
    else
      strm.putback(ch);   // Nope, Put it back and keep going.

    // If we got here, the read must have stopped because the buffer
    // was going to overflow.  Resize and keep going.
    grow_to( round_capacity(nchars + string::get_resize_increment()));
  }

  check_freeboard();
}

//
// Read a token, delimited by whitespace, from the input stream.
//

void TStringRef::read_token( istream _FAR & strm ) THROW_XALLOC
{
  nchars = 0;

  while(1){
    // Extractor will always store null byte
    strm >> setw(capacity-nchars+1) >> (array+nchars);
    nchars += strlen( array+nchars );
    if( !strm.good() || isspace(strm.peek()) )
      break; // Was complete token read?
    grow_to( round_capacity(nchars + string::get_resize_increment()));
  }
  check_freeboard();
}

string _FAR &string::remove( size_t pos, size_t n )
    THROW_XALLOC_OUTOFRANGE
{
  if( pos > length() )
    throw outofrange();
  cow();
  p->splice( pos, __min(n,length()-pos), 0, 0 );
  return *this;
}

size_t string::rfind( const string _FAR &s, size_t startindex ) const THROW_NONE
{
  unsigned patl;
  return rfind_index( s.c_str(), startindex, patl ); // Throws away "patl"
}

size_t string::rfind_index( const char _FAR * pattern,
                            size_t startindex,
                            size_t _FAR & patl ) const
{
    if( get_case_sensitive_flag() )
        return rfind_case_index( pattern, startindex, patl );
    else
        return NS_OWL::to_upper(*this).rfind_case_index(NS_OWL::to_upper(string(pattern)).c_str(), startindex, patl);
}

size_t string::rfind_case_index( const char _FAR *cp,
                                 size_t startindex,
                                 size_t _FAR &patl ) const
{

    const long q = 33554393L;
    const long q32 = q<<5;

    size_t testlength = startindex;
    size_t patternlength = patl = strlen(cp);
    if( testlength < patternlength )
        return NPOS;
    if( patternlength == 0 )
        return 0;

    long patternHash = 0;
    long testHash = 0;

    const char _FAR *testP = c_str()+startindex-1;
    const char _FAR *patP = cp+patternlength-1;
    long x = 1;
    size_t i = patternlength-1;

    while( i-- )
        x = (x<<5)%q;

    for( i=0; i<patternlength; i++ )
        {
        patternHash = ( (patternHash<<5) + *patP--  ) % q;
        testHash    = ( (testHash   <<5) + *testP-- ) % q;
        }

    testP = c_str()+startindex-1;
    const char _FAR *end = c_str() + patternlength - 1;

    while (1)
        {

        if(testHash == patternHash)
            if( !get_paranoid_check_flag() ||
                !strncmp( testP, cp, patternlength) )
              return (size_t)(testP-c_str()-patternlength+1);

        if( testP == end )
            break;

        // Advance & calculate the new hash value:
        testHash = ( testHash + q32 - *testP * x                  ) % q;
        testHash = ( (testHash<<5)  + *(testP-- - patternlength) ) % q;
        }
    return NPOS;          // Not found.
}

void string::resize( size_t n )
{
    cow();
    if( n>length() )
        p->splice( length(), 0, 0, n-length() );      // Grew
    else
        p->splice( n, length()-n, 0, 0 );             // Shrunk
}

void string::reserve( size_t ic ) THROW_XALLOC_OUTOFRANGE
{
    p->reserve( ic );
}

string _FAR &string::replace( size_t pos,
                              size_t n1,
                              const string _FAR &s,
                              size_t orig,
                              size_t n2 )
    THROW_XALLOC_RANGE_LENGTH
{
    if( pos > length() )
        throw outofrange();
    cow();
    size_t loc = __min(orig,s.length());
    size_t len = __min(s.length()-loc,n2);
    p->splice( pos, __min(n1,length()-pos), s.c_str()+loc, len );
    return *this;
}

string _FAR & string::prepend( const char _FAR *cp, size_t orig, size_t n )
    THROW_XALLOC_LENGTHERROR
{
    if( cp != 0 )
        {
        cow();
        size_t slen = strlen(cp);
        size_t loc = __min(orig,slen);
        size_t len = __min(slen-loc,n);
        p->splice( 0, 0, cp+loc, len );
        }
    return *this;
}

string  _FARFUNC operator + ( const char _FAR *cp,
                                    const string _FAR &s )
    THROW_XALLOC_LENGTHERROR
{
    return string(cp) + s.c_str();
}

int TSubString::operator == ( const char _FAR *cp ) const THROW_NONE
{
    if( cp == 0 )
        cp = _T("");

    if( is_null())
        return 0;

    if( string::get_case_sensitive_flag() )
        return !strncmp( s->p->array + begin, cp, extent );
    else
        return !strnicmp( s->p->array + begin, cp, extent );
}

const TSubString string::operator()( const TRegexp _FAR &r, size_t start )
    const THROW_NONE
{
    size_t len;
    size_t begin = find( r, &len, start );
    return TSubString( this, begin, len );
}

TSubString string::operator()( const TRegexp _FAR &r, size_t start ) THROW_NONE
{
    size_t len;
    size_t begin = find( r, &len, start );
    return TSubString( this, begin, len );
}

char _FAR & TSubString::operator()( size_t pos ) THROW_OUTOFRANGE
{
    assert_element(pos);
    s->cow();
    return s->p->array[begin+pos];
}

char _FAR & string::operator()( size_t pos ) THROW_OUTOFRANGE
{
    assert_element(pos);
    cow();
    return p->array[pos];
}

TSubString _FAR & TSubString::operator = ( const string _FAR &str ) THROW_NONE
{
    if( !is_null() )
        {
        s->cow();
        s->p->splice(begin, __min(extent,str.length()),
                     str.c_str(), str.length());
        }
    return *this;
}



string _FAR &string::insert( size_t pos,
                             const string _FAR &s,
                             size_t orig,
                             size_t n )
    THROW_XALLOC_RANGE_LENGTH
{
    if( pos > length() )
        throw outofrange();
    cow();
    size_t loc = __min(orig,s.length());
    size_t len = __min(s.length()-loc,n);
    p->splice( pos, 0, s.c_str()+loc, len );
    return *this;
}

string _FAR &string::insert( size_t pos, const string _FAR &s )
    THROW_XALLOC_RANGE_LENGTH
{
    if( pos > length() )
        throw outofrange();
    cow();
    p->splice( pos, 0, s.c_str(), s.length() );
    return *this;
}

union TCharMask
{
    unsigned in[sizeof(unsigned)];
    char ch[sizeof(unsigned)*sizeof(unsigned)];
};

#ifdef __FLAT__
const union TCharMask Mask = { { 0, 0xFF, 0xFFFF, 0xFFFFFF } };
#endif

unsigned string::hash() const
{
    if( get_case_sensitive_flag() )
        return hash_case();
    else
        return NS_OWL::to_upper(*this).hash_case();
}

unsigned string::hash_case() const
{
    unsigned i, h;
    const unsigned _FAR *c;

    h = length();                 // Mix in the string length.
    i = h/sizeof(unsigned);       // Could do "<<" here, but less portable.
    c = (const unsigned _FAR *)c_str();

    while( i-- )
        h ^= *c++;        // XOR in the characters.

    // If there are any remaining characters,
    // then XOR in the rest, using a mask:
    if( (i = length()%sizeof(unsigned)) != 0 )
        {
        h ^= *c & Mask.in[i];
        }
    return h;
}

size_t string::find_last_not_of( const string _FAR &s, size_t pos )
    const THROW_NONE
{
    if( pos == 0 )
        return NPOS;
    if( pos > length() )
        pos = length();
    else
        pos++;
    const char *target = s.c_str();
    const char *text = c_str();
    while( --pos > 0 )
        {
        if( strchr( target, text[pos] ) == 0 )
            return pos;
        }
    return NPOS;
}

size_t string::find_last_of( const string _FAR &s, size_t pos ) const THROW_NONE
{
    size_t len = length();

    if( pos == 0 || len == 0 )
        return NPOS;
    if( pos > len )
        pos = len;
    else
        pos++;
    const char *target = s.c_str();
    const char *text = c_str();
    while( --pos > 0 )
        {
        if( strchr( target, text[pos] ) != 0 )
            return pos;
        }
    return NPOS;
}

size_t string::find_first_not_of( const string _FAR &s, size_t pos ) const
    THROW_NONE
{
    if( pos >= length() )
        return NPOS;
    size_t res = strspn( p->array+pos, s.c_str() );
    if( res == length() )
        return NPOS;
    else
        return res+pos;
}

size_t string::find_first_of( const string _FAR &s, size_t pos ) const THROW_NONE
{
    if( !valid_element(pos) )
        return NPOS;
    char _FAR * f = strpbrk( p->array+pos, s.c_str() );
    return f ? (size_t)(f-p->array) : NPOS;
}

size_t string::find( const TRegexp _FAR & r,
                     size_t _FAR * extent,
                     size_t start ) const THROW_NONE
{
    return r.find( *this, extent, start );
}

size_t string::find( const TRegexp _FAR &r, size_t start ) const THROW_NONE
{
    size_t len;
    return r.find( *this, &len, start ); // len not used
}

TSubString string::substring( const char _FAR *cp, size_t start ) THROW_NONE
{
    unsigned patl;
    size_t pos = find_index( cp, start, patl );
    return TSubString( this, pos, pos == NPOS ? 0 : patl );
}

const TSubString string::substring( const char _FAR *cp, size_t start )
    const THROW_NONE
{
    unsigned patl;
    size_t pos = find_index( cp, __min(start,strlen(cp)), patl );
    return TSubString( this, pos, pos == NPOS ? 0 : patl );
}

size_t string::find( const string _FAR &s, size_t startindex ) const THROW_NONE
{
    unsigned patl;
    return find_index( s.c_str(), startindex, patl ); // Throws away "patl"
}

size_t string::find_index( const char _FAR * pattern,
                           size_t startindex,
                           unsigned _FAR & patl ) const
{
    if( get_case_sensitive_flag() )
        return find_case_index( pattern, startindex, patl );
    else
        return NS_OWL::to_upper(*this).find_case_index(NS_OWL::to_upper(string(pattern)).c_str(), startindex, patl);
}

size_t string::find_case_index( const char _FAR *cp,
                                size_t startindex,
                                unsigned _FAR &patl ) const
{

    const long q = 33554393L;
    const long q32 = q<<5;

    size_t testlength = length() - startindex;
    size_t patternlength = patl = strlen(cp);
    if( testlength < patternlength )
        return NPOS;
    if( patternlength == 0 )
        return 0;

    long patternHash = 0;
    long testHash = 0;

    const char _FAR *testP = c_str()+startindex;
    const char _FAR *patP = cp;
    long x = 1;
    size_t i = patternlength-1;

    while( i-- )
        x = (x<<5)%q;

    for( i=0; i<patternlength; i++ )
        {
        patternHash = ( (patternHash<<5) + *patP++  ) % q;
        testHash    = ( (testHash   <<5) + *testP++ ) % q;
        }

    testP = c_str()+startindex;
    const char _FAR *end = testP + testlength - patternlength;

    while (1)
        {

        if(testHash == patternHash)
            if( !get_paranoid_check_flag() ||
                !strncmp( testP, cp, patternlength) )
              return (size_t)(testP-c_str());

        if( testP >= end )
            break;

        // Advance & calculate the new hash value:
        testHash = ( testHash + q32 - *testP * x                 ) % q;
        testHash = ( (testHash<<5)  + *(patternlength + testP++) ) % q;
        }
    return NPOS;          // Not found.
}
/*
string::string( char c )
    THROW_XALLOC_LENGTHERROR
{
    p = new TStringRef(c,1);
}
*/
TStringRef::TStringRef( char c, size_t n )
:
  TReference(1), flags(0)
{
  nchars = n;
  capacity = round_capacity( nchars );
  array = (char _FAR *)malloc( capacity + 1 );
  if( array == 0 )
#if defined(BI_STD_EXCEPTIONS)
      throw XALLOC();
#else
      __xalloc.raise();
#endif
  memset( array, c, nchars );
  array[nchars] = _T('\0');
}


string::string( const TSubString _FAR &ss )
    THROW_XALLOC
{
    p = new TStringRef( ss.s->c_str()+ss.start(),
                        ss.is_null()?0:ss.length(), 0, 0, 0 );
}

string::string( const string _FAR &s, size_t orig, size_t n )
    THROW_XALLOC
{
    if( orig != 0 || n < s.length() )
        {
        size_t loc = __min(orig,s.length());
        size_t len = __min(s.length()-loc,n);
        p = new TStringRef( s.c_str()+loc, len, 0, 0, 0 );
        }
    else
        {
        p = s.p;
        p->AddReference();
        }
}

string::string( const char _FAR *pstr, size_t orig, size_t n )
    THROW_XALLOC_LENGTHERROR
{
    unsigned slen = strlen( pstr );
    size_t loc = __min(orig,slen);
    size_t len = __min(slen-loc,n);
    p = new TStringRef( pstr+loc, len, 0, 0, 0 );
}

string::string( const char _FAR *cp ) THROW_XALLOC_LENGTHERROR
{
    p = new TStringRef( cp, cp?strlen(cp):0, 0, 0, 0 );
}

string::string( const string _FAR &s ) THROW_XALLOC
{
    p = s.p;
    p->AddReference();
}

string::string() THROW_XALLOC
{
    p = new TStringRef(0,0,0,0,string::get_initial_capacity());
}

string string::copy() const THROW_XALLOC
{
    return string(*this, (char _FAR *)0); /* actual copy. Not ++reference */
}

size_t string::copy( LPTSTR cb, size_t n, size_t pos )
    THROW_OUTOFRANGE
{
    if( n > length()-pos )
        n = length()-pos;
    memcpy(  cb, c_str()+pos, n );
    return n;
}

size_t string::copy( LPTSTR cb, size_t n ) THROW_OUTOFRANGE
{
    if( n > length() )
        n = length();
    memcpy( cb, c_str(), n );
    return n;
}

int string::contains( const char _FAR *cp ) const THROW_NONE
{
    return string::find(string(cp)) != NPOS;
}

int string::compare( const string _FAR &s, size_t orig, size_t n ) const THROW_NONE
{
    size_t loc = __min(orig,s.length());
    size_t len = __min(s.length()-loc,n);
    if( get_case_sensitive_flag() )
        return strncmp( c_str(), s.c_str()+loc, len );
    else
        return strnicmp( c_str(), s.c_str()+loc, len );
}

int string::compare( const string _FAR &s ) const THROW_NONE
{
    if( get_case_sensitive_flag() )
        return strcmp( c_str(), s.c_str() );
    else
        return stricmp( c_str(), s.c_str() );
}
#endif
//------------------------------------------------------------------------------
const size_t MaxCharsInLong = 20; // maxchar + prefix

/*
 * Convert val>=0 to ascii in buffer b, with implied numeric base.
 * Digits are acquired in reverse order, so we start at the end of the
 * buffer and work forward.
 * 'b' initially points to the end of the buffer, and is assumed big enough.
 * Return a pointer to the beginning address of the null-terminated string.
 */

// decimal conversion
static TCHAR* todec(TCHAR *b, unsigned long val)
{
  *b = _T('\0');
  do  {
    *--b = (
    (int) (val % 10))
     + _T('0');
    val /= 10;
  } while( val );
  return b;
}
// octal conversion
static TCHAR* tooct(TCHAR *b, unsigned long val)
{
  *b = _T('\0');
  do  {
    *--b = (TCHAR)((val & 7) + _T('0'));
    val /= 8;   // let compiler make this a shift if appropriate
  } while( val );
  return b;
}

// hex conversion, with indicator for uppercase or lowercase letters
static TCHAR *tohex(TCHAR *b, unsigned long val, int upper)
{
  static _TCHAR digits[2][16] = {
  {_T('0'),_T('1'),_T('2'),_T('3'),_T('4'),_T('5'),_T('6'),
   _T('7'),_T('8'),_T('9'),_T('a'),_T('b'),_T('c'),_T('d'),_T('e'),_T('f')},
  {_T('0'),_T('1'),_T('2'),_T('3'),_T('4'),_T('5'),_T('6'),
   _T('7'),_T('8'),_T('9'),_T('A'),_T('B'),_T('C'),_T('D'),_T('E'),_T('F')}
  };
  TCHAR *d = upper ? &digits[1][0] : &digits[0][0];
  *b = _T('\0');
  do  {
    *--b = d[(int)val & 0xf];
    val /= 16;  // let compiler make this a shift if appropriate
  }while( val );
  return b;
}
//------------------------------------------------------------------------------
// additional conversion flags
enum cvtFlags{
  cvtDec         = 0x000,
  cvtOct         = 0x001,
  cvtHex         = 0x002,
  cvtShowbase    = 0x004,
  cvtUppercase   = 0x008,
  cvtShowpos     = 0x010,
  cvtFixed       = 0x020,
  cvtScientific  = 0x040,
  cvtShowpoint  = 0x080,
};

#if defined(BI_COMP_BORLANDC)
#define _fcvt fcvt
#define _ecvt ecvt
#define _gcvt gcvt
#endif
//------------------------------------------------------------------------------
void Num2Str(const double d, LPTSTR buffer, int precision, int flags )
{
  _TCHAR buf[60]; // big enough for any floating result
  _TCHAR* prefix = buf+1;

  if( flags & cvtFixed ){
    //format = _T('f');
    int dec, sign;
    strcpy(prefix, _fcvt(d, precision, &dec, &sign));
    if (sign==0 && (flags & cvtShowpos))
      *--prefix = _T('+');
  }
  else if( flags & cvtScientific ){
    //format = _T('e');
    int dec, sign;
    strcpy(prefix, _ecvt(d, precision, &dec, &sign));
    if (sign==0 && (flags & cvtShowpos))
      *--prefix = _T('+');
  }
  else{
    //format = _T('g');
    _gcvt(d, precision, prefix);
    if (*prefix != _T('-')  && (flags & cvtShowpos))
      *--prefix = _T('+');
  }

  // now we have a formatted string for output
  _tcscpy(buffer,prefix);
}
//------------------------------------------------------------------------------
void Num2Str(const int32 l, LPTSTR buffer, int /*precision*/, int flags)
{
  TCHAR buf[MaxCharsInLong];
  TCHAR *p;

  // find conversion base
  int base = (flags & cvtHex) ? 16 : ((flags & cvtOct) ? 8 : 10);

  // do we treat this as negative?  (oct and hex are unsigned)
  int neg = base == 10  &&  l < 0;

  // value to use, exclusive of sign
  unsigned long ul = neg ? -l : l;

  if( base == 10 ){
    p = todec(buf + MaxCharsInLong - 1, ul);
    // compute any sign prefix
    if( ul )
      if( neg )
        *--p = _T('-');
      else if( flags & cvtShowpos )
        *--p = _T('+');
  }
  else if( base == 16 ){
    int upper = (flags & cvtUppercase) != 0;
    p = tohex(buf + MaxCharsInLong - 1, ul, upper);
    // compute any base prefix
    if( flags & cvtShowbase ){
      *--p = upper ? _T('X') : _T('x');
      *--p = _T('0');
    }

  }
  else{ /* base == 8 */
    p = tooct(buf + MaxCharsInLong - 1, ul);
    // compute any base prefix
    if( flags & cvtShowbase )
      *--p = _T('0');
  }

  // now we have a formatted string for output, to be possibly padded
  _tcscpy(buffer,p);
}
//------------------------------------------------------------------------------
void Num2Str(const uint32 ul, LPTSTR buffer, int /*precision*/, int flags)
{
  TCHAR buf[MaxCharsInLong];
  char *p;

  if( flags & cvtHex ){
    int upper = (flags & cvtUppercase) != 0;
    p = tohex(buf + MaxCharsInLong - 1, ul, upper);

    // compute any base prefix
    if( flags & cvtShowbase){
      *--p = _T('0');
      *--p = upper ? _T('X') : _T('x');
    }
  }
  else if( flags & cvtOct ){
    p = tooct(buf + MaxCharsInLong - 1, ul);

    // compute any base prefix
    if( flags & cvtShowbase )
      *--p = _T('0');
  }
  else{
    p = todec(buf + MaxCharsInLong - 1, ul);
    // compute any sign prefix
    if( ul  &&  (flags & cvtShowpos) )
      *--p = _T('+');
  }
  _tcscpy(buffer,p);
}
//------------------------------------------------------------------------------
#if defined(BI_COMP_BORLANDC) && !defined(BI_STD_EXCEPTIONS)
#define flags       string_flags
#define precision    string_precision
#define dec         string_dec
#define oct         string_oct
#define hex         string_hex
#define showpos     string_showpos
#define uppercase   string_uppercase
#define showbase    string_showbase
#define fixed        string_fixed
#define scientific  string_scientific

static unsigned s_flags    = 0;
static size_t precis       = 6;
_OWLFUNC(void) string_flags(unsigned flag)
{
  s_flags = flag;
}
_OWLFUNC(unsigned) string_flags()
{
  return s_flags;
}
_OWLFUNC(size_t) string_precision()
{
  return precis;
}
_OWLFUNC(void) string_precision(size_t pres)
{
  precis = pres;
}
#else
unsigned string::s_flags    = 0;
size_t string::precis       = 6;
#endif

// format and append a signed long
#if defined(BI_COMP_BORLANDC) && !defined(BI_STD_EXCEPTIONS)
_OWLFUNC(string) string_append(const string _FAR &s, const signed long l)
#else
string _FAR & string::append( const signed long l ) const
    THROW_XALLOC_LENGTHERROR
#endif
{
  TCHAR buffer[MaxCharsInLong+4];
  // now we have a formatted string for output, to be possibly padded
  // if BC 5.01/5.02 use RTL cstring else use OWL cstring
#if defined(BI_COMP_BORLANDC) && !defined(BI_STD_EXCEPTIONS)
  Num2Str(l, buffer, string_precision(), string_flags());
  return string(s).append(buffer);
#else
  Num2Str(l, buffer, precision(), flags());
  return ((string*)this)->append(buffer);
#endif
}
//------------------------------------------------------------------------------
#if defined(BI_COMP_BORLANDC) && !defined(BI_STD_EXCEPTIONS)
_OWLFUNC(string) string_append(const string _FAR &s, const unsigned long ul)
#else
string _FAR & string::append( const unsigned long ul ) const
    THROW_XALLOC_LENGTHERROR
#endif
{
  TCHAR buffer[MaxCharsInLong+4];
#if defined(BI_COMP_BORLANDC) && !defined(BI_STD_EXCEPTIONS)
  Num2Str(ul, buffer, string_precision(), string_flags());
  return string(s).append(buffer);
#else
  Num2Str(ul, buffer, precision(), flags());
  return ((string*)this)->append(buffer);
#endif
}
//------------------------------------------------------------------------------
// format and append a signed long
//------------------------------------------------------------------------------
#if defined(BI_COMP_BORLANDC) && !defined(BI_STD_EXCEPTIONS)
_OWLFUNC(string) string_append(const string _FAR &s, const double d)
#else
string _FAR & string::append( const double d ) const
    THROW_XALLOC_LENGTHERROR
#endif
{
  _TCHAR buffer[60]; // big enough for any floating result
#if defined(BI_COMP_BORLANDC) && !defined(BI_STD_EXCEPTIONS)
  Num2Str(d, buffer, string_precision(), string_flags());
  return string(s).append(buffer);
#else
  Num2Str(d, buffer, precision(), flags() );
  return ((string*)this)->append(buffer);
#endif
}

__OWL_END_NAMESPACE

#endif // BI_STD_STRING

//==============================================================================

