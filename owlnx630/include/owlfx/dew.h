//-------------------------------------------------------------------
// OWL File Extensions (OWLFX) Class Library
// OWL NExt Project
//
//$Revision: 1.1.4.2 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:38 $
//
// Database Engine Wrapper
// Copyright © 1994-1996
// B. W. Bush
// All rights reserved
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the
// Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.
//
// Modifyed by Yura Bidus for use with OWL NExt
// Used in OWLFX with permission of author
//-------------------------------------------------------------------
#if !defined(OWLFX_DEW_H) // Sentry, use file only if it's not already included.
#define OWLFX_DEW_H

#include <owlfx/defs.h>

#include <owlfx/abase.h>

// Borland database engine definitions.
#if !defined(__FLAT__)
#define __FLAT__
#endif
#if !defined(__WIN32__)
#define __WIN32__
#endif

#include <api_upd\idapi.h>

_OWLFX_BEGIN_NAMESPACE

// Supplementary type definitions.

typedef const BOOL *				pcBOOL;
typedef const uint8 *			pcBYTE;
typedef const char *				pcCHAR;
typedef const CROpType *		pcCROpType;
typedef const CRTblDesc *	pcCRTblDesc;
typedef const DBIEnv *			pcDBIEnv;
typedef const FLDDesc *		pcFLDDesc;
typedef const IDXDesc *		pcIDXDesc;
typedef const RECProps *		pcRECProps;
typedef const RINTDesc *		pcRINTDesc;
typedef const SECDesc *		pcSECDesc;
typedef const SORTOrder *	pcSORTOrder;
typedef const VCHKDesc *		pcVCHKDesc;
typedef const uint16 *			pcUINT16;

// Make sure the class prefix is defined if it has not already been defined.

// Forward declarations of classes.

class _OWLFXCLASS TDewHandle;

class _OWLFXCLASS TDewBlob;
class _OWLFXCLASS TDewBookmark;
class _OWLFXCLASS TDewBytes;
class _OWLFXCLASS TDewTable;
class _OWLFXCLASS TDewDatabase;
class _OWLFXCLASS TDewEngine;
class _OWLFXCLASS TDewException;
class _OWLFXCLASS TDewFilter;
class _OWLFXCLASS TDewKey;
class _OWLFXCLASS TDewRecord;
class _OWLFXCLASS TDewSession;
class _OWLFXCLASS TDewStatement;
class _OWLFXCLASS TDewTransaction;


// Class declarations.
// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>

// DbiException class.
class _OWLFXCLASS TDewException: public __OWL TXOwl{
  public:
    TDewException(const DBIResult = DBIERR_NONE, const owl_string& = _T(""));

    int Category() const;
    int Code() const;
    owl_string Context(const int) const;
    owl_string Entry(const uint, uint32&) const;
    owl_string ExtraInfo() const;
    void GetInfo(DBIErrInfo&, const bool = true) const;
    owl_string Message() const;
    owl_string FullMessage() const;
    DBIResult Result() const;

    static void CheckResult(const DBIResult, const owl_string& = owl_string());

  private:
    const DBIResult fResult;
};

// Write the error message on a stream.
inline _tostream& operator<<(_tostream& os, const TDewException& exception)
{
	os << exception.FullMessage();
  return os;
}

// Exception subclasses.
class TDewSystemException : public TDewException {public: TDewSystemException(const DBIResult r, const owl_string& extraInfo = _T("")) : TDewException(r, extraInfo) {}};
class TDewNotFoundException : public TDewException {public: TDewNotFoundException(const DBIResult r, const owl_string& extraInfo = _T("")) : TDewException(r, extraInfo) {}};
class TDewDataCorruptException : public TDewException {public: TDewDataCorruptException(const DBIResult r, const owl_string& extraInfo = _T("")) : TDewException(r, extraInfo) {}};
class TDewIoException : public TDewException {public: TDewIoException(const DBIResult r, const owl_string& extraInfo = _T("")) : TDewException(r, extraInfo) {}};
class TDewLimitException : public TDewException {public: TDewLimitException(const DBIResult r, const owl_string& extraInfo = _T("")) : TDewException(r, extraInfo) {}};
class TDewIntegrityException : public TDewException {public: TDewIntegrityException(const DBIResult r, const owl_string& extraInfo = _T("")) : TDewException(r, extraInfo) {}};
class TDewInvalidReqException : public TDewException {public: TDewInvalidReqException(const DBIResult r, const owl_string& extraInfo = _T("")) : TDewException(r, extraInfo) {}};
class TDewLockConflictException : public TDewException {public: TDewLockConflictException(const DBIResult r, const owl_string& extraInfo = _T("")) : TDewException(r, extraInfo) {}};
class TDewSecurityException : public TDewException {public: TDewSecurityException(const DBIResult r, const owl_string& extraInfo = _T("")) : TDewException(r, extraInfo) {}};
class TDewInvalidContextException : public TDewException {public: TDewInvalidContextException(const DBIResult r, const owl_string& extraInfo = _T("")) : TDewException(r, extraInfo) {}};
class TDewOsException : public TDewException {public: TDewOsException(const DBIResult r, const owl_string& extraInfo = _T("")) : TDewException(r, extraInfo) {}};
class TDewNetworkException : public TDewException {public: TDewNetworkException(const DBIResult r, const owl_string& extraInfo = _T("")) : TDewException(r, extraInfo) {}};
class TDewOptParamException : public TDewException {public: TDewOptParamException(const DBIResult r, const owl_string& extraInfo = _T("")) : TDewException(r, extraInfo) {}};
class TDewQueryException : public TDewException {public: TDewQueryException(const DBIResult r, const owl_string& extraInfo = _T("")) : TDewException(r, extraInfo) {}};
class TDewVersionException : public TDewException {public: TDewVersionException(const DBIResult r, const owl_string& extraInfo = _T("")) : TDewException(r, extraInfo) {}};
class TDewCapabilityException : public TDewException {public: TDewCapabilityException(const DBIResult r, const owl_string& extraInfo = _T("")) : TDewException(r, extraInfo) {}};
class TDewConfigException : public TDewException {public: TDewConfigException(const DBIResult r, const owl_string& extraInfo = _T("")) : TDewException(r, extraInfo) {}};
class TDewWarningException : public TDewException {public: TDewWarningException(const DBIResult r, const owl_string& extraInfo = _T("")) : TDewException(r, extraInfo) {}};
class TDewOtherException : public TDewException {public: TDewOtherException(const DBIResult r, const owl_string& extraInfo = _T("")) : TDewException(r, extraInfo) {}};
class TDewCompatibilityException : public TDewException {public: TDewCompatibilityException(const DBIResult r, const owl_string& extraInfo = _T("")) : TDewException(r, extraInfo) {}};
class TDewRepositoryException : public TDewException {public: TDewRepositoryException(const DBIResult r, const owl_string& extraInfo = _T("")) : TDewException(r, extraInfo) {}};
class TDewDriverException : public TDewException {public: TDewDriverException(const DBIResult r, const owl_string& extraInfo = _T("")) : TDewException(r, extraInfo) {}};
class TDewInternalException : public TDewException {public: TDewInternalException(const DBIResult r, const owl_string& extraInfo = _T("")) : TDewException(r, extraInfo) {}};


//-----------------------------------------------------
//
// class TDewHandle
// ~~~~~ ~~~~~~~~~~
// Internal class
//
class _OWLFXCLASS TDewHandle {
  public:

    bool						IsClosed() const	{	return GetHandle() == 0;}
    bool						IsOpen() const		{	return GetHandle() != 0;}
    void						GetProp(const uint32 idx, const pVOID p1, const uint p2, uint16& p3) const;
    virtual hDBIObj GetHandle() const = 0;
    virtual void		SetProp(const uint32 p1, const uint32 p2);
};

//-----------------------------------------------------
//
//
//
class _OWLFXCLASS TDewStatement : public TDewHandle {
  public:
    TDewStatement();
    TDewStatement(const TDewDatabase& db);
    TDewStatement(const TDewDatabase& db, const char* msg, const TDBQueryLang ql = dbqrlSQL);
    ~TDewStatement();

    void			Close();
		void			Open(const TDewDatabase& db, const char* name, const TDBQueryLang ql = dbqrlSQL);
		void			Open(const char* name, const TDBQueryLang ql = dbqrlSQL);
    TDewTable	Query() const;
		void			SetParams(const uint index, const pcFLDDesc p1, const uint8* p2); //????????

    hDBIDb		GetDatabaseHandle() const { return fHDatabase;}
    hDBIStmt	GetHandle() const         {return fHStatement;}

  private:
    TDewStatement(const TDewStatement&) : fHDatabase(0), fHStatement(0) {}
    TDewStatement& operator=(const TDewStatement&) 
			{
				if (IsOpen()) 
					Close(); 
				return *this;
			}

    hDBIDb fHDatabase;
    hDBIStmt fHStatement;
};

//-----------------------------------------------------
//
//
//
class _OWLFXCLASS TDewTransaction : public TDewHandle {
  public:
    TDewTransaction();
    TDewTransaction(const TDewDatabase& db);
    TDewTransaction(const TDewDatabase& db, const eXILType xt);
    ~TDewTransaction();

    void			Close(const eXEnd = xendCOMMITKEEP);
    void			GetTranInfo(XInfo& xi) const;
    void			Open(const TDewDatabase& db, const eXILType xt);
    void			Open(const eXILType xt);

    hDBIDb		GetDatabaseHandle() const 	{return fHDatabase;}
    hDBIXact	GetHandle() const 	{return fHTransaction;}

  private:

    TDewTransaction(const TDewTransaction&) : fHDatabase(0), fHTransaction(0) {}
    TDewTransaction& operator=(const TDewTransaction&) 
			{
				if (IsOpen()) 
					Close(); 
				return *this;
			}

    hDBIDb		fHDatabase;
    hDBIXact	fHTransaction;
};

//-----------------------------------------------------
// class TDewFilter
// ~~~~~ ~~~~~~~~~~
//
class _OWLFXCLASS TDewFilter : public TDewHandle {
  public:
    TDewFilter();
    TDewFilter(const TDewTable& cur);
    TDewFilter(const TDewTable& cur, const pCANExpr);
    TDewFilter(const TDewFilter& cur);
    ~TDewFilter();

    TDewFilter& operator=(const TDewFilter& ft);

    void				Open(const TDewTable&, const pCANExpr);
    void				Open(const pCANExpr);
    void				Close();

    void				Activate();
    void				Deactivate();
    void				GetInfo(FILTERInfo&, const uint p1 = 0, const uint p2 = 0) const;

    hDBICur			GetTable();
    hDBIFilter	GetHandle() const {return fHFilter;}

  private:
    void Clone(const TDewFilter&);

    hDBICur			fHCursor;
    hDBIFilter	fHFilter;
};

//-----------------------------------------------------
//
//
//
class _OWLFXCLASS TDewBlob : public TDBBlob {
  public:
    TDewBlob();
    TDewBlob(const TDewRecord& rec, const uint fldId, const TDBOpenMode md = dbmReadWrite);
    TDewBlob(const TDewRecord& rec, const owl_string& txt, const TDBOpenMode md = dbmReadWrite);
    virtual ~TDewBlob();

		// virtuals
    virtual uint32	Get(const uint32 p1, const uint32 p2, uint8* buf) const;
    virtual void		Set(const uint32 p1, const uint32 p2, const uint8* buf);
    virtual uint32	GetSize() const;
    
		void		Truncate(const uint32);
		void		GetHeading(uint8* buf) const;

		// regular
    void		Close();
    void		Open(TDewRecord& rec, const uint, const TDBOpenMode mode = dbmReadWrite);
    void		Open(TDewRecord& rec, const owl_string& text, const TDBOpenMode mode = dbmReadWrite);

  private:
    TDewBlob(const TDewBlob&) : fBuffer(0), fHCursor(NULL) {}
    TDewBlob& operator=(const TDewBlob&) 
			{	
				if (fBuffer != 0) 
					Close(); 
				return *this;
			}

    pBYTE		fBuffer;
    uint		fField;
    hDBICur fHCursor;
};

//-----------------------------------------------------
// class TDewRecord
// ~~~~~ ~~~~~~~~~~~~
//
class _OWLFXCLASS TDewRecord {
    friend class TDewBlob;
    friend class TDewTable;
  public:
    TDewRecord(TDewTable& cur);
    ~TDewRecord();
    
		uint8*			GetBuffer();
    uint8*			GetBuffer() const;
    TDewTable&	GetTable();
    const TDewTable&	GetTable() const;

		// virtual functions
		owl_string	GetFieldName(const uint) const;
    uint				GetFieldNumber(const owl_string&) const;
		
		owl_string	GetFieldTypeName(const owl_string& fldName) const;
    owl_string	GetFieldTypeName(const uint) const;

    uint32			GetBlobSize(const owl_string&) const;
    uint32			GetBlobSize(const uint) const;
		TDBBlob*		GetBlob(const owl_string& name, const TDBOpenMode mode = dbmReadWrite);
		TDBBlob*		GetBlob(const uint fldId, const TDBOpenMode mode = dbmReadWrite);

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
		void  SetBlank(const owl_string& fldName);
    void  SetField(const owl_string& fldName, const owl_string& value);
    void  SetField(const owl_string& fldName, const char* value);
    void  SetField(const owl_string& fldName, const bool value);
    void  SetField(const owl_string& fldName, const int16 value);
    void  SetField(const owl_string& fldName, const int32 value);
    void  SetField(const owl_string& fldName, const uint value);
    void  SetField(const owl_string& fldName, const uint32 value);
    void  SetField(const owl_string& fldName, const float value);
    void  SetField(const owl_string& fldName, const double value);
    void  SetField(const owl_string& fldName, const long double value);
    void  SetField(const owl_string& fldName, const __OWL TDate& value);
    void  SetField(const owl_string& fldName, const __OWL TTime& value);
    void  SetField(const owl_string& fldName, const __OWL TSystemTime& value);
    bool  VerifyField(const owl_string& fldName) const;
    
		bool GetField(const uint fldId, owl_string& value) const;
    bool GetField(const uint fldId, bool& value) const;
    bool GetField(const uint fldId, int16& value) const;
    bool GetField(const uint fldId, int32& value) const;
    bool GetField(const uint fldId, uint16& value) const;
    bool GetField(const uint fldId, uint32& value) const;
    bool GetField(const uint fldId, float& value) const;
    bool GetField(const uint fldId, double& value) const;
    bool GetField(const uint fldId, long double& value) const;
    bool GetField(const uint fldId, __OWL TDate& value) const;
    bool GetField(const uint fldId, __OWL TTime& value) const;
    bool GetField(const uint fldId, __OWL TSystemTime& value) const;
    bool IsBlank(const uint fldId) const;
    void SetBlank(const uint fldId);
    void SetField(const uint, const owl_string& value);
    void SetField(const uint fldId, const char* value);
    void SetField(const uint fldId, const bool value);
    void SetField(const uint fldId, const int16 value);
    void SetField(const uint fldId, const int32 value);
    void SetField(const uint fldId, const uint value);
    void SetField(const uint fldId, const uint32 value);
    void SetField(const uint fldId, const float value);
    void SetField(const uint fldId, const double value);
    void SetField(const uint fldId, const long double value);
    void SetField(const uint fldId, const __OWL TDate& value);
    void SetField(const uint fldId, const __OWL TTime& value);
    void SetField(const uint fldId, const __OWL TSystemTime& value);
    bool VerifyField(const uint fldId) const;

  private:
    TDewRecord(const TDewRecord&);
    TDewRecord& operator=(const TDewRecord&);

    void CheckFieldNumber(const uint) const;
    void ResizeBuffer();

    uint8*     fBuffer;
    TDewTable& fTable;
};

//-----------------------------------------------------
// class TDewTable
// ~~~~~ ~~~~~~~~~~~
//
class _OWLFXCLASS TDewTable : public TDewHandle {
		friend class TDewRecord;
  public:
    TDewTable();
    TDewTable(const hDBICur);
    TDewTable(const TDewDatabase& db);
    TDewTable(const TDewDatabase&	db, 
			         const pcCHAR		p0, 
							 const pcCHAR		p1 = NULL, 
			         const TDBOpenMode p2 = dbmReadWrite, 
							 const DBIShareMode p3 = dbiOPENSHARED, 
							 const pcCHAR		p4 = NULL, 
							 const pcCHAR		p5 = NULL, 
							 const uint			p6 = NULL, 
							 const XLTMode	p7 = xltFIELD, 
							 const bool			p8 = false, 
							 const pcBYTE		p9 = NULL);
    TDewTable(const TDewTable& cur, const bool p1 = false, const bool p2 = false);
    ~TDewTable();
    TDewTable& operator=(const TDewTable&);

		bool IsFirst();
		bool IsLast();

		// Standard Part
    void AcqTableLock(const TDBLockType lock = dblctReadOnly);
    void AddIndex(const IDXDesc&, const pcCHAR p1 = NULL);
    bool Advance(const TDBLockType lock = dblctNone, const pRECProps p2 = NULL);
    void AppendRecord();
    void ApplyDelayedUpdates(DBIDelayedUpdCmd p1 = dbiDelayedUpdCommit);
    void BeginDelayedUpdates();
    void BeginLinkMode();
    void Close();
    void CloseIndex(const pcCHAR, const uint p1 = NULL);
    CMPBkMkRslt CompareBookMarks(const pcBYTE, const pcBYTE) const;
    int	 CompareKeys(const pcBYTE, const pcBYTE = NULL, const uint = 0, const uint = 0) const;
    const CURProps& GetTableProps() const;
    hDBIDb DatabaseHandle() const {return fHDatabase;}
    void DeleteIndex(const pcCHAR p0, const uint p1, const pcCHAR p2);
    TDewRecord& DeleteRecord();
    void EmptyTable();
    void EndDelayedUpdates();
    void EndLinkMode();
    void ExtractKey(const pBYTE pKeyBuf) const {ExtractKey(pKeyBuf, *fRecord);}
    void ExtractKey(const pBYTE buf, const TDewRecord& rec) const;
    pcFLDDesc FieldDescs() const;
    const TDewRecord& ForceRecordReread() const;
    const TDewRecord& ForceReread() const;
    void GetBookMark(const pBYTE) const;
    void GetCallBack(const CBType cbt, uint32& idx, uint16& cb, pVOID& vf, pfDBICallBack& pfc) const;
    void GetIndexDesc(const uint idx, IDXDesc& desc) const;
    void GetIndexDesc(IDXDesc&) const;
    void GetIndexForField(const uint idx, IDXDesc& desc, const bool bt = false) const;
    TDewRecord& GetNextRecord(const TDBLockType lock = dblctNone, const pRECProps p2 = NULL);
    TDewRecord& GetPriorRecord(const TDBLockType lock = dblctNone, const pRECProps p2 = NULL);
    TDewRecord& GetRecord(const TDBLockType lock = dblctNone, const pRECProps = NULL);
    TDewRecord& GetRecordForKey(const uint iFields = NULL, const uint iLen = NULL) 
					{return GetRecordForKey(*fRecord, iFields, iLen);}
    TDewRecord& GetRecordForKey(const TDewRecord&, const uint = NULL, const uint = NULL);
    TDewRecord& GetRecordForKey(const pcBYTE, const uint = NULL, const uint = NULL);
    TDewRecord& GetRelativeRecord(const int32, const TDBLockType lock = dblctNone, const pRECProps = NULL);
    void GetRintDesc(const uint, RINTDesc&) const;
    void GetVchkDesc(const uint, VCHKDesc&) const;
    hDBICur GetHandle() const {return fHCursor;}
    uint IndexSeqNo(const pcCHAR, const uint = NULL, const pcCHAR = NULL) const;
    TDewRecord& InitRecord();
    TDewRecord& InsertRecord(const TDBLockType lock = dblctNone);
    bool IsRecordLocked() const;
    uint IsTableLocked(const TDBLockType lock = dblctReadOnly) const;
    bool IsTableShared() const;
    void LinkDetail(TDewTable&, const uint, const pcUINT16, const pcUINT16);
    void LinkDetail(TDewTable&, const uint, const pcCHAR);
    hDBICur LockList(const bool = true, const bool = true) const;
    void MakePermanent(const pcCHAR, const bool = true);
    TDewRecord& ModifyRecord(const bool = true);
    void Open(const TDewDatabase&, const pcCHAR, const pcCHAR = NULL, const TDBOpenMode = dbmReadWrite, const DBIShareMode = dbiOPENSHARED, const pcCHAR = NULL, const pcCHAR = NULL, const uint = NULL, const XLTMode = xltFIELD, const bool = false, const pcBYTE = NULL);
    void Open(const pcCHAR, const pcCHAR = NULL, const TDBOpenMode = dbmReadWrite, const DBIShareMode = dbiOPENSHARED, const pcCHAR = NULL, const pcCHAR = NULL, const uint = NULL, const XLTMode = xltFIELD, const bool = false, const pcBYTE = NULL);
    void OpenIndex(const pcCHAR, const uint = 0);
    void PackTable(const bool = true);
    uint32 ReadBlock(const uint32, const pBYTE);
    TDewRecord& GetCurRecord();
    //const TDewRecord& GetCurRecord() const;
    uint32 GetRecordCount() const;
    void RegenIndex(const pcCHAR, const uint = 0, const pcCHAR = NULL);
    void RegenIndexes();
    void RegisterCallBack(const CBType, const uint32, const uint, const pVOID, 
			                    const pfDBICallBack);
    void RelRecordLock(const bool = true);
    void RelTableLock(const TDBLockType lock = dblctReadOnly, const bool = true);
    void ResetRange();
    void SaveChanges() const;
    uint32 SeqNo() const;
    void SetFieldMap(const uint, const pcFLDDesc);
    void SetProp(const uint32 iProp, const uint32 iPropValue);
    void SetRange(const pcBYTE = NULL, const pcBYTE = NULL, const bool = true, 
			            const bool = true, const uint = 0, const uint = 0, 
									const uint = 0, const uint = 0);
    TDewRecord& SetToBegin();
    TDewRecord& SetToEnd();
    TDewRecord& SetToBookMark(const pcBYTE);
    TDewRecord& SetToTable(const TDewTable&);
    TDewRecord& SetToKey(const DBISearchCond eSearchCond = keySEARCHEQ, 
			                   const uint iFields = 0, const uint iLen = 0) 
								{ return SetToKey(*fRecord, eSearchCond, iFields, iLen);}
    TDewRecord& SetToKey(const TDewRecord&, const DBISearchCond = keySEARCHEQ, 
			                   const uint = 0, const uint = 0);
    TDewRecord& SetToRecordNo(const uint32);
    TDewRecord& SetToSeqNo(const uint32);
    void Sort(const uint, const pcUINT16, const pcCHAR = NULL, const pcBOOL = NULL, 
			        const pcSORTOrder = NULL, pfSORTCompFn* = NULL, const bool = false);
    void SwitchToIndex(const pcCHAR, const bool = true, const uint = 0, 
			                 const pcCHAR = NULL);
    TDewRecord& UndeleteRecord();
    void UnlinkDetail();
    uint32 WriteBlock(const uint32, const pcBYTE);

		TDBDataLink& GetDataLink() { return DataLink; }
  protected:
		class TDewDataLink: public TDBDataLink {

			virtual owl_string GetFieldName(const uint fldId) const;
		  virtual uint GetFieldNumber(const owl_string& name) const;
	    virtual owl_string GetFieldTypeName(const uint fldId) const;
		  virtual uint32 GetBlobSize(const uint fldId) const;
			virtual TDBBlob* GetBlob(const uint fldId, const TDBOpenMode mode = dbmReadWrite);

			virtual bool	GetField(const uint fldId, owl_string& value) const;
	    virtual bool	GetField(const uint fldId, bool& value) const;
	    virtual bool	GetField(const uint fldId, int16& value) const;
	    virtual bool	GetField(const uint fldId, int32& value) const;
	    virtual bool	GetField(const uint fldId, uint16& value) const;
	    virtual bool	GetField(const uint fldId, uint32& value) const;
      virtual bool	GetField(const uint fldId, float& value) const;
	    virtual bool	GetField(const uint fldId, double& value) const;
	    virtual bool	GetField(const uint fldId, long double& value) const;
	    virtual bool	GetField(const uint fldId, __OWL TDate& value) const;
	    virtual bool	GetField(const uint fldId, __OWL TTime& value) const;
	    virtual bool	GetField(const uint fldId, __OWL TSystemTime& value) const;
	    virtual bool	IsBlank(const uint fldId) const;
	
      virtual void	SetBlank(const uint fldId);
	    virtual void	SetField(const uint fldId, const owl_string& value);
	    virtual void	SetField(const uint fldId, const char*  value);
	    virtual void	SetField(const uint fldId, const bool value);
	    virtual void	SetField(const uint fldId, const int16 value);
	    virtual void	SetField(const uint fldId, const int32 value);
	    virtual void	SetField(const uint fldId, const uint value);
	    virtual void	SetField(const uint fldId, const uint32 value);
      virtual void	SetField(const uint fldId, const float value);
	    virtual void	SetField(const uint fldId, const double value);
	    virtual void	SetField(const uint fldId, const long double value);
	    virtual void	SetField(const uint fldId, const __OWL TDate& value);
	    virtual void	SetField(const uint fldId, const __OWL TTime& value);
	    virtual void	SetField(const uint fldId, const __OWL TSystemTime& value);
	    virtual bool	VerifyField(const uint fldId) const;
		}DataLink;
		friend class TDewDataLink;
  private:
    void GetProperties();

    CURProps    fCursorProps;
    pFLDDesc    fFieldDesc;
    hDBICur     fHCursor;
    hDBIDb      fHDatabase;
    TDewRecord* fRecord;
};


//-----------------------------------------------------
// class TDewDatabase
// ~~~~~ ~~~~~~~~~~~~
//
class _OWLFXCLASS TDewDatabase : public TDewHandle {
  public:
    TDewDatabase(const pcCHAR = NULL, const pcCHAR = NULL, 
			           const TDBOpenMode = dbmReadWrite, 
								 const DBIShareMode = dbiOPENSHARED, const pcCHAR = NULL, 
								 const uint = NULL, const pcFLDDesc = NULL, 
								 const pcBYTE = NULL);
    ~TDewDatabase();

    void AcqPersistTableLock(const pcCHAR, const pcCHAR = NULL);
    void AddIndex(const pcCHAR, const pcCHAR, const IDXDesc&, const pcCHAR = NULL);
    void Close();
    void CopyTable(const pcCHAR, const pcCHAR, const pcCHAR = NULL, const bool = TRUE);
    void CreateTable(const CRTblDesc&, const bool = TRUE);
    void DeleteIndex(const pcCHAR, const pcCHAR, const uint = NULL, const pcCHAR = NULL, const pcCHAR = NULL);
    void DeleteTable(const pcCHAR, const pcCHAR = NULL);
    owl_string Directory(const bool = false) const;
    void DoRestructure(const uint, const pcCRTblDesc, const pcCHAR, const pcCHAR = NULL, const pcCHAR = NULL, const bool = false);
    void EmptyTable(const pcCHAR, const pcCHAR = NULL);
    hDBICur FamilyList(const pcCHAR, const pcCHAR = NULL);
    hDBICur FieldList(const pcCHAR, const pcCHAR = NULL, const bool = false);
    hDBICur FileList(const pcCHAR);
    owl_string FullName(const pcCHAR, const pcCHAR = NULL) const;
    hDBIDb GetHandle() const {return fHDatabase;}
    hDBICur IndexList(const pcCHAR, const pcCHAR = NULL);
    TDewTable InMemTable(const pcCHAR, const uint, const pcFLDDesc);
    void Open(const pcCHAR = NULL, const pcCHAR = NULL, const TDBOpenMode = dbmReadWrite, const DBIShareMode = dbiOPENSHARED, const pcCHAR = NULL, const uint = NULL, const pcFLDDesc = NULL, const pcBYTE = NULL);
    void PackTable(const pcCHAR, const pcCHAR = NULL, const bool = TRUE);
    TDewTable Query(const pcCHAR, const DBIQryLang = qrylangSQL);
    void RegenIndex(const pcCHAR, const pcCHAR, const uint = NULL, const pcCHAR = NULL, const pcCHAR = NULL);
    void RelPersistTableLock(const pcCHAR, const pcCHAR = NULL);
    void RenameTable(const pcCHAR, const pcCHAR, const pcCHAR = NULL);
    hDBICur RintList(const pcCHAR, const pcCHAR = NULL);
    hDBICur SecurityList(const pcCHAR, const pcCHAR = NULL);
    void SetDirectory(const pcCHAR);
    void Sort(const pcCHAR, const uint, const pcUINT16, const pcCHAR = NULL, const pcCHAR = NULL, const pcBOOL = NULL, const pcSORTOrder = NULL, pfSORTCompFn* = NULL, const bool = false);
    hDBICur TableList(const bool = false, const bool = false, const pcCHAR = NULL);
    uint TableOpenCount(const pcCHAR, const pcCHAR = NULL) const;
    TDewTable TempTable(const CRTblDesc&);
    hDBICur VchkList(const pcCHAR, const pcCHAR = NULL);

  private:
    TDewDatabase(const TDewDatabase&) : fHDatabase(NULL) {}
    TDewDatabase& operator=(const TDewDatabase&) {if (IsOpen()) Close(); return *this;}

    hDBIDb fHDatabase;
};

//-----------------------------------------------------
// class TDewEngine
// ~~~~~ ~~~~~~~~~~
//
class _OWLFXCLASS TDewEngine {
  public:
    TDewEngine(const pcDBIEnv = NULL);
    ~TDewEngine();

    void AddPassword(const pcCHAR);
    hDBICur CfgInfoList(const pcCHAR = "\\", const TDBOpenMode = dbmReadOnly, const CFGMode = cfgPersistent) const;
    void CheckRefresh() const;
    void Close();
    hDBICur DatabaseList() const;
    const FMTDate& DateFormat() const;
    void DebugLayerOptions(const uint, const pcCHAR = NULL);
    hDBICur DriverList() const;
    void DropPassword(const pcCHAR);
    hDBICur FieldTypesList(const pcCHAR, const pcCHAR = NULL) const;
    void GetCallBack(const CBType, UINT32&, UINT16&, pVOID&, pfDBICallBack&) const;
    void GetClientInfo(CLIENTInfo&) const;
    void GetDatabaseDesc(const pcCHAR, DBDesc&) const;
    void GetDriverDesc(const pcCHAR, DRVType&) const;
    void GetFieldTypeDesc(const pcCHAR, const pcCHAR, const pcCHAR, FLDType&) const;
    void GetIndexTypeDesc(const pcCHAR, const pcCHAR, IDXType&) const;
    void GetSesInfo(SESInfo&) const;
    void GetSysConfig(SYSConfig&) const;
    void GetSysInfo(SYSInfo&) const;
    void GetSysVersion(SYSVersion&) const;
    void GetTableTypeDesc(const pcCHAR, const pcCHAR, TBLType&) const;
    hDBICur IndexTypesList(const pcCHAR) const;
    hDBICur LdList() const;
    owl_string LdName(const pcCHAR, const pcCHAR) const;
    void LoadDriver(const pcCHAR);
    owl_string NetUserName() const;
    const FMTNumber& NumberFormat() const;
    void Open(const pcDBIEnv = NULL);
    void RegisterCallBack(const CBType, const UINT32, const uint, const pVOID, const pfDBICallBack);
    void SetCurrSession(TDewSession&);
    void SetDateFormat(const FMTDate&);
    void SetLockRetry(const INT16);
    void SetNumberFormat(const FMTNumber&);
    void SetPrivateDir(const pcCHAR = NULL);
    void SetTimeFormat(const FMTTime&);
    hDBICur TableTypesList(const pcCHAR);
    const FMTTime& TimeFormat() const;
    void UseIdleTime();
    hDBICur UserList() const;

  private:
    TDewEngine(const TDewEngine&) {};
    TDewEngine& operator=(const TDewEngine&) {return *this;}

    FMTDate fFmtDate;
    FMTTime fFmtTime;
    FMTNumber fFmtNumber;
};


//-----------------------------------------------------
// class TDewSession 
// ~~~~~ ~~~~~~~~~~
//
class _OWLFXCLASS TDewSession : public TDewHandle {
  public:
    TDewSession(const pcCHAR = NULL, const pcCHAR = NULL);
    TDewSession(TDewEngine&);
    ~TDewSession();

    void		Close();
    hDBISes GetHandle() const {return fHSession;}
    void		Open(const pcCHAR = NULL, const pcCHAR = NULL);
    void		Open(TDewEngine&);

  private:
    TDewSession(const TDewSession&) : fHSession(NULL) {}
    TDewSession& operator=(const TDewSession&) {if (IsOpen()) Close(); return *this;}

    hDBISes fHSession;
};


//-----------------------------------------------------
// class TDewEnv
// ~~~~~ ~~~~~~~
//
class _OWLFXCLASS TDewEnv : public DBIEnv {
  public:
    TDewEnv();
    TDewEnv(const DBIEnv&);
    TDewEnv(const pcCHAR, const pcCHAR p1 = 0, const bool p2 = false, 
			      const pcCHAR p3 = 0, const pcCHAR p4 = 0);
};


//-----------------------------------------------------
// class TDewFldDesc
// ~~~~~ ~~~~~~~~~~~
//
// Field Descriptor.
class _OWLFXCLASS TDewFldDesc : public FLDDesc {
  public:
    TDewFldDesc();
    TDewFldDesc(const FLDDesc&);
    TDewFldDesc(const uint, const pcCHAR, const uint, const uint p1 = 0, 
			          const int16 p2= 1, const int16 p3 = 0);
};


//-----------------------------------------------------
// class TDewIdxDesc
// ~~~~~ ~~~~~~~~~~~
//
// Index description.
class _OWLFXCLASS TDewIdxDesc  : public IDXDesc {
  public:
    TDewIdxDesc();
    TDewIdxDesc(const IDXDesc&);
    TDewIdxDesc(const DBIKEY, const uint, const pcCHAR, const uint p1 = 0, 
			          const pcCHAR p2 = 0, const bool p3 = false, const bool p4 = false, 
								const bool p5 = false, const bool p6 = true, const bool p7 = false, 
								const bool p8 = false, const pcCHAR p9 = 0, const uint p10 = 2048);
    TDewIdxDesc(const pcCHAR, const uint, const pcCHAR, const uint p1 = 0, 
			          const pcCHAR p2 = 0, const bool p3 = false, const bool p4 = false, 
								const bool p5 = false, const bool p6 = true, const bool p7 = false, 
								const bool p8 = false, const pcCHAR p9 = 0, const uint p10 = 2048);
};


//-----------------------------------------------------
// class TDewVchkDesc
// ~~~~~ ~~~~~~~~~~~~
//
// Val Check structure.
class _OWLFXCLASS TDewVchkDesc  : public VCHKDesc {
  public:
    TDewVchkDesc();
    TDewVchkDesc(const VCHKDesc&);
    TDewVchkDesc(const uint);

    void SetRequired();
    void SetMinVal(const pcBYTE);
    void SetMinValToday();
    void SetMaxVal(const pcBYTE);
    void SetMaxValToday();
    void SetDefVal(const pcBYTE);
    void SetDefValToday();
    void SetPict(const pcCHAR);
    void SetLkup(const LKUPType, const pcCHAR);
};


//-----------------------------------------------------
// class TDewRintDesc
// ~~~~~ ~~~~~~~~~~~~
//
// Ref Integrity Desc.
class _OWLFXCLASS TDewRintDesc : public RINTDesc{
  public:
    TDewRintDesc();
    TDewRintDesc(const RINTDesc&);
    TDewRintDesc(const uint, const pcCHAR, const pcCHAR, const DBIKEY, 
			           const DBIKEY, const uint, const RINTQual p1 = rintCASCADE, 
								 const RINTQual p2 = rintCASCADE, 
								 const RINTType p3 = rintDEPENDENT);
};


//-----------------------------------------------------
// class TDewSecDesc
// ~~~~~ ~~~~~~~~~~~
//
// Security description.
class _OWLFXCLASS TDewSecDesc : public SECDesc{
  public:
    TDewSecDesc();
    TDewSecDesc(const SECDesc&);
    TDewSecDesc(const uint, const PRVType[DBIMAXFLDSINSEC], 
			          const pcCHAR, const PRVType p1 = prvNONE, 
								const uint p2 = NOFAMRIGHTS);
};


//-----------------------------------------------------
// class TDewCrTblDesc
// ~~~~~ ~~~~~~~~~~~~~
//
// Create/Restruct Table descr.
class _OWLFXCLASS TDewCrTblDesc : public CRTblDesc{
  public:
    TDewCrTblDesc();
    TDewCrTblDesc(const CRTblDesc&);
    TDewCrTblDesc(const pcCHAR, const uint, const pcFLDDesc, const uint p1 = 0, 
			            const pcIDXDesc p2 = NULL, const uint p3 = 0, 
									const pcVCHKDesc p4 = NULL, const uint p5 = 0, 
									const pcRINTDesc p6 = NULL, const uint p7 = 0, 
									const pcSECDesc p8 = NULL, const pcCHAR p9 = NULL, 
									const pcCHAR p10= NULL, const pcCROpType p11 = NULL);
};


//-----------------------------------------------------
// class TDewBytes
// ~~~~~ ~~~~~~~~~
//
// TDewBytes class.
class _OWLFXCLASS TDewBytes {
  public:
    ~TDewBytes();

    operator uint8*();
    operator const uint8*() const;
  
	protected:
    TDewBytes(const uint);

  private:
    TDewBytes(const TDewBytes&) {}
    TDewBytes& operator=(const TDewBytes&) {return *this;}

    uint8* fBuffer;
};


//-----------------------------------------------------
// class TDewBookMark
// ~~~~~ ~~~~~~~~~~~~
//
class _OWLFXCLASS TDewBookMark  : public TDewBytes {
  public:
    TDewBookMark(const TDewTable&);
};


//-----------------------------------------------------
// class TDewKey
// ~~~~~ ~~~~~~~
//
class _OWLFXCLASS TDewKey  : public TDewBytes {
  public:
    TDewKey(const TDewTable&);
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

// inlines
//
inline uint32 TDewRecord::GetBlobSize(const owl_string& fldName) const
{ return GetBlobSize(GetFieldNumber(fldName));}
//
inline owl_string TDewRecord::GetFieldTypeName(const owl_string& fldName) const
{	return GetFieldTypeName(GetFieldNumber(fldName));}
//
inline TDBBlob* TDewRecord::GetBlob(const owl_string& fldName, const TDBOpenMode mode)
{	return GetBlob(GetFieldNumber(fldName), mode);}
//
inline bool TDewRecord::GetField(const owl_string& fldName, owl_string& value) const
{	return GetField(GetFieldNumber(fldName), value);}
//
inline bool TDewRecord::GetField(const owl_string& fldName, bool& value) const
{	return GetField(GetFieldNumber(fldName), value);}
//
inline bool TDewRecord::GetField(const owl_string& fldName, int16& value) const
{	return GetField(GetFieldNumber(fldName), value);}
//
inline bool TDewRecord::GetField(const owl_string& fldName, int32& value) const
{	return GetField(GetFieldNumber(fldName), value);}
//
inline bool TDewRecord::GetField(const owl_string& fldName, uint16& value) const
{	return GetField(GetFieldNumber(fldName), value);}
//
inline bool TDewRecord::GetField(const owl_string& fldName, uint32& value) const
{	return GetField(GetFieldNumber(fldName), value);}
//
inline bool TDewRecord::GetField(const owl_string& fldName, float& value) const
{	return GetField(GetFieldNumber(fldName), value);}
//
inline bool TDewRecord::GetField(const owl_string& fldName, double& value) const
{	return GetField(GetFieldNumber(fldName), value);}
//
inline bool TDewRecord::GetField(const owl_string& fldName, long double& value) const
{	return GetField(GetFieldNumber(fldName), value);}
//
inline bool TDewRecord::GetField(const owl_string& fldName, __OWL TDate& value) const
{	return GetField(GetFieldNumber(fldName), value);}
//
inline bool TDewRecord::GetField(const owl_string& fldName, __OWL TTime& value) const
{	return GetField(GetFieldNumber(fldName), value);}
//
inline bool TDewRecord::GetField(const owl_string& fldName, __OWL TSystemTime& value) const
{	return GetField(GetFieldNumber(fldName), value);}
//
inline bool TDewRecord::IsBlank(const owl_string& fldName) const
{	return IsBlank(GetFieldNumber(fldName));}
//
inline void TDewRecord::SetBlank(const owl_string& fldName)
{	SetBlank(GetFieldNumber(fldName));}
//
inline void TDewRecord::SetField(const owl_string& fldName, const owl_string& value)
{	SetField(GetFieldNumber(fldName), value);}
//
inline void TDewRecord::SetField(const owl_string& fldName, const char*  value)
{	SetField(GetFieldNumber(fldName), value);}
//
inline void TDewRecord::SetField(const owl_string& fldName, const bool value)
{	SetField(GetFieldNumber(fldName), value);}
//
inline void TDewRecord::SetField(const owl_string& fldName, const int16 value)
{	SetField(GetFieldNumber(fldName), value);}
//
inline void TDewRecord::SetField(const owl_string& fldName, const int32 value)
{	SetField(GetFieldNumber(fldName), value);}
//
inline void TDewRecord::SetField(const owl_string& fldName, const uint value)
{	SetField(GetFieldNumber(fldName), value);}
//
inline void TDewRecord::SetField(const owl_string& fldName, const uint32 value)
{	SetField(GetFieldNumber(fldName), value);}
//
inline void TDewRecord::SetField(const owl_string& fldName, const float value)
{	SetField(GetFieldNumber(fldName), value);}
//
inline void TDewRecord::SetField(const owl_string& fldName, const double value)
{	SetField(GetFieldNumber(fldName), value);}
//
inline void TDewRecord::SetField(const owl_string& fldName, const long double value)
{	SetField(GetFieldNumber(fldName), value);}
//
inline void TDewRecord::SetField(const owl_string& fldName, const __OWL TDate& value)
{	SetField(GetFieldNumber(fldName), value);}
//
inline void TDewRecord::SetField(const owl_string& fldName, const __OWL TTime& value)
{	SetField(GetFieldNumber(fldName), value);}
//
inline void TDewRecord::SetField(const owl_string& fldName, const __OWL TSystemTime& value)
{	SetField(GetFieldNumber(fldName), value);}
//
inline bool TDewRecord::VerifyField(const owl_string& fldName) const
{	return VerifyField(GetFieldNumber(fldName));}
//


_OWLFX_END_NAMESPACE


#endif // OWLFX_DEW_H
