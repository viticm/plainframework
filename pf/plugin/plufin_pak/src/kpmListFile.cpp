#include "kpmListFile.h"
#include "kpmEncrypt.h"
#include "Util.h"
#include "kpmCompress.h"

namespace KPM
{	
extern int Compress_zlib(char * pbOutBuffer, int * pdwOutLength, char * pbInBuffer, int dwInLength, int * /* pCmpType */, int /* nCmpLevel */);
extern int Decompress_zlib(char * pbOutBuffer, int * pdwOutLength, char * pbInBuffer, int dwInLength);

#define LISTFILE_ENTRY_DELETED   (DWORD_PTR)(-2)
#define LISTFILE_ENTRY_FREE      (DWORD_PTR)(-1)

#define LISTFILE_CACHE_SIZE		 0x1000


	struct TListFileCache
	{
		TKPMFile* hFile;                      // Stormlib file handle
		char  *   szMask;                     // File mask
		DWORD     dwFileSize;                 // Total size of the cached file
		DWORD     dwBuffSize;                 // File of the cache
		DWORD     dwFilePos;                  // Position of the cache in the file
		BYTE  *   pBegin;                     // The begin of the listfile cache
		BYTE  *   pPos;
		BYTE  *   pEnd;                       // The last character in the file cache
		BYTE Buffer[1];                       // Listfile cache itself
	};

	char* ListFile::GetListFileName()
	{
		static TCHAR* g_ListFile = "(list)";
		return g_ListFile;
	}

	//-----------------------------------------------------------------------------
		// Local functions (cache)

		// Reloads the cache. Returns number of characters
		// that has been loaded into the cache.
	static int ReloadCache(TListFileCache * pCache)
	{
		// Check if there is enough characters in the cache
		// If not, we have to reload the next block
		if(pCache->pPos >= pCache->pEnd)
		{
			// If the cache is already at the end, do nothing more
			if((pCache->dwFilePos + pCache->dwBuffSize) >= pCache->dwFileSize)
				return 0;

			pCache->dwFilePos += pCache->dwBuffSize;
			KPM_ReadFile(pCache->hFile, pCache->Buffer, pCache->dwBuffSize, &pCache->dwBuffSize);
			if(pCache->dwBuffSize == 0)
				return 0;

			// Set the buffer pointers
			pCache->pBegin =
				pCache->pPos = &pCache->Buffer[0];
			pCache->pEnd = pCache->pBegin + pCache->dwBuffSize;
		}

		return pCache->dwBuffSize;
	}

	static size_t ReadLine(TListFileCache * pCache, char * szLine, int nMaxChars)
	{
		char * szLineBegin = szLine;
		char * szLineEnd = szLine + nMaxChars - 1;

__BeginLoading:

		// Skip newlines, spaces, tabs and another non-printable stuff
		while(pCache->pPos < pCache->pEnd && *pCache->pPos <= 0x20)
			pCache->pPos++;

		// Copy the remaining characters
		while(pCache->pPos < pCache->pEnd && szLine < szLineEnd)
		{
			// If we have found a newline, stop loading
			if(*pCache->pPos == 0x0D || *pCache->pPos == 0x0A)
				break;

			*szLine++ = *pCache->pPos++;
		}

		// If we now need to reload the cache, do it
		if(pCache->pPos == pCache->pEnd)
		{
			if(ReloadCache(pCache) > 0)
				goto __BeginLoading;
		}

		*szLine = 0;
		return (szLine - szLineBegin);
	}


	DWORD KPM_CreateListFile(TKPMArchive* pArch)
	{
		DWORD dwCount = pArch->pHeader->dwHashTableSize;

		// The listfile should be NULL now
		assert(pArch->pListFile == NULL);

		pArch->pListFile = KPM_ALLOC(TFileNode *, dwCount);
		assert(pArch->pListFile);
		if(pArch->pListFile == NULL)
		{
			return ERROR_NOT_ENOUGH_MEMORY;
		}		
		memset(pArch->pListFile, 0xFF, dwCount * sizeof(TFileNode *));

		return ERROR_SUCCESS;
	}

	static BOOL ListFileFindClose(HANDLE hFind)
	{
		TListFileCache * pCache = (TListFileCache *)hFind;

		if(pCache != NULL)
		{
			if(pCache->hFile != NULL)
				KPM_FileClose(pCache->hFile);
			//dscky add 没发现分配空间，貌似不用释放
			if(pCache->szMask != NULL)
				KPM_FREE(pCache->szMask);

			KPM_FREE(pCache);
			return TRUE;
		}
		return FALSE;
	}

	DWORD   KPM_AddListFilePatch(TKPMArchive* ha, const char * szListFile)
	{
		TListFileCache *	pCache = NULL;
		DWORD				dwCacheSize = 0;
		DWORD				dwFileSize = 0;
		size_t				nLength = 0;
		TKPMFile*			pFile;
		char				szFileName[MAX_PATH + 1];

		DWORD nError = ERROR_SUCCESS;

	
		// Open the local/internal listfile
		if(nError == ERROR_SUCCESS)
		{
			pFile = KPM_FileOpen(ha, LISTFILE_NAME,nError);
		}

		if(nError == ERROR_SUCCESS)
		{
			dwCacheSize = 
				dwFileSize = KPM_GetFileSize(pFile);

			// Try to allocate memory for the complete file. If it fails,
			// load the part of the file
			pCache = (TListFileCache *)KPM_ALLOC(char, (sizeof(TListFileCache) + dwCacheSize));
			if(pCache == NULL)
			{
				dwCacheSize = LISTFILE_CACHE_SIZE;
				pCache = (TListFileCache *)KPM_ALLOC(char, sizeof(TListFileCache) + dwCacheSize);
			}
			if(pCache == NULL)
			{
				nError = ERROR_NOT_ENOUGH_MEMORY;
			}
			else
			{
				memset(pCache,0,sizeof(TListFileCache) + dwCacheSize);
			}
		}

		if(nError == ERROR_SUCCESS)
		{
			// Initialize the file cache
			memset(pCache, 0, sizeof(TListFileCache));
			pCache->hFile      = pFile;
			pCache->dwFileSize = dwFileSize;
			pCache->dwBuffSize = dwCacheSize;
			pCache->dwFilePos  = 0;

			// Fill the cache
			KPM_ReadFile(pFile, pCache->Buffer, pCache->dwBuffSize, &pCache->dwBuffSize);

			// Initialize the pointers
			pCache->pBegin =
				pCache->pPos = &pCache->Buffer[0];
			pCache->pEnd = pCache->pBegin + pCache->dwBuffSize;

			// Load the node tree
			while((nLength = ReadLine(pCache, szFileName, sizeof(szFileName) - 1)) > 0)
				KPM_AddNode2ListFile(ha, szFileName);
		}

		// Cleanup & exit
		if(pCache != NULL)
			ListFileFindClose((HANDLE)pCache);
		return nError;
	}


	DWORD   KPM_AddListFileClient(TKPMArchive* ha, const char * szListFile)
	{
		TListFileCache *	pCache = NULL;
		DWORD				dwCacheSize = 0;
		DWORD				dwFileSize = 0;
		size_t				nLength = 0;
		TKPMFile*			pFile;
		char				szFileName[MAX_PATH + 1];

		DWORD				nError = ERROR_SUCCESS;
		DWORD				dwFileSavedLength = 1024*1024*2;
		DWORD				Used = 0;
		DWORD				dwBytesRead;
		
		BYTE* pListFileCache = KPM_ALLOC(BYTE, dwFileSavedLength);
		assert(pListFileCache);
		if(pListFileCache==NULL)
		{
			nError = ERROR_NOT_ENOUGH_MEMORY;
		}
		else
		{
			memset(pListFileCache,0,dwFileSavedLength);
		}		
		BYTE* pListFileCompress = KPM_ALLOC(BYTE, dwFileSavedLength/2+8);
		assert(pListFileCompress);
		if(pListFileCompress==NULL)
		{
			nError =ERROR_NOT_ENOUGH_MEMORY;
		}
		else
		{
			memset(pListFileCompress,0,dwFileSavedLength/2+8);
		}
		// Open the local/internal listfile
		if(nError == ERROR_SUCCESS)
		{
			pFile = KPM_FileOpen(ha, LISTFILE_NAME,nError);
		}

		if(nError == ERROR_SUCCESS)
		{
			KPM_ReadFile(pFile,pListFileCompress,dwFileSavedLength/2+8,&dwBytesRead);
			if(dwBytesRead != dwFileSavedLength/2+8)
				nError = ERROR_FILE_CORRUPT;

			if(*(DWORD*)pListFileCompress != dwFileSavedLength/2+8)
				nError = ERROR_FILE_CORRUPT;
			else
				Used = *((DWORD*)pListFileCompress+1);
		}

		int OutLength =dwFileSavedLength;
		if(nError == ERROR_SUCCESS)
		{

			Decompress_zlib((char*)pListFileCache,&OutLength,(char*)pListFileCompress+8,Used);
			//if(OutLength!=dwFileSavedLength)
			//	nError = ERROR_FILE_CORRUPT;
		}
		if(nError == ERROR_SUCCESS)
		{
			
			dwFileSize = dwCacheSize =OutLength;
			
		}
		
		if(nError == ERROR_SUCCESS)
		{
			// Try to allocate memory for the complete file. If it fails,
			// load the part of the file
			pCache = (TListFileCache *)KPM_ALLOC(char, (sizeof(TListFileCache) + dwCacheSize));
			assert(pCache);
			if(pCache == NULL)
			{
				nError = ERROR_NOT_ENOUGH_MEMORY;
			}
			else
			{
				memset(pCache,0,sizeof(TListFileCache) + dwCacheSize);
			}
		}
		if(nError == ERROR_SUCCESS)
		{
			// Initialize the file cache
			memset(pCache, 0, sizeof(TListFileCache));
			pCache->hFile      = pFile;
			pCache->dwFileSize = dwFileSize;
			pCache->dwBuffSize = dwCacheSize;
			pCache->dwFilePos  = 0;

			// Fill the cache
			memcpy(pCache->Buffer,pListFileCache, pCache->dwBuffSize);
			// Initialize the pointers
			pCache->pBegin =
				pCache->pPos = &pCache->Buffer[0];
			pCache->pEnd = pCache->pBegin + pCache->dwBuffSize;

			// Load the node tree
			while((nLength = ReadLine(pCache, szFileName, sizeof(szFileName) - 1)) > 0)
				KPM_AddNode2ListFile(ha, szFileName);
		}

		KPM_FREE(pListFileCache);
		KPM_FREE(pListFileCompress);

		// Cleanup & exit
		if(pCache != NULL)
			ListFileFindClose((HANDLE)pCache);
		return nError;
	}

	DWORD KPM_CreateAttrFile(TKPMArchive* ha)
	{

		TKPMAttr * pNewAttr;
		
		DWORD nError = ERROR_SUCCESS;

		// There should NOW be any attributes
		assert(ha->pFileAttr == NULL);

		pNewAttr = KPM_ALLOC(TKPMAttr, 1);
		assert(pNewAttr);
		if(pNewAttr != NULL)
		{
			memset(pNewAttr,0,sizeof(TKPMAttr));
			// Pre-set the structure
			pNewAttr->dwVersion = 1400;
			pNewAttr->dwFlags = 0;

			// Allocate array for CRC32
			pNewAttr->pCrc32 = KPM_ALLOC(TKPMCRC32, ha->pHeader->dwHashTableSize);
			assert(pNewAttr->pCrc32);
			if(pNewAttr->pCrc32 != NULL)
			{
				pNewAttr->dwFlags |= KPM_ATTRIBUTE_CRC32;
				memset(pNewAttr->pCrc32, 0, sizeof(TKPMCRC32) * (ha->pHeader->dwHashTableSize));
			}
			else
				nError = ERROR_NOT_ENOUGH_MEMORY;

			// Allocate array for FILETIME
			pNewAttr->pFileTime = KPM_ALLOC(TKPMFileTime, ha->pHeader->dwHashTableSize);
			assert(pNewAttr->pFileTime);
			if(pNewAttr->pFileTime != NULL)
			{
				pNewAttr->dwFlags |= KPM_ATTRIBUTE_FILETIME;
				memset(pNewAttr->pFileTime, 0, sizeof(TKPMFileTime) * (ha->pHeader->dwHashTableSize));
			}
			else
				nError = ERROR_NOT_ENOUGH_MEMORY;

			// Allocate array for MD5
			pNewAttr->pMd5 = KPM_ALLOC(TKPMMD5, ha->pHeader->dwHashTableSize);
			assert(pNewAttr->pMd5);
			if(pNewAttr->pMd5 != NULL)
			{
				pNewAttr->dwFlags |= KPM_ATTRIBUTE_MD5;
				memset(pNewAttr->pMd5, 0, sizeof(TKPMMD5) * (ha->pHeader->dwHashTableSize));
			}
			else
				nError = ERROR_NOT_ENOUGH_MEMORY;
		}

		// If something failed, then free the attributes structure
		if(nError != ERROR_SUCCESS)
		{
			FreeAttributes(pNewAttr);
			pNewAttr = NULL;
		}

		ha->pFileAttr = pNewAttr;
		return nError;
	
	}

	DWORD KPM_AddNode2ListFile(TKPMArchive* ha,const char* szFileName)
	{
		TFileNode * pNode   = NULL;
		TKPMHashKey * pHashEnd = ha->pHashKeyTable + ha->pHeader->dwHashTableSize;
		TKPMHashKey * pHash0   = GetHashEntry(ha, szFileName);
		TKPMHashKey * pHash    = pHash0;
		DWORD dwHashIndex = 0;
		size_t nLength;                     // File name lentgth
		DWORD dwName1;
		DWORD dwName2;

		// If the file does not exist within the KPM, do nothing
		if(pHash == NULL)
			return ERROR_SUCCESS;

		// If the listfile entry already exists, do nothing
		dwHashIndex = (DWORD)(pHash - ha->pHashKeyTable);
		dwName1     = pHash->dwName1;
		dwName2     = pHash->dwName2;
		if((DWORD_PTR)ha->pListFile[dwHashIndex] < LISTFILE_ENTRY_DELETED)
			return ERROR_ALREADY_EXISTS;

		// Create the listfile node and insert it into the listfile table
		nLength = strlen(szFileName);
		pNode = (TFileNode *)KPM_ALLOC(char, sizeof(TFileNode) + nLength);
		assert(pNode);
		memset(pNode,0,sizeof(TFileNode)+nLength);
		pNode->dwRefCount = 0;
		pNode->nLength    = (DWORD)nLength;
		strcpy(pNode->szFileName, szFileName);

		// Fill the nodes for all language versions
		while(pHash->dwBlockIndex < HASH_ENTRY_DELETED)
		{
			if(pHash->dwName1 == dwName1 && pHash->dwName2 == dwName2)
			{
				pNode->dwRefCount++;
				ha->pListFile[pHash - ha->pHashKeyTable] = pNode;
			}

			if(++pHash >= pHashEnd)
				pHash = ha->pHashKeyTable;
			if(pHash == pHash0)
				break;
		}

		return ERROR_SUCCESS;
	}

	DWORD KPM_ListFileRemoveNode(TKPMArchive * pArch, const char * szFileName)
	{
		TFileNode	*	pNode = NULL;
		TKPMHashKey *	pHash = GetHashEntry(pArch, szFileName);
		size_t		nHashIndex = 0;

		if(pHash != NULL)
		{
			nHashIndex = pHash - pArch->pHashKeyTable;
			pNode = pArch->pListFile[nHashIndex];
			pArch->pListFile[nHashIndex] = (TFileNode *)LISTFILE_ENTRY_DELETED;

			// If the reference count has reached zero, do nothing
			if(--pNode->dwRefCount == 0)
				KPM_FREE(pNode);
		}
		return ERROR_SUCCESS;
	}
	DWORD KPM_ListFileRenameNode(TKPMArchive * pArch, const char * szOldFileName, const char * szNewFileName)
	{
		DWORD nError  = KPM_ListFileRemoveNode(pArch, szOldFileName);
		if(nError == ERROR_SUCCESS)
			return KPM_AddNode2ListFile(pArch, szNewFileName);

		return nError;
	}

	DWORD KPM_LoadAttrFileClient(TKPMArchive * ha)
	{
		TKPMAttr *	pAttr = NULL;
		DWORD		dwBytesRead;
		DWORD		dwToRead;
		TKPMFile*	hFile	= NULL;
		DWORD		Used = 0;
			
		

		DWORD	dwFileSavedLength = sizeof(DWORD) + sizeof(DWORD);
		dwFileSavedLength+=sizeof(TKPMCRC32) * ha->pHeader->dwBlockTableSize;
		dwFileSavedLength+=sizeof(TKPMFileTime) * ha->pHeader->dwBlockTableSize;
		dwFileSavedLength+=sizeof(TKPMMD5) * ha->pHeader->dwBlockTableSize;;
		


		DWORD			nError = ERROR_SUCCESS;

		// Initially, set the attrobutes to NULL
		ha->pFileAttr = NULL;

		// Attempt to open the "(attributes)" file.
		// If it's not there, we don't support attributes
		hFile = KPM_FileOpen(ha, ATTRIBUTES_NAME,nError);



		BYTE* pAttrFileCache = KPM_ALLOC(BYTE, dwFileSavedLength);
		assert(pAttrFileCache);
		memset(pAttrFileCache,0,dwFileSavedLength);
		if(pAttrFileCache==NULL)
		{
			nError = ERROR_NOT_ENOUGH_MEMORY;
		}

		BYTE* pAttrFileCompress = KPM_ALLOC(BYTE, dwFileSavedLength+8);
		assert(pAttrFileCompress);
		memset(pAttrFileCompress,0,dwFileSavedLength+8);
		if(pAttrFileCompress==NULL)
		{
			nError =ERROR_NOT_ENOUGH_MEMORY;
		}
		
		if(nError == ERROR_SUCCESS)
		{
			KPM_ReadFile(hFile,pAttrFileCompress,dwFileSavedLength+8,&dwBytesRead);

			if(dwBytesRead != dwFileSavedLength+8)
				nError = ERROR_FILE_CORRUPT;

			if(*(DWORD*)pAttrFileCompress != dwFileSavedLength)
				nError = ERROR_FILE_CORRUPT;
			else
				Used = *((DWORD*)pAttrFileCompress+1);
		}
		
		int OutLength = dwFileSavedLength;
		if(nError == ERROR_SUCCESS)
		{

			Decompress_zlib((char*)pAttrFileCache,&OutLength,(char*)pAttrFileCompress+8,Used);
			if(OutLength!=dwFileSavedLength)
				nError = ERROR_FILE_CORRUPT;
		}


		// Allocate space for the TMPQAttributes
		if(nError == ERROR_SUCCESS)
		{
			pAttr = KPM_ALLOC(TKPMAttr, 1);
			assert(pAttr);
			if(pAttr == NULL)
				nError = ERROR_NOT_ENOUGH_MEMORY;
			else
				memset(pAttr,0,sizeof(TKPMAttr));
		}


		BYTE* pCopyBuffer = pAttrFileCache;
		// Load the content of the attributes file
		if(nError == ERROR_SUCCESS)
		{
			memset(pAttr, 0, sizeof(TKPMAttr));
			dwToRead = sizeof(DWORD) + sizeof(DWORD);
			memcpy(pAttr,pCopyBuffer,dwToRead);
			pCopyBuffer+= dwToRead;
		}

		// Verify format of the attributes
		if(nError == ERROR_SUCCESS)
		{
			if(pAttr->dwVersion > 1400)
				nError = ERROR_BAD_FORMAT;
		}

		// Load the CRC32 (if any)
		if(nError == ERROR_SUCCESS && (pAttr->dwFlags & KPM_ATTRIBUTE_CRC32))
		{
			pAttr->pCrc32 = KPM_ALLOC(TKPMCRC32, ha->pHeader->dwHashTableSize);
			assert(pAttr->pCrc32);
			if(pAttr->pCrc32 != NULL)
			{
				memset(pAttr->pCrc32, 0, sizeof(TKPMCRC32) * (ha->pHeader->dwHashTableSize));
				dwToRead = sizeof(TKPMCRC32) * ha->pHeader->dwBlockTableSize;
				memcpy(pAttr->pCrc32,pCopyBuffer,dwToRead);
				pCopyBuffer+=dwToRead;
				
			}
			else
			{
				nError = ERROR_NOT_ENOUGH_MEMORY;
			}
		}


		// Read the FILETIMEs (if any)
		if(nError == ERROR_SUCCESS && (pAttr->dwFlags & KPM_ATTRIBUTE_FILETIME))
		{
			pAttr->pFileTime = KPM_ALLOC(TKPMFileTime, ha->pHeader->dwHashTableSize);
			assert(pAttr->pFileTime);
			if(pAttr->pFileTime != NULL)
			{
				memset(pAttr->pFileTime, 0, sizeof(TKPMFileTime) * (ha->pHeader->dwHashTableSize));
				dwToRead = sizeof(TKPMFileTime) * ha->pHeader->dwBlockTableSize;
				memcpy(pAttr->pFileTime,pCopyBuffer,dwToRead);
				pCopyBuffer+=dwToRead;
			}
			else
			{
				nError = ERROR_NOT_ENOUGH_MEMORY;
			}
		}
		// Read the MD5 (if any)
		if(nError == ERROR_SUCCESS && (pAttr->dwFlags & KPM_ATTRIBUTE_MD5))
		{
			pAttr->pMd5 = KPM_ALLOC(TKPMMD5, ha->pHeader->dwHashTableSize);
			assert(pAttr->pMd5);
			if(pAttr->pMd5 != NULL)
			{
				memset(pAttr->pMd5, 0, sizeof(TKPMMD5) * (ha->pHeader->dwHashTableSize));
				dwToRead = sizeof(TKPMMD5) * ha->pHeader->dwBlockTableSize;
				memcpy(pAttr->pMd5,pCopyBuffer,dwToRead);
				pCopyBuffer+=dwToRead;
			}
			else
			{
				nError = ERROR_NOT_ENOUGH_MEMORY;
			}
		}

		KPM_FREE(pAttrFileCache);
		KPM_FREE(pAttrFileCompress);

		// Set the attributes into the MPQ archive
		if(nError == ERROR_SUCCESS)
		{
			ha->pFileAttr = pAttr;
			pAttr = NULL;
		}

		// Cleanup & exit
		//dscky edit 解决内存泄漏////////////////////////
		//FreeAttributes(pAttr);
		//dscky edit 解决内存泄漏////////////////////////
		KPM_FileClose(hFile);
		return nError;
	
	}

	DWORD KPM_LoadAttrFilePatch(TKPMArchive * ha)
	{
		TKPMAttr *	pAttr = NULL;
		DWORD		dwBytesRead;
		DWORD		dwToRead;
		TKPMFile*	hFile	= NULL;

		DWORD			nError = ERROR_SUCCESS;

		// Initially, set the attrobutes to NULL
		ha->pFileAttr = NULL;

		// Attempt to open the "(attributes)" file.
		// If it's not there, we don't support attributes
		hFile = KPM_FileOpen(ha, ATTRIBUTES_NAME,nError);

		// Allocate space for the TMPQAttributes
		if(nError == ERROR_SUCCESS)
		{
			pAttr = KPM_ALLOC(TKPMAttr, 1);
			assert(pAttr);
			if(pAttr == NULL)
				nError = ERROR_NOT_ENOUGH_MEMORY;
			else
				memset(pAttr,0,sizeof(TKPMAttr));
		}

		// Load the content of the attributes file
		if(nError == ERROR_SUCCESS)
		{
			memset(pAttr, 0, sizeof(TKPMAttr));
			dwToRead = sizeof(DWORD) + sizeof(DWORD);
			KPM_ReadFile(hFile, pAttr, dwToRead, &dwBytesRead);
			if(dwBytesRead != dwToRead)
				nError = ERROR_FILE_CORRUPT;
		}

		// Verify format of the attributes
		if(nError == ERROR_SUCCESS)
		{
			if(pAttr->dwVersion > 1400)
				nError = ERROR_BAD_FORMAT;
		}

		// Load the CRC32 (if any)
		if(nError == ERROR_SUCCESS && (pAttr->dwFlags & KPM_ATTRIBUTE_CRC32))
		{
			pAttr->pCrc32 = KPM_ALLOC(TKPMCRC32, ha->pHeader->dwHashTableSize);
			assert(pAttr->pCrc32);
			if(pAttr->pCrc32 != NULL)
			{
				memset(pAttr->pCrc32, 0, sizeof(TKPMCRC32) * (ha->pHeader->dwHashTableSize));
				dwToRead = sizeof(TKPMCRC32) * ha->pHeader->dwBlockTableSize;
				KPM_ReadFile(hFile, pAttr->pCrc32, dwToRead, &dwBytesRead);
				if(dwBytesRead != dwToRead)
					nError = ERROR_FILE_CORRUPT;
			}
			else
			{
				nError = ERROR_NOT_ENOUGH_MEMORY;
			}
		}

		// Read the FILETIMEs (if any)
		if(nError == ERROR_SUCCESS && (pAttr->dwFlags & KPM_ATTRIBUTE_FILETIME))
		{
			pAttr->pFileTime = KPM_ALLOC(TKPMFileTime, ha->pHeader->dwHashTableSize);
			assert(pAttr->pFileTime);
			if(pAttr->pFileTime != NULL)
			{
				memset(pAttr->pFileTime, 0, sizeof(TKPMFileTime) * (ha->pHeader->dwHashTableSize));
				dwToRead = sizeof(TKPMFileTime) * ha->pHeader->dwBlockTableSize;
				KPM_ReadFile(hFile, pAttr->pFileTime, dwToRead, &dwBytesRead);
				if(dwBytesRead != dwToRead)
					nError = ERROR_FILE_CORRUPT;
			}
			else
			{
				nError = ERROR_NOT_ENOUGH_MEMORY;
			}
		}

		// Read the MD5 (if any)
		if(nError == ERROR_SUCCESS && (pAttr->dwFlags & KPM_ATTRIBUTE_MD5))
		{
			pAttr->pMd5 = KPM_ALLOC(TKPMMD5, ha->pHeader->dwHashTableSize);
			assert(pAttr->pMd5);
			if(pAttr->pMd5 != NULL)
			{
				memset(pAttr->pMd5, 0, sizeof(TKPMMD5) * (ha->pHeader->dwHashTableSize));
				dwToRead = sizeof(TKPMMD5) * ha->pHeader->dwBlockTableSize;
				KPM_ReadFile(hFile, pAttr->pMd5, dwToRead, &dwBytesRead);
				if(dwBytesRead != dwToRead)
					nError = ERROR_FILE_CORRUPT;
			}
			else
			{
				nError = ERROR_NOT_ENOUGH_MEMORY;
			}
		}

		// Set the attributes into the MPQ archive
		if(nError == ERROR_SUCCESS)
		{
			ha->pFileAttr = pAttr;
			pAttr = NULL;
		}

		// Cleanup & exit
		//dscky edit 解决内存泄漏////////////////////////
		//FreeAttributes(pAttr);
		//dscky edit 解决内存泄漏////////////////////////
		KPM_FileClose(hFile);
		return nError;
	}

}
