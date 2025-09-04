//-------------------------------------------------------------------
// OWL File Extensions (OWLFX) Class Library
// OWL NExt Project
// All rights reserved
//
//$Revision: 1.1.4.3 $
//$Author: jogybl $
//$Date: 2010-02-23 10:45:43 $
// Abstarc DBase layer
//-------------------------------------------------------------------
#if !defined(OWLFX_ABASE_H) // Sentry, use file only if it's not already included.
#define OWLFX_ABASE_H

#include <owlfx/defs.h>

// Date and Time definitions.
#include <owl/date.h>
#include <owl/time.h>

#if defined(BI_STD_RTL)
#include <vector>
#else
#include <owl/template.h>
#endif

// will be in OWL core
#if !defined(MEMBER_METHOD_PROLOGUE)
#define MEMBER_METHOD_PROLOGUE(theClass, localClass, resultName) \
	theClass* resultName = \
		((theClass*)((BYTE*)this - offsetof(theClass, localClass))); \
	resultName; // avoid warning from compiler

#endif

_OWLFX_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>

enum TDBOpenMode {
	dbmReadWrite	= 0, // like IDAPI32
	dbmReadOnly		= 1, // like IDAPI32
};

enum TDBLockType {
	dblctNone				= 0, // like IDAPI32
	dblctWryteOnly	= 1, // like IDAPI32
	dblctReadOnly		= 2, // like IDAPI32
};

enum  TDBSharedMode {                           // Database/Table Share type
	dbOpenShared = 0,                 // Open shared  (Default)
  dbOpenExcl   = 1,                 // Open exclusive
};

// the same as IDEAPI32
enum TDBQueryLang {
   dbqrlUNKNOWN = 0,  // UNKNOWN (Error)
   dbqrlQBE = 1,      // QBE
   dbqrlSQL = 2       // SQL
};

//{{GENERIC = TDBaseEvent}}
struct _OWLFXCLASS TDBaseEvent{
	enum TType{cbNone, cbRefresh,cbCommit};
	virtual long FireEvent(TType type, long p1 = 0, long p2 = 0) = 0;
}; //{{TDBaseEvent}}

//
//-----------------------------------------------------
// class TDBBlob
// ~~~~~ ~~~~~~~
//
class _OWLFXCLASS TDBBlob {
  public:
    virtual ~TDBBlob(){}

    virtual uint32	Get(const uint32 p1, const uint32 p2, uint8* buf) const = 0;
    virtual void		Set(const uint32 p1, const uint32 p2, const uint8* buf) = 0;
//    virtual void    GetHeading(uint8* buf) const = 0;//???
    virtual uint32  GetSize() const = 0;
//    virtual void  Truncate(const uint32 p1) = 0;//???
};

//{{GENERIC = TDBDataLink}}
class _OWLFXCLASS TDBDataLink {
  public:
    TDBDataLink ();
    virtual ~TDBDataLink();
		// common operations
		owl_string GetFieldTypeName(const owl_string& fldName) const;
    uint32 GetBlobSize(const owl_string&) const;

		TDBBlob* GetBlob(const owl_string& fldName, const TDBOpenMode mode = dbmReadWrite);

		virtual owl_string GetFieldName(const uint) const = 0;
    virtual uint GetFieldNumber(const owl_string&) const = 0;
    virtual owl_string GetFieldTypeName(const uint) const = 0;
    virtual uint32 GetBlobSize(const uint) const = 0;
		virtual TDBBlob* GetBlob(const uint fldId, const TDBOpenMode mode = dbmReadWrite) = 0;

    bool	GetField(const owl_string& fldName, owl_string& value) const;
    bool	GetField(const owl_string& fldName, bool& value) const;
    bool	GetField(const owl_string& fldName, int16& value) const;
    bool	GetField(const owl_string& fldName, int32& value) const;
    bool	GetField(const owl_string& fldName, uint16& value) const;
    bool	GetField(const owl_string& fldName, uint32& value) const;
    bool	GetField(const owl_string& fldName, float& value) const;
    bool	GetField(const owl_string& fldName, double& value) const;
    bool	GetField(const owl_string& fldName, long double& value) const;
    bool	GetField(const owl_string& fldName, __OWL TDate& value) const;
    bool	GetField(const owl_string& fldName, __OWL TTime& value) const;
    bool	GetField(const owl_string& fldName, __OWL TSystemTime& value) const;
    bool	IsBlank(const owl_string& fldName) const;

    void	SetBlank(const owl_string& fldName);
    void	SetField(const owl_string& fldName, const owl_string& value);
    void	SetField(const owl_string& fldName, const char*  value);
    void	SetField(const owl_string& fldName, const bool value);
    void	SetField(const owl_string& fldName, const int16 value);
    void	SetField(const owl_string& fldName, const int32 value);
    void	SetField(const owl_string& fldName, const uint value);
    void	SetField(const owl_string& fldName, const uint32 value);
    void	SetField(const owl_string& fldName, const float value);
    void	SetField(const owl_string& fldName, const double value);
    void	SetField(const owl_string& fldName, const long double value);
    void	SetField(const owl_string& fldName, const __OWL TDate& value);
    void	SetField(const owl_string& fldName, const __OWL TTime& value);
    void	SetField(const owl_string& fldName, const __OWL TSystemTime& value);
    bool	VerifyField(const owl_string& fldName) const;

		virtual bool	GetField(const uint fldId, owl_string& value) const = 0;
    virtual bool	GetField(const uint fldId, bool& value) const = 0;
    virtual bool	GetField(const uint fldId, int16& value) const = 0;
    virtual bool	GetField(const uint fldId, int32& value) const = 0;
    virtual bool	GetField(const uint fldId, uint16& value) const = 0;
    virtual bool	GetField(const uint fldId, uint32& value) const = 0;
    virtual bool	GetField(const uint fldId, float& value) const = 0;
    virtual bool	GetField(const uint fldId, double& value) const = 0;
    virtual bool	GetField(const uint fldId, long double& value) const = 0;
    virtual bool	GetField(const uint fldId, __OWL TDate& value) const = 0;
    virtual bool	GetField(const uint fldId, __OWL TTime& value) const = 0;
    virtual bool	GetField(const uint fldId, __OWL TSystemTime& value) const = 0;
    virtual bool	IsBlank(const uint fldId) const = 0;

    virtual void	SetBlank(const uint fldId) = 0;
    virtual void	SetField(const uint fldId, const owl_string& value) = 0;
    virtual void	SetField(const uint fldId, const char*  value) = 0;
    virtual void	SetField(const uint fldId, const bool value) = 0;
    virtual void	SetField(const uint fldId, const int16 value) = 0;
    virtual void	SetField(const uint fldId, const int32 value) = 0;
    virtual void	SetField(const uint fldId, const uint value) = 0;
    virtual void	SetField(const uint fldId, const uint32 value) = 0;
    virtual void	SetField(const uint fldId, const float value) = 0;
    virtual void	SetField(const uint fldId, const double value) = 0;
    virtual void	SetField(const uint fldId, const long double value) = 0;
    virtual void	SetField(const uint fldId, const __OWL TDate& value) = 0;
    virtual void	SetField(const uint fldId, const __OWL TTime& value) = 0;
    virtual void	SetField(const uint fldId, const __OWL TSystemTime& value) = 0;
    virtual bool	VerifyField(const uint fldId) const = 0;

		// register callback
		virtual bool  BeforeFireEvent(TDBaseEvent::TType type) { return true;}
    virtual void  AfterFireEvent(TDBaseEvent::TType type) {}
		void Register(TDBaseEvent* event);
		void Unregister(TDBaseEvent* event);
		void ForEach(TDBaseEvent::TType type, long p1 = 0, long p2 = 0);

		void NotifyControls(TDBaseEvent::TType type)
			{
				if(BeforeFireEvent(type)){
					ForEach(type);
					AfterFireEvent(type);
				}
			}
		void CommitControls()
			{
				NotifyControls(TDBaseEvent::cbCommit);
			}
		void RefreshControls()
			{
				NotifyControls(TDBaseEvent::cbRefresh);
			}

	protected:
#if defined(BI_STD_RTL)
		typedef std::vector<TDBaseEvent*> TDBaseEvents;
#else
		typedef TPtrArray<TDBaseEvent*> TDBaseEvents;
#endif
		TDBaseEvents	Events;
}; //{{TDBDataLink}}


// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>


//
inline uint32 TDBDataLink::GetBlobSize(const owl_string& fldName) const
{ return GetBlobSize(GetFieldNumber(fldName));}
//
inline owl_string TDBDataLink::GetFieldTypeName(const owl_string& fldName) const
{	return GetFieldTypeName(GetFieldNumber(fldName));}
//
inline TDBBlob* TDBDataLink::GetBlob(const owl_string& fldName, const TDBOpenMode mode)
{	return GetBlob(GetFieldNumber(fldName),mode);}
//
inline bool TDBDataLink::GetField(const owl_string& fldName, owl_string& value) const
{	return GetField(GetFieldNumber(fldName), value);}
//
inline bool TDBDataLink::GetField(const owl_string& fldName, bool& value) const
{	return GetField(GetFieldNumber(fldName), value);}
//
inline bool TDBDataLink::GetField(const owl_string& fldName, int16& value) const
{	return GetField(GetFieldNumber(fldName), value);}
//
inline bool TDBDataLink::GetField(const owl_string& fldName, int32& value) const
{	return GetField(GetFieldNumber(fldName), value);}
//
inline bool TDBDataLink::GetField(const owl_string& fldName, uint16& value) const
{	return GetField(GetFieldNumber(fldName), value);}
//
inline bool TDBDataLink::GetField(const owl_string& fldName, uint32& value) const
{	return GetField(GetFieldNumber(fldName), value);}
//
inline bool TDBDataLink::GetField(const owl_string& fldName, float& value) const
{	return GetField(GetFieldNumber(fldName), value);}
//
inline bool TDBDataLink::GetField(const owl_string& fldName, double& value) const
{	return GetField(GetFieldNumber(fldName), value);}
//
inline bool TDBDataLink::GetField(const owl_string& fldName, long double& value) const
{	return GetField(GetFieldNumber(fldName), value);}
//
inline bool TDBDataLink::GetField(const owl_string& fldName, __OWL TDate& value) const
{	return GetField(GetFieldNumber(fldName), value);}
//
inline bool TDBDataLink::GetField(const owl_string& fldName, __OWL TTime& value) const
{	return GetField(GetFieldNumber(fldName), value);}
//
inline bool TDBDataLink::GetField(const owl_string& fldName, __OWL TSystemTime& value) const
{	return GetField(GetFieldNumber(fldName), value);}
//
inline bool TDBDataLink::IsBlank(const owl_string& fldName) const
{	return IsBlank(GetFieldNumber(fldName));}
//
inline void TDBDataLink::SetBlank(const owl_string& fldName)
{	SetBlank(GetFieldNumber(fldName));}
//
inline void TDBDataLink::SetField(const owl_string& fldName, const owl_string& value)
{	SetField(GetFieldNumber(fldName), value);}
//
inline void TDBDataLink::SetField(const owl_string& fldName, const char*  value)
{	SetField(GetFieldNumber(fldName), value);}
//
inline void TDBDataLink::SetField(const owl_string& fldName, const bool value)
{	SetField(GetFieldNumber(fldName), value);}
//
inline void TDBDataLink::SetField(const owl_string& fldName, const int16 value)
{	SetField(GetFieldNumber(fldName), value);}
//
inline void TDBDataLink::SetField(const owl_string& fldName, const int32 value)
{	SetField(GetFieldNumber(fldName), value);}
//
inline void TDBDataLink::SetField(const owl_string& fldName, const uint value)
{	SetField(GetFieldNumber(fldName), value);}
//
inline void TDBDataLink::SetField(const owl_string& fldName, const uint32 value)
{	SetField(GetFieldNumber(fldName), value);}
//
inline void TDBDataLink::SetField(const owl_string& fldName, const float value)
{	SetField(GetFieldNumber(fldName), value);}
//
inline void TDBDataLink::SetField(const owl_string& fldName, const double value)
{	SetField(GetFieldNumber(fldName), value);}
//
inline void TDBDataLink::SetField(const owl_string& fldName, const long double value)
{	SetField(GetFieldNumber(fldName), value);}
//
inline void TDBDataLink::SetField(const owl_string& fldName, const __OWL TDate& value)
{	SetField(GetFieldNumber(fldName), value);}
//
inline void TDBDataLink::SetField(const owl_string& fldName, const __OWL TTime& value)
{	SetField(GetFieldNumber(fldName), value);}
//
inline void TDBDataLink::SetField(const owl_string& fldName, const __OWL TSystemTime& value)
{	SetField(GetFieldNumber(fldName), value);}
//
inline bool TDBDataLink::VerifyField(const owl_string& fldName) const
{	return VerifyField(GetFieldNumber(fldName));}
//

_OWLFX_END_NAMESPACE

#endif  // DBCURSOR_H sentry.

