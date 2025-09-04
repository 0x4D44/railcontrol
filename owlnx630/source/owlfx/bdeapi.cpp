//----------------------------------------------------------------------------
// OWL File Extensions (OWLFX) Class Library
// Database Engine Wrapper, by Yura Bidus
// All rights reserved
//
//$Revision: 1.1.4.2 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:57 $
//
//----------------------------------------------------------------------------
#include <owlfx/pch.h>
#pragma hdrstop
#include <owlfx/bdeapi.h>
#include <owl/registry.h>

_OWLFX_BEGIN_NAMESPACE

__OWL_USING_NAMESPACE

//
// class TIdApi32
// ~~~~~ ~~~~~~~
// delay loading IDAPI32.DLL
static const _TCHAR idapiStr[]					= _T("IDAPI32.DLL");
static const char DbiInitFnStr[]				= "DbiInitFn";
static const char DbiExitStr[]					= "DbiExit";
static const char DbiGetSysVersionStr[]	= "DbiGetSysVersion";
static const char DbiGetSysConfigStr[]	= "DbiGetSysConfig";
static const char DbiGetClientInfoStr[]	= "DbiGetClientInfo";
static const char DbiGetSysInfoStr[]		= "DbiGetSysInfo";
static const char DbiLoadDriverStr[]		= "DbiLoadDriver";
static const char DbiStartSessionStr[]	= "DbiStartSession";
static const char DbiGetCurrSessionStr[]= "DbiGetCurrSession";
static const char DbiSetCurrSessionStr[]= "DbiSetCurrSession";
static const char DbiCloseSessionStr[]	= "DbiCloseSession";
static const char DbiGetSesInfoStr[]		= "DbiGetSesInfo";
static const char DbiSetPrivateDirStr[]	= "DbiSetPrivateDir";
static  const char DbiOpenDatabaseStr[]	= "DbiOpenDatabase";
static  const char DbiSetDirectoryStr[]	= "DbiSetDirectory";
static  const char DbiGetDirectoryStr[]	= "DbiGetDirectory";
static  const char DbiOpenTableListStr[]	= "DbiOpenTableList";
static  const char DbiOpenFileListStr[]				= "DbiOpenFileList";
static  const char DbiOpenIndexListStr[]			= "DbiOpenIndexList";
static  const char DbiOpenFieldListStr[]			= "DbiOpenFieldList";
static  const char DbiOpenVchkListStr[]				= "DbiOpenVchkList";
static  const char DbiOpenRintListStr[]				= "DbiOpenRintList";
static  const char DbiOpenSecurityListStr[]		= "DbiOpenSecurityList";
static  const char DbiOpenFamilyListStr[]			= "DbiOpenFamilyList";
static  const char DbiOpenSPListStr[]					= "DbiOpenSPList";
static  const char DbiOpenSPParamListStr[]		= "DbiOpenSPParamList";
static  const char DbiOpenFunctionListStr[]		= "DbiOpenFunctionList";
static  const char DbiOpenFunctionArgListStr[]= "DbiOpenFunctionArgList";
static  const char DbiCloseDatabaseStr[]			= "DbiCloseDatabase";
static  const char DbiOpenDriverListStr[]			= "DbiOpenDriverList";
static  const char DbiGetDriverDescStr[]			= "DbiGetDriverDesc";
static  const char DbiOpenDatabaseListStr[]		= "DbiOpenDatabaseList";
static  const char DbiGetDatabaseDescStr[]		= "DbiGetDatabaseDesc";
static  const char DbiOpenTableTypesListStr[]	= "DbiOpenTableTypesList";
static  const char DbiGetTableTypeDescStr[]		= "DbiGetTableTypeDesc";
static  const char DbiOpenFieldTypesListStr[]	= "DbiOpenFieldTypesList";
static  const char DbiGetFieldTypeDescStr[]		= "DbiGetFieldTypeDesc";
static  const char DbiOpenIndexTypesListStr[]	= "DbiOpenIndexTypesList";
static  const char DbiGetIndexTypeDescStr[]		= "DbiGetIndexTypeDesc";
static  const char DbiOpenLdListStr[]					= "DbiOpenLdList";
// Table Open, Properties & Structure
static  const char DbiOpenTableStr[]					= "DbiOpenTable";
static  const char DbiGetCursorPropsStr[]			= "DbiGetCursorProps";
static  const char DbiGetObjFromNameStr[]			= "DbiGetObjFromName";
static  const char DbiGetObjFromObjStr[]			= "DbiGetObjFromObj";
static  const char DbiGetPropStr[]						= "DbiGetProp";
static  const char DbiSetPropStr[]						= "DbiSetProp";
static  const char DbiValidatePropStr[]				= "DbiValidateProp";
static  const char DbiGetFieldDescsStr[]			= "DbiGetFieldDescs";
static  const char DbiGetCursorForTableStr[]	= "DbiGetCursorForTable";
static  const char DbiCloneCursorStr[]				= "DbiCloneCursor";
static  const char DbiCloseCursorStr[]				= "DbiCloseCursor";
// Index Manipulation & Usage
static  const char DbiOpenIndexStr[]					= "DbiOpenIndex";
static  const char DbiCloseIndexStr[]					= "DbiCloseIndex";
static  const char DbiSwitchToIndexStr[]			= "DbiSwitchToIndex";
static  const char DbiGetIndexDescStr[]				= "DbiGetIndexDesc";
static  const char DbiGetIndexDescsStr[]			= "DbiGetIndexDescs";
static  const char DbiGetIndexForFieldStr[]		= "DbiGetIndexForField";
static  const char DbiGetIndexSeqNoStr[]			= "DbiGetIndexSeqNo";
static  const char DbiSetToKeyStr[]						= "DbiSetToKey";
static  const char DbiExtractKeyStr[]					= "DbiExtractKey";
static  const char DbiSetRangeStr[]						= "DbiSetRange";
static  const char DbiResetRangeStr[]					= "DbiResetRange";
static  const char DbiCompareKeysStr[]				= "DbiCompareKeys";
static  const char DbiGetRecordForKeyStr[]		= "DbiGetRecordForKey";
// Validity check and referential integrity
static  const char DbiGetVchkDescStr[]				= "DbiGetVchkDesc";
static  const char DbiGetRintDescStr[]				= "DbiGetRintDesc";
// Cursor Maintenance
static  const char DbiSetToBeginStr[]					= "DbiSetToBegin";
static  const char DbiSetToEndStr[]						= "DbiSetToEnd";
static  const char DbiSetToCursorStr[]				= "DbiSetToCursor";
static  const char DbiGetBookMarkStr[]				= "DbiGetBookMark";
static  const char DbiSetToBookMarkStr[]			= "DbiSetToBookMark";
static  const char DbiCompareBookMarksStr[]		= "DbiCompareBookMarks";
// Data Access: Logical Record Level
static  const char DbiGetNextRecordStr[]			= "DbiGetNextRecord";
static  const char DbiGetPriorRecordStr[]			= "DbiGetPriorRecord";
static  const char DbiGetRecordStr[]					= "DbiGetRecord";
static  const char DbiGetRelativeRecordStr[]	= "DbiGetRelativeRecord";
static  const char DbiInitRecordStr[]					= "DbiInitRecord";
static  const char DbiInsertRecordStr[]				= "DbiInsertRecord";
static  const char DbiModifyRecordStr[]				= "DbiModifyRecord";
static  const char DbiDeleteRecordStr[]				= "DbiDeleteRecord";
static  const char DbiReadBlockStr[]					= "DbiReadBlock";
static  const char DbiWriteBlockStr[]					= "DbiWriteBlock";
static  const char DbiAppendRecordStr[]				= "DbiAppendRecord";
static  const char DbiUndeleteRecordStr[]			= "DbiUndeleteRecord";
static  const char DbiGetSeqNoStr[]						= "DbiGetSeqNo";
static  const char DbiSetToSeqNoStr[]					= "DbiSetToSeqNo";
static  const char DbiGetRecordCountStr[]			= "DbiGetRecordCount";
static  const char DbiSetToRecordNoStr[]			= "DbiSetToRecordNo";
static  const char DbiSaveChangesStr[]				= "DbiSaveChanges";
static  const char DbiForceRereadStr[]				= "DbiForceReread";
static  const char DbiCheckRefreshStr[]				= "DbiCheckRefresh";
static  const char DbiMakePermanentStr[]			= "DbiMakePermanent";
static  const char DbiForceRecordRereadStr[]	= "DbiForceRecordReread";
// Field Level Access
static  const char DbiGetFieldStr[]						= "DbiGetField";
static  const char DbiPutFieldStr[]						= "DbiPutField";
static  const char DbiVerifyFieldStr[]				= "DbiVerifyField";
static  const char DbiOpenBlobStr[]						= "DbiOpenBlob";
static  const char DbiGetBlobSizeStr[]				= "DbiGetBlobSize";
static  const char DbiGetBlobStr[]						= "DbiGetBlob";
static  const char DbiPutBlobStr[]						= "DbiPutBlob";
static  const char DbiTruncateBlobStr[]				= "DbiTruncateBlob";
static  const char DbiFreeBlobStr[]						= "DbiFreeBlob";
static  const char DbiGetBlobHeadingStr[]			= "DbiGetBlobHeading";
static  const char DbiSetFieldMapStr[]				= "DbiSetFieldMap";
static  const char DbiBeginTranStr[]					= "DbiBeginTran";
static  const char DbiEndTranStr[]						= "DbiEndTran";
static  const char DbiGetTranInfoStr[]				= "DbiGetTranInfo";
static  const char DbiAcqTableLockStr[]				= "DbiAcqTableLock";
static  const char DbiAcqPersistTableLockStr[]= "DbiAcqPersistTableLock";
static  const char DbiRelPersistTableLockStr[]= "DbiRelPersistTableLock";
static  const char DbiRelTableLockStr[]				= "DbiRelTableLock";
static  const char DbiRelRecordLockStr[]			= "DbiRelRecordLock";
static  const char DbiIsRecordLockedStr[]			= "DbiIsRecordLocked";
static  const char DbiIsTableLockedStr[]			= "DbiIsTableLocked";
static  const char DbiIsTableSharedStr[]			= "DbiIsTableShared";
static  const char DbiOpenLockListStr[]				= "DbiOpenLockList";
static  const char DbiOpenUserListStr[]				= "DbiOpenUserList";
static  const char DbiSetLockRetryStr[]				= "DbiSetLockRetry";
// Batch Operations
static  const char DbiBatchMoveStr[]					= "DbiBatchMove";
static  const char DbiCopyTableStr[]					= "DbiCopyTable";
static  const char DbiEmptyTableStr[]					= "DbiEmptyTable";
static  const char DbiPackTableStr[]					= "DbiPackTable";
static  const char DbiRegenIndexStr[]					= "DbiRegenIndex";
static  const char DbiRegenIndexesStr[]				= "DbiRegenIndexes";
static  const char DbiSortTableStr[]					= "DbiSortTable";
//Create & Restructure
static  const char DbiCreateTableStr[]				= "DbiCreateTable";
static  const char DbiCreateInMemTableStr[]		= "DbiCreateInMemTable";
static  const char DbiCreateTempTableStr[]		= "DbiCreateTempTable";
static  const char DbiDoRestructureStr[]			= "DbiDoRestructure";
static  const char DbiRenameTableStr[]				= "DbiRenameTable";
static  const char DbiDeleteTableStr[]				= "DbiDeleteTable";
static  const char DbiAddIndexStr[]						= "DbiAddIndex";
static  const char DbiDeleteIndexStr[]				= "DbiDeleteIndex";
// Error Info
static  const char DbiGetErrorEntryStr[]			= "DbiGetErrorEntry";
static  const char DbiGetErrorInfoStr[]				= "DbiGetErrorInfo";
static  const char DbiGetErrorStringStr[]			= "DbiGetErrorString";
static  const char DbiGetErrorContextStr[]		= "DbiGetErrorContext";
//Dbi Services
static  const char DbiDateEncodeStr[]					= "DbiDateEncode";
static  const char DbiDateDecodeStr[]					= "DbiDateDecode";
static  const char DbiTimeEncodeStr[]					= "DbiTimeEncode";
static  const char DbiTimeDecodeStr[]					= "DbiTimeDecode";
static  const char DbiTimeStampEncodeStr[]		= "DbiTimeStampEncode";
static  const char DbiTimeStampDecodeStr[]		= "DbiTimeStampDecode";
static  const char DbiBcdFromFloatStr[]				= "DbiBcdFromFloat";
static  const char DbiBcdToFloatStr[]					= "DbiBcdToFloat";
//CallBacks
static  const char DbiRegisterCallBackStr[]		= "DbiRegisterCallBack";
static  const char DbiGetCallBackStr[]				= "DbiGetCallBack";
//Date, time formats
static  const char DbiGetDateFormatStr[]			= "DbiGetDateFormat";
static  const char DbiSetDateFormatStr[]			= "DbiSetDateFormat";
static  const char DbiGetTimeFormatStr[]			= "DbiGetTimeFormat";
static  const char DbiSetTimeFormatStr[]			= "DbiSetTimeFormat";
static  const char DbiGetNumberFormatStr[]		= "DbiGetNumberFormat";
static  const char DbiSetNumberFormatStr[]		= "DbiSetNumberFormat";
//Conversions
//...
//Filters
static  const char DbiAddFilterStr[]					= "DbiAddFilter";
static  const char DbiDropFilterStr[]					= "DbiDropFilter";
static  const char DbiActivateFilterStr[]			= "DbiActivateFilter";
static  const char DbiDeactivateFilterStr[]		= "DbiDeactivateFilter";
static  const char DbiGetFilterInfoStr[]			= "DbiGetFilterInfo";
//Linked Cursors
static  const char DbiBeginLinkModeStr[]			= "DbiBeginLinkMode";
static  const char DbiEndLinkModeStr[]				= "DbiEndLinkMode";
static  const char DbiLinkDetailStr[]					= "DbiLinkDetail";
static  const char DbiLinkDetailToExpStr[]		= "DbiLinkDetailToExp";
static  const char DbiUnlinkDetailStr[]				= "DbiUnlinkDetail";
static  const char DbiGetLinkStatusStr[]			= "DbiGetLinkStatus";
//Translation
static  const char DbiNativeToAnsiStr[]				= "DbiNativeToAnsi";
static  const char DbiAnsiToNativeStr[]		  	= "DbiAnsiToNative";
//Delayed Updates
//...
static  const char DbiBeginDelayedUpdatesStr[]= "DbiBeginDelayedUpdates";
static  const char DbiEndDelayedUpdatesStr[]	= "DbiEndDelayedUpdates";
static  const char DbiApplyDelayedUpdatesStr[]= "DbiApplyDelayedUpdates";
//MISC.
static  const char DbiGetTableOpenCountStr[]	= "DbiGetTableOpenCount";
static  const char DbiUseIdleTimeStr[]				= "DbiUseIdleTime";
static  const char DbiGetLdObjStr[]						= "DbiGetLdObj";
static  const char DbiGetLdNameStr[]					= "DbiGetLdName";
static  const char DbiFormFullNameStr[]				= "DbiFormFullName";
static  const char DbiAddPasswordStr[]				= "DbiAddPassword";
static  const char DbiDropPasswordStr[]				= "DbiDropPassword";
static  const char DbiGetNetUserNameStr[]			= "DbiGetNetUserName";
static  const char DbiDebugLayerOptionsStr[]	= "DbiDebugLayerOptions";
static  const char DbiOpenCfgInfoListStr[]		= "DbiOpenCfgInfoList";
static  const char DbiAddAliasStr[]						= "DbiAddAlias";
static  const char DbiDeleteAliasStr[]				= "DbiDeleteAlias";
//Query Management
static  const char DbiQExecDirectStr[]				= "DbiQExecDirect";
static  const char DbiQAllocStr[]							= "DbiQAlloc";
static  const char DbiQPrepareStr[]						= "DbiQPrepare";
static  const char DbiQPrepareExtStr[]				= "DbiQPrepareExt";
static  const char DbiQExecStr[]							= "DbiQExec";
static  const char DbiQFreeStr[]							= "DbiQFree";
static  const char DbiQSetParamsStr[]					= "DbiQSetParams";
static  const char DbiQInstantiateAnswerStr[] = "DbiQInstantiateAnswer";
static  const char DbiQExecProcDirectStr[]		= "DbiQExecProcDirect";
static  const char DbiQPrepareProcStr[]				= "DbiQPrepareProc";
static  const char DbiQSetProcParamsStr[]			= "DbiQSetProcParams";
static  const char DbiQGetBaseDescsStr[]			= "DbiQGetBaseDescs";

# if defined(UNICODE)
//    static const char ChooseColorStr[]		= "ChooseColorW";
# else
//    static const char ChooseColorStr[]		= "ChooseColorA";
# endif

//
//
//
static owl_string GetDLLPath()
{
	owl_string dll_name = _T("");
	TCHAR szData[MAX_PATH];
	uint32 size		= COUNTOF(szData);
	uint32 type;
	if(TRegKey(HKEY_LOCAL_MACHINE, _T("Software\\Borland\\Database Engine")).QueryValue(
		_T("DLLPATH"),&type,(uint8*)szData, &size)==ERROR_SUCCESS){
			dll_name = szData;
			dll_name += _T("\\");
	}
	dll_name += idapiStr;
	return dll_name;
}

//-------------------------------------------------------------------
// Returns TModule object wrapping the handle of the IDAPI32.DLL module
//
TModule&
TIdApi32::GetModule()
{
	static TModule idapiModule(GetDLLPath().c_str(), true, true, false);
	return idapiModule;
}
//
DBIResult TIdApi32::DbiInit(pDBIEnv pEnv, UINT16 iVer)
{
	static TModuleProc2<DBIResult,UINT16, pDBIEnv>
		dbiInit(GetModule(), DbiInitFnStr);
	return dbiInit(iVer, pEnv);
}
//
DBIResult TIdApi32::DbiExit()
{
	static TModuleProc0<DBIResult> dbiExit(GetModule(), DbiExitStr);
	return dbiExit();
}
//
DBIResult TIdApi32::DbiGetSysVersion (pSYSVersion psysVersion)
{
	static TModuleProc1<DBIResult,pSYSVersion> 
		dbiGetSysVersion(GetModule(), DbiGetSysVersionStr);
	return dbiGetSysVersion(psysVersion);
}
//
DBIResult TIdApi32::DbiGetSysConfig (pSYSConfig psysConfig)
{
	static TModuleProc1<DBIResult,pSYSConfig> 
		dbiGetSysConfig(GetModule(), DbiGetSysConfigStr);
	return dbiGetSysConfig(psysConfig);
}
//
DBIResult TIdApi32::DbiGetClientInfo (pCLIENTInfo pclientInfo)
{
	static TModuleProc1<DBIResult,pCLIENTInfo> 
		dbiGetClientInfo(GetModule(), DbiGetClientInfoStr);
	return dbiGetClientInfo(pclientInfo);
}
//
DBIResult TIdApi32::DbiGetSysInfo (pSYSInfo psysInfo)
{
	static TModuleProc1<DBIResult,pSYSInfo> 
		dbiGetSysInfo(GetModule(), DbiGetSysInfoStr);
	return dbiGetSysInfo(psysInfo);
}
//
DBIResult TIdApi32::DbiLoadDriver (pCHAR pszDriverType)
{
	static TModuleProc1<DBIResult, pCHAR>
		dbiLoadDriver(GetModule(), DbiLoadDriverStr);
	return dbiLoadDriver(pszDriverType);
}
//
DBIResult TIdApi32::DbiStartSession (pCHAR pszName, phDBISes phSes, pCHAR pNetDir)
{
	static TModuleProc3<DBIResult, pCHAR, phDBISes, pCHAR>
		dbiStartSession(GetModule(), DbiStartSessionStr);
	return dbiStartSession(pszName, phSes, pNetDir);
}
//
DBIResult TIdApi32::DbiGetCurrSession ( phDBISes phSes)
{
	static TModuleProc1<DBIResult, phDBISes>
		dbiGetCurrSession(GetModule(), DbiGetCurrSessionStr);
	return dbiGetCurrSession(phSes);
}
//
DBIResult TIdApi32::DbiSetCurrSession ( hDBISes hSes)
{
	static TModuleProc1<DBIResult, hDBISes>
		dbiSetCurrSession(GetModule(), DbiSetCurrSessionStr);
	return dbiSetCurrSession(hSes);
}
//
DBIResult TIdApi32::DbiCloseSession (hDBISes hSes)
{
	static TModuleProc1<DBIResult, hDBISes>
		dbiCloseSession(GetModule(), DbiCloseSessionStr);
	return dbiCloseSession(hSes);
}
//
DBIResult TIdApi32::DbiGetSesInfo (pSESInfo psesInfo)
{
	static TModuleProc1<DBIResult, pSESInfo>
		dbiGetSesInfo(GetModule(), DbiGetSesInfoStr);
	return dbiGetSesInfo(psesInfo);
}
//
DBIResult TIdApi32::DbiSetPrivateDir (pCHAR pszDir)
{
	static TModuleProc1<DBIResult, pCHAR>
		dbiSetPrivateDir(GetModule(), DbiSetPrivateDirStr);
	return dbiSetPrivateDir(pszDir);
}
//
DBIResult TIdApi32::DbiOpenDatabase (pCHAR pszDbName, pCHAR pszDbType, DBIOpenMode eOpenMode,DBIShareMode eShareMode, pCHAR pszPassword, UINT16 iOptFlds, pFLDDesc pOptFldDesc, pBYTE pOptParams, phDBIDb phDb)
{
	static TModuleProc9<DBIResult,pCHAR,pCHAR,DBIOpenMode,DBIShareMode,pCHAR,UINT16,pFLDDesc,pBYTE,phDBIDb>
		dbiOpenDatabase(GetModule(), DbiOpenDatabaseStr);
	return dbiOpenDatabase(pszDbName,pszDbType,eOpenMode,eShareMode,pszPassword,iOptFlds,pOptFldDesc,pOptParams,phDb);
}
//
DBIResult TIdApi32::DbiSetDirectory (hDBIDb hDb, pCHAR pszDir)
{
	static TModuleProc2<DBIResult, hDBIDb,pCHAR>
		dbiSetDirectory(GetModule(), DbiSetDirectoryStr);
	return dbiSetDirectory(hDb, pszDir);
}
//
DBIResult TIdApi32::DbiGetDirectory (hDBIDb hDb, BOOL bDefault, pCHAR pszDir)
{
	static TModuleProc3<DBIResult, hDBIDb,BOOL,pCHAR>
		dbiGetDirectory(GetModule(), DbiGetDirectoryStr);
	return dbiGetDirectory(hDb,bDefault,pszDir);
}
//
DBIResult TIdApi32::DbiOpenTableList (hDBIDb  hDb, BOOL bExtended, BOOL bSystem, pCHAR pszWild, phDBICur phCur)
{
	static TModuleProc5<DBIResult,hDBIDb,BOOL,BOOL,pCHAR,phDBICur>
		dbiOpenTableList(GetModule(), DbiOpenTableListStr);
	return dbiOpenTableList(hDb,bExtended,bSystem,pszWild,phCur);
}
//
DBIResult TIdApi32::DbiOpenFileList (hDBIDb hDb,pCHAR pszWild,phDBICur phCur)
{
	static TModuleProc3<DBIResult, hDBIDb,pCHAR,phDBICur>
		dbiOpenFileList(GetModule(), DbiOpenFileListStr);
	return dbiOpenFileList(hDb,pszWild,phCur);
}
//
DBIResult TIdApi32::DbiOpenIndexList(hDBIDb hDb, pCHAR pszTableName, pCHAR pszDriverType, phDBICur phCur)
{
	static TModuleProc4<DBIResult, hDBIDb,pCHAR,pCHAR,phDBICur>
		dbiOpenIndexList(GetModule(), DbiOpenIndexListStr);
	return dbiOpenIndexList(hDb, pszTableName, pszDriverType, phCur);
}
//
DBIResult TIdApi32::DbiOpenFieldList(hDBIDb hDb, pCHAR pszTableName, pCHAR pszDriverType, BOOL bPhyTypes, phDBICur phCur)
{
	static TModuleProc5<DBIResult, hDBIDb, pCHAR, pCHAR, BOOL, phDBICur>
		dbiOpenFieldList(GetModule(), DbiOpenFieldListStr);
	return dbiOpenFieldList(hDb, pszTableName, pszDriverType, bPhyTypes, phCur);
}
//
DBIResult TIdApi32::DbiOpenVchkList ( hDBIDb hDb,pCHAR pszTableName,pCHAR pszDriverType,phDBICur phChkCur)
{
	static TModuleProc4<DBIResult, hDBIDb,pCHAR,pCHAR,phDBICur>
		dbiOpenVchkList(GetModule(), DbiOpenVchkListStr);
	return dbiOpenVchkList(hDb,pszTableName,pszDriverType,phChkCur);
}
//
DBIResult TIdApi32::DbiOpenRintList (hDBIDb hDb, pCHAR pszTableName, pCHAR pszDriverType,phDBICur phChkCur)
{
	static TModuleProc4<DBIResult, hDBIDb, pCHAR, pCHAR,phDBICur>
		dbiOpenRintList(GetModule(), DbiOpenRintListStr);
	return dbiOpenRintList(hDb,pszTableName,pszDriverType,phChkCur);
}
//
DBIResult TIdApi32::DbiOpenSecurityList ( hDBIDb hDb,pCHAR pszTableName,pCHAR pszDriverType,phDBICur phSecCur)
{
	static TModuleProc4<DBIResult, hDBIDb,pCHAR,pCHAR,phDBICur>
		dbiOpenSecurityList(GetModule(), DbiOpenSecurityListStr);
	return dbiOpenSecurityList(hDb,pszTableName,pszDriverType,phSecCur);
}
//
DBIResult TIdApi32::DbiOpenFamilyList ( hDBIDb hDb, pCHAR pszTableName, pCHAR pszDriverType, phDBICur phFmlCur)
{
	static TModuleProc4<DBIResult, hDBIDb, pCHAR, pCHAR, phDBICur>
		dbiOpenFamilyList(GetModule(), DbiOpenFamilyListStr);
	return dbiOpenFamilyList(hDb, pszTableName, pszDriverType, phFmlCur);
}
//
DBIResult TIdApi32::DbiOpenSPList( hDBIDb hDb, BOOL bExtended, BOOL bSystem, pCHAR pszQual, phDBICur phCur)
{
	static TModuleProc5<DBIResult,hDBIDb,BOOL,BOOL,pCHAR,phDBICur>
		dbiOpenSPList(GetModule(), DbiOpenSPListStr);
	return dbiOpenSPList(hDb,bExtended,bSystem,pszQual,phCur);
}
//
DBIResult TIdApi32::DbiOpenSPParamList (hDBIDb hDb,pCHAR pszSPName,BOOL bPhyTypes,UINT16 uOverload,phDBICur phCur)
{
	static TModuleProc5<DBIResult,hDBIDb,pCHAR,BOOL,UINT16,phDBICur>
		dbiOpenSPParamList(GetModule(), DbiOpenSPParamListStr);
	return dbiOpenSPParamList(hDb,pszSPName,bPhyTypes,uOverload,phCur);
}
//
DBIResult TIdApi32::DbiOpenFunctionList (hDBIDb hDb,DBIFUNCOpts eoptBits,phDBICur phCur)
{
	static TModuleProc3<DBIResult, hDBIDb,DBIFUNCOpts,phDBICur>
		dbiOpenFunctionList(GetModule(), DbiOpenFunctionListStr);
	return dbiOpenFunctionList(hDb,eoptBits,phCur);
}
//
DBIResult TIdApi32::DbiOpenFunctionArgList (hDBIDb hDb,pCHAR pszFuncName,UINT16 uOverload,phDBICur phCur)
{
	static TModuleProc4<DBIResult, hDBIDb,pCHAR,UINT16,phDBICur>
		dbiOpenFunctionArgList(GetModule(), DbiOpenFunctionArgListStr);
	return dbiOpenFunctionArgList(hDb,pszFuncName,uOverload,phCur);
}
//
DBIResult TIdApi32::DbiCloseDatabase (phDBIDb phDb)
{
	static TModuleProc1<DBIResult, phDBIDb>
		dbiCloseDatabase(GetModule(), DbiCloseDatabaseStr);
	return dbiCloseDatabase(phDb);
}
//
DBIResult TIdApi32::DbiOpenDriverList (phDBICur phCur)
{
	static TModuleProc1<DBIResult, phDBICur>
		dbiOpenDriverList(GetModule(), DbiOpenDriverListStr);
	return dbiOpenDriverList(phCur);
}
//
DBIResult TIdApi32::DbiGetDriverDesc (pCHAR pszDriverType, pDRVType pdrvType)
{
	static TModuleProc2<DBIResult, pCHAR,pDRVType>
		dbiGetDriverDesc(GetModule(), DbiGetDriverDescStr);
	return dbiGetDriverDesc(pszDriverType,pdrvType);
}
//
DBIResult TIdApi32::DbiOpenDatabaseList(phDBICur phCur)
{
	static TModuleProc1<DBIResult, phDBICur>
		dbiOpenDatabaseList(GetModule(), DbiOpenDatabaseListStr);
	return dbiOpenDatabaseList(phCur);
}
//
DBIResult TIdApi32::DbiGetDatabaseDesc (pCHAR pszName, pDBDesc pdbDesc)
{
	static TModuleProc2<DBIResult, pCHAR,pDBDesc>
		dbiGetDatabaseDesc(GetModule(), DbiGetDatabaseDescStr);
	return dbiGetDatabaseDesc(pszName,pdbDesc);
}
//
DBIResult TIdApi32::DbiOpenTableTypesList (pCHAR pszDriverType,phDBICur phCur)
{
	static TModuleProc2<DBIResult, pCHAR,phDBICur>
		dbiOpenTableTypesList(GetModule(), DbiOpenTableTypesListStr);
	return dbiOpenTableTypesList(pszDriverType,phCur);
}
//
DBIResult TIdApi32::DbiGetTableTypeDesc (pCHAR pszDriverType,pCHAR pszTableType,pTBLType ptblType)
{
	static TModuleProc3<DBIResult,pCHAR,pCHAR,pTBLType>
		dbiGetTableTypeDesc(GetModule(), DbiGetTableTypeDescStr);
	return dbiGetTableTypeDesc(pszDriverType,pszTableType,ptblType);
}
//
DBIResult TIdApi32::DbiOpenFieldTypesList (pCHAR pszDriverType,pCHAR pszTblType,phDBICur phCur)
{
	static TModuleProc3<DBIResult, pCHAR,pCHAR,phDBICur>
		dbiOpenFieldTypesList(GetModule(), DbiOpenFieldTypesListStr);
	return dbiOpenFieldTypesList(pszDriverType,pszTblType,phCur);
}
//
DBIResult TIdApi32::DbiGetFieldTypeDesc (pCHAR pszDriverType,pCHAR pszTableType,pCHAR pszFieldType,pFLDType pfldType)
{
	static TModuleProc4<DBIResult, pCHAR,pCHAR,pCHAR,pFLDType>
		dbiGetFieldTypeDesc(GetModule(), DbiGetFieldTypeDescStr);
	return dbiGetFieldTypeDesc(pszDriverType,pszTableType,pszFieldType,pfldType);
}
//
DBIResult TIdApi32::DbiOpenIndexTypesList (pCHAR pszDriverType,phDBICur phCur)
{
	static TModuleProc2<DBIResult, pCHAR,phDBICur>
		dbiOpenIndexTypesList(GetModule(), DbiOpenIndexTypesListStr);
	return dbiOpenIndexTypesList(pszDriverType,phCur);
}
//
DBIResult TIdApi32::DbiGetIndexTypeDesc (pCHAR pszDriverType,pCHAR pszIndexType,pIDXType pidxType)
{
	static TModuleProc3<DBIResult, pCHAR,pCHAR,pIDXType>
		dbiGetIndexTypeDesc(GetModule(), DbiGetIndexTypeDescStr);
	return dbiGetIndexTypeDesc(pszDriverType,pszIndexType,pidxType);
}
//
DBIResult TIdApi32::DbiOpenLdList (phDBICur phCur)
{
	static TModuleProc1<DBIResult, phDBICur>
		dbiOpenLdList(GetModule(), DbiOpenLdListStr);
	return dbiOpenLdList(phCur);
}
// Table Open, Properties & Structure
//
DBIResult TIdApi32::DbiOpenTable (hDBIDb hDb,pCHAR pszTableName,pCHAR pszDriverType,pCHAR pszIndexName,pCHAR pszIndexTagName,UINT16 iIndexId,DBIOpenMode eOpenMode,DBIShareMode eShareMode,XLTMode exltMode,BOOL bUniDirectional,pBYTE pOptParams,phDBICur phCursor)
{
	static TModuleProc12<DBIResult, hDBIDb,pCHAR,pCHAR,pCHAR,pCHAR,UINT16,DBIOpenMode,DBIShareMode,XLTMode,BOOL,pBYTE,phDBICur>
		dbiOpenTable(GetModule(), DbiOpenTableStr);
	return dbiOpenTable(hDb,pszTableName,pszDriverType,pszIndexName,pszIndexTagName,iIndexId,eOpenMode,eShareMode,exltMode,bUniDirectional,pOptParams,phCursor);
}
//
DBIResult TIdApi32::DbiGetCursorProps (hDBICur hCursor,pCURProps pcurProps)
{
	static TModuleProc2<DBIResult, hDBICur,pCURProps>
		dbiGetCursorProps(GetModule(), DbiGetCursorPropsStr);
	return dbiGetCursorProps(hCursor,pcurProps);
}
//
DBIResult TIdApi32::DbiGetObjFromName (DBIOBJType eObjType,pCHAR pszObjName,phDBIObj phObj)
{
	static TModuleProc3<DBIResult, DBIOBJType,pCHAR,phDBIObj>
		dbiGetObjFromName(GetModule(), DbiGetObjFromNameStr);
	return dbiGetObjFromName(eObjType,pszObjName,phObj);
}
//
DBIResult TIdApi32::DbiGetObjFromObj (hDBIObj hObj,DBIOBJType eObjType,phDBIObj phObj)
{
	static TModuleProc3<DBIResult, hDBIObj,DBIOBJType,phDBIObj>
		dbiGetObjFromObj(GetModule(), DbiGetObjFromObjStr);
	return dbiGetObjFromObj(hObj,eObjType,phObj);
}
//
DBIResult TIdApi32::DbiGetProp (hDBIObj hObj,UINT32 iProp,pVOID pPropValue,UINT16 iMaxLen,pUINT16 piLen)
{
	static TModuleProc5<DBIResult, hDBIObj,UINT32,pVOID,UINT16,pUINT16>
		dbiGetProp(GetModule(), DbiGetPropStr);
	return dbiGetProp(hObj,iProp,pPropValue,iMaxLen,piLen);
}
//
DBIResult TIdApi32::DbiSetProp (hDBIObj hObj,UINT32 iProp,UINT32 iPropValue)
{
	static TModuleProc3<DBIResult, hDBIObj,UINT32,UINT32>
		dbiSetProp(GetModule(), DbiSetPropStr);
	return dbiSetProp(hObj,iProp,iPropValue);
}
//
DBIResult TIdApi32::DbiValidateProp (hDBIObj hObj,UINT32 iProp,BOOL bSetting)
{
	static TModuleProc3<DBIResult, hDBIObj,UINT32,BOOL>
		dbiValidateProp(GetModule(), DbiValidatePropStr);
	return dbiValidateProp(hObj,iProp,bSetting);
}
//
DBIResult TIdApi32::DbiGetFieldDescs(hDBICur hCursor,pFLDDesc pfldDesc)
{
	static TModuleProc2<DBIResult, hDBICur,pFLDDesc>
		dbiGetFieldDescs(GetModule(), DbiGetFieldDescsStr);
	return dbiGetFieldDescs(hCursor,pfldDesc);
}
//
DBIResult TIdApi32::DbiGetCursorForTable (hDBIDb hDb,pCHAR pszTableName,pCHAR pszDriverType,phDBICur phCursor)
{
	static TModuleProc4<DBIResult, hDBIDb,pCHAR,pCHAR,phDBICur>
		dbiGetCursorForTable(GetModule(), DbiGetCursorForTableStr);
	return dbiGetCursorForTable(hDb,pszTableName,pszDriverType,phCursor);
}
//
DBIResult TIdApi32::DbiCloneCursor (hDBICur hCurSrc,BOOL bReadOnly,BOOL bUniDirectional,phDBICur phCurNew)
{
	static TModuleProc4<DBIResult, hDBICur,BOOL,BOOL,phDBICur>
		dbiCloneCursor(GetModule(), DbiCloneCursorStr);
	return dbiCloneCursor(hCurSrc,bReadOnly,bUniDirectional,phCurNew);
}
//
DBIResult TIdApi32::DbiCloseCursor (phDBICur phCursor)
{
	static TModuleProc1<DBIResult, phDBICur>
		dbiCloseCursor(GetModule(), DbiCloseCursorStr);
	return dbiCloseCursor(phCursor);
}
//
// Index Manipulation & Usage
//
DBIResult TIdApi32::DbiOpenIndex (hDBICur hCursor,pCHAR pszIndexName,UINT16 iIndexId)
{
	static TModuleProc3<DBIResult, hDBICur,pCHAR,UINT16>
		dbiOpenIndex(GetModule(), DbiOpenIndexStr);
	return dbiOpenIndex(hCursor,pszIndexName,iIndexId);
}
//
DBIResult TIdApi32::DbiCloseIndex (hDBICur hCursor,pCHAR pszIndexName,UINT16 iIndexId)
{
	static TModuleProc3<DBIResult, hDBICur,pCHAR,UINT16>
		dbiCloseIndex(GetModule(), DbiCloseIndexStr);
	return dbiCloseIndex(hCursor,pszIndexName,iIndexId);
}
//
DBIResult TIdApi32::DbiSwitchToIndex (phDBICur phCursor,pCHAR pszIndexName,pCHAR pszTagName,UINT16 iIndexId,BOOL bCurrRec)
{
	static TModuleProc5<DBIResult, phDBICur,pCHAR,pCHAR,UINT16,BOOL>
		dbiSwitchToIndex(GetModule(), DbiSwitchToIndexStr);
	return dbiSwitchToIndex(phCursor,pszIndexName,pszTagName,iIndexId,bCurrRec);
}
//
DBIResult TIdApi32::DbiGetIndexDesc (hDBICur hCursor,UINT16 iIndexSeqNo,pIDXDesc pidxDesc)
{
	static TModuleProc3<DBIResult, hDBICur,UINT16,pIDXDesc>
		dbiGetIndexDesc(GetModule(), DbiGetIndexDescStr);
	return dbiGetIndexDesc(hCursor,iIndexSeqNo,pidxDesc);
}
//
DBIResult TIdApi32::DbiGetIndexDescs (hDBICur hCursor,pIDXDesc pidxDesc)
{
	static TModuleProc2<DBIResult, hDBICur,pIDXDesc>
		dbiGetIndexDescs(GetModule(), DbiGetIndexDescsStr);
	return dbiGetIndexDescs(hCursor,pidxDesc);
}
//
DBIResult TIdApi32::DbiGetIndexForField (hDBICur hCursor,UINT16 iFld,BOOL bProdTagOnly,pIDXDesc pidxDesc)
{
	static TModuleProc4<DBIResult, hDBICur,UINT16,BOOL,pIDXDesc>
		dbiGetIndexForField(GetModule(), DbiGetIndexForFieldStr);
	return dbiGetIndexForField(hCursor,iFld,bProdTagOnly,pidxDesc);
}
//
DBIResult TIdApi32::DbiGetIndexSeqNo(hDBICur hCursor,pCHAR pszIndexName,pCHAR pszTagName,UINT16 iIndexId,pUINT16 piIndexSeqNo)
{
	static TModuleProc5<DBIResult, hDBICur,pCHAR,pCHAR,UINT16,pUINT16>
		dbiGetIndexSeqNo(GetModule(), DbiGetIndexSeqNoStr);
	return dbiGetIndexSeqNo(hCursor,pszIndexName,pszTagName,iIndexId,piIndexSeqNo);
}
//
DBIResult TIdApi32::DbiSetToKey(hDBICur hCursor,DBISearchCond eSearchCond,BOOL bDirectKey,UINT16 iFields,UINT16 iLen,pBYTE pBuff)
{
	static TModuleProc6<DBIResult, hDBICur,DBISearchCond,BOOL,UINT16,UINT16,pBYTE>
		dbiSetToKey(GetModule(), DbiSetToKeyStr);
	return dbiSetToKey(hCursor,eSearchCond,bDirectKey,iFields,iLen,pBuff);
}
//
DBIResult TIdApi32::DbiExtractKey(hDBICur hCursor,pBYTE pRecBuf,pBYTE pKeyBuf)
{
	static TModuleProc3<DBIResult, hDBICur,pBYTE,pBYTE>
		dbiExtractKey(GetModule(), DbiExtractKeyStr);
	return dbiExtractKey(hCursor,pRecBuf,pKeyBuf);
}
//
DBIResult TIdApi32::DbiSetRange(hDBICur hCursor,BOOL bKeyItself,UINT16 iFields1,UINT16 iLen1,pBYTE pKey1,BOOL bKey1Incl,UINT16 iFields2,UINT16 iLen2,pBYTE pKey2,BOOL bKey2Incl)
{
	static TModuleProc10<DBIResult, hDBICur,BOOL,UINT16,UINT16,pBYTE,BOOL,UINT16,UINT16,pBYTE,BOOL>
		dbiSetRange(GetModule(), DbiSetRangeStr);
	return dbiSetRange(hCursor,bKeyItself,iFields1,iLen1,pKey1,bKey1Incl,iFields2,iLen2,pKey2,bKey2Incl);
}
//
DBIResult TIdApi32::DbiResetRange (hDBICur hCursor)
{
	static TModuleProc1<DBIResult, hDBICur>
		dbiResetRange(GetModule(), DbiResetRangeStr);
	return dbiResetRange(hCursor);
}
//
DBIResult TIdApi32::DbiCompareKeys (hDBICur hCursor,pBYTE pKey1,pBYTE pKey2,UINT16 iFields,UINT16 iLen,pINT16 piResult)
{
	static TModuleProc6<DBIResult, hDBICur,pBYTE,pBYTE,UINT16,UINT16,pINT16>
		dbiCompareKeys(GetModule(), DbiCompareKeysStr);
	return dbiCompareKeys(hCursor,pKey1,pKey2,iFields,iLen,piResult);
}
//
DBIResult TIdApi32::DbiGetRecordForKey(hDBICur hCursor,BOOL bDirectKey,UINT16 iFields,UINT16 iLen,pBYTE pKey,pBYTE pRecBuff)
{
	static TModuleProc6<DBIResult, hDBICur,BOOL,UINT16,UINT16,pBYTE,pBYTE>
		dbiGetRecordForKey(GetModule(), DbiGetRecordForKeyStr);
	return dbiGetRecordForKey(hCursor,bDirectKey,iFields,iLen,pKey,pRecBuff);
}
// Validity check and referential integrity
DBIResult TIdApi32::DbiGetVchkDesc(hDBICur hCursor,UINT16 iValSeqNo,pVCHKDesc pvalDesc)
{
	static TModuleProc3<DBIResult, hDBICur,UINT16,pVCHKDesc>
		dbiGetVchkDesc(GetModule(), DbiGetVchkDescStr);
	return dbiGetVchkDesc(hCursor,iValSeqNo,pvalDesc);
}
//
DBIResult TIdApi32::DbiGetRintDesc (hDBICur hCursor,UINT16 iRintSeqNo,pRINTDesc printDesc)
{
	static TModuleProc3<DBIResult, hDBICur,UINT16,pRINTDesc>
		dbiGetRintDesc(GetModule(), DbiGetRintDescStr);
	return dbiGetRintDesc(hCursor,iRintSeqNo,printDesc);
}
// Cursor Maintenance
DBIResult TIdApi32::DbiSetToBegin (hDBICur hCursor)
{
	static TModuleProc1<DBIResult, hDBICur>
		dbiSetToBegin(GetModule(), DbiSetToBeginStr);
	return dbiSetToBegin(hCursor);
}
//
DBIResult TIdApi32::DbiSetToEnd(hDBICur hCursor)
{
	static TModuleProc1<DBIResult, hDBICur>
		dbiSetToEnd(GetModule(), DbiSetToEndStr);
	return dbiSetToEnd(hCursor);
}
//
DBIResult TIdApi32::DbiSetToCursor(hDBICur hDest, hDBICur hSrc)
{
	static TModuleProc2<DBIResult, hDBICur,hDBICur>
		dbiSetToCursor(GetModule(), DbiSetToCursorStr);
	return dbiSetToCursor(hDest, hSrc);
}
//
DBIResult TIdApi32::DbiGetBookMark(hDBICur hCur,pBYTE pBookMark)
{
	static TModuleProc2<DBIResult, hDBICur,pBYTE>
		dbiGetBookMark(GetModule(), DbiGetBookMarkStr);
	return dbiGetBookMark(hCur, pBookMark);
}
//
DBIResult TIdApi32::DbiSetToBookMark (hDBICur hCur,pBYTE pBookMark)
{
	static TModuleProc2<DBIResult, hDBICur,pBYTE>
		dbiSetToBookMark(GetModule(), DbiSetToBookMarkStr);
	return dbiSetToBookMark(hCur,pBookMark);
}
//
DBIResult TIdApi32::DbiCompareBookMarks (hDBICur hCur,pBYTE pBookMark1,pBYTE pBookMark2,pCMPBkMkRslt pCmpBkmkResult)
{
	static TModuleProc4<DBIResult, hDBICur,pBYTE,pBYTE,pCMPBkMkRslt>
		dbiCompareBookMarks(GetModule(), DbiCompareBookMarksStr);
	return dbiCompareBookMarks(hCur,pBookMark1,pBookMark2,pCmpBkmkResult);
}
// Data Access: Logical Record Level
DBIResult TIdApi32::DbiGetNextRecord (hDBICur hCursor,DBILockType eLock,pBYTE pRecBuff,pRECProps precProps)
{
	static TModuleProc4<DBIResult, hDBICur,DBILockType,pBYTE,pRECProps>
		dbiGetNextRecord(GetModule(), DbiGetNextRecordStr);
	return dbiGetNextRecord(hCursor,eLock,pRecBuff,precProps);
}
//
DBIResult TIdApi32::DbiGetPriorRecord (hDBICur hCursor,DBILockType eLock,pBYTE pRecBuff,pRECProps precProps)
{
	static TModuleProc4<DBIResult, hDBICur,DBILockType,pBYTE,pRECProps>
		dbiGetPriorRecord(GetModule(), DbiGetPriorRecordStr);
	return dbiGetPriorRecord(hCursor,eLock,pRecBuff,precProps);
}
//
DBIResult TIdApi32::DbiGetRecord (hDBICur hCursor,DBILockType eLock,pBYTE pRecBuff,pRECProps precProps)
{
	static TModuleProc4<DBIResult, hDBICur,DBILockType,pBYTE,pRECProps>
		dbiGetRecord(GetModule(), DbiGetRecordStr);
	return dbiGetRecord(hCursor,eLock,pRecBuff,precProps);
}
//
DBIResult TIdApi32::DbiGetRelativeRecord(hDBICur hCursor,INT32 iPosOffset,DBILockType eLock,pBYTE pRecBuff,pRECProps precProps)
{
	static TModuleProc5<DBIResult, hDBICur,INT32,DBILockType,pBYTE,pRECProps>
		dbiGetRelativeRecord(GetModule(), DbiGetRelativeRecordStr);
	return dbiGetRelativeRecord(hCursor,iPosOffset,eLock,pRecBuff,precProps);
}
//
DBIResult TIdApi32::DbiInitRecord(hDBICur hCursor,pBYTE pRecBuff)
{
	static TModuleProc2<DBIResult, hDBICur,pBYTE>
		dbiInitRecord(GetModule(), DbiInitRecordStr);
	return dbiInitRecord(hCursor, pRecBuff);
}
//
DBIResult TIdApi32::DbiInsertRecord(hDBICur hCursor,DBILockType eLock,pBYTE pRecBuff)
{
	static TModuleProc3<DBIResult, hDBICur,DBILockType,pBYTE>
		dbiInsertRecord(GetModule(), DbiInsertRecordStr);
	return dbiInsertRecord(hCursor,eLock,pRecBuff);
}
//
DBIResult TIdApi32::DbiModifyRecord (hDBICur hCursor,pBYTE pRecBuf,BOOL bFreeLock)
{
	static TModuleProc3<DBIResult, hDBICur,pBYTE,BOOL>
		dbiModifyRecord(GetModule(), DbiModifyRecordStr);
	return dbiModifyRecord(hCursor,pRecBuf,bFreeLock);
}
//
DBIResult TIdApi32::DbiDeleteRecord (hDBICur hCursor,pBYTE pRecBuf)
{
	static TModuleProc2<DBIResult, hDBICur,pBYTE>
		dbiDeleteRecord(GetModule(), DbiDeleteRecordStr);
	return dbiDeleteRecord(hCursor,pRecBuf);
}
//
DBIResult TIdApi32::DbiReadBlock (hDBICur hCursor,pUINT32 piRecords,pBYTE pBuf)
{
	static TModuleProc3<DBIResult, hDBICur,pUINT32,pBYTE>
		dbiReadBlock(GetModule(), DbiReadBlockStr);
	return dbiReadBlock(hCursor,piRecords,pBuf);
}
//
DBIResult TIdApi32::DbiWriteBlock (hDBICur hCursor,pUINT32 piRecords,pBYTE pBuf)
{
	static TModuleProc3<DBIResult, hDBICur,pUINT32,pBYTE>
		dbiWriteBlock(GetModule(), DbiWriteBlockStr);
	return dbiWriteBlock(hCursor,piRecords,pBuf);
}
//
DBIResult TIdApi32::DbiAppendRecord (hDBICur hCursor,pBYTE pRecBuff)
{
	static TModuleProc2<DBIResult, hDBICur,pBYTE>
		dbiAppendRecord(GetModule(), DbiAppendRecordStr);
	return dbiAppendRecord(hCursor,pRecBuff);
}
//
DBIResult TIdApi32::DbiUndeleteRecord (hDBICur hCursor)
{
	static TModuleProc1<DBIResult, hDBICur>
		dbiUndeleteRecord(GetModule(), DbiUndeleteRecordStr);
	return dbiUndeleteRecord(hCursor);
}
//
DBIResult TIdApi32::DbiGetSeqNo(hDBICur hCursor,pUINT32 piSeqNo)
{
	static TModuleProc2<DBIResult, hDBICur,pUINT32>
		dbiGetSeqNo(GetModule(), DbiGetSeqNoStr);
	return dbiGetSeqNo(hCursor,piSeqNo);
}
//
DBIResult TIdApi32::DbiSetToSeqNo (hDBICur hCursor,UINT32 iSeqNo)
{
	static TModuleProc2<DBIResult, hDBICur,UINT32>
		dbiSetToSeqNo(GetModule(), DbiSetToSeqNoStr);
	return dbiSetToSeqNo(hCursor,iSeqNo);
}
//
DBIResult TIdApi32::DbiGetRecordCount(hDBICur hCursor,pUINT32 piRecCount)
{
	static TModuleProc2<DBIResult, hDBICur,pUINT32>
		dbiGetRecordCount(GetModule(), DbiGetRecordCountStr);
	return dbiGetRecordCount(hCursor,piRecCount);
}
//
DBIResult TIdApi32::DbiSetToRecordNo (hDBICur hCursor,UINT32 iRecNo)
{
	static TModuleProc2<DBIResult, hDBICur,UINT32>
		dbiSetToRecordNo(GetModule(), DbiSetToRecordNoStr);
	return dbiSetToRecordNo(hCursor,iRecNo);
}
//
DBIResult TIdApi32::DbiSaveChanges (hDBICur hCursor)
{
	static TModuleProc1<DBIResult, hDBICur>
		dbiSaveChanges(GetModule(), DbiSaveChangesStr);
	return dbiSaveChanges(hCursor);
}
//
DBIResult TIdApi32::DbiForceReread (hDBICur hCursor)
{
	static TModuleProc1<DBIResult, hDBICur>
		dbiForceReread(GetModule(), DbiForceRereadStr);
	return dbiForceReread(hCursor);
}
//
DBIResult TIdApi32::DbiCheckRefresh ()
{
	static TModuleProc0<DBIResult>
		dbiCheckRefresh(GetModule(), DbiCheckRefreshStr);
	return dbiCheckRefresh();
}
//
DBIResult TIdApi32::DbiMakePermanent (hDBICur hCursor,pCHAR pszName,BOOL bOverWrite)
{
	static TModuleProc3<DBIResult, hDBICur,pCHAR,BOOL>
		dbiMakePermanent(GetModule(), DbiMakePermanentStr);
	return dbiMakePermanent(hCursor,pszName,bOverWrite);
}
//
DBIResult TIdApi32::DbiForceRecordReread (hDBICur hCursor,pBYTE pRecBuff)
{
	static TModuleProc2<DBIResult, hDBICur,pBYTE>
		dbiForceRecordReread(GetModule(), DbiForceRecordRereadStr);
	return dbiForceRecordReread(hCursor,pRecBuff);
}
// Field Level Access
DBIResult TIdApi32::DbiGetField(hDBICur hCursor,UINT16 iField,pBYTE pRecBuff,pBYTE pDest,pBOOL pbBlank)
{
	static TModuleProc5<DBIResult, hDBICur,UINT16,pBYTE,pBYTE,pBOOL>
		dbiGetField(GetModule(), DbiGetFieldStr);
	return dbiGetField(hCursor,iField,pRecBuff,pDest,pbBlank);
}
//
DBIResult TIdApi32::DbiPutField(hDBICur hCursor,UINT16 iField,pBYTE pRecBuff,pBYTE pSrc)
{
	static TModuleProc4<DBIResult, hDBICur,UINT16,pBYTE,pBYTE>
		dbiPutField(GetModule(), DbiPutFieldStr);
	return dbiPutField(hCursor,iField,pRecBuff,pSrc);
}
//
DBIResult TIdApi32::DbiVerifyField(hDBICur hCursor,UINT16 iField,pBYTE pSrc,pBOOL pbBlank)
{
	static TModuleProc4<DBIResult, hDBICur,UINT16,pBYTE,pBOOL>
		dbiVerifyField(GetModule(), DbiVerifyFieldStr);
	return dbiVerifyField(hCursor,iField,pSrc,pbBlank);
}
//
DBIResult TIdApi32::DbiOpenBlob(hDBICur hCursor,pBYTE pRecBuf,UINT16 iField,DBIOpenMode eOpenMode)
{
	static TModuleProc4<DBIResult, hDBICur,pBYTE,UINT16,DBIOpenMode>
		dbiOpenBlob(GetModule(), DbiOpenBlobStr);
	return dbiOpenBlob(hCursor,pRecBuf,iField,eOpenMode);
}
//
DBIResult TIdApi32::DbiGetBlobSize(hDBICur hCursor,pBYTE pRecBuf, UINT16 iField, pUINT32 piSize)
{
	static TModuleProc4<DBIResult, hDBICur,pBYTE, UINT16, pUINT32>
		dbiGetBlobSize(GetModule(), DbiGetBlobSizeStr);
	return dbiGetBlobSize(hCursor,pRecBuf,iField,piSize);
}
//
DBIResult TIdApi32::DbiGetBlob(hDBICur hCursor,pBYTE pRecBuf,UINT16 iField,UINT32 iOffSet,UINT32 iLen,pBYTE pDest,pUINT32 piRead)
{
	static TModuleProc7<DBIResult, hDBICur,pBYTE,UINT16,UINT32,UINT32,pBYTE,pUINT32>
		dbiGetBlob(GetModule(), DbiGetBlobStr);
	return dbiGetBlob(hCursor,pRecBuf,iField,iOffSet,iLen,pDest,piRead);
}
//
DBIResult TIdApi32::DbiPutBlob(hDBICur hCursor,pBYTE pRecBuf,UINT16 iField,UINT32 iOffSet,UINT32 iLen,pBYTE pSrc)
{
	static TModuleProc6<DBIResult, hDBICur,pBYTE,UINT16,UINT32,UINT32,pBYTE>
		dbiPutBlob(GetModule(), DbiPutBlobStr);
	return dbiPutBlob(hCursor,pRecBuf,iField,iOffSet,iLen,pSrc);
}
//
DBIResult TIdApi32::DbiTruncateBlob (hDBICur hCursor,pBYTE pRecBuf,UINT16 iField,UINT32 iLen)
{
	static TModuleProc4<DBIResult, hDBICur,pBYTE,UINT16,UINT32>
		dbiTruncateBlob(GetModule(), DbiTruncateBlobStr);
	return dbiTruncateBlob(hCursor,pRecBuf,iField,iLen);
}
//
DBIResult TIdApi32::DbiFreeBlob(hDBICur hCursor,pBYTE pRecBuf,UINT iField)
{
	static TModuleProc3<DBIResult, hDBICur,pBYTE,UINT>
		dbiFreeBlob(GetModule(), DbiFreeBlobStr);
	return dbiFreeBlob(hCursor,pRecBuf,iField);
}
//
DBIResult TIdApi32::DbiGetBlobHeading(hDBICur hCursor,UINT16 iField,pBYTE pRecBuf,pBYTE pDest)
{
	static TModuleProc4<DBIResult, hDBICur,UINT16,pBYTE,pBYTE>
		dbiGetBlobHeading(GetModule(), DbiGetBlobHeadingStr);
	return dbiGetBlobHeading(hCursor,iField,pRecBuf,pDest);
}
//
DBIResult TIdApi32::DbiSetFieldMap(hDBICur hCur,UINT16 iFields,pFLDDesc pFldDesc)
{
	static TModuleProc3<DBIResult, hDBICur,UINT16,pFLDDesc>
		dbiSetFieldMap(GetModule(), DbiSetFieldMapStr);
	return dbiSetFieldMap(hCur,iFields,pFldDesc);
}
//
//TRANSACTIONS
//
DBIResult TIdApi32::DbiBeginTran(hDBIDb hDb,eXILType eXIL,phDBIXact phXact)
{
	static TModuleProc3<DBIResult, hDBIDb,eXILType,phDBIXact>
		dbiBeginTran(GetModule(), DbiBeginTranStr);
	return dbiBeginTran(hDb,eXIL,phXact);
}
//
DBIResult TIdApi32::DbiEndTran (hDBIDb hDb,hDBIXact hXact,eXEnd eEnd)
{
	static TModuleProc3<DBIResult, hDBIDb,hDBIXact,eXEnd>
		dbiEndTran(GetModule(), DbiEndTranStr);
	return dbiEndTran(hDb,hXact,eEnd);
}
//
DBIResult TIdApi32::DbiGetTranInfo (hDBIDb hDb,hDBIXact hXact,pXInfo pxInfo)
{
	static TModuleProc3<DBIResult, hDBIDb,hDBIXact,pXInfo>
		dbiGetTranInfo(GetModule(), DbiGetTranInfoStr);
	return dbiGetTranInfo(hDb,hXact,pxInfo);
}
//
//LOCKING
DBIResult TIdApi32::DbiAcqTableLock(hDBICur hCursor,DBILockType eLockType)
{
	static TModuleProc2<DBIResult, hDBICur,DBILockType>
		dbiAcqTableLock(GetModule(), DbiAcqTableLockStr);
	return dbiAcqTableLock(hCursor,eLockType);
}
//
DBIResult TIdApi32::DbiAcqPersistTableLock (hDBIDb hDb,pCHAR pszTableName,pCHAR pszDriverType)
{
	static TModuleProc3<DBIResult, hDBIDb,pCHAR,pCHAR>
		dbiAcqPersistTableLock(GetModule(), DbiAcqPersistTableLockStr);
	return dbiAcqPersistTableLock(hDb,pszTableName,pszDriverType);
}
//
DBIResult TIdApi32::DbiRelPersistTableLock (hDBIDb hDb,pCHAR pszTableName,pCHAR pszDriverType)
{
	static TModuleProc3<DBIResult, hDBIDb,pCHAR,pCHAR>
		dbiRelPersistTableLock(GetModule(), DbiRelPersistTableLockStr);
	return dbiRelPersistTableLock(hDb,pszTableName,pszDriverType);
}
//
DBIResult TIdApi32::DbiRelTableLock (hDBICur hCursor,BOOL bAll,DBILockType eLockType)
{
	static TModuleProc3<DBIResult, hDBICur,BOOL,DBILockType>
		dbiRelTableLock(GetModule(), DbiRelTableLockStr);
	return dbiRelTableLock(hCursor,bAll,eLockType);
}
//
DBIResult TIdApi32::DbiRelRecordLock(hDBICur hCursor,BOOL bAll)
{
	static TModuleProc2<DBIResult, hDBICur,BOOL>
		dbiRelRecordLock(GetModule(), DbiRelRecordLockStr);
	return dbiRelRecordLock(hCursor,bAll);
}
//
DBIResult TIdApi32::DbiIsRecordLocked (hDBICur hCursor,pBOOL pbLocked)
{
	static TModuleProc2<DBIResult, hDBICur,pBOOL>
		dbiIsRecordLocked(GetModule(), DbiIsRecordLockedStr);
	return dbiIsRecordLocked(hCursor,pbLocked);
}
//
DBIResult TIdApi32::DbiIsTableLocked (hDBICur hCursor,DBILockType epdxLock,pUINT16 piLocks)
{
	static TModuleProc3<DBIResult, hDBICur,DBILockType,pUINT16>
		dbiIsTableLocked(GetModule(), DbiIsTableLockedStr);
	return dbiIsTableLocked(hCursor,epdxLock,piLocks);
}
//
DBIResult TIdApi32::DbiIsTableShared (hDBICur hCursor,pBOOL pbShared)
{
	static TModuleProc2<DBIResult, hDBICur,pBOOL>
		dbiIsTableShared(GetModule(), DbiIsTableSharedStr);
	return dbiIsTableShared(hCursor,pbShared);
}
//
DBIResult TIdApi32::DbiOpenLockList (hDBICur hCursor,BOOL bAllUsers,BOOL bAllLockTypes,phDBICur phLocks)
{
	static TModuleProc4<DBIResult, hDBICur,BOOL,BOOL,phDBICur>
		dbiSetToBegin(GetModule(), DbiOpenLockListStr);
	return dbiSetToBegin(hCursor,bAllUsers,bAllLockTypes,phLocks);
}
//
DBIResult TIdApi32::DbiOpenUserList (phDBICur phUsers)
{
	static TModuleProc1<DBIResult, phDBICur>
		dbiOpenUserList(GetModule(), DbiOpenUserListStr);
	return dbiOpenUserList(phUsers);
}
//
DBIResult TIdApi32::DbiSetLockRetry (INT16 iWait)
{
	static TModuleProc1<DBIResult, INT16>
		dbiSetLockRetry(GetModule(), DbiSetLockRetryStr);
	return dbiSetLockRetry(iWait);
}
//
// Batch Operations
//
DBIResult TIdApi32::DbiBatchMove (pBATTblDesc p1,hDBICur p2,pBATTblDesc p3,hDBICur p4,
								  eBATMode p5,UINT16 p6,pUINT16 p7,pCHAR p8,pCHAR p9,UINT16 p10,pCHAR p11,pCHAR p12,
								  pCHAR p13,pUINT32 p14,pUINT32 p15,pUINT32 p16,BOOL p17,BOOL p18,pUINT32 p19,BOOL p20)
{
	static TModuleProc20<DBIResult, pBATTblDesc,hDBICur,pBATTblDesc,hDBICur,eBATMode,UINT16,
		pUINT16,pCHAR,pCHAR,UINT16,pCHAR,pCHAR,pCHAR,pUINT32,pUINT32,pUINT32,BOOL,BOOL,
		pUINT32,BOOL>
		dbiBatchMove(GetModule(), DbiBatchMoveStr);
	return dbiBatchMove(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20);
}
//
DBIResult TIdApi32::DbiCopyTable(hDBIDb p1,BOOL p2,pCHAR p3,pCHAR p4,pCHAR p5)
{
	static TModuleProc5<DBIResult, hDBIDb,BOOL,pCHAR,pCHAR,pCHAR>
		dbiCopyTable(GetModule(), DbiCopyTableStr);
	return dbiCopyTable(p1,p2,p3,p4,p5);
}
//
DBIResult TIdApi32::DbiEmptyTable(hDBIDb p1,hDBICur p2,pCHAR p3,pCHAR p4)
{
	static TModuleProc4<DBIResult, hDBIDb,hDBICur,pCHAR,pCHAR>
		dbiEmptyTable(GetModule(), DbiEmptyTableStr);
	return dbiEmptyTable(p1,p2,p3,p4);
}
//
DBIResult TIdApi32::DbiPackTable (hDBIDb p1,hDBICur p2,pCHAR p3,pCHAR p4,BOOL p5)
{
	static TModuleProc5<DBIResult, hDBIDb,hDBICur,pCHAR,pCHAR,BOOL>
		dbiPackTable(GetModule(), DbiPackTableStr);
	return dbiPackTable(p1,p2,p3,p4,p5);
}
//
DBIResult TIdApi32::DbiRegenIndex (hDBIDb p1,hDBICur p2,pCHAR p3,pCHAR p4,pCHAR p5,
								   pCHAR p6,UINT16 p7)
{
	static TModuleProc7<DBIResult, hDBIDb,hDBICur,pCHAR,pCHAR,pCHAR,pCHAR,UINT16>
		dbiRegenIndex(GetModule(), DbiRegenIndexStr);
	return dbiRegenIndex(p1,p2,p3,p4,p5,p6,p7);
}
//
DBIResult TIdApi32::DbiRegenIndexes (hDBICur p1)
{
	static TModuleProc1<DBIResult, hDBICur>
		dbiRegenIndexes(GetModule(), DbiRegenIndexesStr);
	return dbiRegenIndexes(p1);
}
//
DBIResult TIdApi32::DbiSortTable (hDBIDb p1,pCHAR p2,pCHAR p3,hDBICur p4,
								  pCHAR p5,phDBICur p6,hDBICur p7,UINT16 p8,pUINT16 p9,pBOOL p10,pSORTOrder p11,
								  pfSORTCompFn* p12,BOOL p13,hDBICur p14,pUINT32 p15)
{
	static TModuleProc15<DBIResult, hDBIDb,pCHAR,pCHAR,hDBICur,pCHAR,phDBICur,hDBICur,
		UINT16,pUINT16,pBOOL,pSORTOrder,pfSORTCompFn*,BOOL,hDBICur,pUINT32>
		dbiSortTable(GetModule(), DbiSortTableStr);
	return dbiSortTable(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15);
}
//
//Create & Restructure
DBIResult TIdApi32::DbiCreateTable(hDBIDb hDb,BOOL bOverWrite,pCRTblDesc pcrTblDsc)
{
	static TModuleProc3<DBIResult, hDBIDb,BOOL,pCRTblDesc>
		dbiCreateTable(GetModule(), DbiCreateTableStr);
	return dbiCreateTable(hDb,bOverWrite,pcrTblDsc);
}
//
DBIResult TIdApi32::DbiCreateInMemTable(hDBIDb hDb,pCHAR pszName,UINT16 iFields,pFLDDesc pfldDesc,phDBICur phCursor)
{
	static TModuleProc5<DBIResult, hDBIDb,pCHAR,UINT16,pFLDDesc,phDBICur>
		dbiCreateInMemTable(GetModule(), DbiCreateInMemTableStr);
	return dbiCreateInMemTable(hDb,pszName,iFields,pfldDesc,phCursor);
}
//
DBIResult TIdApi32::DbiCreateTempTable (hDBIDb hDb,pCRTblDesc pcrTblDsc,phDBICur phCursor)
{
	static TModuleProc3<DBIResult, hDBIDb,pCRTblDesc,phDBICur>
		dbiCreateTempTable(GetModule(), DbiCreateTempTableStr);
	return dbiCreateTempTable(hDb,pcrTblDsc,phCursor);
}
//
DBIResult TIdApi32::DbiDoRestructure (hDBIDb hDb,UINT16 iTblDescCount,pCRTblDesc pTblDesc,pCHAR pszSaveAs,pCHAR pszKeyviolName,pCHAR pszProblemsName,BOOL bAnalyzeOnly)
{
	static TModuleProc7<DBIResult, hDBIDb,UINT16,pCRTblDesc,pCHAR,pCHAR,pCHAR,BOOL>
		dbiDoRestructure(GetModule(), DbiDoRestructureStr);
	return dbiDoRestructure(hDb,iTblDescCount,pTblDesc,pszSaveAs,pszKeyviolName,pszProblemsName,bAnalyzeOnly);
}
//
DBIResult TIdApi32::DbiRenameTable (hDBIDb hDb,pCHAR pszOldName,pCHAR pszDriverType,pCHAR pszNewName)
{
	static TModuleProc4<DBIResult, hDBIDb,pCHAR,pCHAR,pCHAR>
		dbiRenameTable(GetModule(), DbiRenameTableStr);
	return dbiRenameTable(hDb,pszOldName,pszDriverType,pszNewName);
}
//
DBIResult TIdApi32::DbiDeleteTable(hDBIDb hDb,pCHAR pszTableName,pCHAR pszDriverType)
{
	static TModuleProc3<DBIResult, hDBIDb,pCHAR,pCHAR>
		dbiDeleteTable(GetModule(), DbiDeleteTableStr);
	return dbiDeleteTable(hDb,pszTableName,pszDriverType);
}
//
DBIResult TIdApi32::DbiAddIndex(hDBIDb hDb,hDBICur hCursor,pCHAR pszTableName,pCHAR pszDriverType,pIDXDesc pIdxDesc,pCHAR pszKeyviolName)
{
	static TModuleProc6<DBIResult, hDBIDb,hDBICur,pCHAR,pCHAR,pIDXDesc,pCHAR>
		dbiAddIndex(GetModule(), DbiAddIndexStr);
	return dbiAddIndex(hDb,hCursor,pszTableName,pszDriverType,pIdxDesc,pszKeyviolName);
}
//
DBIResult TIdApi32::DbiDeleteIndex(hDBIDb hDb,hDBICur hCursor,pCHAR pszTableName,pCHAR pszDriverType,pCHAR pszIndexName,pCHAR pszIndexTagName,UINT16 iIndexId)
{
	static TModuleProc7<DBIResult, hDBIDb,hDBICur,pCHAR,pCHAR,pCHAR,pCHAR,UINT16>
		dbiDeleteIndex(GetModule(), DbiDeleteIndexStr);
	return dbiDeleteIndex(hDb,hCursor,pszTableName,pszDriverType,pszIndexName,pszIndexTagName,iIndexId);
}
//
// Error Info
DBIResult TIdApi32::DbiGetErrorEntry(UINT16 uEntry, pUINT32 pulNativeError,pCHAR pszError)
{
	static TModuleProc3<DBIResult, UINT16, pUINT32,pCHAR>
		dbiGetErrorEntry(GetModule(), DbiGetErrorEntryStr);
	return dbiGetErrorEntry(uEntry, pulNativeError,pszError);
}
//
DBIResult TIdApi32::DbiGetErrorInfo(BOOL bFull,pDBIErrInfo pErrInfo)
{
	static TModuleProc2<DBIResult, BOOL,pDBIErrInfo>
		dbiOpenUserList(GetModule(), DbiGetErrorInfoStr);
	return dbiOpenUserList(bFull,pErrInfo);
}
//
DBIResult TIdApi32::DbiGetErrorString(DBIResult rslt,pCHAR pszError)
{
	static TModuleProc2<DBIResult, DBIResult, pCHAR>
		dbiGetErrorString(GetModule(), DbiGetErrorStringStr);
	return dbiGetErrorString(rslt,pszError);
}
//
DBIResult TIdApi32::DbiGetErrorContext(INT16 eContext,pCHAR pszContext)
{
	static TModuleProc2<DBIResult, INT16,pCHAR>
		dbiGetErrorContext(GetModule(), DbiGetErrorContextStr);
	return dbiGetErrorContext(eContext,pszContext);
}
//
//Dbi Services
DBIResult TIdApi32::DbiDateEncode(UINT16 iMon,UINT16 iDay,INT16 iYear,pDBIDATE pdateD)
{
	static TModuleProc4<DBIResult, UINT16,UINT16,INT16,pDBIDATE>
		dbiDateEncode(GetModule(), DbiDateEncodeStr);
	return dbiDateEncode(iMon,iDay,iYear,pdateD);
}
//
DBIResult TIdApi32::DbiDateDecode(DBIDATE dateD,pUINT16 piMon,pUINT16 piDay,pINT16 piYear)
{
	static TModuleProc4<DBIResult, DBIDATE,pUINT16,pUINT16,pINT16>
		dbiDateDecode(GetModule(), DbiDateDecodeStr);
	return dbiDateDecode(dateD,piMon,piDay,piYear);
}
//
DBIResult TIdApi32::DbiTimeEncode(UINT16 iHour,UINT16 iMin,UINT16 iMilSec,pTIME ptimeT)
{
	static TModuleProc4<DBIResult, UINT16,UINT16,UINT16,pTIME>
		dbiTimeEncode(GetModule(), DbiTimeEncodeStr);
	return dbiTimeEncode(iHour,iMin,iMilSec,ptimeT);
}
//
DBIResult TIdApi32::DbiTimeDecode(TIME timeT,pUINT16 piHour,pUINT16 piMin,pUINT16 piMilSec)
{
	static TModuleProc4<DBIResult, TIME,pUINT16,pUINT16,pUINT16>
		dbiTimeDecode(GetModule(), DbiTimeDecodeStr);
	return dbiTimeDecode(timeT,piHour,piMin,piMilSec);
}
//
DBIResult TIdApi32::DbiTimeStampEncode(DBIDATE dateD,TIME timeT,pTIMESTAMP ptsTS)
{
	static TModuleProc3<DBIResult, DBIDATE,TIME,pTIMESTAMP>
		dbiTimeStampEncode(GetModule(), DbiTimeStampEncodeStr);
	return dbiTimeStampEncode(dateD,timeT,ptsTS);
}
//
DBIResult TIdApi32::DbiTimeStampDecode(TIMESTAMP tsTS,pDBIDATE pdateD,pTIME ptimeT)
{
	static TModuleProc3<DBIResult, TIMESTAMP,pDBIDATE,pTIME>
		dbiTimeStampDecode(GetModule(), DbiTimeStampDecodeStr);
	return dbiTimeStampDecode(tsTS,pdateD,ptimeT);
}
//
DBIResult TIdApi32::DbiBcdFromFloat(pDFLOAT piVal,UINT16 iPrecision,UINT16 iPlaces,pFMTBcd pBcd)
{
	static TModuleProc4<DBIResult, pDFLOAT,UINT16,UINT16,pFMTBcd>
		dbiBcdFromFloat(GetModule(), DbiBcdFromFloatStr);
	return dbiBcdFromFloat(piVal,iPrecision,iPlaces,pBcd);
}
//
DBIResult TIdApi32::DbiBcdToFloat(pFMTBcd pBcd,pDFLOAT piVal)
{
	static TModuleProc2<DBIResult, pFMTBcd,pDFLOAT>
		dbiBcdToFloat(GetModule(), DbiBcdToFloatStr);
	return dbiBcdToFloat(pBcd,piVal);
}
//
//CallBacks
DBIResult TIdApi32::DbiRegisterCallBack(hDBICur p1,CBType p2,UINT32 p3,UINT16 p4,pVOID p5,pfDBICallBack p6)
{
	static TModuleProc6<DBIResult, hDBICur,CBType,UINT32,UINT16,pVOID,pfDBICallBack>
		dbiRegisterCallBack(GetModule(), DbiRegisterCallBackStr);
	return dbiRegisterCallBack(p1,p2,p3,p4,p5,p6);
}
//
DBIResult TIdApi32::DbiGetCallBack(hDBICur p1,CBType p2,pUINT32 p3,pUINT16 p4,ppVOID p5,ppfDBICallBack p6)
{
	static TModuleProc6<DBIResult, hDBICur,CBType,pUINT32,pUINT16,ppVOID,ppfDBICallBack>
		dbiGetCallBack(GetModule(), DbiGetCallBackStr);
	return dbiGetCallBack(p1,p2,p3,p4,p5,p6);
}
//...
//Date, time formats
DBIResult TIdApi32::DbiGetDateFormat(pFMTDate pfmtDate)
{
	static TModuleProc1<DBIResult, pFMTDate>
		dbiGetDateFormat(GetModule(), DbiGetDateFormatStr);
	return dbiGetDateFormat(pfmtDate);
}
//
DBIResult TIdApi32::DbiSetDateFormat(pFMTDate pfmtDate)
{
	static TModuleProc1<DBIResult, pFMTDate>
		dbiSetDateFormat(GetModule(), DbiSetDateFormatStr);
	return dbiSetDateFormat(pfmtDate);
}
//
DBIResult TIdApi32::DbiGetTimeFormat(pFMTTime pfmtTime)
{
	static TModuleProc1<DBIResult, pFMTTime>
		dbiGetTimeFormat(GetModule(), DbiGetTimeFormatStr);
	return dbiGetTimeFormat(pfmtTime);
}
//
DBIResult TIdApi32::DbiSetTimeFormat(pFMTTime pfmtTime)
{
	static TModuleProc1<DBIResult, pFMTTime>
		dbiSetTimeFormat(GetModule(), DbiSetTimeFormatStr);
	return dbiSetTimeFormat(pfmtTime);
}
//
DBIResult TIdApi32::DbiGetNumberFormat(pFMTNumber pfmtNumber)
{
	static TModuleProc1<DBIResult, pFMTNumber>
		dbiGetNumberFormat(GetModule(), DbiGetNumberFormatStr);
	return dbiGetNumberFormat(pfmtNumber);
}
//
DBIResult TIdApi32::DbiSetNumberFormat(pFMTNumber pfmtNumber)
{
	static TModuleProc1<DBIResult, pFMTNumber>
		dbiSetNumberFormat(GetModule(), DbiSetNumberFormatStr);
	return dbiSetNumberFormat(pfmtNumber);
}
//
//Conversions
//...
//Filters
DBIResult TIdApi32::DbiAddFilter(hDBICur p1,UINT32 p2,UINT16 p3,BOOL p4,pCANExpr p5,pfGENFilter p6,phDBIFilter p7)
{
	static TModuleProc7<DBIResult, hDBICur,UINT32,UINT16,BOOL,pCANExpr,pfGENFilter,phDBIFilter>
		dbiAddFilter(GetModule(), DbiAddFilterStr);
	return dbiAddFilter(p1,p2,p3,p4,p5,p6,p7);
}
//
DBIResult TIdApi32::DbiDropFilter(hDBICur hCursor,hDBIFilter hFilter)
{
	static TModuleProc2<DBIResult, hDBICur,hDBIFilter>
		dbiDropFilter(GetModule(), DbiDropFilterStr);
	return dbiDropFilter(hCursor,hFilter);
}
//
DBIResult TIdApi32::DbiActivateFilter(hDBICur hCursor,hDBIFilter hFilter)
{
	static TModuleProc2<DBIResult, hDBICur,hDBIFilter>
		dbiActivateFilter(GetModule(), DbiActivateFilterStr);
	return dbiActivateFilter(hCursor,hFilter);
}
//
DBIResult TIdApi32::DbiDeactivateFilter(hDBICur hCursor,hDBIFilter hFilter)
{
	static TModuleProc2<DBIResult, hDBICur,hDBIFilter>
		dbiDeactivateFilter(GetModule(), DbiDeactivateFilterStr);
	return dbiDeactivateFilter(hCursor,hFilter);
}
//
DBIResult TIdApi32::DbiGetFilterInfo(hDBICur hCur,hDBIFilter hFilter,UINT16 iFilterId,UINT16 iFilterSeqNo,pFILTERInfo pFilterinfo)
{
	static TModuleProc5<DBIResult, hDBICur,hDBIFilter,UINT16,UINT16,pFILTERInfo>
		dbiGetFilterInfo(GetModule(), DbiGetFilterInfoStr);
	return dbiGetFilterInfo(hCur,hFilter,iFilterId,iFilterSeqNo,pFilterinfo);
}
//
//Linked Cursors
DBIResult TIdApi32::DbiBeginLinkMode(phDBICur phCursor)
{
	static TModuleProc1<DBIResult, phDBICur>
		dbiBeginLinkMode(GetModule(), DbiBeginLinkModeStr);
	return dbiBeginLinkMode(phCursor);
}
//
DBIResult TIdApi32::DbiEndLinkMode(phDBICur phCursor)
{
	static TModuleProc1<DBIResult, phDBICur>
		dbiEndLinkMode(GetModule(), DbiEndLinkModeStr);
	return dbiEndLinkMode(phCursor);
}
//
DBIResult TIdApi32::DbiLinkDetail(hDBICur p1,hDBICur p2,UINT16 p3,pUINT16 p4,pUINT16 p5)
{
	static TModuleProc5<DBIResult, hDBICur,hDBICur,UINT16,pUINT16,pUINT16>
		dbiLinkDetail(GetModule(), DbiLinkDetailStr);
	return dbiLinkDetail(p1,p2,p3,p4,p5);
}
//
DBIResult TIdApi32::DbiLinkDetailToExp(hDBICur p1,hDBICur p2,UINT16 p3,pCHAR p4)
{
	static TModuleProc4<DBIResult, hDBICur,hDBICur,UINT16,pCHAR>
		dbiLinkDetailToExp(GetModule(), DbiLinkDetailToExpStr);
	return dbiLinkDetailToExp(p1,p2,p3,p4);
}
//
DBIResult TIdApi32::DbiUnlinkDetail(hDBICur hDetlCursor)
{
	static TModuleProc1<DBIResult, hDBICur>
		dbiUnlinkDetail(GetModule(), DbiUnlinkDetailStr);
	return dbiUnlinkDetail(hDetlCursor);
}
//
DBIResult TIdApi32::DbiGetLinkStatus(hDBICur p1,phDBICur p2,phDBICur p3,phDBICur p4)
{
	static TModuleProc4<DBIResult, hDBICur,phDBICur,phDBICur,phDBICur>
		dbiGetLinkStatus(GetModule(), DbiGetLinkStatusStr);
	return dbiGetLinkStatus(p1,p2,p3,p4);
}
//
//Translation
DBIResult TIdApi32::DbiNativeToAnsi(pVOID p1,pCHAR p2,pCHAR p3,UINT16 p4, pBOOL p5)
{
	static TModuleProc5<DBIResult, pVOID,pCHAR,pCHAR,UINT16,pBOOL>
		dbiNativeToAnsi(GetModule(), DbiNativeToAnsiStr);
	return dbiNativeToAnsi(p1,p2,p3,p4,p5);
}
//
DBIResult TIdApi32::DbiAnsiToNative(pVOID p1,pCHAR p2,pCHAR p3,UINT16 p4, pBOOL p5)
{
	static TModuleProc5<DBIResult, pVOID,pCHAR,pCHAR,UINT16,pBOOL>
		dbiAnsiToNative(GetModule(), DbiAnsiToNativeStr);
	return dbiAnsiToNative(p1,p2,p3,p4,p5);
}
//Delayed Updates
DBIResult TIdApi32::DbiBeginDelayedUpdates(phDBICur phCursor)
{
	static TModuleProc1<DBIResult, phDBICur>
		dbiBeginDelayedUpdates(GetModule(), DbiBeginDelayedUpdatesStr);
	return dbiBeginDelayedUpdates(phCursor);
}
//
DBIResult TIdApi32::DbiEndDelayedUpdates(phDBICur phCursor)
{
	static TModuleProc1<DBIResult, phDBICur>
		dbiEndDelayedUpdates(GetModule(), DbiEndDelayedUpdatesStr);
	return dbiEndDelayedUpdates(phCursor);
}
//
DBIResult TIdApi32::DbiApplyDelayedUpdates(hDBICur hCursor,DBIDelayedUpdCmd eUpdCmd)
{
	static TModuleProc2<DBIResult, hDBICur,DBIDelayedUpdCmd>
		dbiApplyDelayedUpdates(GetModule(), DbiApplyDelayedUpdatesStr);
	return dbiApplyDelayedUpdates(hCursor,eUpdCmd);
}
//MISC.
DBIResult TIdApi32::DbiGetTableOpenCount(hDBIDb p1,pCHAR p2,pCHAR p3,pUINT16 p4)
{
	static TModuleProc4<DBIResult, hDBIDb,pCHAR,pCHAR,pUINT16>
		dbiGetTableOpenCount(GetModule(), DbiGetTableOpenCountStr);
	return dbiGetTableOpenCount(p1,p2,p3,p4);
}
//
DBIResult TIdApi32::DbiUseIdleTime ()
{
	static TModuleProc0<DBIResult>
		dbiUseIdleTime(GetModule(), DbiUseIdleTimeStr);
	return dbiUseIdleTime();
}
//
DBIResult TIdApi32::DbiGetLdObj (hDBICur hCursor,pVOID* ppLdObj)
{
	static TModuleProc2<DBIResult, hDBICur,pVOID*>
		dbiGetLdObj(GetModule(), DbiGetLdObjStr);
	return dbiGetLdObj(hCursor,ppLdObj);
}
//
DBIResult TIdApi32::DbiGetLdName (pCHAR pszDriver,pCHAR pObjName,pCHAR pLdName)
{
	static TModuleProc3<DBIResult, pCHAR,pCHAR,pCHAR>
		dbiGetLdName(GetModule(), DbiGetLdNameStr);
	return dbiGetLdName(pszDriver,pObjName,pLdName);
}
//
DBIResult TIdApi32::DbiFormFullName (hDBIDb hDb,pCHAR pszTableName,pCHAR pszDriverType,pCHAR pszFullName)
{
	static TModuleProc4<DBIResult, hDBIDb,pCHAR,pCHAR,pCHAR>
		dbiFormFullName(GetModule(), DbiFormFullNameStr);
	return dbiFormFullName(hDb,pszTableName,pszDriverType,pszFullName);
}
//
DBIResult TIdApi32::DbiAddPassword (pCHAR pszPassword)
{
	static TModuleProc1<DBIResult, pCHAR>
		dbiAddPassword(GetModule(), DbiAddPasswordStr);
	return dbiAddPassword(pszPassword);
}
//
DBIResult TIdApi32::DbiDropPassword (pCHAR pszPassword)
{
	static TModuleProc1<DBIResult, pCHAR>
		dbiDropPassword(GetModule(), DbiDropPasswordStr);
	return dbiDropPassword(pszPassword);
}
//
DBIResult TIdApi32::DbiGetNetUserName (pCHAR pszNetUserName)
{
	static TModuleProc1<DBIResult, pCHAR>
		dbiGetNetUserName(GetModule(), DbiGetNetUserNameStr);
	return dbiGetNetUserName(pszNetUserName);
}
//
DBIResult TIdApi32::DbiDebugLayerOptions (UINT16 iOption,pCHAR pDebugFile)
{
	static TModuleProc2<DBIResult, UINT16,pCHAR>
		dbiDebugLayerOptions(GetModule(), DbiDebugLayerOptionsStr);
	return dbiDebugLayerOptions(iOption,pDebugFile);
}
//
DBIResult TIdApi32::DbiOpenCfgInfoList(hDBICfg hCfg,DBIOpenMode eOpenMode,CFGMode eConfigMode,pCHAR pszCfgPath,phDBICur phCur)
{
	static TModuleProc5<DBIResult, hDBICfg,DBIOpenMode,CFGMode,pCHAR,phDBICur>
		dbiOpenCfgInfoList(GetModule(), DbiOpenCfgInfoListStr);
	return dbiOpenCfgInfoList(hCfg,eOpenMode,eConfigMode,pszCfgPath,phCur);
}
//
DBIResult TIdApi32::DbiAddAlias(hDBICfg hCfg,pCHAR pszAliasName,pCHAR pszDriverType,pCHAR pszParams,BOOL bPersist)
{
	static TModuleProc5<DBIResult, hDBICfg,pCHAR,pCHAR,pCHAR,BOOL>
		dbiAddAlias(GetModule(), DbiAddAliasStr);
	return dbiAddAlias(hCfg,pszAliasName,pszDriverType,pszParams,bPersist);
}
//
DBIResult TIdApi32::DbiDeleteAlias (hDBICfg hCfg,pCHAR pszAliasName)
{
	static TModuleProc2<DBIResult, hDBICfg,pCHAR>
		dbiDeleteAlias(GetModule(), DbiDeleteAliasStr);
	return dbiDeleteAlias(hCfg,pszAliasName);
}
//Query Management
DBIResult TIdApi32::DbiQExecDirect(hDBIDb hDb,DBIQryLang eQryLang,pCHAR pszQuery,phDBICur phCur)
{
	static TModuleProc4<DBIResult, hDBIDb,DBIQryLang,pCHAR,phDBICur>
		dbiQExecDirect(GetModule(), DbiQExecDirectStr);
	return dbiQExecDirect(hDb,eQryLang,pszQuery,phCur);
}
//
DBIResult TIdApi32::DbiQAlloc(hDBIDb hDb,DBIQryLang eQryLang,phDBIStmt phStmt)
{
	static TModuleProc3<DBIResult, hDBIDb,DBIQryLang,phDBIStmt>
		dbiQAlloc(GetModule(), DbiQAllocStr);
	return dbiQAlloc(hDb,eQryLang,phStmt);
}
//
DBIResult TIdApi32::DbiQPrepare(hDBIStmt hStmt,pCHAR pszQuery)
{
	static TModuleProc2<DBIResult, hDBIStmt,pCHAR>
		dbiQPrepare(GetModule(), DbiQPrepareStr);
	return dbiQPrepare(hStmt,pszQuery);
}
//
DBIResult TIdApi32::DbiQPrepareExt(hDBIDb hDb,DBIQryLang eQryLang,pCHAR pszQuery,UINT16 propBits,phDBIStmt phStmt)
{
	static TModuleProc5<DBIResult, hDBIDb,DBIQryLang,pCHAR,UINT16,phDBIStmt>
		dbiQPrepareExt(GetModule(), DbiQPrepareExtStr);
	return dbiQPrepareExt(hDb,eQryLang,pszQuery,propBits,phStmt);
}
//
DBIResult TIdApi32::DbiQExec(hDBIStmt hStmt,phDBICur phCur)
{
	static TModuleProc2<DBIResult, hDBIStmt,phDBICur>
		dbiQExec(GetModule(), DbiQExecStr);
	return dbiQExec(hStmt,phCur);
}
//
DBIResult TIdApi32::DbiQFree(phDBIStmt phStmt)
{
	static TModuleProc1<DBIResult, phDBIStmt>
		dbiQFree(GetModule(), DbiQFreeStr);
	return dbiQFree(phStmt);
}
//
DBIResult TIdApi32::DbiQSetParams(hDBIStmt hStmt,UINT16 uFldDescs,pFLDDesc paFldDescs,pBYTE pRecBuff)
{
	static TModuleProc4<DBIResult, hDBIStmt,UINT16,pFLDDesc,pBYTE>
		dbiQSetParams(GetModule(), DbiQSetParamsStr);
	return dbiQSetParams(hStmt,uFldDescs,paFldDescs,pRecBuff);
}
//
DBIResult TIdApi32::DbiQInstantiateAnswer(hDBIStmt p1,hDBICur p2,pCHAR p3,pCHAR p4,BOOL p5,phDBICur p6)
{
	static TModuleProc6<DBIResult, hDBIStmt,hDBICur,pCHAR,pCHAR,BOOL,phDBICur>
		dbiQInstantiateAnswer(GetModule(), DbiQInstantiateAnswerStr);
	return dbiQInstantiateAnswer(p1,p2,p3,p4,p5,p6);
}
//
DBIResult TIdApi32::DbiQExecProcDirect (hDBIDb p1,pCHAR p2,UINT16 p3,pSPParamDesc p4,pBYTE p5,phDBICur p6)
{
	static TModuleProc6<DBIResult, hDBIDb,pCHAR,UINT16,pSPParamDesc,pBYTE,phDBICur>
		dbiQExecProcDirect(GetModule(), DbiQExecProcDirectStr);
	return dbiQExecProcDirect(p1,p2,p3,p4,p5,p6);
}
//
DBIResult TIdApi32::DbiQPrepareProc(hDBIDb p1,pCHAR p2,UINT16 p3,pSPParamDesc p4,pBYTE p5,phDBIStmt p6)
{
	static TModuleProc6<DBIResult, hDBIDb,pCHAR,UINT16,pSPParamDesc,pBYTE,phDBIStmt>
		dbiQPrepareProc(GetModule(), DbiQPrepareProcStr);
	return dbiQPrepareProc(p1,p2,p3,p4,p5,p6);
}
//
DBIResult TIdApi32::DbiQSetProcParams(hDBIStmt p1,UINT16 p2,pSPParamDesc p3,pBYTE p4)
{
	static TModuleProc4<DBIResult, hDBIStmt,UINT16,pSPParamDesc,pBYTE>
		dbiQSetProcParams(GetModule(), DbiQSetProcParamsStr);
	return dbiQSetProcParams(p1,p2,p3,p4);
}
//
DBIResult TIdApi32::DbiQGetBaseDescs(hDBIStmt hStmt,phDBICur phCur)
{
	static TModuleProc2<DBIResult, hDBIStmt,phDBICur>
		dbiEndDelayedUpdates(GetModule(), DbiEndDelayedUpdatesStr);
	return dbiEndDelayedUpdates(hStmt,phCur);
}
//
_OWLFX_END_NAMESPACE

/* ========================================================================== */
