//-------------------------------------------------------------------
// OWL File Extensions (OWLFX) Class Library
// OWL NExt Project
// All rights reserved
//
//$Revision: 1.1.4.2 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:38 $
//
//-------------------------------------------------------------------
#if !defined(OWLFX_BDEAPI_H) // Sentry, use file only if it's not already included.
#define OWLFX_BDEAPI_H

#include <owlfx/defs.h>

// Borland database engine definitions.
#if !defined(__FLAT__)
#define __FLAT__
#endif
#if !defined(__WIN32__)
#define __WIN32__
#endif

#include <api_upd\idapi.h>



_OWLFX_BEGIN_NAMESPACE

#undef DbiInit

//
// class TIdApi32
// ~~~~~ ~~~~~~~~
// delay loading IDAPI32.DLL
class _OWLFXCLASS TIdApi32 {
  public:
    static  __OWL TModule&	GetModule();

		//  Environmental
    static  DBIResult DbiInit(pDBIEnv pEnv = 0, UINT16 iVer = DBIINTFVER);
    static  DBIResult DbiExit();        // Exit engine

		//  System Level Info
    static  DBIResult DbiGetSysVersion (pSYSVersion psysVersion);
    static  DBIResult DbiGetSysConfig (pSYSConfig psysConfig);
    static  DBIResult DbiGetClientInfo (pCLIENTInfo pclientInfo);
    static  DBIResult DbiGetSysInfo (pSYSInfo psysInfo);
    static  DBIResult DbiLoadDriver (pCHAR pszDriverType);
		//	Sessions
    static  DBIResult DbiStartSession (pCHAR pszName,phDBISes phSes, pCHAR pNetDir);
    static  DBIResult DbiGetCurrSession ( phDBISes phSes);
    static  DBIResult DbiSetCurrSession ( hDBISes hSes);
    static  DBIResult DbiCloseSession (hDBISes hSes);
    static  DBIResult DbiGetSesInfo (pSESInfo psesInfo);
    static  DBIResult DbiSetPrivateDir (pCHAR pszDir);
		// Datababase, Schema and File inquiry
    static  DBIResult DbiOpenDatabase (pCHAR pszDbName, pCHAR pszDbType, DBIOpenMode eOpenMode,DBIShareMode eShareMode, pCHAR pszPassword, UINT16 iOptFlds, pFLDDesc pOptFldDesc, pBYTE pOptParams, phDBIDb phDb);
    static  DBIResult DbiSetDirectory (hDBIDb hDb, pCHAR pszDir );
    static  DBIResult DbiGetDirectory (hDBIDb hDb, BOOL bDefault, pCHAR pszDir );
    static  DBIResult DbiOpenTableList (hDBIDb  hDb, BOOL bExtended, BOOL bSystem, pCHAR pszWild, phDBICur phCur);
    static  DBIResult DbiOpenFileList (hDBIDb hDb, pCHAR pszWild, phDBICur phCur);
    static  DBIResult DbiOpenIndexList ( hDBIDb hDb, pCHAR pszTableName, pCHAR pszDriverType, phDBICur phCur);
    static  DBIResult DbiOpenFieldList ( hDBIDb hDb, pCHAR pszTableName, pCHAR pszDriverType, BOOL bPhyTypes, phDBICur phCur);
    static  DBIResult DbiOpenVchkList ( hDBIDb hDb,pCHAR pszTableName,pCHAR pszDriverType,phDBICur phChkCur);
    static  DBIResult DbiOpenRintList (hDBIDb hDb,pCHAR pszTableName,pCHAR pszDriverType,phDBICur phChkCur);
    static  DBIResult DbiOpenSecurityList ( hDBIDb hDb,pCHAR pszTableName,pCHAR pszDriverType,phDBICur phSecCur);
    static  DBIResult DbiOpenFamilyList ( hDBIDb hDb, pCHAR pszTableName, pCHAR pszDriverType, phDBICur phFmlCur);
    static  DBIResult DbiOpenSPList( hDBIDb hDb, BOOL bExtended, BOOL bSystem, pCHAR pszQual, phDBICur phCur);
    static  DBIResult DbiOpenSPParamList ( hDBIDb hDb,pCHAR pszSPName,BOOL bPhyTypes,UINT16 uOverload,phDBICur phCur);
    static  DBIResult DbiOpenFunctionList (hDBIDb hDb,DBIFUNCOpts eoptBits,phDBICur phCur);
    static  DBIResult DbiOpenFunctionArgList ( hDBIDb hDb,pCHAR pszFuncName,UINT16 uOverload,phDBICur phCur);
    static  DBIResult DbiCloseDatabase (phDBIDb phDb);
		// Capabilities
    static  DBIResult DbiOpenDriverList (phDBICur phCur);
    static  DBIResult DbiGetDriverDesc (pCHAR pszDriverType, pDRVType pdrvType);
    static  DBIResult DbiOpenDatabaseList(phDBICur phCur);
    static  DBIResult DbiGetDatabaseDesc (pCHAR pszName, pDBDesc pdbDesc);
    static  DBIResult DbiOpenTableTypesList (pCHAR pszDriverType,phDBICur phCur);
    static  DBIResult DbiGetTableTypeDesc (pCHAR pszDriverType,pCHAR pszTableType,pTBLType ptblType);
    static  DBIResult DbiOpenFieldTypesList (pCHAR pszDriverType,pCHAR pszTblType,phDBICur phCur);
    static  DBIResult DbiGetFieldTypeDesc (pCHAR pszDriverType,pCHAR pszTableType,pCHAR pszFieldType,pFLDType pfldType);
    static  DBIResult DbiOpenIndexTypesList (pCHAR pszDriverType,phDBICur phCur);
    static  DBIResult DbiGetIndexTypeDesc (pCHAR pszDriverType,pCHAR pszIndexType,pIDXType pidxType);
    static  DBIResult DbiOpenLdList (phDBICur phCur);
		// Table Open, Properties & Structure
    static  DBIResult DbiOpenTable (hDBIDb hDb,pCHAR pszTableName,pCHAR pszDriverType,pCHAR pszIndexName,pCHAR pszIndexTagName,UINT16 iIndexId,DBIOpenMode eOpenMode,DBIShareMode eShareMode,XLTMode exltMode,BOOL bUniDirectional,pBYTE pOptParams,phDBICur phCursor);
    static  DBIResult DbiGetCursorProps (hDBICur hCursor,pCURProps pcurProps);
    static  DBIResult DbiGetObjFromName (DBIOBJType eObjType,pCHAR pszObjName,phDBIObj phObj);
    static  DBIResult DbiGetObjFromObj (hDBIObj hObj,DBIOBJType eObjType,phDBIObj phObj);
    static  DBIResult DbiGetProp (hDBIObj hObj,UINT32 iProp,pVOID pPropValue,UINT16 iMaxLen,pUINT16 piLen);
    static  DBIResult DbiSetProp (hDBIObj hObj,UINT32 iProp,UINT32 iPropValue);
    static  DBIResult DbiValidateProp (hDBIObj hObj,UINT32 iProp,BOOL bSetting);
    static  DBIResult DbiGetFieldDescs(hDBICur hCursor,pFLDDesc pfldDesc);
    static  DBIResult DbiGetCursorForTable (hDBIDb hDb,pCHAR pszTableName,pCHAR pszDriverType,phDBICur phCursor);
    static  DBIResult DbiCloneCursor (hDBICur hCurSrc,BOOL bReadOnly,BOOL bUniDirectional,phDBICur phCurNew);
    static  DBIResult DbiCloseCursor (phDBICur phCursor);
    // Index Manipulation & Usage
    static  DBIResult DbiOpenIndex (hDBICur hCursor,pCHAR pszIndexName,UINT16 iIndexId);
    static  DBIResult DbiCloseIndex (hDBICur hCursor,pCHAR pszIndexName,UINT16 iIndexId);
    static  DBIResult DbiSwitchToIndex (phDBICur phCursor,pCHAR pszIndexName,pCHAR pszTagName,UINT16 iIndexId,BOOL bCurrRec);
    static  DBIResult DbiGetIndexDesc (hDBICur hCursor,UINT16 iIndexSeqNo,pIDXDesc pidxDesc);
    static  DBIResult DbiGetIndexDescs (hDBICur hCursor,pIDXDesc pidxDesc);
    static  DBIResult DbiGetIndexForField (hDBICur hCursor,UINT16 iFld,BOOL bProdTagOnly,pIDXDesc pidxDesc);
    static  DBIResult DbiGetIndexSeqNo(hDBICur hCursor,pCHAR pszIndexName,pCHAR pszTagName,UINT16 iIndexId,pUINT16 piIndexSeqNo);
    static  DBIResult DbiSetToKey  (hDBICur hCursor,DBISearchCond eSearchCond,BOOL bDirectKey,UINT16 iFields,UINT16 iLen,pBYTE pBuff);
    static  DBIResult DbiExtractKey (hDBICur hCursor,pBYTE pRecBuf,pBYTE pKeyBuf);
    static  DBIResult DbiSetRange (hDBICur hCursor,BOOL bKeyItself,UINT16 iFields1,UINT16 iLen1,pBYTE pKey1,BOOL bKey1Incl,UINT16 iFields2,UINT16 iLen2,pBYTE pKey2,BOOL bKey2Incl);
    static  DBIResult DbiResetRange (hDBICur hCursor);
    static  DBIResult DbiCompareKeys (hDBICur hCursor,pBYTE pKey1,pBYTE pKey2,UINT16 iFields,UINT16 iLen,pINT16 piResult);
    static  DBIResult DbiGetRecordForKey (hDBICur hCursor,BOOL bDirectKey,UINT16 iFields,UINT16 iLen,pBYTE pKey,pBYTE pRecBuff);
		// Validity check and referential integrity
    static  DBIResult DbiGetVchkDesc (hDBICur hCursor,UINT16 iValSeqNo,pVCHKDesc pvalDesc);
    static  DBIResult DbiGetRintDesc (hDBICur hCursor,UINT16 iRintSeqNo,pRINTDesc printDesc);
		// Cursor Maintenance
    static  DBIResult DbiSetToBegin (hDBICur hCursor);
    static  DBIResult DbiSetToEnd (hDBICur hCursor);
    static  DBIResult DbiSetToCursor (hDBICur hDest,hDBICur hSrc);
    static  DBIResult DbiGetBookMark (hDBICur hCur,pBYTE pBookMark);
    static  DBIResult DbiSetToBookMark (hDBICur hCur,pBYTE pBookMark);
    static  DBIResult DbiCompareBookMarks (hDBICur hCur,pBYTE pBookMark1,pBYTE pBookMark2,pCMPBkMkRslt pCmpBkmkResult);
		// Data Access: Logical Record Level
    static  DBIResult DbiGetNextRecord (hDBICur hCursor,DBILockType eLock,pBYTE pRecBuff,pRECProps precProps);
    static  DBIResult DbiGetPriorRecord (hDBICur hCursor,DBILockType eLock,pBYTE pRecBuff,pRECProps precProps);
    static  DBIResult DbiGetRecord (hDBICur hCursor,DBILockType eLock,pBYTE pRecBuff,pRECProps precProps);
    static  DBIResult DbiGetRelativeRecord (hDBICur hCursor,INT32 iPosOffset,DBILockType eLock,pBYTE pRecBuff,pRECProps precProps);
    static  DBIResult DbiInitRecord (hDBICur hCursor,pBYTE pRecBuff);
    static  DBIResult DbiInsertRecord (hDBICur hCursor,DBILockType eLock,pBYTE pRecBuff);
    static  DBIResult DbiModifyRecord (hDBICur hCursor,pBYTE pRecBuf,BOOL bFreeLock);
    static  DBIResult DbiDeleteRecord (hDBICur hCursor,pBYTE pRecBuf);
    static  DBIResult DbiReadBlock (hDBICur hCursor,pUINT32 piRecords,pBYTE pBuf);
    static  DBIResult DbiWriteBlock (hDBICur hCursor,pUINT32 piRecords,pBYTE pBuf);
    static  DBIResult DbiAppendRecord (hDBICur hCursor,pBYTE pRecBuff);
    static  DBIResult DbiUndeleteRecord (hDBICur hCursor);
    static  DBIResult DbiGetSeqNo (hDBICur hCursor,pUINT32 piSeqNo);
    static  DBIResult DbiSetToSeqNo (hDBICur hCursor,UINT32 iSeqNo);
    static  DBIResult DbiGetRecordCount (hDBICur hCursor,pUINT32 piRecCount);
    static  DBIResult DbiSetToRecordNo (hDBICur hCursor,UINT32 iRecNo);
    static  DBIResult DbiSaveChanges (hDBICur hCursor);
    static  DBIResult DbiForceReread (hDBICur hCursor);
    static  DBIResult DbiCheckRefresh (); 
    static  DBIResult DbiMakePermanent (hDBICur hCursor,pCHAR pszName,BOOL bOverWrite);
    static  DBIResult DbiForceRecordReread (hDBICur hCursor,pBYTE pRecBuff);
		// Field Level Access
    static  DBIResult DbiGetField(hDBICur hCursor,UINT16 iField,pBYTE pRecBuff,pBYTE pDest,pBOOL pbBlank);
    static  DBIResult DbiPutField(hDBICur hCursor,UINT16 iField,pBYTE pRecBuff,pBYTE pSrc);
    static  DBIResult DbiVerifyField(hDBICur hCursor,UINT16 iField,pBYTE pSrc,pBOOL pbBlank);
    static  DBIResult DbiOpenBlob (hDBICur hCursor,pBYTE pRecBuf,UINT16 iField,DBIOpenMode eOpenMode);
    static  DBIResult DbiGetBlobSize ( hDBICur hCursor,pBYTE pRecBuf, UINT16 iField, pUINT32 piSize);
    static  DBIResult DbiGetBlob (hDBICur hCursor,pBYTE pRecBuf,UINT16 iField,UINT32 iOffSet,UINT32 iLen,pBYTE pDest,pUINT32 piRead);
    static  DBIResult DbiPutBlob (hDBICur hCursor,pBYTE pRecBuf,UINT16 iField,UINT32 iOffSet,UINT32 iLen,pBYTE pSrc);
    static  DBIResult DbiTruncateBlob (hDBICur hCursor,pBYTE pRecBuf,UINT16 iField,UINT32 iLen);
    static  DBIResult DbiFreeBlob (hDBICur hCursor,pBYTE pRecBuf,UINT iField);
    static  DBIResult DbiGetBlobHeading (hDBICur hCursor,UINT16 iField,pBYTE pRecBuf,pBYTE pDest);
    static  DBIResult DbiSetFieldMap(hDBICur hCur,UINT16 iFields,pFLDDesc pFldDesc);
		//TRANSACTIONS
    static  DBIResult DbiBeginTran (hDBIDb hDb,eXILType eXIL,phDBIXact phXact);
    static  DBIResult DbiEndTran (hDBIDb hDb,hDBIXact hXact,eXEnd eEnd);
    static  DBIResult DbiGetTranInfo (hDBIDb hDb,hDBIXact hXact,pXInfo pxInfo);
		//LOCKING
    static  DBIResult DbiAcqTableLock (hDBICur hCursor,DBILockType eLockType);
    static  DBIResult DbiAcqPersistTableLock (hDBIDb hDb,pCHAR pszTableName,pCHAR pszDriverType);
    static  DBIResult DbiRelPersistTableLock (hDBIDb hDb,pCHAR pszTableName,pCHAR pszDriverType);
    static  DBIResult DbiRelTableLock (hDBICur hCursor,BOOL bAll,DBILockType eLockType);
    static  DBIResult DbiRelRecordLock (hDBICur hCursor,BOOL bAll);
    static  DBIResult DbiIsRecordLocked (hDBICur hCursor,pBOOL pbLocked);
    static  DBIResult DbiIsTableLocked (hDBICur hCursor,DBILockType epdxLock,pUINT16 piLocks);
    static  DBIResult DbiIsTableShared (hDBICur hCursor,pBOOL pbShared);
    static  DBIResult DbiOpenLockList (hDBICur hCursor,BOOL bAllUsers,BOOL bAllLockTypes,phDBICur phLocks);
    static  DBIResult DbiOpenUserList (phDBICur phUsers);
    static  DBIResult DbiSetLockRetry (INT16 iWait);
		// Batch Operations
    static  DBIResult DbiBatchMove (pBATTblDesc pSrcTblDesc,hDBICur hSrcCur,pBATTblDesc pDstTblDesc,hDBICur hDstCur,
			                              eBATMode ebatMode,UINT16 iFldCount,pUINT16 pSrcFldMap,pCHAR pszIndexName,
																		pCHAR pszIndexTagName,UINT16 iIndexId,pCHAR pszKeyviolName,pCHAR pszProblemsName,
																		pCHAR pszChangedName,pUINT32 plProbRecs,pUINT32 plKeyvRecs,pUINT32 plChangedRecs,
																		BOOL bAbortOnFirstProb,BOOL bAbortOnFirstKeyviol,pUINT32 plRecsToMove,BOOL bTransliterate);
    static  DBIResult DbiCopyTable (hDBIDb hDb,BOOL bOverWrite,pCHAR pszSrcTableName,pCHAR pszSrcDriverType,pCHAR pszDestTableName);
    static  DBIResult DbiEmptyTable (hDBIDb hDb,hDBICur hCursor,pCHAR pszTableName,pCHAR pszDriverType);
    static  DBIResult DbiPackTable (hDBIDb hDb,hDBICur hCursor,pCHAR pszTableName,pCHAR pszDriverType,BOOL bRegenIdxs);
    static  DBIResult DbiRegenIndex (hDBIDb hDb,hDBICur hCursor,pCHAR pszTableName,pCHAR pszDriverType,pCHAR pszIndexName,
			                               pCHAR pszIndexTagName,UINT16 iIndexId);
    static  DBIResult DbiRegenIndexes (hDBICur hCursor);
    static  DBIResult DbiSortTable (hDBIDb hDb,pCHAR pszTableName,pCHAR pszDriverType,hDBICur hSrcCur,
			                              pCHAR pszSortedName,phDBICur phSortedCur,hDBICur hDstCur,UINT16 iSortFields,
																		pUINT16 piFieldNum,pBOOL pbCaseInsensitive,pSORTOrder pSortOrder,
																		pfSORTCompFn* ppfSortFn,BOOL bRemoveDups,hDBICur hDuplicatesCur,
																		pUINT32 plRecsSort);
		//Create & Restructure
    static  DBIResult DbiCreateTable (hDBIDb hDb,BOOL bOverWrite,pCRTblDesc pcrTblDsc);
    static  DBIResult DbiCreateInMemTable (hDBIDb hDb,pCHAR pszName,UINT16 iFields,pFLDDesc pfldDesc,phDBICur phCursor);
    static  DBIResult DbiCreateTempTable (hDBIDb hDb,pCRTblDesc pcrTblDsc,phDBICur phCursor);
    static  DBIResult DbiDoRestructure (hDBIDb hDb,UINT16 iTblDescCount,pCRTblDesc pTblDesc,pCHAR pszSaveAs,
			                                  pCHAR pszKeyviolName,pCHAR pszProblemsName,BOOL bAnalyzeOnly);
    static  DBIResult DbiRenameTable (hDBIDb hDb,pCHAR pszOldName,pCHAR pszDriverType,pCHAR pszNewName);
    static  DBIResult DbiDeleteTable (hDBIDb hDb,pCHAR pszTableName,pCHAR pszDriverType);
    static  DBIResult DbiAddIndex (hDBIDb hDb,hDBICur hCursor,pCHAR pszTableName,pCHAR pszDriverType,pIDXDesc pIdxDesc,pCHAR pszKeyviolName);
    static  DBIResult DbiDeleteIndex (hDBIDb hDb,hDBICur hCursor,pCHAR pszTableName,pCHAR pszDriverType,pCHAR pszIndexName,
			                                pCHAR pszIndexTagName,UINT16 iIndexId);
		// Error Info
    static  DBIResult DbiGetErrorEntry (UINT16 uEntry, pUINT32 pulNativeError,pCHAR pszError);
    static  DBIResult DbiGetErrorInfo (BOOL bFull,pDBIErrInfo pErrInfo);
    static  DBIResult DbiGetErrorString (DBIResult rslt,pCHAR pszError);
    static  DBIResult DbiGetErrorContext (INT16 eContext,pCHAR pszContext);
		//Dbi Services
    static  DBIResult DbiDateEncode (UINT16 iMon,UINT16 iDay,INT16 iYear,pDBIDATE pdateD);
    static  DBIResult DbiDateDecode (DBIDATE dateD,pUINT16 piMon,pUINT16 piDay,pINT16 piYear);
    static  DBIResult DbiTimeEncode (UINT16 iHour,UINT16 iMin,UINT16 iMilSec,pTIME ptimeT);
    static  DBIResult DbiTimeDecode (TIME timeT,pUINT16 piHour,pUINT16 piMin,pUINT16 piMilSec);
    static  DBIResult DbiTimeStampEncode (DBIDATE dateD,TIME timeT,pTIMESTAMP ptsTS);
    static  DBIResult DbiTimeStampDecode (TIMESTAMP tsTS,pDBIDATE pdateD,pTIME ptimeT);
    static  DBIResult DbiBcdFromFloat(pDFLOAT piVal,UINT16 iPrecision,UINT16 iPlaces,pFMTBcd pBcd);
    static  DBIResult DbiBcdToFloat(pFMTBcd pBcd,pDFLOAT piVal);
		//CallBacks
    static  DBIResult DbiRegisterCallBack (hDBICur hCursor,CBType ecbType,UINT32 iClientData,UINT16 iCbBufLen,pVOID pCbBuf,pfDBICallBack pfCb);
    static  DBIResult DbiGetCallBack (hDBICur hCursor,CBType ecbType,pUINT32 piClientData,pUINT16 piCbBufLen,ppVOID ppCbBuf,ppfDBICallBack ppfCb);
		//Date, time formats
    static  DBIResult DbiGetDateFormat (pFMTDate pfmtDate);
    static  DBIResult DbiSetDateFormat (pFMTDate pfmtDate);
    static  DBIResult DbiGetTimeFormat (pFMTTime pfmtTime);
    static  DBIResult DbiSetTimeFormat (pFMTTime pfmtTime);
    static  DBIResult DbiGetNumberFormat (pFMTNumber pfmtNumber);
    static  DBIResult DbiSetNumberFormat (pFMTNumber pfmtNumber);
		//Conversions
		//...
		//Filters
#if !defined(MIDL)
    static  DBIResult DbiAddFilter(hDBICur hCursor,UINT32 iClientData,UINT16 iPriority,BOOL bCanAbort,pCANExpr pcanExpr,pfGENFilter pfFilter,phDBIFilter phFilter);
#endif // !defined(MIDL)
    static  DBIResult DbiDropFilter(hDBICur hCursor,hDBIFilter hFilter);
    static  DBIResult DbiActivateFilter(hDBICur hCursor,hDBIFilter hFilter);
    static  DBIResult DbiDeactivateFilter(hDBICur hCursor,hDBIFilter hFilter);
#if !defined(MIDL)
    static  DBIResult DbiGetFilterInfo(hDBICur hCur,hDBIFilter hFilter,UINT16 iFilterId,UINT16 iFilterSeqNo,pFILTERInfo pFilterinfo);
#endif // !defined(MIDL)
		//Linked Cursors
    static  DBIResult DbiBeginLinkMode(phDBICur phCursor);
    static  DBIResult DbiEndLinkMode (phDBICur phCursor);
    static  DBIResult DbiLinkDetail(hDBICur hMstrCursor,hDBICur hDetlCursor,UINT16 iLnkFields,pUINT16 piMstrFields,pUINT16 piDetlFields);
    static  DBIResult DbiLinkDetailToExp(hDBICur hCursorMstr,hDBICur hCursorDetl,UINT16 iKeyLen,pCHAR pszMstrExp);
    static  DBIResult DbiUnlinkDetail (hDBICur hDetlCursor);
    static  DBIResult DbiGetLinkStatus(hDBICur hCursor,phDBICur phCursorMstr,phDBICur phCursorDet,phDBICur phCursorSib);
		//Translation
    static  DBIResult DbiNativeToAnsi(pVOID pLdObj,pCHAR pAnsiStr,pCHAR pOemStr,UINT16 iLen,pBOOL pbDataLoss);
    static  DBIResult DbiAnsiToNative(pVOID pLdObj,pCHAR pOemStr,pCHAR pAnsiStr,UINT16 iLen,pBOOL pbDataLoss);
		//Delayed Updates
    static  DBIResult DbiBeginDelayedUpdates(phDBICur phCursor);
    static  DBIResult DbiEndDelayedUpdates(phDBICur phCursor);
    static  DBIResult DbiApplyDelayedUpdates(hDBICur hCursor,DBIDelayedUpdCmd eUpdCmd);
		//MISC.
    static  DBIResult DbiGetTableOpenCount (hDBIDb hDb,pCHAR pszTableName,pCHAR pszDriverType,pUINT16 piOpenCount);
    static  DBIResult DbiUseIdleTime ();
    static  DBIResult DbiGetLdObj (hDBICur hCursor,pVOID* ppLdObj);
    static  DBIResult DbiGetLdName (pCHAR pszDriver,pCHAR pObjName,pCHAR pLdName);
    static  DBIResult DbiFormFullName (hDBIDb hDb,pCHAR pszTableName,pCHAR pszDriverType,pCHAR pszFullName);
    static  DBIResult DbiAddPassword (pCHAR pszPassword);
    static  DBIResult DbiDropPassword (pCHAR pszPassword);
    static  DBIResult DbiGetNetUserName (pCHAR pszNetUserName);
    static  DBIResult DbiDebugLayerOptions (UINT16 iOption,pCHAR pDebugFile);
    static  DBIResult DbiOpenCfgInfoList (hDBICfg hCfg,DBIOpenMode eOpenMode,CFGMode eConfigMode,pCHAR pszCfgPath,phDBICur phCur);
    static  DBIResult DbiAddAlias (hDBICfg hCfg,pCHAR pszAliasName,pCHAR pszDriverType,pCHAR pszParams,BOOL bPersist);
    static  DBIResult DbiDeleteAlias (hDBICfg hCfg,pCHAR pszAliasName);
		//Query Management
    static  DBIResult DbiQExecDirect (hDBIDb hDb,DBIQryLang eQryLang,pCHAR pszQuery,phDBICur phCur);
    static  DBIResult DbiQAlloc(hDBIDb hDb,DBIQryLang eQryLang,phDBIStmt phStmt);
    static  DBIResult DbiQPrepare (hDBIStmt hStmt,pCHAR pszQuery);
    static  DBIResult DbiQPrepareExt (hDBIDb hDb,DBIQryLang eQryLang,pCHAR pszQuery,UINT16 propBits,phDBIStmt phStmt);
    static  DBIResult DbiQExec (hDBIStmt hStmt,phDBICur phCur);
    static  DBIResult DbiQFree (phDBIStmt phStmt);
    static  DBIResult DbiQSetParams (hDBIStmt hStmt,UINT16 uFldDescs,pFLDDesc paFldDescs,pBYTE pRecBuff);
    static  DBIResult DbiQInstantiateAnswer (hDBIStmt hStmt,hDBICur hCur,pCHAR pszAnswerName,pCHAR pszAnswerType,BOOL bOverWrite,phDBICur phCur);
    static  DBIResult DbiQExecProcDirect (hDBIDb hDb,pCHAR pszProc,UINT16 uParamDescs,pSPParamDesc paParamDescs,pBYTE pRecBuff,phDBICur phCur);
    static  DBIResult DbiQPrepareProc (hDBIDb hDb,pCHAR pszProc,UINT16 uParamDescs,pSPParamDesc paParamDescs,pBYTE pRecBuff,phDBIStmt phStmt);
    static  DBIResult DbiQSetProcParams (hDBIStmt hStmt,UINT16 uParamDescs,pSPParamDesc paParamDescs,pBYTE pRecBuff);
    static  DBIResult DbiQGetBaseDescs(hDBIStmt hStmt,phDBICur phCur);
};


_OWLFX_END_NAMESPACE


#endif // OWLFX_DEW_H
