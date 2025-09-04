//----------------------------------------------------------------------------
// OWL File Extensions (OWLFX) Class Library
// Copyright(c) 1999 by Yura Bidus.
// All rights reserved.
//
//$Revision: 1.1.4.3 $
//$Author: jogybl $
//$Date: 2010-02-23 10:46:13 $
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
//----------------------------------------------------------------------------
#include <owlfx/pch.h>
#pragma hdrstop
#include <owlfx/dew.h>
#include <owlfx/bdeapi.h>
#include <stdio.h>

_OWLFX_BEGIN_NAMESPACE

__OWL_USING_NAMESPACE
__OWL_USING_STD_NAMESPACE

//------------------------------------------------------------------------------------------------
// class TDewHandle
// ~~~~~ ~~~~~~~~~~
//
// Get property value.
void TDewHandle::GetProp(const uint32 iProp, const pVOID pPropValue, const uint iMaxLen, uint16& iLen) const
{
	TDewException::CheckResult(TIdApi32::DbiGetProp(GetHandle(), iProp, pPropValue, (uint16)iMaxLen, &iLen));
}
// Set property value.
void TDewHandle::SetProp(const uint32 iProp, const uint32 iPropValue)
{
	TDewException::CheckResult(TIdApi32::DbiSetProp(GetHandle(), iProp, iPropValue));
}
//------------------------------------------------------------------------------------------------
// class TDewHandle
// ~~~~~ ~~~~~~~~~~
//
TDewBlob::TDewBlob()
: 
fBuffer(0),fHCursor(0)
{
}
//
TDewBlob::TDewBlob(const TDewRecord& record, const uint iField, const TDBOpenMode eOpenMode)
: 
fBuffer(record.fBuffer),fField(iField),fHCursor(record.GetTable().GetHandle())
{
	record.CheckFieldNumber(iField);

	TDewException::CheckResult(TIdApi32::DbiOpenBlob(fHCursor, fBuffer, (uint16)fField, (DBIOpenMode)eOpenMode));
}
//
TDewBlob::TDewBlob(const TDewRecord& record, const owl_string& fieldName, const TDBOpenMode eOpenMode)
: fBuffer(record.fBuffer),
fField(record.GetFieldNumber(fieldName)),
fHCursor(record.GetTable().GetHandle())
{
	TDewException::CheckResult(TIdApi32::DbiOpenBlob(fHCursor, fBuffer, (uint16)fField, (DBIOpenMode)eOpenMode));
}
// Destructor.
TDewBlob::~TDewBlob() 
{
	if (fBuffer != 0)
		Close();
}
// Close the blob.
void TDewBlob::Close()
{
	TDewException::CheckResult(TIdApi32::DbiFreeBlob(fHCursor, fBuffer, (uint16)fField));
	fBuffer = 0;
	fHCursor = NULL;
}
// Read bytes from blob.
uint32 TDewBlob::Get(const uint32 iOffset, const uint32 iLen, uint8* pDest) const
{
	uint32 iRead;
	TDewException::CheckResult(TIdApi32::DbiGetBlob(fHCursor, fBuffer, (uint16)fField, iOffset, iLen, pDest, &iRead));
	return iRead;
}
// Get blob heading.
void TDewBlob::GetHeading(uint8* pDest) const
{
	TDewException::CheckResult(TIdApi32::DbiGetBlobHeading(fHCursor, (uint16)fField, fBuffer, pDest));
}
// Open blob.
void TDewBlob::Open(TDewRecord& record, const uint iField, const TDBOpenMode eOpenMode)
{
	if (fBuffer != 0)
		Close();
	record.CheckFieldNumber(iField);
	fHCursor = record.GetTable().GetHandle();
	fBuffer = record.fBuffer;
	fField = iField;
	TDewException::CheckResult(TIdApi32::DbiOpenBlob(fHCursor, fBuffer, (uint16)fField, (DBIOpenMode)eOpenMode));
}
//
void TDewBlob::Open(TDewRecord& record, const owl_string& fieldName, const TDBOpenMode eOpenMode)
{
	if (fBuffer != 0)
		Close();
	fHCursor = record.GetTable().GetHandle();
	fBuffer = record.fBuffer;
	fField = record.GetFieldNumber(fieldName);
	TDewException::CheckResult(TIdApi32::DbiOpenBlob(fHCursor, fBuffer, (uint16)fField, (DBIOpenMode)eOpenMode));
}
// Write bytes to blob.
void TDewBlob::Set(const uint32 iOffset, const uint32 iLen, const uint8* pSrc)
{
	TDewException::CheckResult(TIdApi32::DbiPutBlob(fHCursor, fBuffer, (uint16)fField, iOffset, iLen, (pBYTE)pSrc));
}
// Get the size of a blob.
uint32 TDewBlob::GetSize() const
{
	uint32 iSize;
	TDewException::CheckResult(TIdApi32::DbiGetBlobSize(fHCursor, fBuffer, (uint16)fField, &iSize));
	return iSize;
}
// Reduce the blob size.
void TDewBlob::Truncate(const uint32 iLen)
{
	TDewException::CheckResult(TIdApi32::DbiTruncateBlob(fHCursor, fBuffer, (uint16)fField, iLen));
}
//------------------------------------------------------------------------------------------------
// class TDewBytes
// ~~~~~ ~~~~~~~~~~
//
TDewBytes::TDewBytes(const uint size)
: 
fBuffer(0)
{
	fBuffer = new BYTE[size];
}
// TDewBytes destructor.
TDewBytes::~TDewBytes()
{
	delete [] fBuffer;
};
// TDewBytes casts.
TDewBytes::operator pBYTE()
{
	return fBuffer;
}
//
TDewBytes::operator pcBYTE() const
{
	return fBuffer;
}
// TDewBookmark constructor.
TDewBookMark::TDewBookMark(const TDewTable& table)
: 
TDewBytes(table.GetTableProps().iBookMarkSize)
{
}
// TDewKey constructor.
TDewKey::TDewKey(const TDewTable& table)
: 
TDewBytes(table.GetTableProps().iKeySize)
{
}
//------------------------------------------------------------------------------------------------
// class TDewTable
// ~~~~~ ~~~~~~~~~~
//
//
owl_string 
TDewTable::TDewDataLink::GetFieldName(const uint fldId) const
{
	MEMBER_METHOD_PROLOGUE(TDewTable, DataLink, theTable)
		return theTable->GetCurRecord().GetFieldName(fldId);
}
//
uint 
TDewTable::TDewDataLink::GetFieldNumber(const owl_string& name) const
{
	MEMBER_METHOD_PROLOGUE(TDewTable, DataLink, theTable)
		return theTable->GetCurRecord().GetFieldNumber(name);
}
//
owl_string 
TDewTable::TDewDataLink::GetFieldTypeName(const uint fldId) const
{
	MEMBER_METHOD_PROLOGUE(TDewTable, DataLink, theTable)
		return theTable->GetCurRecord().GetFieldTypeName(fldId);
}
//
uint32 
TDewTable::TDewDataLink::GetBlobSize(const uint fldId) const
{
	MEMBER_METHOD_PROLOGUE(TDewTable, DataLink, theTable)
		return theTable->GetCurRecord().GetBlobSize(fldId);
}
//
TDBBlob* 
TDewTable::TDewDataLink::GetBlob(const uint fldId, const TDBOpenMode mode)
{
	MEMBER_METHOD_PROLOGUE(TDewTable, DataLink, theTable)
		return new TDewBlob(theTable->GetCurRecord(), fldId, mode);
}
//
bool	
TDewTable::TDewDataLink::GetField(const uint fldId, owl_string& value) const
{
	MEMBER_METHOD_PROLOGUE(TDewTable, DataLink, theTable)
		return theTable->GetCurRecord().GetField(fldId, value);
}
//
bool	
TDewTable::TDewDataLink::GetField(const uint fldId, bool& value) const
{
	MEMBER_METHOD_PROLOGUE(TDewTable, DataLink, theTable)
		return theTable->GetCurRecord().GetField(fldId, value);
}
//
bool	
TDewTable::TDewDataLink::GetField(const uint fldId, int16& value) const
{
	MEMBER_METHOD_PROLOGUE(TDewTable, DataLink, theTable)
		return theTable->GetCurRecord().GetField(fldId, value);
}
//
bool	
TDewTable::TDewDataLink::GetField(const uint fldId, int32& value) const
{
	MEMBER_METHOD_PROLOGUE(TDewTable, DataLink, theTable)
		return theTable->GetCurRecord().GetField(fldId, value);
}
//
bool	
TDewTable::TDewDataLink::GetField(const uint fldId, uint16& value) const
{
	MEMBER_METHOD_PROLOGUE(TDewTable, DataLink, theTable)
		return theTable->GetCurRecord().GetField(fldId, value);
}
//
bool	
TDewTable::TDewDataLink::GetField(const uint fldId, uint32& value) const
{
	MEMBER_METHOD_PROLOGUE(TDewTable, DataLink, theTable)
		return theTable->GetCurRecord().GetField(fldId, value);
}
//
bool	
TDewTable::TDewDataLink::GetField(const uint fldId, float& value) const
{
	MEMBER_METHOD_PROLOGUE(TDewTable, DataLink, theTable)
		return theTable->GetCurRecord().GetField(fldId, value);
}
//
bool	
TDewTable::TDewDataLink::GetField(const uint fldId, double& value) const
{
	MEMBER_METHOD_PROLOGUE(TDewTable, DataLink, theTable)
		return theTable->GetCurRecord().GetField(fldId, value);
}
//
bool	
TDewTable::TDewDataLink::GetField(const uint fldId, long double& value) const
{
	MEMBER_METHOD_PROLOGUE(TDewTable, DataLink, theTable)
		return theTable->GetCurRecord().GetField(fldId, value);
}
//
bool	
TDewTable::TDewDataLink::GetField(const uint fldId, TDate& value) const
{
	MEMBER_METHOD_PROLOGUE(TDewTable, DataLink, theTable)
		return theTable->GetCurRecord().GetField(fldId, value);
}
//
bool	
TDewTable::TDewDataLink::GetField(const uint fldId, TTime& value) const
{
	MEMBER_METHOD_PROLOGUE(TDewTable, DataLink, theTable)
		return theTable->GetCurRecord().GetField(fldId, value);
}
//
bool	
TDewTable::TDewDataLink::GetField(const uint fldId, TSystemTime& value) const
{
	MEMBER_METHOD_PROLOGUE(TDewTable, DataLink, theTable)
		return theTable->GetCurRecord().GetField(fldId, value);
}
//
bool	
TDewTable::TDewDataLink::IsBlank(const uint fldId) const
{
	MEMBER_METHOD_PROLOGUE(TDewTable, DataLink, theTable)
		return theTable->GetCurRecord().IsBlank(fldId);
}
//
void	
TDewTable::TDewDataLink::SetBlank(const uint fldId)
{
	MEMBER_METHOD_PROLOGUE(TDewTable, DataLink, theTable)
		theTable->GetCurRecord().SetBlank(fldId);
}
//
void	
TDewTable::TDewDataLink::SetField(const uint fldId, const owl_string& value)
{
	MEMBER_METHOD_PROLOGUE(TDewTable, DataLink, theTable)
		theTable->GetCurRecord().SetField(fldId, value);
}
//
void	
TDewTable::TDewDataLink::SetField(const uint fldId, const char*  value)
{
	MEMBER_METHOD_PROLOGUE(TDewTable, DataLink, theTable)
		theTable->GetCurRecord().SetField(fldId, value);
}
//
void	
TDewTable::TDewDataLink::SetField(const uint fldId, const bool value)
{
	MEMBER_METHOD_PROLOGUE(TDewTable, DataLink, theTable)
		theTable->GetCurRecord().SetField(fldId, value);
}
//
void	
TDewTable::TDewDataLink::SetField(const uint fldId, const int16 value)
{
	MEMBER_METHOD_PROLOGUE(TDewTable, DataLink, theTable)
		theTable->GetCurRecord().SetField(fldId, value);
}
//
void	
TDewTable::TDewDataLink::SetField(const uint fldId, const int32 value)
{
	MEMBER_METHOD_PROLOGUE(TDewTable, DataLink, theTable)
		theTable->GetCurRecord().SetField(fldId, value);
}
//
void	
TDewTable::TDewDataLink::SetField(const uint fldId, const uint value)
{
	MEMBER_METHOD_PROLOGUE(TDewTable, DataLink, theTable)
		theTable->GetCurRecord().SetField(fldId, value);
}
//
void	
TDewTable::TDewDataLink::SetField(const uint fldId, const uint32 value)
{
	MEMBER_METHOD_PROLOGUE(TDewTable, DataLink, theTable)
		theTable->GetCurRecord().SetField(fldId, value);
}
//
void	
TDewTable::TDewDataLink::SetField(const uint fldId, const float value)
{
	MEMBER_METHOD_PROLOGUE(TDewTable, DataLink, theTable)
		theTable->GetCurRecord().SetField(fldId, value);
}
//
void	
TDewTable::TDewDataLink::SetField(const uint fldId, const double value)
{
	MEMBER_METHOD_PROLOGUE(TDewTable, DataLink, theTable)
		theTable->GetCurRecord().SetField(fldId, value);
}
//
void	
TDewTable::TDewDataLink::SetField(const uint fldId, const long double value)
{
	MEMBER_METHOD_PROLOGUE(TDewTable, DataLink, theTable)
		theTable->GetCurRecord().SetField(fldId, value);
}
//
void	
TDewTable::TDewDataLink::SetField(const uint fldId, const TDate& value)
{
	MEMBER_METHOD_PROLOGUE(TDewTable, DataLink, theTable)
		theTable->GetCurRecord().SetField(fldId, value);
}
//
void	
TDewTable::TDewDataLink::SetField(const uint fldId, const TTime& value)
{
	MEMBER_METHOD_PROLOGUE(TDewTable, DataLink, theTable)
		theTable->GetCurRecord().SetField(fldId, value);
}
//
void	
TDewTable::TDewDataLink::SetField(const uint fldId, const TSystemTime& value)
{
	MEMBER_METHOD_PROLOGUE(TDewTable, DataLink, theTable)
		theTable->GetCurRecord().SetField(fldId, value);
}
//
bool	
TDewTable::TDewDataLink::VerifyField(const uint fldId) const
{
	MEMBER_METHOD_PROLOGUE(TDewTable, DataLink, theTable)
		return theTable->GetCurRecord().VerifyField(fldId);
}
/////////////////////////////////////////////////////////////////////////////////////
// Constructors.
TDewTable::TDewTable()
: 
fHCursor(NULL),  fHDatabase(NULL),  fFieldDesc(0),  fRecord(0)
{
}
//
TDewTable::TDewTable(const hDBICur hCursor)
: 
fHCursor(hCursor),  fFieldDesc(0),  fRecord(0)
{
	if (fHCursor != NULL) {
		TDewException::CheckResult(TIdApi32::DbiGetObjFromObj(fHCursor, objDATABASE, &fHDatabase));
		GetProperties();
	}
}
//
TDewTable::TDewTable(const TDewDatabase& database)
: 
fHCursor(NULL),fHDatabase(database.GetHandle()),fFieldDesc(0),fRecord(0)
{
}
//
TDewTable::TDewTable(const TDewDatabase& database, const pcCHAR pszTableName, 
					 const pcCHAR pszDriverType, const TDBOpenMode eOpenMode, 
					 const DBIShareMode eShareMode, const pcCHAR pszIndexName, 
					 const pcCHAR pszIndexTagName, const uint iIndexId, 
					 const XLTMode exltMode, const bool bUniDirectional, 
					 const pcBYTE pOptParams)
					 : 
fHCursor(NULL),fHDatabase(database.GetHandle()),fFieldDesc(0),fRecord(0)
{
	Open(pszTableName, pszDriverType, eOpenMode, eShareMode, pszIndexName, pszIndexTagName, iIndexId, exltMode, bUniDirectional, pOptParams);
}
//
TDewTable::TDewTable(const TDewTable& cursor, const bool bReadOnly, const bool bUniDirectional)
: 
fHCursor(NULL),fFieldDesc(0),fRecord(0)
{
	if (cursor.IsOpen()) {
		TDewException::CheckResult(TIdApi32::DbiGetObjFromObj(cursor.fHCursor, objDATABASE, &fHDatabase));
		TDewException::CheckResult(TIdApi32::DbiCloneCursor(cursor.fHCursor, bReadOnly, bUniDirectional, &fHCursor));
		GetProperties();
	}
}
// Destructor.
TDewTable::~TDewTable()
{
	if (IsOpen())
		Close();
}
// Assignment operator.
TDewTable& TDewTable::operator=(const TDewTable& cursor)
{
	if (IsOpen())
		Close();
	if (cursor.IsOpen()) {
		TDewException::CheckResult(TIdApi32::DbiGetObjFromObj(cursor.fHCursor, objDATABASE, &fHDatabase));
		TDewException::CheckResult(TIdApi32::DbiCloneCursor(cursor.fHCursor, FALSE, FALSE, &fHCursor));
		GetProperties();
	}
	return *this;
}
//
bool TDewTable::IsLast()
{
	bool retval = false;
	DBIResult result = TIdApi32::DbiGetNextRecord(fHCursor,  dbiNOLOCK, NULL, 0);

	if (result == DBIERR_EOF)
		retval = true;
	TIdApi32::DbiGetPriorRecord(fHCursor, dbiNOLOCK, NULL, 0);
	return retval;
}
//
bool TDewTable::IsFirst()
{
	bool retval = false;
	DBIResult result = TIdApi32::DbiGetPriorRecord(fHCursor,  dbiNOLOCK, NULL, 0);

	if (result == DBIERR_BOF)
		retval = true;
	TIdApi32::DbiGetNextRecord(fHCursor, dbiNOLOCK, NULL, 0);
	return retval;
}
// Lock a table.
void TDewTable::AcqTableLock(const TDBLockType lock)
{
	TDewException::CheckResult(TIdApi32::DbiAcqTableLock(fHCursor, (DBILockType)lock));
}
// Add a new index.
void TDewTable::AddIndex(const IDXDesc& idxDesc, const pcCHAR pszKeyviolName)
{
	TDewException::CheckResult(TIdApi32::DbiAddIndex(fHDatabase, fHCursor, NULL, NULL, (pIDXDesc) &idxDesc, (pCHAR) pszKeyviolName));
	GetProperties();
}
// Find/get the next record.
bool TDewTable::Advance(const TDBLockType eLock, const pRECProps pRecProps)
{
	const DBIResult result = TIdApi32::DbiGetNextRecord(fHCursor, (DBILockType)eLock, fRecord->fBuffer, pRecProps);

	if (result == DBIERR_NONE){
		DataLink.RefreshControls();
		return true;
	}
	else if (result == DBIERR_EOF)
		return false;
	else
		throw TDewException(result);
	return false;
}
// Insert a new record.
void TDewTable::AppendRecord()
{
	TDewException::CheckResult(TIdApi32::DbiAppendRecord(fHCursor, fRecord->fBuffer));
	DataLink.RefreshControls();
}
// Perform the specified operation.
void TDewTable::ApplyDelayedUpdates(DBIDelayedUpdCmd eUpdCmd)
{
	TDewException::CheckResult(TIdApi32::DbiApplyDelayedUpdates(fHCursor, eUpdCmd));
	GetProperties();
}
// Put cursor in delayed update mode.
void TDewTable::BeginDelayedUpdates()
{
	TDewException::CheckResult(TIdApi32::DbiBeginDelayedUpdates(&fHCursor));
	GetProperties();
}
// Convert cursor to a link cursor.
void TDewTable::BeginLinkMode()
{
	TDewException::CheckResult(TIdApi32::DbiBeginLinkMode(&fHCursor));
	GetProperties();
}
// Close the cursor.
void TDewTable::Close()
{
	delete fRecord;
	delete [] fFieldDesc;

	TDewException::CheckResult(TIdApi32::DbiCloseCursor(&fHCursor));
	fHCursor = NULL;
	fRecord = 0;
	fFieldDesc = 0;
}
// Close an index.
void TDewTable::CloseIndex(const pcCHAR pszIndexName, const uint iIndexId)
{
	TDewException::CheckResult(TIdApi32::DbiCloseIndex(fHCursor, (pCHAR) pszIndexName, (uint16)iIndexId));
	GetProperties();
}
// Compare two book-marks.
CMPBkMkRslt TDewTable::CompareBookMarks(const pcBYTE pBookMark1, const pcBYTE pBookMark2) const
{
	CMPBkMkRslt cmpBkMkRslt;
	TDewException::CheckResult(TIdApi32::DbiCompareBookMarks(fHCursor, (pBYTE) pBookMark1, (pBYTE) pBookMark2, &cmpBkMkRslt));
	return cmpBkMkRslt;
}
// Compare two keys.
int TDewTable::CompareKeys(const pcBYTE pKey1, const pcBYTE pKey2, const uint iFields, const uint iLen) const
{
	int16 result;
	TDewException::CheckResult(TIdApi32::DbiCompareKeys(fHCursor, (pBYTE) pKey1, (pBYTE) pKey2, (uint16)iFields, (uint16)iLen, &result));
	return result;
}
//
const CURProps& TDewTable::GetTableProps() const
{
	return fCursorProps;
}
// Delete index.
void TDewTable::DeleteIndex(const pcCHAR pszIndexName, const uint iIndexNum, const pcCHAR pszIndexTagName)
{
	TDewException::CheckResult(TIdApi32::DbiDeleteIndex(fHDatabase, fHCursor, NULL, NULL, (pCHAR) pszIndexName, (pCHAR) pszIndexTagName, (uint16)iIndexNum));
	GetProperties();
}
// Delete the current record.
TDewRecord& TDewTable::DeleteRecord()
{
	TDewException::CheckResult(TIdApi32::DbiDeleteRecord(fHCursor, fRecord->fBuffer));
	return *fRecord;
}
// Delete all records.
void TDewTable::EmptyTable()
{
	TDewException::CheckResult(TIdApi32::DbiEmptyTable(fHDatabase, fHCursor, NULL, NULL));
}
// Convert cursor back to normal cursor.
void TDewTable::EndDelayedUpdates()
{
	TDewException::CheckResult(TIdApi32::DbiEndDelayedUpdates(&fHCursor));
	GetProperties();
}
// Convert cursor back to normal cursor.
void TDewTable::EndLinkMode()
{
	TDewException::CheckResult(TIdApi32::DbiEndLinkMode(&fHCursor));
	GetProperties();
}
// Extract the key value for a record.
void TDewTable::ExtractKey(const pBYTE pKey, const TDewRecord& record) const
{
	TDewException::CheckResult(TIdApi32::DbiExtractKey(fHCursor, (pBYTE) record.fBuffer, pKey));
}
// Get field descriptions.
pcFLDDesc TDewTable::FieldDescs() const
{
	if(!fFieldDesc)
		TDewException::CheckResult(DBIERR_NOTSUPPORTED, _T("TDewTable::FieldDescs - No field descriptors available."));
	return fFieldDesc;
}
// Force Reread of current record from Server.
const TDewRecord& TDewTable::ForceRecordReread() const
{
	TDewException::CheckResult(TIdApi32::DbiForceRecordReread(fHCursor, fRecord->fBuffer));
	((TDewTable*)this)->DataLink.RefreshControls();
	return *fRecord;
}
// Force reread of buffers from disk.
const TDewRecord& TDewTable::ForceReread() const
{
	TDewException::CheckResult(TIdApi32::DbiForceReread(fHCursor));

	return *fRecord;
}
// Get a book-mark.
void TDewTable::GetBookMark(const pBYTE pBookMark) const
{
	TDewException::CheckResult(TIdApi32::DbiGetBookMark(fHCursor, pBookMark));
}
// Get a callback function.
void TDewTable::GetCallBack(const CBType ecbType, uint32& iClientData, uint16& iCbBufLen, pVOID& pCbBuf, pfDBICallBack& pfCb) const
{
	TDewException::CheckResult(TIdApi32::DbiGetCallBack(fHCursor, ecbType, &iClientData, &iCbBufLen, &pCbBuf, &pfCb));
}
// Get index properties.
void TDewTable::GetIndexDesc(const uint iIndexSeqNo, IDXDesc& idxDesc) const
{
	TDewException::CheckResult(TIdApi32::DbiGetIndexDesc(fHCursor, (uint16)iIndexSeqNo, &idxDesc));
}
//
void TDewTable::GetIndexDesc(IDXDesc& idxDesc) const
{
	TDewException::CheckResult(TIdApi32::DbiGetIndexDescs(fHCursor, &idxDesc));
}
// Get index descriptor for given field.
void TDewTable::GetIndexForField(const uint iFld, IDXDesc& idxDesc, const bool bProdTagOnly) const
{
	TDewException::CheckResult(TIdApi32::DbiGetIndexForField(fHCursor, (uint16)iFld, bProdTagOnly, &idxDesc));
}
// Find/get the next record.
TDewRecord& TDewTable::GetNextRecord(const TDBLockType eLock, const pRECProps pRecProps)
{
	TDewException::CheckResult(TIdApi32::DbiGetNextRecord(fHCursor, (DBILockType)eLock, fRecord->fBuffer, pRecProps));
	DataLink.RefreshControls();
	return *fRecord;
}
// Find/get the prior record.
TDewRecord& TDewTable::GetPriorRecord(const TDBLockType eLock, const pRECProps pRecProps)
{
	TDewException::CheckResult(TIdApi32::DbiGetPriorRecord(fHCursor, (DBILockType)eLock, fRecord->fBuffer, pRecProps));
	DataLink.RefreshControls();
	return *fRecord;
}
// Get the cursor properties.
void TDewTable::GetProperties()
{
	TDewException::CheckResult(TIdApi32::DbiGetCursorProps(fHCursor, &fCursorProps));

	delete [] fFieldDesc;
	fFieldDesc = new FLDDesc[fCursorProps.iFields];
	DBIResult result = TIdApi32::DbiGetFieldDescs(fHCursor, fFieldDesc);
	if (result == DBIERR_NOTSUPPORTED) {
		delete [] fFieldDesc;
		fFieldDesc = 0;
	} 
	else
		TDewException::CheckResult(result);

	if (fRecord != 0)
		fRecord->ResizeBuffer();
	else
		fRecord = new TDewRecord(*this);
}
// Get the current record.
TDewRecord& TDewTable::GetRecord(const TDBLockType eLock, const pRECProps pRecProps)
{
	TDewException::CheckResult(TIdApi32::DbiGetRecord(fHCursor, (DBILockType)eLock, fRecord->fBuffer, pRecProps));
	return *fRecord;
}
// Find a record matching key.
TDewRecord& TDewTable::GetRecordForKey(const TDewRecord& record, const uint iFields, const uint iLen)
{
	TDewException::CheckResult(TIdApi32::DbiGetRecordForKey(fHCursor, FALSE, (uint16)iFields, (uint16)iLen, record.fBuffer, fRecord->fBuffer));
	DataLink.RefreshControls();
	return *fRecord;
}
//
TDewRecord& TDewTable::GetRecordForKey(const pcBYTE pKey, const uint iFields, const uint iLen)
{
	TDewException::CheckResult(TIdApi32::DbiGetRecordForKey(fHCursor, TRUE, (uint16)iFields, (uint16)iLen, (pBYTE) pKey, fRecord->fBuffer));
	DataLink.RefreshControls();
	return *fRecord;
}
// Find/get a record by record number.
TDewRecord& TDewTable::GetRelativeRecord(const int32 iPosOffset, const TDBLockType eLock, const pRECProps pRecProps)
{
	TDewException::CheckResult(TIdApi32::DbiGetRelativeRecord(fHCursor, iPosOffset, (DBILockType)eLock, fRecord->fBuffer, pRecProps));
	DataLink.RefreshControls();
	return *fRecord;
}
// Get referential integrity descriptor.
void TDewTable::GetRintDesc(const uint iRintSeqNo, RINTDesc& rintDesc) const
{
	TDewException::CheckResult(TIdApi32::DbiGetRintDesc(fHCursor, (uint16)iRintSeqNo, &rintDesc));
}
// Get validity check descriptor.
void TDewTable::GetVchkDesc(const uint iValSeqNo, VCHKDesc& valDesc) const
{
	TDewException::CheckResult(TIdApi32::DbiGetVchkDesc(fHCursor, (uint16)iValSeqNo, &valDesc));
}
// Get index sequence number.
uint TDewTable::IndexSeqNo(const pcCHAR pszIndexName, const uint iIndexId, const pcCHAR pszTagName) const
{
	uint16 iIndexSeqNo;
	TDewException::CheckResult(TIdApi32::DbiGetIndexSeqNo(fHCursor, (pCHAR) pszIndexName, (pCHAR) pszTagName, (uint16)iIndexId, &iIndexSeqNo));
	return iIndexSeqNo;
}
// Initialize record area.
TDewRecord& TDewTable::InitRecord()
{
	TDewException::CheckResult(TIdApi32::DbiInitRecord(fHCursor, fRecord->fBuffer));
	return *fRecord;
}
// Insert a new record.
TDewRecord& TDewTable::InsertRecord(const TDBLockType eLock)
{
	TDewException::CheckResult(TIdApi32::DbiInsertRecord(fHCursor, (DBILockType)eLock, fRecord->fBuffer));
	DataLink.RefreshControls();
	return *fRecord;
}
// Check if current record is locked.
bool TDewTable::IsRecordLocked() const
{
	BOOL bLocked;
	TDewException::CheckResult(TIdApi32::DbiIsRecordLocked(fHCursor, &bLocked));
	return bLocked==TRUE;
}
// Verify if table is locked.
uint TDewTable::IsTableLocked(const TDBLockType eLockType) const
{
	uint16 iLocks;
	TDewException::CheckResult(TIdApi32::DbiIsTableLocked(fHCursor, (DBILockType)eLockType, &iLocks));
	return iLocks;
}
// Verify if table is shared.
bool TDewTable::IsTableShared() const
{
	BOOL bShared;
	TDewException::CheckResult(TIdApi32::DbiIsTableShared(fHCursor, &bShared));
	return bShared==TRUE;
}
// Link detail to master.
void TDewTable::LinkDetail(TDewTable& master, const uint iLnkFields, const pcUINT16 piMstrFields, const pcUINT16 piDetlFields)
{
	TDewException::CheckResult(TIdApi32::DbiLinkDetail(master.fHCursor, fHCursor, (uint16)iLnkFields, (pUINT16) piMstrFields, (pUINT16) piDetlFields));
	GetProperties();
}
//
void TDewTable::LinkDetail(TDewTable& master, const uint iKeyLen, const pcCHAR pszMstrExp)
{
	TDewException::CheckResult(TIdApi32::DbiLinkDetailToExp(master.fHCursor, fHCursor, (uint16)iKeyLen, (pCHAR) pszMstrExp));
	GetProperties();
}
// Get a list of locks.
hDBICur TDewTable::LockList(const bool bAllUsers, const bool bAllLockTypes) const
{
	hDBICur hCursor;
	TDewException::CheckResult(TIdApi32::DbiOpenLockList(fHCursor, bAllUsers, bAllLockTypes, &hCursor));
	return hCursor;
}
// Make temporary table permanent.
void TDewTable::MakePermanent(const pcCHAR pszName, const bool bOverWrite)
{
	TDewException::CheckResult(TIdApi32::DbiMakePermanent(fHCursor, (pCHAR) pszName, bOverWrite));
	GetProperties();
}
// Update the current record.
TDewRecord& TDewTable::ModifyRecord(const bool bFreeLock)
{
	TDewException::CheckResult(TIdApi32::DbiModifyRecord(fHCursor, fRecord->fBuffer, bFreeLock));
	return *fRecord;
}
// Open a table.
void TDewTable::Open(const TDewDatabase& database, const pcCHAR pszTableName, const pcCHAR pszDriverType, const TDBOpenMode eOpenMode, const DBIShareMode eShareMode, const pcCHAR pszIndexName, const pcCHAR pszIndexTagName, const uint iIndexId, const XLTMode exltMode, const bool bUniDirectional, const pcBYTE pOptParams)
{
	if (IsOpen())
		Close();

	fHDatabase = database.GetHandle();
	TDewException::CheckResult(TIdApi32::DbiOpenTable(fHDatabase, (pCHAR)pszTableName, (pCHAR) pszDriverType, (pCHAR) pszIndexName, (pCHAR) pszIndexTagName, (uint16)iIndexId, (DBIOpenMode)eOpenMode, eShareMode, exltMode, bUniDirectional, (pBYTE) pOptParams, &fHCursor));
	GetProperties();
}
//
void TDewTable::Open(const pcCHAR pszTableName, const pcCHAR pszDriverType, const TDBOpenMode eOpenMode, const DBIShareMode eShareMode, const pcCHAR pszIndexName, const pcCHAR pszIndexTagName, const uint iIndexId, const XLTMode exltMode, const bool bUniDirectional, const pcBYTE pOptParams)
{
	if (IsOpen())
		Close();
	TDewException::CheckResult(TIdApi32::DbiOpenTable(fHDatabase, (pCHAR) pszTableName, (pCHAR) pszDriverType, (pCHAR) pszIndexName, (pCHAR) pszIndexTagName, (uint16)iIndexId, (DBIOpenMode)eOpenMode, eShareMode, exltMode, bUniDirectional, (pBYTE) pOptParams, &fHCursor));
	GetProperties();
}
// Open an index.
void TDewTable::OpenIndex(const pcCHAR pszIndexName, const uint iIndexId)
{
	TDewException::CheckResult(TIdApi32::DbiOpenIndex(fHCursor, (pCHAR) pszIndexName, (uint16)iIndexId));
	GetProperties();
}
// Pack a table.
void TDewTable::PackTable(const bool bRegenIdxs)
{
	TDewException::CheckResult(TIdApi32::DbiPackTable(fHDatabase, fHCursor, NULL, NULL, bRegenIdxs));
}
// Read a block of records.
uint32 TDewTable::ReadBlock(const uint32 iRecords, const pBYTE pBuf)
{
	uint32 records = iRecords;
	TDewException::CheckResult(TIdApi32::DbiReadBlock(fHCursor, &records, pBuf));
	return records;
}
// Answer the record.
TDewRecord& TDewTable::GetCurRecord()
{
	return *fRecord;
}
/*
const TDewRecord& TDewTable::GetCurRecord() const
{
return *fRecord;
}
*/
// Get the current number of records.
uint32 TDewTable::GetRecordCount() const
{
	uint32 iRecCount;
	TDewException::CheckResult(TIdApi32::DbiGetRecordCount(fHCursor, &iRecCount));
	return iRecCount;
}
// Regenerate an index.
void TDewTable::RegenIndex(const pcCHAR pszIndexName, const uint iIdxNum, const pcCHAR pszIndexTagName)
{
	TDewException::CheckResult(TIdApi32::DbiRegenIndex(fHDatabase, fHCursor, NULL, NULL, (pCHAR) pszIndexName, (pCHAR) pszIndexTagName, (uint16)iIdxNum));
}
// Regenerate all indexes.
void TDewTable::RegenIndexes()
{
	TDewException::CheckResult(TIdApi32::DbiRegenIndexes(fHCursor));
}
// Register callback function.
void TDewTable::RegisterCallBack(const CBType ecbType, const uint32 iClientData, const uint iCbBufLen, const pVOID pCbBuf, const pfDBICallBack pfCb)
{
	TDewException::CheckResult(TIdApi32::DbiRegisterCallBack(fHCursor, ecbType, iClientData, (uint16)iCbBufLen, pCbBuf, pfCb));
}
// Release record level locks.
void TDewTable::RelRecordLock(const bool bAll)
{
	TDewException::CheckResult(TIdApi32::DbiRelRecordLock(fHCursor, bAll));
}
// Release table level locks.
void TDewTable::RelTableLock(const TDBLockType eLockType, const bool bAll)
{
	TDewException::CheckResult(TIdApi32::DbiRelTableLock(fHCursor, bAll, (DBILockType)eLockType));
}
// Remove the range limit.
void TDewTable::ResetRange()
{
	TDewException::CheckResult(TIdApi32::DbiResetRange(fHCursor));
}
// Flush all buffered changes.
void TDewTable::SaveChanges() const
{
	TDewException::CheckResult(TIdApi32::DbiSaveChanges(fHCursor));
}
// Get logical record number.
uint32 TDewTable::SeqNo() const
{
	uint32 iSeqNo;
	TDewException::CheckResult(TIdApi32::DbiGetSeqNo(fHCursor, &iSeqNo));
	return iSeqNo;
}
// Set field map.
void TDewTable::SetFieldMap(const uint iFields, const pcFLDDesc paFldDesc)
{
	TDewException::CheckResult(TIdApi32::DbiSetFieldMap(fHCursor, (uint16)iFields, (pFLDDesc) paFldDesc));
	GetProperties();
}
// Set property value.
void TDewTable::SetProp(const uint32 iProp, const uint32 iPropValue)
{
	TDewHandle::SetProp(iProp, iPropValue);
	GetProperties();
}
// Constrain the result set to the subset bounded by two keys.
void TDewTable::SetRange(const pcBYTE pKey1, const pcBYTE pKey2, const bool bKey1Incl, const bool bKey2Incl, const uint iFields1, const uint iLen1, const uint iFields2, const uint iLen2)
{
	TDewException::CheckResult(TIdApi32::DbiSetRange(fHCursor, TRUE, (uint16)iFields1, (uint16)iLen1, (pBYTE) pKey1, bKey1Incl, (uint16)iFields2, (uint16)iLen2, (pBYTE) pKey2, bKey2Incl));
}
// Reset cursor to beginning.
TDewRecord& TDewTable::SetToBegin()
{
	TDewException::CheckResult(TIdApi32::DbiSetToBegin(fHCursor));
	return *fRecord;
}
// Position to a book-mark.
TDewRecord& TDewTable::SetToBookMark(const pcBYTE pBookMark)
{
	TDewException::CheckResult(TIdApi32::DbiSetToBookMark(fHCursor, (pBYTE) pBookMark));
	return *fRecord;
}
// Set cursor to another cursor position.
TDewRecord& TDewTable::SetToTable(const TDewTable& table)
{
	TDewException::CheckResult(TIdApi32::DbiSetToCursor(fHCursor, table.fHCursor));
	return *fRecord;
}
// Reset cursor to ending.
TDewRecord& TDewTable::SetToEnd()
{
	TDewException::CheckResult(TIdApi32::DbiSetToEnd(fHCursor));
	return *fRecord;
}
// Set key condition.
TDewRecord& TDewTable::SetToKey(const TDewRecord& record, const DBISearchCond eSearchCond, const uint iFields, const uint iLen)
{
	TDewException::CheckResult(TIdApi32::DbiSetToKey(fHCursor, eSearchCond, FALSE, (uint16)iFields, (uint16)iLen, record.fBuffer));
	return *fRecord;
}
// Position to physical record number.
TDewRecord& TDewTable::SetToRecordNo(const uint32 iRecNo)
{
	TDewException::CheckResult(TIdApi32::DbiSetToRecordNo(fHCursor, iRecNo));
	return *fRecord;
}
// Position to a logical record.
TDewRecord& TDewTable::SetToSeqNo(const uint32 iSeqNo)
{
	TDewException::CheckResult(TIdApi32::DbiSetToSeqNo(fHCursor, iSeqNo));
	return *fRecord;
}
// Sort the table.
void TDewTable::Sort(const uint iSortFields, const pcUINT16 piFieldNum, const pcCHAR pszSortedName, const pcBOOL pbCaseInsensitive, const pcSORTOrder pSortOrder, pfSORTCompFn* ppfSortFn, const bool bRemoveDups)
{
	TDewException::CheckResult(TIdApi32::DbiSortTable(NULL, NULL, NULL, fHCursor, (pCHAR) pszSortedName, NULL, NULL, (uint16)iSortFields, (pUINT16) piFieldNum, (pBOOL) pbCaseInsensitive, (pSORTOrder) pSortOrder, ppfSortFn, bRemoveDups, NULL, NULL));
}
// Change index order of access.
void TDewTable::SwitchToIndex(const pcCHAR pszIndexName, const bool bCurrRec, const uint iIndexId, const pcCHAR pszTagName)
{
	TDewException::CheckResult(TIdApi32::DbiSwitchToIndex(&fHCursor, (pCHAR) pszIndexName, (pCHAR) pszTagName, (uint16)iIndexId, bCurrRec));
	GetProperties();
}
// Undelete the current record.
TDewRecord& TDewTable::UndeleteRecord()
{
	TDewException::CheckResult(TIdApi32::DbiUndeleteRecord(fHCursor));
	return *fRecord;
}
// Unlink detail from master.
void TDewTable::UnlinkDetail()
{
	TDewException::CheckResult(TIdApi32::DbiUnlinkDetail(fHCursor));
	GetProperties();
}
// Write a block of records.
uint32 TDewTable::WriteBlock(const uint32 iRecords, const pcBYTE pBuf)
{
	uint32 records = iRecords;
	TDewException::CheckResult(TIdApi32::DbiWriteBlock(fHCursor, &records, (pBYTE) pBuf));
	return records;
}
//------------------------------------------------------------------------------------------------
// class TDewDatabase
// ~~~~~ ~~~~~~~~~~~~
//
// Constructor.
TDewDatabase::TDewDatabase(const pcCHAR pszDbName, const pcCHAR pszDbType, const TDBOpenMode eOpenMode, const DBIShareMode eShareMode, const pcCHAR pszPassword, const uint iOptFlds, const pcFLDDesc pOptFldDesc, const pcBYTE pOptParams)
: 
fHDatabase(NULL)
{
	Open(pszDbName, pszDbType, eOpenMode, eShareMode, pszPassword, iOptFlds, pOptFldDesc, pOptParams);
}
// Destructor.
TDewDatabase::~TDewDatabase()
{
	if (IsOpen())
		Close();
}
// Get a persistent lock.
void TDewDatabase::AcqPersistTableLock(const pcCHAR pszTableName, const pcCHAR pszDriverType)
{
	TDewException::CheckResult(TIdApi32::DbiAcqPersistTableLock(fHDatabase, (pCHAR) pszTableName, (pCHAR) pszDriverType));
}
// Add a new index.
void TDewDatabase::AddIndex(const pcCHAR pszTableName, const pcCHAR pszDriverType, const IDXDesc& idxDesc, const pcCHAR pszKeyviolName)
{
	TDewException::CheckResult(TIdApi32::DbiAddIndex(fHDatabase, NULL, (pCHAR) pszTableName, (pCHAR) pszDriverType, (pIDXDesc) &idxDesc, (pCHAR) pszKeyviolName));
}
// Close database.
void TDewDatabase::Close()
{
	TDewException::CheckResult(TIdApi32::DbiCloseDatabase(&fHDatabase));
	fHDatabase = NULL;
}
// Copy one table to another.
void TDewDatabase::CopyTable(const pcCHAR pszSrcTblName, const pcCHAR pszDestTblName, const pcCHAR pszSrcDriverType, const bool bOverWrite)
{
	TDewException::CheckResult(TIdApi32::DbiCopyTable(fHDatabase, bOverWrite, (pCHAR) pszSrcTblName, (pCHAR) pszSrcDriverType, (pCHAR) pszDestTblName));
}
// Create a new table.
void TDewDatabase::CreateTable(const CRTblDesc& crTblDesc, const bool bOverWrite)
{
	TDewException::CheckResult(TIdApi32::DbiCreateTable(fHDatabase, bOverWrite, (pCRTblDesc) &crTblDesc));
}
// Delete index.
void TDewDatabase::DeleteIndex(const pcCHAR pszTableName, const pcCHAR pszIndexName, const uint iIndexNum, const pcCHAR pszDriverType, const pcCHAR pszIndexTagName)
{
	TDewException::CheckResult(TIdApi32::DbiDeleteIndex(fHDatabase, NULL, (pCHAR) pszTableName, (pCHAR) pszDriverType, (pCHAR) pszIndexName, (pCHAR) pszIndexTagName, (uint16)iIndexNum));
}
// Delete a table.
void TDewDatabase::DeleteTable(const pcCHAR pszTableName, const pcCHAR pszDriverType)
{
	TDewException::CheckResult(TIdApi32::DbiDeleteTable(fHDatabase, (pCHAR) pszTableName, (pCHAR) pszDriverType));
}
// Get the database directory.
owl_string TDewDatabase::Directory(const bool bDefault) const
{
	DBIPATH directory;
	TDewException::CheckResult(TIdApi32::DbiGetDirectory(fHDatabase, bDefault, directory));
	return owl_string(directory);
}
// Restructure a table.
void TDewDatabase::DoRestructure(const uint iTblDescCount, const pcCRTblDesc pTblDesc, const pcCHAR pszSaveAs, const pcCHAR pszKeyviolName, const pcCHAR pszProblemsName, const bool bAnalyzeOnly)
{
	TDewException::CheckResult(TIdApi32::DbiDoRestructure(fHDatabase, (uint16)iTblDescCount, (pCRTblDesc) pTblDesc, (pCHAR) pszSaveAs, (pCHAR) pszKeyviolName, (pCHAR) pszProblemsName, bAnalyzeOnly));
}
// Delete all records.
void TDewDatabase::EmptyTable(const pcCHAR pszTblName, const pcCHAR pszDriverType)
{
	TDewException::CheckResult(TIdApi32::DbiEmptyTable(fHDatabase, NULL, (pCHAR) pszTblName, (pCHAR) pszDriverType));
}
// Open a cursor on "family".
hDBICur TDewDatabase::FamilyList(const pcCHAR pszTableName, const pcCHAR pszDriverType)
{
	hDBICur hCursor;
	TDewException::CheckResult(TIdApi32::DbiOpenFamilyList(fHDatabase, (pCHAR) pszTableName, (pCHAR) pszDriverType, &hCursor));
	return hCursor;
}
// Return "fields" for a table.
hDBICur TDewDatabase::FieldList(const pcCHAR pszTableName, const pcCHAR pszDriverType, const bool bPhyTypes)
{
	hDBICur hCursor;
	TDewException::CheckResult(TIdApi32::DbiOpenFieldList(fHDatabase, (pCHAR) pszTableName, (pCHAR) pszDriverType, bPhyTypes, &hCursor));
	return hCursor;
}
// Open a cursor on "files".
hDBICur TDewDatabase::FileList(const pcCHAR pszWild)
{
	hDBICur hCursor;
	TDewException::CheckResult(TIdApi32::DbiOpenFileList(fHDatabase, (pCHAR) pszWild, &hCursor));
	return hCursor;
}
// Form full name.
owl_string TDewDatabase::FullName(const pcCHAR pszTableName, const pcCHAR pszDriverType) const
{
	char fullName[DBIMAXTBLNAMELEN+1];
	TDewException::CheckResult(TIdApi32::DbiFormFullName(fHDatabase, (pCHAR) pszTableName, (pCHAR) pszDriverType, fullName));
	return owl_string(fullName);
}
// Return "indexes" for a table.
hDBICur TDewDatabase::IndexList(const pcCHAR pszTableName, const pcCHAR pszDriverType)
{
	hDBICur hCursor;
	TDewException::CheckResult(TIdApi32::DbiOpenIndexList(fHDatabase, (pCHAR) pszTableName, (pCHAR) pszDriverType, &hCursor));
	return hCursor;
}
// Create a temporary table.
TDewTable TDewDatabase::InMemTable(const pcCHAR pszName, const uint iFields, const pcFLDDesc pFldDesc)
{
	hDBICur hCursor;
	TDewException::CheckResult(TIdApi32::DbiCreateInMemTable(fHDatabase, (pCHAR) pszName, (uint16)iFields, (pFLDDesc) pFldDesc, &hCursor));
	return TDewTable(hCursor);
}
// Open a database.
void TDewDatabase::Open(const pcCHAR pszDbName, const pcCHAR pszDbType, const TDBOpenMode eOpenMode, const DBIShareMode eShareMode, const pcCHAR pszPassword, const uint iOptFlds, const pcFLDDesc pOptFldDesc, const pcBYTE pOptParams)
{
	if (IsOpen())
		Close();
	TDewException::CheckResult(TIdApi32::DbiOpenDatabase((pCHAR) pszDbName, (pCHAR) pszDbType, (DBIOpenMode)eOpenMode, eShareMode, (pCHAR) pszPassword, (uint16)iOptFlds, (pFLDDesc) pOptFldDesc, (pBYTE) pOptParams, &fHDatabase));
}
// Pack a table.
void TDewDatabase::PackTable(const pcCHAR pszTblName, const pcCHAR pszDriverType, const bool bRegenIdxs)
{
	TDewException::CheckResult(TIdApi32::DbiPackTable(fHDatabase, NULL, (pCHAR) pszTblName, (pCHAR) pszDriverType, bRegenIdxs));
}
// Execute a query.
TDewTable TDewDatabase::Query(const pcCHAR pszQuery, const DBIQryLang eQryLang)
{
	hDBICur hCursor;
	TDewException::CheckResult(TIdApi32::DbiQExecDirect(fHDatabase, eQryLang, (pCHAR) pszQuery, &hCursor));
	return TDewTable(hCursor);
}
// Regenerate an index.
void TDewDatabase::RegenIndex(const pcCHAR pszTableName, const pcCHAR pszIndexName, const uint iIdxNum, const pcCHAR pszDriverType, const pcCHAR pszIndexTagName)
{
	TDewException::CheckResult(TIdApi32::DbiRegenIndex(fHDatabase, NULL, (pCHAR) pszTableName, (pCHAR) pszDriverType, (pCHAR) pszIndexName, (pCHAR) pszIndexTagName, (uint16)iIdxNum));
}
// Release a persistent lock.
void TDewDatabase::RelPersistTableLock(const pcCHAR pszTableName, const pcCHAR pszTableType)
{
	TDewException::CheckResult(TIdApi32::DbiRelPersistTableLock(fHDatabase, (pCHAR) pszTableName, (pCHAR) pszTableType));
}
// Rename table and family.
void TDewDatabase::RenameTable(const pcCHAR pszOldName, const pcCHAR pszNewName, const pcCHAR pszDriverType)
{
	TDewException::CheckResult(TIdApi32::DbiRenameTable(fHDatabase, (pCHAR) pszOldName, (pCHAR) pszDriverType, (pCHAR) pszNewName));
}
// Return integrity checks.
hDBICur TDewDatabase::RintList(const pcCHAR pszTableName, const pcCHAR pszDriverType)
{
	hDBICur hCursor;
	TDewException::CheckResult(TIdApi32::DbiOpenRintList(fHDatabase, (pCHAR) pszTableName, (pCHAR) pszDriverType, &hCursor));
	return hCursor;
}
// Return security descriptions.
hDBICur TDewDatabase::SecurityList(const pcCHAR pszTableName, const pcCHAR pszDriverType)
{
	hDBICur hCursor;
	TDewException::CheckResult(TIdApi32::DbiOpenSecurityList(fHDatabase, (pCHAR) pszTableName, (pCHAR) pszDriverType, &hCursor));
	return hCursor;
}
// Set the database directory.
void TDewDatabase::SetDirectory(const pcCHAR pszDir)
{
	TDewException::CheckResult(TIdApi32::DbiSetDirectory(fHDatabase, (pCHAR) pszDir));
}
// Sort a table.
void TDewDatabase::Sort(const pcCHAR pszTableName, const uint iSortFields, const pcUINT16 piFieldNum, const pcCHAR pszSortedName, const pcCHAR pszDriverType, const pcBOOL pbCaseInsensitive, const pcSORTOrder pSortOrder, pfSORTCompFn* ppfSortFn, const bool bRemoveDups)
{
	TDewException::CheckResult(TIdApi32::DbiSortTable(fHDatabase, (pCHAR) pszTableName, (pCHAR) pszDriverType, NULL, (pCHAR) pszSortedName, NULL, NULL, (uint16)iSortFields, (pUINT16) piFieldNum, (pBOOL) pbCaseInsensitive, (pSORTOrder) pSortOrder, ppfSortFn, bRemoveDups, NULL, NULL));
}
// Open a cursor on "tables".
hDBICur TDewDatabase::TableList(const bool bExtended, const bool bSystem, const pcCHAR pszWild)
{
	hDBICur hCursor;
	TDewException::CheckResult(TIdApi32::DbiOpenTableList(fHDatabase, bExtended, bSystem, (pCHAR) pszWild, &hCursor));
	return hCursor;
}
// Get local cursor count.
uint TDewDatabase::TableOpenCount(const pcCHAR pszTableName, const pcCHAR pszDriverType) const
{
	uint16 iOpenCount;
	TDewException::CheckResult(TIdApi32::DbiGetTableOpenCount(fHDatabase, (pCHAR) pszTableName, (pCHAR) pszDriverType, &iOpenCount));
	return iOpenCount;
}
// Create temporary table.
TDewTable TDewDatabase::TempTable(const CRTblDesc& crTblDesc)
{
	hDBICur hCursor;
	TDewException::CheckResult(TIdApi32::DbiCreateTempTable(fHDatabase, (pCRTblDesc) &crTblDesc, &hCursor));
	return TDewTable(hCursor);
}
// Return "checks" for a table.
hDBICur TDewDatabase::VchkList(const pcCHAR pszTableName, const pcCHAR pszDriverType)
{
	hDBICur hCursor;
	TDewException::CheckResult(TIdApi32::DbiOpenVchkList(fHDatabase, (pCHAR) pszTableName, (pCHAR) pszDriverType, &hCursor));
	return hCursor;
}
//------------------------------------------------------------------------------------------------
// class TDewEngine
// ~~~~~ ~~~~~~~~~~
//
TDewEngine::TDewEngine(const pcDBIEnv pEnv)
{
	Open(pEnv);
}
// Destructor.
TDewEngine::~TDewEngine()
{
	Close();
}
// Add a password to current session.
void TDewEngine::AddPassword(const pcCHAR pszPassword)
{
	TDewException::CheckResult(TIdApi32::DbiAddPassword((pCHAR) pszPassword));
}
// Open a cursor on "config".
hDBICur TDewEngine::CfgInfoList(const pcCHAR pszCfgPath, const TDBOpenMode eOpenMode, const CFGMode eConfigMode) const
{
	hDBICur hCursor;
	TDewException::CheckResult(TIdApi32::DbiOpenCfgInfoList(NULL, (DBIOpenMode)eOpenMode, eConfigMode, (pCHAR) pszCfgPath, &hCursor));
	return hCursor;
}
// Check refresh for session.
void TDewEngine::CheckRefresh() const
{
	TDewException::CheckResult(TIdApi32::DbiCheckRefresh());
}
// Close engine.
void TDewEngine::Close()
{
	TDewException::CheckResult(TIdApi32::DbiExit());
}
// Get a list of registered databases.
hDBICur TDewEngine::DatabaseList() const
{
	hDBICur hCursor;
	TDewException::CheckResult(TIdApi32::DbiOpenDatabaseList(&hCursor));
	return hCursor;
}
// Get current date format.
const FMTDate& TDewEngine::DateFormat() const
{
	return fFmtDate;
}
// Set debug layer options.
void TDewEngine::DebugLayerOptions(const uint iOption, const pcCHAR pszDebugFile)
{
	TDewException::CheckResult(TIdApi32::DbiDebugLayerOptions((uint16)iOption, (pCHAR) pszDebugFile));
}
// Get a list of driver names.
hDBICur TDewEngine::DriverList() const
{
	hDBICur hCursor;
	TDewException::CheckResult(TIdApi32::DbiOpenDriverList(&hCursor));
	return hCursor;
}
// Drop a password from current session.
void TDewEngine::DropPassword(const pcCHAR pszPassword)
{
	TDewException::CheckResult(TIdApi32::DbiDropPassword((pCHAR) pszPassword));
}
// Get a list of field types.
hDBICur TDewEngine::FieldTypesList(const pcCHAR pszDriverType, const pcCHAR pszTblType) const
{
	hDBICur hCursor;
	TDewException::CheckResult(TIdApi32::DbiOpenFieldTypesList((pCHAR) pszDriverType, (pCHAR) pszTblType, &hCursor));
	return hCursor;
}
// Get callback function.
void TDewEngine::GetCallBack(const CBType ecbType, uint32& iClientData, uint16& iCbBufLen, pVOID& pCbBuf, pfDBICallBack& pfCb) const
{
	TDewException::CheckResult(TIdApi32::DbiGetCallBack(NULL, ecbType, &iClientData, &iCbBufLen, &pCbBuf, &pfCb));
}
// Get client info.
void TDewEngine::GetClientInfo(CLIENTInfo& clientInfo) const
{
	TDewException::CheckResult(TIdApi32::DbiGetClientInfo(&clientInfo));
}
// Get a description of a logical db.
void TDewEngine::GetDatabaseDesc(const pcCHAR pszName, DBDesc& dbDesc) const
{
	TDewException::CheckResult(TIdApi32::DbiGetDatabaseDesc((pCHAR) pszName, &dbDesc));
}
// Get description for a given type.
void TDewEngine::GetDriverDesc(const pcCHAR pszDrvType, DRVType& drvType) const
{
	TDewException::CheckResult(TIdApi32::DbiGetDriverDesc((pCHAR) pszDrvType, &drvType));
}
// Get description of a field type.
void TDewEngine::GetFieldTypeDesc(const pcCHAR pszDriverType, const pcCHAR pszTableType, const pcCHAR pszFldType, FLDType& fldType) const
{
	TDewException::CheckResult(TIdApi32::DbiGetFieldTypeDesc((pCHAR) pszDriverType, (pCHAR) pszTableType, (pCHAR) pszFldType, &fldType));
}
// Get description of given idx type.
void TDewEngine::GetIndexTypeDesc(const pcCHAR pszDriverType, const pcCHAR pszIdxType, IDXType& idxType) const
{
	TDewException::CheckResult(TIdApi32::DbiGetIndexTypeDesc((pCHAR) pszDriverType, (pCHAR) pszIdxType, &idxType));
}
// Get current session info.
void TDewEngine::GetSesInfo(SESInfo& sesInfo) const
{
	TDewException::CheckResult(TIdApi32::DbiGetSesInfo(&sesInfo));
}
// Get system configuration.
void TDewEngine::GetSysConfig(SYSConfig& sysConfig) const
{
	TDewException::CheckResult(TIdApi32::DbiGetSysConfig(&sysConfig));
}
// Get system status info.
void TDewEngine::GetSysInfo(SYSInfo& sysInfo) const
{
	TDewException::CheckResult(TIdApi32::DbiGetSysInfo(&sysInfo));
}
// Get system version info.
void TDewEngine::GetSysVersion(SYSVersion& sysVersion) const
{
	TDewException::CheckResult(TIdApi32::DbiGetSysVersion(&sysVersion));
}
// Get table capabilities.
void TDewEngine::GetTableTypeDesc(const pcCHAR pszDrvType, const pcCHAR pszTableType, TBLType& tblType) const
{
	TDewException::CheckResult(TIdApi32::DbiGetTableTypeDesc((pCHAR) pszDrvType, (pCHAR) pszTableType, &tblType));
}
// Get list of index types.
hDBICur TDewEngine::IndexTypesList(const pcCHAR pszDriverType) const
{
	hDBICur hCursor;
	TDewException::CheckResult(TIdApi32::DbiOpenIndexTypesList((pCHAR) pszDriverType, &hCursor));
	return hCursor;
}
// Get a list of lang drivers.
hDBICur TDewEngine::LdList() const
{
	hDBICur hCursor;
	TDewException::CheckResult(TIdApi32::DbiOpenLdList(&hCursor));
	return hCursor;
}
// Get lang driver name.
owl_string TDewEngine::LdName(const pcCHAR pszDriver, const pcCHAR pObjName) const
{
	DBINAME name;
	TDewException::CheckResult(TIdApi32::DbiGetLdName((pCHAR) pszDriver, (pCHAR) pObjName, name));
	return owl_string(name);
}
// Load a given driver.
void TDewEngine::LoadDriver(const pcCHAR pszDrvType)
{
	TDewException::CheckResult(TIdApi32::DbiLoadDriver((pCHAR) pszDrvType));
}
// Get network username.
owl_string TDewEngine::NetUserName() const
{
	DBIUSERNAME netUserName;
	TDewException::CheckResult(TIdApi32::DbiGetNetUserName(netUserName));
	return owl_string(netUserName);
}
// Get current number format.
const FMTNumber& TDewEngine::NumberFormat() const
{
	return fFmtNumber;
}
// Open the engine.
void TDewEngine::Open(const pcDBIEnv pEnv)
{
	TDewException::CheckResult(TIdApi32::DbiInit((pDBIEnv) pEnv));
	TDewException::CheckResult(TIdApi32::DbiGetDateFormat(&fFmtDate));
	TDewException::CheckResult(TIdApi32::DbiGetTimeFormat(&fFmtTime));
	TDewException::CheckResult(TIdApi32::DbiGetNumberFormat(&fFmtNumber));
}
// Register callback function.
void TDewEngine::RegisterCallBack(const CBType ecbType, const uint32 iClientData, const uint iCbBufLen, const pVOID pCbBuf, const pfDBICallBack pfCb)
{
	TDewException::CheckResult(TIdApi32::DbiRegisterCallBack(NULL, ecbType, iClientData, (uint16)iCbBufLen, pCbBuf, pfCb));
}
// Set the current session.
void TDewEngine::SetCurrSession(TDewSession& session)
{
	TDewException::CheckResult(TIdApi32::DbiSetCurrSession(session.GetHandle()));
}
// Set current date format.
void TDewEngine::SetDateFormat(const FMTDate& format)
{
	fFmtDate = format;
	TDewException::CheckResult(TIdApi32::DbiSetDateFormat(&fFmtDate));
}
// Set lock wait time.
void TDewEngine::SetLockRetry(const INT16 iWait)
{
	TDewException::CheckResult(TIdApi32::DbiSetLockRetry(iWait));
}
// Set current number format.
void TDewEngine::SetNumberFormat(const FMTNumber& format)
{
	fFmtNumber = format;
	TDewException::CheckResult(TIdApi32::DbiSetNumberFormat(&fFmtNumber));
}
// Set private directory for session.
void TDewEngine::SetPrivateDir(const pcCHAR pszDir)
{
	TDewException::CheckResult(TIdApi32::DbiSetPrivateDir((pCHAR) pszDir));
}
// Set current time format.
void TDewEngine::SetTimeFormat(const FMTTime& format)
{
	fFmtTime = format;
	TDewException::CheckResult(TIdApi32::DbiSetTimeFormat(&fFmtTime));
}
// Get a list of table types.
hDBICur TDewEngine::TableTypesList(const pcCHAR pszDriverType)
{
	hDBICur hCursor;
	TDewException::CheckResult(TIdApi32::DbiOpenTableTypesList((pCHAR) pszDriverType, &hCursor));
	return hCursor;
}
// Get current time format.
const FMTTime& TDewEngine::TimeFormat() const
{
	return fFmtTime;
}
// Use idle time.
void TDewEngine::UseIdleTime()
{
	TDewException::CheckResult(TIdApi32::DbiUseIdleTime());
}
// Get a list of users loggedin.
hDBICur TDewEngine::UserList() const
{
	hDBICur hCursor;
	TDewException::CheckResult(TIdApi32::DbiOpenUserList(&hCursor));
	return hCursor;
}
//------------------------------------------------------------------------------------------------
// class TDewException
// ~~~~~ ~~~~~~~~~~
//
// Constructor.
TDewException::TDewException(const DBIResult result, const owl_string& extraInfo)
: 
TXOwl(extraInfo),
fResult(result)
{
}
// Answer the category.
int TDewException::Category() const
{
	return int(ErrCat(fResult));
}
// Answer the code.
int TDewException::Code() const
{
	return int(ErrCode(fResult));
}
// Answer the context.
owl_string TDewException::Context(const int eContext) const
{
	char context[DBIMAXMSGLEN+1];
	TIdApi32::DbiGetErrorContext((int16)eContext, context);
	return owl_string(context);
}
// Answer the error entry.
owl_string TDewException::Entry(const uint uEntry, uint32& ulNativeError) const
{
	char entry[DBIMAXMSGLEN+1];
	TIdApi32::DbiGetErrorEntry((uint16)uEntry, &ulNativeError, entry);
	return owl_string(entry);
}
// Answer the extra information.
owl_string TDewException::ExtraInfo() const
{
	return XWHY_STR;
}
// Answer the error information.
void TDewException::GetInfo(DBIErrInfo& errInfo, const bool bFull) const
{
	TIdApi32::DbiGetErrorInfo(bFull, &errInfo);
}
// Answer the message.
owl_string TDewException::Message() const
{
	char message[DBIMAXMSGLEN+1];
	TIdApi32::DbiGetErrorString(fResult, message);
	return owl_string(message);
}
// Answer the message, with full information.
owl_string TDewException::FullMessage() const
{
#if defined(BI_STD_NEWSSTREAM)
	_tostrstream message;
#else
	_TCHAR buffer[1000];
	_tostrstream message(buffer, COUNTOF(buffer));
#endif  

	//    message << "BDE error (category = " << Category() << ", code = " << Code() << ")";
	//    message << endl << "detected by DEW in line " << __throwLineNumber << " of file " << __throwFileName;

	DBIErrInfo errInfo;
	memset(&errInfo, 0, sizeof(errInfo));
	GetInfo(errInfo, TRUE);

	if(*errInfo.szErrCode)
		message << endl << "code = " << errInfo.szErrCode;
	else
		message << endl << Message();
	if (*errInfo.szContext1)
		message << endl << "context #1 = " << errInfo.szContext1;
	if (*errInfo.szContext2)
		message << endl << "context #2 = " << errInfo.szContext2;
	if (*errInfo.szContext3)
		message << endl << "context #3 = " << errInfo.szContext3;
	if (*errInfo.szContext4)
		message << endl << "context #4 = " << errInfo.szContext4;
	if (ExtraInfo() != "")
		message << endl << "extra = " << ExtraInfo();

	message << ends;

#if defined(BI_STD_NEWSSTREAM)
	return owl_string(message.str());
#else
	return owl_string(buffer);
#endif
}
// Answer the result.
DBIResult TDewException::Result() const
{
	return fResult;
}
// Throw an exception if an error has occurred.
void TDewException::CheckResult(const DBIResult result, const owl_string& extraInfo)
{
	switch (ErrCat(result)) {
	case ERRCAT_NONE:
		break;
	case ERRCAT_SYSTEM:
		throw TDewSystemException(result, extraInfo);
	case ERRCAT_NOTFOUND:
		throw TDewNotFoundException(result, extraInfo);
	case ERRCAT_DATACORRUPT:
		throw TDewDataCorruptException(result, extraInfo);
	case ERRCAT_IO:
		throw TDewIoException(result, extraInfo);
	case ERRCAT_LIMIT:
		throw TDewLimitException(result, extraInfo);
	case ERRCAT_INTEGRITY:
		throw TDewIntegrityException(result, extraInfo);
	case ERRCAT_INVALIDREQ:
		throw TDewInvalidReqException(result, extraInfo);
	case ERRCAT_LOCKCONFLICT:
		throw TDewLockConflictException(result, extraInfo);
	case ERRCAT_SECURITY:
		throw TDewSecurityException(result, extraInfo);
	case ERRCAT_INVALIDCONTEXT:
		throw TDewInvalidContextException(result, extraInfo);
	case ERRCAT_OS:
		throw TDewOsException(result, extraInfo);
	case ERRCAT_NETWORK:
		throw TDewNetworkException(result, extraInfo);
	case ERRCAT_OPTPARAM:
		throw TDewOptParamException(result, extraInfo);
	case ERRCAT_QUERY:
		throw TDewQueryException(result, extraInfo);
	case ERRCAT_VERSION:
		throw TDewVersionException(result, extraInfo);
	case ERRCAT_CAPABILITY:
		throw TDewCapabilityException(result, extraInfo);
	case ERRCAT_CONFIG:
		throw TDewConfigException(result, extraInfo);
	case ERRCAT_WARNING:
		throw TDewWarningException(result, extraInfo);
	case ERRCAT_OTHER:
		throw TDewOtherException(result, extraInfo);
	case ERRCAT_COMPATIBILITY:
		throw TDewCompatibilityException(result, extraInfo);
	case ERRCAT_REPOSITORY:
		throw TDewRepositoryException(result, extraInfo);
	case ERRCAT_DRIVER:
		throw TDewDriverException(result, extraInfo);
	case ERRCAT_RC:
		throw TDewInternalException(result, extraInfo);
	default:
		throw TDewException(result, extraInfo);
	}
}
//------------------------------------------------------------------------------------------------
// class TDewFilter
// ~~~~~ ~~~~~~~~~~
//
TDewFilter::TDewFilter()
: 
fHCursor(0),fHFilter(0)
{
}
//
TDewFilter::TDewFilter(const TDewTable& table)
: 
fHCursor(table.GetHandle()),fHFilter(0)
{
}
//
TDewFilter::TDewFilter(const TDewTable& table, const pCANExpr pCanExpr)
: 
fHCursor(table.GetHandle()),fHFilter(0)
{
	Open(pCanExpr);
}
//
TDewFilter::TDewFilter(const TDewFilter& filter)
: 
fHCursor(0),fHFilter(0)
{
	Clone(filter);
}
// Destructor.
TDewFilter::~TDewFilter()
{
	if (IsOpen())
		Close();
}
// Assignment operator.
TDewFilter& TDewFilter::operator=(const TDewFilter& filter)
{
	if (IsOpen())
		Close();
	Clone(filter);
	return *this;
}
// Activate a filter.
void TDewFilter::Activate()
{
	TDewException::CheckResult(TIdApi32::DbiActivateFilter(fHCursor, fHFilter));
}
// Clone a filter.
void TDewFilter::Clone(const TDewFilter& filter)
{
	fHCursor = filter.fHCursor;
	FILTERInfo info;
	filter.GetInfo(info);
	Open((pCANExpr) info.pCanExpr);
}
// Drop a filter.
void TDewFilter::Close()
{
	TDewException::CheckResult(TIdApi32::DbiDropFilter(fHCursor, fHFilter));
	fHFilter = NULL;
}
// Deactivate a filter.
void TDewFilter::Deactivate()
{
	TDewException::CheckResult(TIdApi32::DbiDeactivateFilter(fHCursor, fHFilter));
}
// Get filter information.
void TDewFilter::GetInfo(FILTERInfo& filterInfo, const uint iFilterId, const uint iFilterSeqNo) const
{
	TDewException::CheckResult(TIdApi32::DbiGetFilterInfo(fHCursor, fHFilter, (uint16)iFilterId, (uint16)iFilterSeqNo, &filterInfo));
}
// Add a filter to the cursor.
void TDewFilter::Open(const TDewTable& table, const pCANExpr pCanExpr)
{
	if (IsOpen())
		Close();

	fHCursor = table.GetHandle();
	TDewException::CheckResult(TIdApi32::DbiAddFilter(fHCursor, 0, 1, FALSE, pCanExpr, NULL, &fHFilter));
}
//
void TDewFilter::Open(const pCANExpr pCanExpr)
{
	if (IsOpen())
		Close();
	TDewException::CheckResult(TIdApi32::DbiAddFilter(fHCursor, 0, 1, FALSE, pCanExpr, NULL, &fHFilter));
}
//
//------------------------------------------------------------------------------------------------
// class TDewRecord
// ~~~~~ ~~~~~~~~~~
//
// Constructor.
TDewRecord::TDewRecord(TDewTable& cursor)
: 
fTable(cursor),fBuffer(0)
{
	fBuffer = new BYTE[fTable.fCursorProps.iRecBufSize];
}
// Destructor.
TDewRecord::~TDewRecord()
{
	delete [] fBuffer;
}
// Get the size of a blob.
uint32 TDewRecord::GetBlobSize(const uint iField) const
{
	CheckFieldNumber(iField);
	uint32 iSize;
	TDewException::CheckResult(TIdApi32::DbiGetBlobSize(fTable.fHCursor, fBuffer, (uint16)iField, &iSize));
	return iSize;
}
//
TDBBlob* TDewRecord::GetBlob(const uint fldId, const TDBOpenMode mode)
{
	return new TDewBlob(*this, fldId, mode);
}
// Answer the record's buffer.
uint8* TDewRecord::GetBuffer()
{
	return fBuffer;
}
//
uint8* TDewRecord::GetBuffer() const
{
	return fBuffer;
}
// Check the validity of the field number.
void TDewRecord::CheckFieldNumber(const uint iField) const
{
	if ((int16)iField > fTable.fCursorProps.iFields)
		TDewException::CheckResult(DBIERR_OUTOFRANGE, "DEW:TRecord::CheckFieldNumber - Field number out of range.");
}
// Answer the record's fCursor.
TDewTable& TDewRecord::GetTable()
{
	return fTable;
}
//
const TDewTable& TDewRecord::GetTable() const
{
	return fTable;
}
// Find the field name.
owl_string TDewRecord::GetFieldName(const uint iField) const
{
	CheckFieldNumber(iField);
	if (fTable.fFieldDesc == 0)
		TDewException::CheckResult(DBIERR_NOTSUPPORTED, "TDewRecord::FieldName - No field descriptors available.");
	return owl_string(fTable.fFieldDesc[iField - 1].szName);
}
// Find the field number.
uint TDewRecord::GetFieldNumber(const owl_string& fieldName) const
{
	if (fTable.fFieldDesc == 0)
		TDewException::CheckResult(DBIERR_NOTSUPPORTED, "TDewRecord::GetFieldNumber - No field descriptors available.");

	for (uint i = 0; i < (uint)fTable.fCursorProps.iFields; ++i)
		if (fieldName == fTable.fFieldDesc[i].szName)
			return i + 1;

	TDewException::CheckResult(DBIERR_INVALIDFIELDNAME, "TDewRecord::GetFieldNumber - Field name not found.");

	return (uint)-1;
}
// Find the field type name.
owl_string TDewRecord::GetFieldTypeName(const uint iField) const
{
	if (fTable.fFieldDesc == 0)
		TDewException::CheckResult(DBIERR_NOTSUPPORTED, "TDewRecord::GetFieldTypeName - No field descriptors available.");

	switch (fTable.fFieldDesc[iField - 1].iFldType) {
	case fldZSTRING:
		return "ZSTRING";
	case fldDATE:
		return "DATE";
	case fldBLOB:
		return "BLOB";
	case fldBOOL:
		return "bool";
	case fldINT16:
		return "int16";
	case fldINT32:
		return "INT32";
	case fldFLOAT:
		return "FLOAT";
	case fldBCD:
		return "BCD";
	case fldBYTES:
		return "BYTES";
	case fldTIME:
		return "TIME";
	case fldTIMESTAMP:
		return "TIMESTAMP";
	case fldUINT16:
		return "UINT16";
	case fldUINT32:
		return "UINT32";
	case fldFLOATIEEE:
		return "FLOATIEEE";
	case fldVARBYTES:
		return "VARBYTES";
	case fldLOCKINFO:
		return "LOCKINFO";
	default:
		return "UNKNOWN";
	}
}
// Check for a blank.
//
bool TDewRecord::IsBlank(const uint iField) const
{
	CheckFieldNumber(iField);

	BOOL bBlank;
	TDewException::CheckResult(TIdApi32::DbiGetField(fTable.fHCursor, (uint16)iField, fBuffer, NULL, &bBlank));
	return bBlank == TRUE;
}
// Get a field as a owl_string.
//
bool TDewRecord::GetField(const uint iField, owl_string& answer) const
{
	CheckFieldNumber(iField);

	BOOL bBlank;
	int size = __max(fTable.fFieldDesc[iField - 1].iLen,1);
	TTmpBuffer<char> __ClnObj(size);
	char* dest = __ClnObj;
	TTmpBuffer<char> __ClnObj1(MAX_PATH);
	char* temp_oem = __ClnObj1;
	TTmpBuffer<char> __ClnObj2(MAX_PATH);
	char* temp_ansi = __ClnObj2;
	pLDDesc LdObj;
	pVOID pLdObj = &LdObj;

	TDewException::CheckResult(TIdApi32::DbiGetField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) dest, &bBlank));

	if (bBlank)
		temp_oem[0] = 0;
	else{
		switch (fTable.fFieldDesc[iField - 1].iFldType) {
	  case fldZSTRING:
	  case fldBYTES:
		  strcpy(temp_oem, dest);
		  break;
	  case fldDATE:
		  {
			  uint16 mo, da; int16 ye;
			  TDewException::CheckResult(TIdApi32::DbiDateDecode(*(DBIDATE*) dest, &mo, &da, &ye));
			  wsprintf(temp_oem, "%02u/%02u/%i", mo, da, ye);
		  }
		  break;
	  case fldBOOL:
		  strcpy(temp_oem, *(BOOL16*) dest != 0 ? "True" : "False");
		  break;
	  case fldINT16:
		  sprintf(temp_oem, "%i", *(int16*) dest);
		  break;
	  case fldINT32:
		  sprintf(temp_oem, "%li", *(INT32*) dest);
		  break;
	  case fldFLOAT:
		  sprintf(temp_oem, (fTable.fFieldDesc[iField - 1].iSubType == fldstMONEY ? "$%lg" : "%lg"), *(DFLOAT*) dest);
		  break;
	  case fldBCD:{
		  DFLOAT x;
		  TDewException::CheckResult(TIdApi32::DbiBcdToFloat((pFMTBcd) dest, &x));
		  sprintf(temp_oem, "%lg", x);
				  }
				  break;
	  case fldTIME:{
		  uint16 ho, mi, se;
		  TDewException::CheckResult(TIdApi32::DbiTimeDecode(*(TIME*) dest, &ho, &mi, &se));
		  sprintf(temp_oem, "%02u:%02u:%02i", ho, mi, se / 1000);
				   }
				   break;
	  case fldTIMESTAMP:{
		  DBIDATE date;
		  TIME time;
		  uint16 da, mo, ho, mi, se; int16 ye;
		  TDewException::CheckResult(TIdApi32::DbiTimeStampDecode(*(TIMESTAMP*) dest, &date, &time));
		  TDewException::CheckResult(TIdApi32::DbiDateDecode(date, &mo, &da, &ye));
		  TDewException::CheckResult(TIdApi32::DbiTimeDecode(time, &ho, &mi, &se));
		  sprintf(temp_oem, "%02u/%02u/%i %02u:%02u:%02d", da, mo, ye, ho, mi, se / 1000);
						}
						break;
	  case fldUINT16:
		  sprintf(temp_oem, "%u", *(uint16*) dest);
		  break;
	  case fldUINT32:
		  sprintf(temp_oem, "%lu", *(uint32*) dest);
		  break;
	  case fldFLOATIEEE:
		  sprintf(temp_oem, "%Lg", *(long double*) dest);
		  break;
	  default:
		  TDewException::CheckResult(DBIERR_NOTSUPPORTED, "TDewRecord::GetField - Field type " + GetFieldTypeName(iField) + " cannot be converted to C++ type string.");
		}
	}
	TDewException::CheckResult(TIdApi32::DbiGetLdObj(fTable.fHCursor, &pLdObj));
	TDewException::CheckResult(TIdApi32::DbiNativeToAnsi(pLdObj, temp_ansi, temp_oem, 0, &bBlank));

	if(bBlank)	
		answer = owl_string(temp_oem);
	else
		answer = owl_string(temp_ansi);
	return bBlank == TRUE;
}
// Get a field as a boolean.
bool TDewRecord::GetField(const uint iField, bool& answer) const
{
	CheckFieldNumber(iField);

	BOOL bBlank;
	int size = __max(fTable.fFieldDesc[iField - 1].iLen,1);
	TTmpBuffer<char> __ClnObj(size);
	char* dest = __ClnObj;

	TDewException::CheckResult(TIdApi32::DbiGetField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE)dest, &bBlank));

	if (bBlank)
		answer = false;
	else{
		switch (fTable.fFieldDesc[iField - 1].iFldType) {
	  case fldBOOL:
		  answer = (*(BOOL16*) dest) != 0;
		  break;
	  case fldINT16:
		  answer = (*(int16*) dest) != 0;
		  break;
	  case fldINT32:
		  answer = (*(INT32*) dest) != 0;
		  break;
	  case fldUINT16:
		  answer = (*(uint16*) dest) != 0;
		  break;
	  case fldUINT32:
		  answer = (*(uint32*) dest) != 0;
		  break;
	  default:
		  TDewException::CheckResult(DBIERR_NOTSUPPORTED, "TDewRecord::GetField - Field type " + GetFieldTypeName(iField) + " cannot be converted to C++ type bool.");
		}
	}
	return bBlank == TRUE;
}
// Get a field as an integer.
bool TDewRecord::GetField(const uint iField, int16& answer) const
{
	CheckFieldNumber(iField);

	BOOL bBlank;
	int size = __max(fTable.fFieldDesc[iField - 1].iLen,1);
	TTmpBuffer<char> __ClnObj(size);
	char* dest = __ClnObj;

	TDewException::CheckResult(TIdApi32::DbiGetField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) dest, &bBlank));

	if (bBlank)
		answer = 0;
	else{
		switch (fTable.fFieldDesc[iField - 1].iFldType) {
			case fldBOOL:
				answer = (int16) *(BOOL16*) dest;
				break;
			case fldINT16:
				answer = *(int16*) dest;
				break;
			default:
				TDewException::CheckResult(DBIERR_NOTSUPPORTED, "TDewRecord::GetField - Field type " + GetFieldTypeName(iField) + " cannot be converted to C++ type int16.");
		}
	}
	return bBlank == TRUE;
}
//
bool TDewRecord::GetField(const uint iField, INT32& answer) const
{
	CheckFieldNumber(iField);

	int size = __max(fTable.fFieldDesc[iField - 1].iLen,1);
	TTmpBuffer<char> __ClnObj(size);
	char* dest = __ClnObj;

	BOOL bBlank;
	TDewException::CheckResult(TIdApi32::DbiGetField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) dest, &bBlank));

	if (bBlank)
		answer = 0;
	else{
		switch (fTable.fFieldDesc[iField - 1].iFldType) {
			case fldBOOL:
				answer = (INT32) *(BOOL16*) dest;
				break;
			case fldINT16:
				answer = *(int16*) dest;
				break;
			case fldINT32:
				answer = *(INT32*) dest;
				break;
			case fldUINT16:
				answer = *(uint16*) dest;
				break;
			default:
				TDewException::CheckResult(DBIERR_NOTSUPPORTED, "TDewRecord::GetField - Field type " + GetFieldTypeName(iField) + " cannot be converted to C++ type INT32.");
		}
	}
	return bBlank == TRUE;
}
//
bool TDewRecord::GetField(const uint iField, uint16& answer) const
{
	CheckFieldNumber(iField);

	int size = __max(fTable.fFieldDesc[iField - 1].iLen,1);
	TTmpBuffer<char> __ClnObj(size);
	char* dest = __ClnObj;

	BOOL bBlank;
	TDewException::CheckResult(TIdApi32::DbiGetField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) dest, &bBlank));

	if (bBlank)
		answer = 0;
	else{
		switch (fTable.fFieldDesc[iField - 1].iFldType) {
			case fldBOOL:
				answer = (uint16) *(BOOL16*) dest;
				break;
			case fldUINT16:
				answer = *(uint16*) dest;
				break;
			default:
				TDewException::CheckResult(DBIERR_NOTSUPPORTED, "TDewRecord::GetField - Field type " + GetFieldTypeName(iField) + " cannot be converted to C++ type uint16.");
		}
	}
	return bBlank == TRUE;
}
//
bool TDewRecord::GetField(const uint iField, uint32& answer) const
{
	CheckFieldNumber(iField);

	BOOL bBlank;
	int size = __max(fTable.fFieldDesc[iField - 1].iLen,1);
	TTmpBuffer<char> __ClnObj(size);
	char* dest = __ClnObj;

	TDewException::CheckResult(TIdApi32::DbiGetField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) dest, &bBlank));

	if (bBlank)
		answer = 0;
	else{
		switch (fTable.fFieldDesc[iField - 1].iFldType) {
			case fldBOOL:
				answer = (uint32) *(BOOL16*) dest;
				break;
			case fldUINT16:
				answer = *(uint16*) dest;
				break;
			case fldUINT32:
				answer = *(uint32*) dest;
				break;
			default:
				TDewException::CheckResult(DBIERR_NOTSUPPORTED, "TDewRecord::GetField - Field type " + GetFieldTypeName(iField) + " cannot be converted to C++ type uint32.");
		}
	}
	return bBlank == TRUE;
}
// Get field as a real.
bool TDewRecord::GetField(const uint iField, float& answer) const
{
	CheckFieldNumber(iField);

	BOOL bBlank;
	int size = __max(fTable.fFieldDesc[iField - 1].iLen,1);
	TTmpBuffer<char> __ClnObj(size);
	char* dest = __ClnObj;

	TDewException::CheckResult(TIdApi32::DbiGetField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) dest, &bBlank));

	if (bBlank)
		answer = 0;
	else{
		switch (fTable.fFieldDesc[iField - 1].iFldType) {
	  case fldINT16:
		  answer = *(int16*) dest;
		  break;
	  case fldINT32:
		  answer = *(INT32*) dest;
		  break;
	  case fldFLOAT:
		  answer = (float) *(DFLOAT*) dest;
		  break;
	  case fldBCD:{
		  DFLOAT x;
		  TDewException::CheckResult(TIdApi32::DbiBcdToFloat((pFMTBcd) dest, &x));
		  answer = (float) x;
				  }
				  break;
	  case fldUINT16:
		  answer = *(uint16*) dest;
		  break;
	  case fldUINT32:
		  answer = *(uint32*) dest;
		  break;
	  case fldFLOATIEEE:
		  answer = (float) *(long double*) dest;
		  break;
	  default:
		  TDewException::CheckResult(DBIERR_NOTSUPPORTED, "TDewRecord::GetField - Field type " + GetFieldTypeName(iField) + " cannot be converted to C++ type float.");
		}
	}
	return bBlank == TRUE;
}
//
bool TDewRecord::GetField(const uint iField, double& answer) const
{
	CheckFieldNumber(iField);

	BOOL bBlank;
	int size = __max(fTable.fFieldDesc[iField - 1].iLen,1);
	TTmpBuffer<char> __ClnObj(size);
	char* dest = __ClnObj;

	TDewException::CheckResult(TIdApi32::DbiGetField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) dest, &bBlank));

	if (bBlank)
		answer = 0;
	else{
		switch (fTable.fFieldDesc[iField - 1].iFldType) {
	  case fldINT16:
		  answer = *(int16*) dest;
		  break;
	  case fldINT32:
		  answer = *(INT32*) dest;
		  break;
	  case fldFLOAT:
		  answer = *(DFLOAT*) dest;
		  break;
	  case fldBCD:
		  {
			  DFLOAT x;
			  TDewException::CheckResult(TIdApi32::DbiBcdToFloat((pFMTBcd) dest, &x));
			  answer = x;
		  }
		  break;
	  case fldUINT16:
		  answer = *(uint16*) dest;
		  break;
	  case fldUINT32:
		  answer = *(uint32*) dest;
		  break;
	  case fldFLOATIEEE:
		  answer = (double) *(long double*) dest;
		  break;
	  default:
		  TDewException::CheckResult(DBIERR_NOTSUPPORTED, "TDewRecord::GetField - Field type " + GetFieldTypeName(iField) + " cannot be converted to C++ type double.");
		}
	} 
	return bBlank == TRUE;
}
//
bool TDewRecord::GetField(const uint iField, long double& answer) const
{
	CheckFieldNumber(iField);

	BOOL bBlank;
	int size = __max(fTable.fFieldDesc[iField - 1].iLen,1);
	TTmpBuffer<char> __ClnObj(size);
	char* dest = __ClnObj;

	TDewException::CheckResult(TIdApi32::DbiGetField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) dest, &bBlank));

	if (bBlank)
		answer = 0;
	else{
		switch (fTable.fFieldDesc[iField - 1].iFldType) {
			case fldINT16:
				answer = *(int16*) dest;
				break;
			case fldINT32:
				answer = *(INT32*) dest;
				break;
			case fldFLOAT:
				answer = *(DFLOAT*) dest;
				break;
			case fldBCD:{
				DFLOAT x;
				TDewException::CheckResult(TIdApi32::DbiBcdToFloat((pFMTBcd) dest, &x));
				answer = x;
						}
						break;
			case fldUINT16:
				answer = *(uint16*) dest;
				break;
			case fldUINT32:
				answer = *(uint32*) dest;
				break;
			case fldFLOATIEEE:
				answer = *(long double*) dest;
				break;
			default:
				TDewException::CheckResult(DBIERR_NOTSUPPORTED, "TDewRecord::GetField - Field type " + GetFieldTypeName(iField) + " cannot be converted to C++ type long double.");
		}
	} 
	return bBlank = TRUE;
}
// Get a field as a date.
bool TDewRecord::GetField(const uint iField, TDate& answer) const
{
	CheckFieldNumber(iField);

	BOOL bBlank;
	int size = __max(fTable.fFieldDesc[iField - 1].iLen,1);
	TTmpBuffer<char> __ClnObj(size);
	char* dest = __ClnObj;

	TDewException::CheckResult(TIdApi32::DbiGetField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) dest, &bBlank));

	if (bBlank)
		answer = TDate();
	else{
		switch (fTable.fFieldDesc[iField - 1].iFldType) {
	  case fldDATE:{
		  uint16 mo, da; int16 ye;
		  TDewException::CheckResult(TIdApi32::DbiDateDecode(*(DBIDATE*) dest, &mo, &da, &ye));
		  answer = TDate(da, mo, ye);
				   }
				   break;
	  case fldTIMESTAMP:{
		  DBIDATE date;
		  TIME time;
		  TDewException::CheckResult(TIdApi32::DbiTimeStampDecode(*(TIMESTAMP*) dest, &date, &time));
		  uint16 da, mo; int16 ye;
		  TDewException::CheckResult(TIdApi32::DbiDateDecode(date, &mo, &da, &ye));
		  answer = TDate(da, mo, ye);
						}
						break;
	  default:
		  TDewException::CheckResult(DBIERR_NOTSUPPORTED, "TDewRecord::GetField - Field type " + GetFieldTypeName(iField) + " cannot be converted to C++ type TDate.");
		}
	}	
	return bBlank == TRUE;
}
// Get a field as a time.
bool TDewRecord::GetField(const uint iField, TTime& answer) const
{
	CheckFieldNumber(iField);

	BOOL bBlank;
	int size = __max(fTable.fFieldDesc[iField - 1].iLen,1);
	TTmpBuffer<char> __ClnObj(size);
	char* dest = __ClnObj;

	TDewException::CheckResult(TIdApi32::DbiGetField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) dest, &bBlank));

	if (bBlank)
		answer = TTime();
	else{
		switch (fTable.fFieldDesc[iField - 1].iFldType) {
			case fldTIME:{
				uint16 ho, mi, se;
				TDewException::CheckResult(TIdApi32::DbiTimeDecode(*(TIME*) dest, &ho, &mi, &se));
				answer = TTime(ho, mi, se / 1000);
						 }
						 break;
			case fldTIMESTAMP:{
				DBIDATE date;
				TIME time;
				TDewException::CheckResult(TIdApi32::DbiTimeStampDecode(*(TIMESTAMP*) dest, &date, &time));
				uint16 da, mo; int16 ye;
				TDewException::CheckResult(TIdApi32::DbiDateDecode(date, &mo, &da, &ye));
				uint16 ho, mi, se;
				TDewException::CheckResult(TIdApi32::DbiTimeDecode(time, &ho, &mi, &se));
				answer = TTime(TDate(da, mo, ye), ho, mi, se / 1000);
							  }
							  break;
			default:
				TDewException::CheckResult(DBIERR_NOTSUPPORTED, "TDewRecord::GetField - Field type " + GetFieldTypeName(iField) + " cannot be converted to C++ type TTime.");
		}
	} 
	return bBlank == TRUE;
}
// Get a field as a time stamp.
bool TDewRecord::GetField(const uint iField, TSystemTime& answer) const
{
	CheckFieldNumber(iField);

	BOOL bBlank;
	int size = __max(fTable.fFieldDesc[iField - 1].iLen,1);
	TTmpBuffer<char> __ClnObj(size);
	char* dest = __ClnObj;

	TDewException::CheckResult(TIdApi32::DbiGetField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) dest, &bBlank));

	if (bBlank) {
		answer = TSystemTime::LocalTime();
	} 
	else{
		switch (fTable.fFieldDesc[iField - 1].iFldType) {
			case fldTIMESTAMP:{
				DBIDATE date;
				TIME time;
				TDewException::CheckResult(TIdApi32::DbiTimeStampDecode(*(TIMESTAMP*) dest, &date, &time));
				uint16 da, mo; int16 ye;
				TDewException::CheckResult(TIdApi32::DbiDateDecode(date, &mo, &da, &ye));
				//answer1 = TDate(da, mo, ye);
				uint16 ho, mi, se;
				TDewException::CheckResult(TIdApi32::DbiTimeDecode(time, &ho, &mi, &se));
				//answer2 = TTime(answer1, ho, mi, se / 1000);
				answer = TSystemTime(da, mo, ye, ho, mi, se / 1000, se);
							  }
							  break;
			default:
				TDewException::CheckResult(DBIERR_NOTSUPPORTED, "TDewRecord::GetField - Field type " + GetFieldTypeName(iField) + " cannot be converted to C++ types TDate and TTime.");
		}

	} 
	return bBlank == TRUE;
}
//
void TDewRecord::SetBlank(const uint iField)
{
	CheckFieldNumber(iField);
	TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, NULL));
}
//
void TDewRecord::SetField(const uint iField, const owl_string& answer)
{
	CheckFieldNumber(iField);
	if (answer.length() > 255)
		TDewException::CheckResult(DBIERR_INVALIDPARAM);
	switch (fTable.fFieldDesc[iField - 1].iFldType) {
	case fldZSTRING:
	case fldBYTES:{
		BOOL		bBlank;
		pLDDesc LdObj;
		pVOID pLdObj = &LdObj;
		TTmpBuffer<char> __ClnObj1(MAX_PATH);
		char* sour_oem = __ClnObj1;
		TTmpBuffer<char> __ClnObj2(MAX_PATH);
		char* sour_ansi = __ClnObj2;
		strcpy(sour_ansi, answer.c_str());
		TDewException::CheckResult(TIdApi32::DbiGetLdObj(fTable.fHCursor, &pLdObj));
		TDewException::CheckResult(TIdApi32::DbiAnsiToNative(pLdObj, sour_oem, sour_ansi, 0, &bBlank));
		if(bBlank)
			TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) sour_ansi));
		else
			TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) sour_oem));
				  }
				  break;
	default:
		TDewException::CheckResult(DBIERR_NOTSUPPORTED, "TDewRecord::PutField - Field type " + GetFieldTypeName(iField) + " cannot be converted from C++ type string.");
	}
}
//
void TDewRecord::SetField(const uint iField, const char* answer)
{
	CheckFieldNumber(iField);
	if (strlen(answer) > 255)
		TDewException::CheckResult(DBIERR_INVALIDPARAM);

	switch (fTable.fFieldDesc[iField - 1].iFldType) {
	case fldZSTRING:
	case fldBYTES:{
		BOOL		bBlank;
		pLDDesc LdObj;
		pVOID pLdObj = &LdObj;
		TTmpBuffer<char> __ClnObj1(MAX_PATH);
		char* sour_oem = __ClnObj1;
		TTmpBuffer<char> __ClnObj2(MAX_PATH);
		char* sour_ansi = __ClnObj2;
		strcpy(sour_ansi, answer);
		TDewException::CheckResult(TIdApi32::DbiGetLdObj(fTable.fHCursor, &pLdObj));
		TDewException::CheckResult(TIdApi32::DbiAnsiToNative(pLdObj, sour_oem, sour_ansi, 0, &bBlank));
		if(bBlank)
			TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) sour_ansi));
		else
			TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) sour_oem));
				  }
				  break;
	default:
		TDewException::CheckResult(DBIERR_NOTSUPPORTED, "TDewRecord::PutField - Field type " + GetFieldTypeName(iField) + " cannot be converted from C++ type string.");
	}
}
//
void TDewRecord::SetField(const uint iField, const bool answer)
{
	CheckFieldNumber(iField);

	switch (fTable.fFieldDesc[iField - 1].iFldType) {
	case fldBOOL:{
		BOOL16 sour = answer;
		TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) &sour));
				 }
				 break;
	case fldINT16:{
		int16 sour = answer;
		TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) &sour));
				  }
				  break;
	case fldINT32:{
		INT32 sour = answer;
		TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) &sour));
				  }
				  break;
	case fldUINT16:{
		uint16 sour = answer;
		TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) &sour));
				   }
				   break;
	case fldUINT32:{
		uint32 sour = answer;
		TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) &sour));
				   }
				   break;
	default:
		TDewException::CheckResult(DBIERR_NOTSUPPORTED, "TDewRecord::PutField - Field type " + GetFieldTypeName(iField) + " cannot be converted from C++ type bool.");
	}
}
//
void TDewRecord::SetField(const uint iField, const int16 answer)
{
	CheckFieldNumber(iField);

	switch (fTable.fFieldDesc[iField - 1].iFldType) {
	case fldBOOL:{
		BOOL16 sour = answer != 0 ? (BOOL16) 1 : (BOOL16) 0;
		TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) &sour));
				 }
				 break;
	case fldINT16:{
		int16 sour = answer;
		TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) &sour));
				  }
				  break;
	case fldINT32:{
		INT32 sour = answer;
		TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) &sour));
				  }
				  break;
	case fldFLOAT:{
		DFLOAT sour = answer;
		TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) &sour));
				  }
				  break;
	case fldBCD:{
		DFLOAT x = answer;
		FMTBcd sour;
		TDewException::CheckResult(TIdApi32::DbiBcdFromFloat(&x, fTable.fFieldDesc->iUnits1, fTable.fFieldDesc->iUnits2, &sour));
		TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) &sour));
				}
				break;
	case fldFLOATIEEE:{
		long double sour = answer;
		TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) &sour));
					  }
					  break;
	default:
		TDewException::CheckResult(DBIERR_NOTSUPPORTED, "TDewRecord::PutField - Field type " + GetFieldTypeName(iField) + " cannot be converted from C++ type int16.");
	}
}
//
void TDewRecord::SetField(const uint iField, const INT32 answer)
{
	CheckFieldNumber(iField);

	switch (fTable.fFieldDesc[iField - 1].iFldType) {
		case fldBOOL:{
			BOOL16 sour = answer != 0 ? (BOOL16) 1 : (BOOL16) 0;
			TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) &sour));
					 }
					 break;
		case fldINT32:{
			INT32 sour = answer;
			TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) &sour));
					  }
					  break;
		case fldFLOAT:{
			DFLOAT sour = answer;
			TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) &sour));
					  }
					  break;
		case fldBCD:{
			DFLOAT x = answer;
			FMTBcd sour;
			TDewException::CheckResult(TIdApi32::DbiBcdFromFloat(&x, fTable.fFieldDesc->iUnits1, fTable.fFieldDesc->iUnits2, &sour));
			TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) &sour));
					}
					break;
		case fldFLOATIEEE:{
			long double sour = answer;
			TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) &sour));
						  }
						  break;
		default:
			TDewException::CheckResult(DBIERR_NOTSUPPORTED, "TDewRecord::PutField - Field type " + GetFieldTypeName(iField) + " cannot be converted from C++ type INT32.");
	}
}
//
void TDewRecord::SetField(const uint iField, const uint answer)
{
	CheckFieldNumber(iField);

	switch (fTable.fFieldDesc[iField - 1].iFldType) {
	case fldBOOL:{
		BOOL16 sour = answer != 0 ? (BOOL16) 1 : (BOOL16) 0;
		TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) &sour));
				 }
				 break;
	case fldUINT16:{
		uint16 sour = (uint16)answer;
		TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) &sour));
				   }
				   break;
	case fldUINT32:{
		uint32 sour = answer;
		TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) &sour));
				   }
				   break;
	case fldFLOAT:{
		DFLOAT sour = answer;
		TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) &sour));
				  }
				  break;
	case fldBCD:{
		DFLOAT x = answer;
		FMTBcd sour;
		TDewException::CheckResult(TIdApi32::DbiBcdFromFloat(&x, fTable.fFieldDesc->iUnits1, fTable.fFieldDesc->iUnits2, &sour));
		TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) &sour));
				}
				break;
	case fldFLOATIEEE:{
		long double sour = answer;
		TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) &sour));
					  }
					  break;
	default:
		TDewException::CheckResult(DBIERR_NOTSUPPORTED, "TDewRecord::PutField - Field type " + GetFieldTypeName(iField) + " cannot be converted from C++ type uint16.");
	}
}
//
void TDewRecord::SetField(const uint iField, const uint32 answer)
{
	CheckFieldNumber(iField);

	switch (fTable.fFieldDesc[iField - 1].iFldType) {
	case fldBOOL:{
		BOOL16 sour = answer != 0 ? (BOOL16) 1 : (BOOL16) 0;
		TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) &sour));
				 }
				 break;
	case fldUINT32:{
		uint32 sour = answer;
		TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) &sour));
				   }
				   break;
	case fldFLOAT:{
		DFLOAT sour = answer;
		TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) &sour));
				  }
				  break;
	case fldBCD:{
		DFLOAT x = answer;
		FMTBcd sour;
		TDewException::CheckResult(TIdApi32::DbiBcdFromFloat(&x, fTable.fFieldDesc->iUnits1, fTable.fFieldDesc->iUnits2, &sour));
		TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) &sour));
				}
				break;
	case fldFLOATIEEE:{
		long double sour = answer;
		TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) &sour));
					  }
					  break;
	default:
		TDewException::CheckResult(DBIERR_NOTSUPPORTED, "TDewRecord::PutField - Field type " + GetFieldTypeName(iField) + " cannot be converted from C++ type uint32.");
	}
}
// Put a field as a real.
void TDewRecord::SetField(const uint iField, const float answer)
{
	CheckFieldNumber(iField);

	switch (fTable.fFieldDesc[iField - 1].iFldType) {
	case fldFLOAT:{
		DFLOAT sour = answer;
		TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) &sour));
				  }
				  break;
	case fldBCD:{
		DFLOAT x = answer;
		FMTBcd sour;
		TDewException::CheckResult(TIdApi32::DbiBcdFromFloat(&x, fTable.fFieldDesc->iUnits1, fTable.fFieldDesc->iUnits2, &sour));
		TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) &sour));
				}
				break;
	case fldFLOATIEEE:{
		long double sour = answer;
		TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) &sour));
					  }
					  break;
	default:
		TDewException::CheckResult(DBIERR_NOTSUPPORTED, "TDewRecord::PutField - Field type " + GetFieldTypeName(iField) + " cannot be converted from C++ type float.");
	}
}
//
void TDewRecord::SetField(const uint iField, const double answer)
{
	CheckFieldNumber(iField);

	switch (fTable.fFieldDesc[iField - 1].iFldType) {
	case fldFLOAT:{
		DFLOAT sour = answer;
		TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) &sour));
				  }
				  break;
	case fldBCD:{
		DFLOAT x = answer;
		FMTBcd sour;
		TDewException::CheckResult(TIdApi32::DbiBcdFromFloat(&x, fTable.fFieldDesc->iUnits1, fTable.fFieldDesc->iUnits2, &sour));
		TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) &sour));
				}
				break;
	case fldFLOATIEEE:{
		long double sour = answer;
		TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) &sour));
					  }
					  break;
	default:
		TDewException::CheckResult(DBIERR_NOTSUPPORTED, "TDewRecord::PutField - Field type " + GetFieldTypeName(iField) + " cannot be converted from C++ type double.");
	}
}
//
void TDewRecord::SetField(const uint iField, const long double answer)
{
	CheckFieldNumber(iField);

	switch (fTable.fFieldDesc[iField - 1].iFldType) {
	case fldFLOAT:{
		DFLOAT sour = (DFLOAT) answer;
		TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) &sour));
				  }
				  break;
	case fldBCD:{
		DFLOAT x = (DFLOAT) answer;
		FMTBcd sour;
		TDewException::CheckResult(TIdApi32::DbiBcdFromFloat(&x, fTable.fFieldDesc->iUnits1, fTable.fFieldDesc->iUnits2, &sour));
		TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) &sour));
				}
				break;
	case fldFLOATIEEE:{
		long double sour = answer;
		TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) &sour));
					  }
					  break;
	default:
		TDewException::CheckResult(DBIERR_NOTSUPPORTED, "TDewRecord::PutField - Field type " + GetFieldTypeName(iField) + " cannot be converted from C++ type long double.");
	}
}
// Put a field as a date.
void TDewRecord::SetField(const uint iField, const TDate& answer)
{
	CheckFieldNumber(iField);

	switch (fTable.fFieldDesc[iField - 1].iFldType) {
	case fldDATE:{
		DBIDATE sour;
		TDewException::CheckResult(TIdApi32::DbiDateEncode((uint16) answer.Month(), (uint16) answer.DayOfMonth(), (int16) answer.Year(), &sour));
		TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) &sour));
				 }
				 break;
	case fldTIMESTAMP:{
		DBIDATE date;
		TDewException::CheckResult(TIdApi32::DbiDateEncode((uint16) answer.Month(), (uint16) answer.DayOfMonth(), (int16) answer.Year(), &date));
		TIME time;
		TDewException::CheckResult(TIdApi32::DbiTimeEncode(0, 0, 0, &time));
		TIMESTAMP sour;
		TDewException::CheckResult(TIdApi32::DbiTimeStampEncode(date, time, &sour));
		TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) &sour));
					  }
					  break;
	default:
		TDewException::CheckResult(DBIERR_NOTSUPPORTED, "TDewRecord::PutField - Field type " + GetFieldTypeName(iField) + " cannot be converted from C++ type TDate.");
	}
}
// Put a field as a time.
void TDewRecord::SetField(const uint iField, const TTime& answer)
{
	CheckFieldNumber(iField);

	switch (fTable.fFieldDesc[iField - 1].iFldType) {
	case fldTIME:{
		TIME sour;
		TDewException::CheckResult(TIdApi32::DbiTimeEncode((uint16) answer.Hour(), (uint16) answer.Minute(), (uint16) answer.Second(), &sour));
		TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) &sour));
				 }
				 break;
	case fldTIMESTAMP:{
		TDate today(answer);
		DBIDATE date;
		TDewException::CheckResult(TIdApi32::DbiDateEncode((uint16) today.Month(), (uint16) today.DayOfMonth(), (int16) today.Year(), &date));
		TIME time;
		TDewException::CheckResult(TIdApi32::DbiTimeEncode((uint16) answer.Hour(), (uint16) answer.Minute(), (uint16) answer.Second(), &time));
		TIMESTAMP sour;
		TDewException::CheckResult(TIdApi32::DbiTimeStampEncode(date, time, &sour));
		TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) &sour));
					  }
					  break;
	default:
		TDewException::CheckResult(DBIERR_NOTSUPPORTED, "TDewRecord::PutField - Field type " + GetFieldTypeName(iField) + " cannot be converted from C++ type TTime.");
	}
}
// Put a field as a time stamp.
void TDewRecord::SetField(const uint iField, const TSystemTime& answer)
{
	CheckFieldNumber(iField);

	switch (fTable.fFieldDesc[iField - 1].iFldType) {
	case fldTIMESTAMP:{
		DBIDATE date;
		TDewException::CheckResult(TIdApi32::DbiDateEncode((uint16)answer.GetMonth(), (uint16)answer.GetDay(), (int16)answer.GetYear(), &date));
		TIME time;
		TDewException::CheckResult(TIdApi32::DbiTimeEncode((uint16)answer.GetHour(), (uint16)answer.GetMinute(), (uint16)answer.GetSecond(), &time));
		TIMESTAMP sour;
		TDewException::CheckResult(TIdApi32::DbiTimeStampEncode(date, time, &sour));
		TDewException::CheckResult(TIdApi32::DbiPutField(fTable.fHCursor, (uint16)iField, fBuffer, (pBYTE) &sour));
					  }
					  break;
	default:
		TDewException::CheckResult(DBIERR_NOTSUPPORTED, "TDewRecord::PutField - Field type " + GetFieldTypeName(iField) + " cannot be converted from C++ type TDate and TTime.");
	}
}
// Resize the buffer.
void TDewRecord::ResizeBuffer()
{
	pBYTE oldBuffer = fBuffer;
	fBuffer = new BYTE[fTable.fCursorProps.iRecBufSize];
	delete [] oldBuffer;
}
// Verify the field value.
bool TDewRecord::VerifyField(const uint iField) const
{
	CheckFieldNumber(iField);
	BOOL bBlank;
	TDewException::CheckResult(TIdApi32::DbiVerifyField(fTable.fHCursor, (uint16)iField, fBuffer, &bBlank));
	return bBlank;
}
//------------------------------------------------------------------------------------------------
// class TDewSession
// ~~~~~ ~~~~~~~~~~~~
//
// Constructor.
TDewSession::TDewSession(const pcCHAR pszName, const pcCHAR pNetDir)
: fHSession(NULL)
{
	Open(pszName, pNetDir);
}
//
TDewSession::TDewSession(TDewEngine&)
: 
fHSession(0)
{
	TDewException::CheckResult(TIdApi32::DbiGetCurrSession(&fHSession));
}
// Destructor.
TDewSession::~TDewSession()
{
	if (IsOpen())
		Close();
}
// Close session.
void TDewSession::Close()
{
	TDewException::CheckResult(TIdApi32::DbiCloseSession(fHSession));
	fHSession = NULL;
}
// Open session.
void TDewSession::Open(const pcCHAR pszName, const pcCHAR pNetDir)
{
	if (IsOpen())
		Close();

	TDewException::CheckResult(TIdApi32::DbiStartSession((pCHAR) pszName, &fHSession, (pCHAR) pNetDir));
}
//
void TDewSession::Open(TDewEngine&)
{
	if (IsOpen())
		Close();

	TDewException::CheckResult(TIdApi32::DbiGetCurrSession(&fHSession));
}
//------------------------------------------------------------------------------------------------
// class TDewStatement
// ~~~~~ ~~~~~~~~~~~~~
//
// Constructors.
TDewStatement::TDewStatement()
: 
fHDatabase(0),fHStatement(0)
{
}
//
TDewStatement::TDewStatement( const TDewDatabase& database)
: 
fHDatabase(database.GetHandle()),fHStatement(0)
{
}
//
TDewStatement::TDewStatement(const TDewDatabase& database, const char* pszQuery, const TDBQueryLang eQryLang)
: 
fHDatabase(database.GetHandle()),fHStatement(NULL)
{
	Open(pszQuery, eQryLang);
}
// Destructor.
TDewStatement::~TDewStatement()
{
	if (IsOpen())
		Close();
}
// Close statement.
void TDewStatement::Close()
{
	TDewException::CheckResult(TIdApi32::DbiQFree(&fHStatement));
	fHStatement = NULL;
}
// Execute query.
TDewTable TDewStatement::Query() const
{
	hDBICur hCursor;
	TDewException::CheckResult(TIdApi32::DbiQExec(fHStatement, &hCursor));
	return TDewTable(hCursor);
}
// Prepare a statement.
void TDewStatement::Open(const TDewDatabase& database, const char* pszQuery, const TDBQueryLang eQryLang)
{
	if(IsOpen())
		Close();

	fHDatabase = database.GetHandle();
	TDewException::CheckResult(TIdApi32::DbiQAlloc(fHDatabase, (DBIQryLang)eQryLang,&fHStatement));
	TDewException::CheckResult(TIdApi32::DbiQPrepare(fHStatement, (pCHAR) pszQuery));
}
//
void TDewStatement::Open(const char* pszQuery, const TDBQueryLang eQryLang)
{
	if (IsOpen())
		Close();

	TDewException::CheckResult(TIdApi32::DbiQAlloc(fHDatabase, (DBIQryLang)eQryLang,&fHStatement));
	TDewException::CheckResult(TIdApi32::DbiQPrepare(fHStatement, (pCHAR) pszQuery));
}
// Set parameters.
void TDewStatement::SetParams(const uint uFldDescs, const pcFLDDesc paFldDescs, const pcBYTE pRecBuff)
{
	TDewException::CheckResult(TIdApi32::DbiQSetParams(fHStatement, (uint16)uFldDescs, (pFLDDesc) paFldDescs, (pBYTE) pRecBuff));
}
//------------------------------------------------------------------------------------------------
// class TDewEnv
// ~~~~~ ~~~~~~~
//
// Struct for defining the environ.
TDewEnv::TDewEnv()
{
	memset((DBIEnv*) this, 0, sizeof(DBIEnv));
}
//
TDewEnv::TDewEnv(const DBIEnv& dbiEnv)
//: 
//	DBIEnv(dbiEnv)   //Jogy - causes problem under BCB6
{
	memcpy((DBIEnv*) this, &dbiEnv, sizeof(DBIEnv));
}
//
TDewEnv::TDewEnv(const pcCHAR workDir, const pcCHAR iniFile, 
				 const bool forceLocalInit, const pcCHAR lang, 
				 const pcCHAR clientName)
{
	memset((DBIEnv*)this, 0, sizeof(DBIEnv));

	strcpy(szWorkDir, workDir != NULL ? workDir : "");
	strcpy(szIniFile, iniFile != NULL ? iniFile : "");
	bForceLocalInit = forceLocalInit;
	strcpy(szLang, lang != NULL ? lang : "");
	strcpy(szClientName, clientName != NULL ? clientName : "");
}
//------------------------------------------------------------------------------------------------
// class TDewFldDesc
// ~~~~~ ~~~~~~~~~~~
//
// Field Descriptor.
TDewFldDesc::TDewFldDesc()
{
	memset((FLDDesc*) this, 0, sizeof(FLDDesc));
}
//
TDewFldDesc::TDewFldDesc(const FLDDesc& fldDesc)
//: 
//	FLDDesc(fldDesc)  //Jogy - causes problem under BCB6
{
	memcpy((FLDDesc*) this, & fldDesc, sizeof(FLDDesc));
}
//
TDewFldDesc::TDewFldDesc(const uint fldNum, const pcCHAR name, const uint fldType, const uint subType, const INT16 units1, const INT16 units2)
{
	memset((FLDDesc*) this, 0, sizeof(FLDDesc));

	iFldNum = (uint16)fldNum;
	strcpy(szName, name != NULL ? name : "");
	iFldType = (uint16)fldType;
	iSubType = subType;
	iUnits1 = (int16)units1;
	iUnits2 = (int16)units2;
}
//------------------------------------------------------------------------------------------------
// class TDewIdxDesc
// ~~~~~ ~~~~~~~~~~~
//
// Index description.
TDewIdxDesc::TDewIdxDesc()
{
	memset((IDXDesc*) this, 0, sizeof(IDXDesc));
}
//
TDewIdxDesc::TDewIdxDesc(const IDXDesc& idxDesc)
//: 
//	IDXDesc(idxDesc)   //Jogy - causes problem under BCB6
{
	memcpy((IDXDesc*) this, & idxDesc, sizeof(IDXDesc));
}
//
TDewIdxDesc::TDewIdxDesc(const DBIKEY keyFld, const uint fldsInKey, 
						 const pcCHAR name, const uint indexId, 
						 const pcCHAR tagName, const bool primary, const bool unique, const bool descending, const bool maintained, const bool caseInsensitive, const bool subset, const pcCHAR keyCond, const uint blockSize)
{
	memset((IDXDesc*) this, 0, sizeof(IDXDesc));

	strcpy(szName, name != NULL ? name : "");
	iIndexId = (uint16)indexId;
	strcpy(szTagName, tagName != NULL ? tagName : "");
	bPrimary = primary;
	bUnique = unique;
	bDescending = descending;
	bMaintained = maintained;
	bSubset = subset;
	bExpIdx = FALSE;
	iFldsInKey = (uint16)fldsInKey;
	memcpy(aiKeyFld, keyFld, sizeof(DBIKEY));
	strcpy(szKeyCond, keyCond != NULL ? keyCond : "");
	bCaseInsensitive = caseInsensitive;
	iBlockSize = (uint16)blockSize;
}
//
TDewIdxDesc::TDewIdxDesc(const pcCHAR keyExp, const uint keyExpType, const pcCHAR name, const uint indexId, const pcCHAR tagName, const bool primary, const bool unique, const bool descending, const bool maintained, const bool caseInsensitive, const bool subset, const pcCHAR keyCond , const uint blockSize)
{
	memset((IDXDesc*) this, 0, sizeof(IDXDesc));

	strcpy(szName, name != NULL ? name : "");
	iIndexId = indexId;
	strcpy(szTagName, tagName != NULL ? tagName : "");
	bPrimary = primary;
	bUnique = unique;
	bDescending = descending;
	bMaintained = maintained;
	bSubset = subset;
	bExpIdx = TRUE;
	iFldsInKey = 0;
	iKeyExpType = keyExpType;
	strcpy(szKeyExp, keyExp != NULL ? keyExp : "");
	strcpy(szKeyCond, keyCond != NULL ? keyCond : "");
	bCaseInsensitive = caseInsensitive;
	iBlockSize = blockSize;
}
//------------------------------------------------------------------------------------------------
// class TDewVchkDesc
// ~~~~~ ~~~~~~~~~~~~
//
// Val Check structure.
TDewVchkDesc::TDewVchkDesc()
{
	memset((VCHKDesc*) this, 0, sizeof(VCHKDesc));
}
//
TDewVchkDesc::TDewVchkDesc(const VCHKDesc& vchkDesc)
//: 
//	VCHKDesc(vchkDesc)    //Jogy - causes problem under BCB6
{
	memcpy((VCHKDesc*) this, & vchkDesc, sizeof(VCHKDesc));
}
//
TDewVchkDesc::TDewVchkDesc(const uint fldNum)
{
	memset((VCHKDesc*) this, 0, sizeof(VCHKDesc));

	iFldNum = fldNum;
	bRequired = FALSE;
	bHasMinVal = FALSE;
	bHasMaxVal = FALSE;
	bHasDefVal = FALSE;
	strcpy(szPict, "");
	elkupType = lkupNONE;
	strcpy(szLkupTblName, "");
}
//
void TDewVchkDesc::SetRequired()
{
	bRequired = TRUE;
}
//
void TDewVchkDesc::SetMinVal(const pcBYTE minVal)
{
	bHasMinVal = TRUE;
	memcpy(aMinVal, minVal, sizeof(DBIVCHK));
}
///
void TDewVchkDesc::SetMinValToday()
{
	bHasMinVal = TODAYVAL;
}
//
void TDewVchkDesc::SetMaxVal(const pcBYTE maxVal)
{
	bHasMaxVal = TRUE;
	memcpy(aMaxVal, maxVal, sizeof(DBIVCHK));
}
//
void TDewVchkDesc::SetMaxValToday()
{
	bHasMaxVal = TODAYVAL;
}
//
void TDewVchkDesc::SetDefVal(const pcBYTE defVal)
{
	bHasDefVal = TRUE;
	memcpy(aDefVal, defVal, sizeof(DBIVCHK));
}
//
void TDewVchkDesc::SetDefValToday()
{
	bHasDefVal = TODAYVAL;
}
//
void TDewVchkDesc::SetPict(const pcCHAR pict)
{
	strcpy(szPict, pict);
}
//
void TDewVchkDesc::SetLkup(const LKUPType lkupType, const pcCHAR lkupTblName)
{
	elkupType = lkupType;
	strcpy(szLkupTblName, lkupTblName);
}
//
//------------------------------------------------------------------------------------------------
// class TDewVchkDesc
// ~~~~~ ~~~~~~~~~~~~
//
// Ref Integrity Desc.
TDewRintDesc::TDewRintDesc()
{
	memset((RINTDesc*) this, 0, sizeof(RINTDesc));
}
//
TDewRintDesc::TDewRintDesc(const RINTDesc& rintDesc)
//: 
//	RINTDesc(rintDesc)    //Jogy - causes problem under BCB6
{
	memcpy((RINTDesc*) this, & rintDesc, sizeof(RINTDesc));
}
//
TDewRintDesc::TDewRintDesc(const uint rintNum, const pcCHAR rintName, const pcCHAR tblName, const DBIKEY thisTabFld, const DBIKEY othTabFld, const uint fldCount, const RINTQual modOp, const RINTQual delOp, const RINTType type)
{
	memset((RINTDesc*) this, 0, sizeof(RINTDesc));

	iRintNum = rintNum;
	strcpy(szRintName, rintName);
	eType = type;
	strcpy(szTblName, tblName);
	eModOp = modOp;
	eDelOp = delOp;
	iFldCount = fldCount;
	memcpy(aiThisTabFld, thisTabFld, sizeof(DBIKEY));
	memcpy(aiOthTabFld, othTabFld, sizeof(DBIKEY));
}
// Security description.
TDewSecDesc::TDewSecDesc()
{
	memset((SECDesc*) this, 0, sizeof(SECDesc));
}
//
TDewSecDesc::TDewSecDesc(const SECDesc& secDesc)
//: 
//	SECDesc(secDesc)    //Jogy - causes problem under BCB6
{
	memcpy((SECDesc*) this, & secDesc, sizeof(SECDesc));
}
//
TDewSecDesc::TDewSecDesc(const uint secNum, const PRVType prvFld[DBIMAXFLDSINSEC], const pcCHAR password, const PRVType prvTable, const uint famRights)
{
	memset((SECDesc*) this, 0, sizeof(SECDesc));

	iSecNum = secNum;
	eprvTable = prvTable;
	iFamRights = famRights;
	strcpy(szPassword, password);
	memcpy(aprvFld, prvFld, sizeof(PRVType) * DBIMAXFLDSINSEC);
}
// Create/Restruct Table descr.
TDewCrTblDesc::TDewCrTblDesc()
{
	memset((CRTblDesc*) this, 0, sizeof(CRTblDesc));
}
//
TDewCrTblDesc::TDewCrTblDesc(const CRTblDesc& crTblDesc)
//: 
//	CRTblDesc(crTblDesc)     //Jogy - causes problem under BCB6
{
	memcpy((CRTblDesc*) this, & crTblDesc, sizeof(CRTblDesc));
}
//
TDewCrTblDesc::TDewCrTblDesc(const pcCHAR tblName, const uint fldCount, const pcFLDDesc fldDesc, const uint idxCount, const pcIDXDesc idxDesc, const uint valChkCount, const pcVCHKDesc vchkDesc, const uint rintCount, const pcRINTDesc rintDesc, const uint secRecCount, const pcSECDesc secDesc, const pcCHAR password, const pcCHAR tblType, const pcCROpType crOpType)
{
	memset((CRTblDesc*) this, 0, sizeof(CRTblDesc));

	strcpy(szTblName, tblName);
	strcpy(szTblType, tblType != NULL ? tblType : "");
	strcpy(szPassword, password != NULL ? password : "");
	bProtected = password != NULL;
	iFldCount = fldCount;
	pfldDesc = (pFLDDesc) fldDesc;
	iIdxCount = idxCount;
	pidxDesc = (pIDXDesc) idxDesc;
	iSecRecCount = secRecCount;
	psecDesc = (pSECDesc) secDesc;
	iValChkCount = valChkCount;
	pvchkDesc = (pVCHKDesc) vchkDesc;
	iRintCount = rintCount;
	printDesc = (pRINTDesc) rintDesc;
	pecrRintOp = (pCROpType) crOpType;
	iOptParams = 0;
}
//------------------------------------------------------------------------------------------------
// class TDewTransaction
// ~~~~~ ~~~~~~~~~~~~~~~
//
TDewTransaction::TDewTransaction()
: 
fHDatabase(0),fHTransaction(0)
{
}
//
TDewTransaction::TDewTransaction(const TDewDatabase& database)
: 
fHDatabase(database.GetHandle()),fHTransaction(NULL)
{
}
//
TDewTransaction::TDewTransaction(const TDewDatabase& database, const eXILType eXIL)
: 
fHDatabase(database.GetHandle()),fHTransaction(0)
{
	Open(eXIL);
}
// Destructor.
TDewTransaction::~TDewTransaction()
{
	if (IsOpen())
		Close();
}
// Close transaction.
void TDewTransaction::Close(const eXEnd eEnd)
{
	TDewException::CheckResult(TIdApi32::DbiEndTran(fHDatabase, fHTransaction, eEnd));
	fHTransaction = NULL;
}
// Get transaction info.
void TDewTransaction::GetTranInfo(XInfo& xInfo) const
{
	TDewException::CheckResult(TIdApi32::DbiGetTranInfo(fHDatabase, fHTransaction, &xInfo));
}
// Begin a transaction.
void TDewTransaction::Open(const TDewDatabase& database, const eXILType eXIL)
{
	if (IsOpen())
		Close();
	fHDatabase = database.GetHandle();
	TDewException::CheckResult(TIdApi32::DbiBeginTran(fHDatabase, eXIL, &fHTransaction));
}
//
void TDewTransaction::Open(const eXILType eXIL)
{
	if (IsOpen())
		Close();
	TDewException::CheckResult(TIdApi32::DbiBeginTran(fHDatabase, eXIL, &fHTransaction));
}

#if defined(BI_COMP_MSC)
static void neverUsed()
{
	TDewStatement		p1;
	TDewTransaction p2;
	TDewFilter			p3;
	TDewBlob				p4;
	TDewTable			  p6;
	TDewDatabase		p7;
	TDewEngine			p8;
	TDewSession			p9;
	TDewEnv					p10;
	TDewFldDesc			p11;
	TDewIdxDesc			p12;
	TDewVchkDesc		p13;
	TDewRintDesc		p14;
	TDewSecDesc			p15;
	TDewCrTblDesc		p16;
}
#endif
//
_OWLFX_END_NAMESPACE

/* ========================================================================== */
