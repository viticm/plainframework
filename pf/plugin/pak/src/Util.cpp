#include "Util.h"
#include "kpmEncrypt.h"
#include "crc32.h"
#include "md5.h"
#include "kpmCompress.h"

namespace KPM
{
// Compression types for multilpe compressions
#define KPM_COMPRESSION_HUFFMANN    0x01    // Huffmann compression (used on WAVE files only)
#define KPM_COMPRESSION_ZLIB        0x02    // ZLIB compression
#define KPM_COMPRESSION_PKWARE      0x08    // PKWARE DCL compression
#define KPM_COMPRESSION_BZIP2       0x10    // BZIP2 compression
#define KPM_COMPRESSION_WAVE_MONO   0x40    // 
#define KPM_COMPRESSION_WAVE_STEREO 0x80    // 

int nDataCmp = KPM_COMPRESSION_ZLIB;
extern int Compress_zlib(char * pbOutBuffer, int * pdwOutLength, char * pbInBuffer, int dwInLength, int * /* pCmpType */, int /* nCmpLevel */);
extern int Decompress_zlib(char * pbOutBuffer, int * pdwOutLength, char * pbInBuffer, int dwInLength);
//
// WAVE compressions by quality level
//

static int uWaveCmpLevel[] = {-1, 4, 2};
static int uWaveCmpType[] = {KPM_COMPRESSION_PKWARE, 0x81, 0x81};



  BOOL IsValidArchiveHandle(TKPMArchive * ha)
  {
    if(ha == NULL )
      return FALSE;
    if(ha->pHeader == NULL )
      return FALSE;
    return (ha->pHeader->dwMagic == KPM_MAGIC);
  }
  
  BOOL IsValidFileHandle(TKPMFile * hf)
  {
    if(hf == NULL )
      return FALSE;

    if(hf->hFile != INVALID_HANDLE_VALUE)
      return TRUE;

    return IsValidArchiveHandle(hf->ha);
  }

  // Frees the KPM archive
  void FreeArchive(TKPMArchive *& ha)
  {
    if(ha != NULL)
    {
      KPM_FREE(ha->BlockCache.pbBuffer);
      KPM_FREE(ha->pBlockTable);
      KPM_FREE(ha->pHashKeyTable);

      if(ha->hArchiveFile != INVALID_HANDLE_VALUE)
        CloseHandle(ha->hArchiveFile);
      //dscky add 解决内存泄漏////////////////////////
      FreeAttributes(ha->pFileAttr);
      for(DWORD i=0;i<ha->pHeader->dwHashTableSize;++i)
      {
        if(ha->pListFile)
        {
          TFileNode *  pNode = ha->pListFile[i];
          if((DWORD_PTR)pNode < LISTFILE_ENTRY_DELETED)
          {
            KPM_FREE(pNode);  
          }
        }
      }  
      KPM_FREE(ha->pListFile);
      //dscky add 解决内存泄漏////////////////////////
      KPM_FREE(ha);
      ha = NULL;
    }
  }

  void FreeFile(TKPMFile *& hf)
  {
    if(hf != NULL)
    {
      if(hf->hFile!= INVALID_HANDLE_VALUE)
        CloseHandle(hf->hFile);
      if(hf->pllBlockOffSet != NULL)
        KPM_FREE(hf->pllBlockOffSet);
      if(hf->pFileBuffer != NULL)
        KPM_FREE(hf->pFileBuffer);
      KPM_FREE(hf);
      hf = NULL;
    }
  }
  
  void FreeAttributes(TKPMAttr * pAttr)
  {
    if(pAttr != NULL)
    {
      if(pAttr->pCrc32 != NULL)
        KPM_FREE(pAttr->pCrc32);
      if(pAttr->pFileTime != NULL)
        KPM_FREE(pAttr->pFileTime);
      if(pAttr->pMd5 != NULL)
        KPM_FREE(pAttr->pMd5);

      KPM_FREE(pAttr);
    }
  }

  int  SaveKPMTables(TKPMArchive * ha)
  {
  
    int nError = ERROR_SUCCESS;

    DWORD  dwBuffSize = max(ha->pHeader->dwHashTableSize*sizeof(TKPMHashKey), 
                ha->pHeader->dwBlockTableSize*sizeof(TKPMBlock));
    BYTE *  pbBuffer = NULL;
    DWORD  dwWritten;
    DWORD  dwBytes;

    // Allocate buffer for encrypted tables
    if(nError == ERROR_SUCCESS)
    {
      // Allocate temporary buffer for tables encryption
      pbBuffer = KPM_ALLOC(BYTE, dwBuffSize);
      assert(pbBuffer);
      if(pbBuffer == NULL)
        nError = ERROR_NOT_ENOUGH_MEMORY;
      else
        memset(pbBuffer,0,dwBuffSize);
    }

    // Write the KPM Header
    if(nError == ERROR_SUCCESS)
    {
      DWORD dwHeaderSize = ha->pHeader->dwHeaderSize;
      
      LONG dwFilePointerHigh = 0;
      // Write the KPM header
      SetFilePointer(ha->hArchiveFile, 0, &dwFilePointerHigh, FILE_BEGIN);
      
      WriteFile(ha->hArchiveFile, ha->pHeader, dwHeaderSize, &dwWritten, NULL);

      if(dwWritten != ha->pHeader->dwHeaderSize)
        nError = ERROR_DISK_FULL;
    }

    // Write the hash table
    if(nError == ERROR_SUCCESS)
    {
      // Copy the hash table to temporary buffer
      dwBytes = ha->pHeader->dwHashTableSize * sizeof(TKPMHashKey);
      memcpy(pbBuffer, ha->pHashKeyTable, dwBytes);

      // Convert to little endian for file save
      Encrypt((DWORD *)pbBuffer, (BYTE *)"(hashkey table)", dwBytes >> 2);

      // Set the file pointer to the offset of the hash table and write it
      SetFilePointer(ha->hArchiveFile, ha->HashTableOffset.LowPart, (PLONG)&ha->HashTableOffset.HighPart, FILE_BEGIN);
      WriteFile(ha->hArchiveFile, pbBuffer, dwBytes, &dwWritten, NULL);
      if(dwWritten != dwBytes)
        nError = ERROR_DISK_FULL;
    }

    // Write the block table
    if(nError == ERROR_SUCCESS)
    {
      // Copy the block table to temporary buffer
      dwBytes = ha->pHeader->dwBlockTableSize * sizeof(TKPMBlock);
      memcpy(pbBuffer, ha->pBlockTable, dwBytes);

      // Encrypt the block table and write it to the file
      Encrypt((DWORD *)pbBuffer, (BYTE *)"(block table)", dwBytes >> 2);

      // Convert to little endian for file save
      WriteFile(ha->hArchiveFile, pbBuffer, dwBytes, &dwWritten, NULL);
      if(dwWritten != dwBytes)
        nError = ERROR_DISK_FULL;
    }

    // Set end of file here
    //if(nError == ERROR_SUCCESS)
    //{
    //  SetEndOfFile(ha->hArchiveFile);
    //}

    // Cleanup and exit
    if(pbBuffer != NULL)
      KPM_FREE(pbBuffer);
    return nError;

    
  }
  int  SaveListFile(TKPMArchive * ha)
  {
    int        nError    = ERROR_SUCCESS;
    HANDLE      hFile    = INVALID_HANDLE_VALUE;
    TKPMHashKey *  pHashEnd  = NULL;
    TKPMHashKey *  pHash0    = NULL;
    TKPMHashKey *  pHash    = NULL;
    DWORD      dwName1 = 0;
    DWORD      dwName2 = 0;
    TFileNode *    pNode = NULL;
    char      szBuffer[MAX_PATH+4];
    DWORD      dwTransferred;
    size_t      nLength = 0;
    DWORD      dwFileSavedLength = 0;

    // If no listfile, do nothing
    if(ha->pListFile == NULL)
      return ERROR_SUCCESS;

    if(nError == ERROR_SUCCESS)
    {
      hFile = CreateFile(LISTFILE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_DELETE_ON_CLOSE, NULL);
      if(hFile == INVALID_HANDLE_VALUE)
        nError = GetLastError();
    }

    // Find the hash entry corresponding to listfile
    pHashEnd = ha->pHashKeyTable + ha->pHeader->dwHashTableSize;
    pHash0 = pHash = GetHashEntry(ha, 0);
    if(pHash == NULL)
      pHash0 = pHash = ha->pHashKeyTable;

    // Save the file
    if(nError == ERROR_SUCCESS)
    {
      for(;;)
      {
        if(pHash->dwName1 != dwName1 && pHash->dwName2 != dwName2 && pHash->dwBlockIndex < HASH_ENTRY_DELETED)
        {
          dwName1 = pHash->dwName1;
          dwName2 = pHash->dwName2;
          pNode = ha->pListFile[pHash - ha->pHashKeyTable];

          if((DWORD_PTR)pNode < LISTFILE_ENTRY_DELETED)
          {
            memcpy(szBuffer, pNode->szFileName, pNode->nLength);
            szBuffer[pNode->nLength + 0] = 0x0D;
            szBuffer[pNode->nLength + 1] = 0x0A;
            WriteFile(hFile, szBuffer, (DWORD)(pNode->nLength + 2), &dwTransferred, NULL);
            dwFileSavedLength+=dwTransferred;
          }
        }

        if(++pHash >= pHashEnd)
          pHash = ha->pHashKeyTable;
        if(pHash == pHash0)
          break;
      }

      // Write the listfile name (if not already there)
      if(GetHashEntry(ha, LISTFILE_NAME) == NULL)
      {
        nLength = strlen(LISTFILE_NAME);
        memcpy(szBuffer, LISTFILE_NAME, nLength);
        szBuffer[nLength + 0] = 0x0D;
        szBuffer[nLength + 1] = 0x0A;
        WriteFile(hFile, szBuffer, (DWORD)(nLength + 2), &dwTransferred, NULL);
        dwFileSavedLength+=dwTransferred;
      } 

      //if(dwFileSavedLength>ha->Header.dwMaxListFileCache)
      // Add the listfile into the archive.
      {  nError = AddFileToArchive(ha,
                    hFile,
                    LISTFILE_NAME,
                    KPM_FILE_ENCRYPTED | KPM_FILE_COMPRESS | KPM_FILE_REPLACEEXISTING,
                    0,
                    FILE_TYPE_DATA,
                    NULL);
      }
      
      
    }

    // Close the temporary file. This will delete it too.
    if(hFile != INVALID_HANDLE_VALUE)
      CloseHandle(hFile);
    return nError;
  }


  int  SaveListFile(TKPMArchive * ha,BOOL bInternal)
  {
    int        nError    = ERROR_SUCCESS;
    HANDLE      hFile    = INVALID_HANDLE_VALUE;
    TKPMHashKey *  pHashEnd  = NULL;
    TKPMHashKey *  pHash0    = NULL;
    TKPMHashKey *  pHash    = NULL;
    DWORD      dwName1 = 0;
    DWORD      dwName2 = 0;
    TFileNode *    pNode = NULL;
    DWORD      dwTransferred;
    size_t      nLength = 0;
    
    DWORD  dwFileSavedLength  = 1024*1024*2;

    // If no listfile, do nothing
    if(ha->pListFile == NULL)
      return ERROR_SUCCESS;

    if(nError == ERROR_SUCCESS)
    {
      hFile = CreateFile(LISTFILE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_DELETE_ON_CLOSE, NULL);
      if(hFile == INVALID_HANDLE_VALUE)
        nError = GetLastError();
    }
    
    BYTE* pAttrFileCache = KPM_ALLOC(BYTE, dwFileSavedLength);
    assert(pAttrFileCache);
    if(pAttrFileCache==NULL)
    {
      nError = ERROR_NOT_ENOUGH_MEMORY;
    }
    else
    {
      memset(pAttrFileCache,0x00,dwFileSavedLength);
    }

    BYTE* pAttrFileCompress = KPM_ALLOC(BYTE, dwFileSavedLength+8);
    assert(pAttrFileCompress);
    if(pAttrFileCompress==NULL)
    {
      nError =ERROR_NOT_ENOUGH_MEMORY;
    }
    else
    {
      memset(pAttrFileCompress,0x00,dwFileSavedLength+8);
    }

    *(DWORD*)pAttrFileCompress = dwFileSavedLength/2+8;

    DWORD dwMemCpyed = 0;
    BYTE* pCopyBuffer = pAttrFileCache;

    // Find the hash entry corresponding to listfile
    pHashEnd = ha->pHashKeyTable + ha->pHeader->dwHashTableSize;
    pHash0 = pHash = GetHashEntry(ha, 0);
    if(pHash == NULL)
      pHash0 = pHash = ha->pHashKeyTable;

    // Save the file
    if(nError == ERROR_SUCCESS)
    {
      for(;;)
      {
        if(pHash->dwName1 != dwName1 && pHash->dwName2 != dwName2 && pHash->dwBlockIndex < HASH_ENTRY_DELETED)
        {
          dwName1 = pHash->dwName1;
          dwName2 = pHash->dwName2;
          pNode = ha->pListFile[pHash - ha->pHashKeyTable];

          if((DWORD_PTR)pNode < LISTFILE_ENTRY_DELETED)
          {
            dwMemCpyed+=pNode->nLength+2;
            
            if(dwMemCpyed>dwFileSavedLength)
              nError = ERROR_INSUFFICIENT_BUFFER;
            else
            {
              memcpy(pCopyBuffer, pNode->szFileName, pNode->nLength);
              pCopyBuffer[pNode->nLength + 0] = 0x0D;
              pCopyBuffer[pNode->nLength + 1] = 0x0A;
              pCopyBuffer+=(pNode->nLength+2);
            }
            
          }
        }

        if(++pHash >= pHashEnd)
          pHash = ha->pHashKeyTable;
        if(pHash == pHash0)
          break;
      }

      // Write the listfile name (if not already there)
      if(GetHashEntry(ha, LISTFILE_NAME) == NULL)
      {
        nLength = strlen(LISTFILE_NAME);
        dwMemCpyed+=(DWORD)nLength;

        if(dwMemCpyed>dwFileSavedLength)
          nError = ERROR_INSUFFICIENT_BUFFER;
        else
        {
          memcpy(pCopyBuffer, LISTFILE_NAME, nLength);
          pCopyBuffer[nLength + 0] = 0x0D;
          pCopyBuffer[nLength + 1] = 0x0A;
          pCopyBuffer+=(nLength+2);
        }
      
      } 

      if(nError == ERROR_SUCCESS)
      {
        int nOutLength;
        int nCmpType = 0;
        Compress_zlib((char *)(pAttrFileCompress)+8, &nOutLength, (char *)pAttrFileCache, dwMemCpyed, &nCmpType, 0);
        *((DWORD*)pAttrFileCompress+1) = nOutLength;
      }



      if(nError == ERROR_SUCCESS)
      {
        WriteFile(hFile,pAttrFileCompress, dwFileSavedLength/2+8, &dwTransferred, NULL);
        if((dwFileSavedLength/2+8) != dwTransferred)
          nError = ERROR_DISK_FULL;
      }


      KPM_FREE(pAttrFileCache);
      KPM_FREE(pAttrFileCompress);

      if(nError == ERROR_SUCCESS)
      {
        if(bInternal)
        {
          DWORD R;
          TKPMFile* pFile = KPM_FileOpen(ha,LISTFILE_NAME,R);
          if(pFile && (pFile->pBlock->dwFSize>0))
            nError = InternalUpdateFile(ha, hFile,pFile, LISTFILE_NAME, KPM_FILE_REPLACEEXISTING, 0, FILE_TYPE_DATA, NULL);
          KPM_FileClose(pFile);
        }
        else
          nError = AddFileToArchive(ha, hFile, LISTFILE_NAME, KPM_FILE_REPLACEEXISTING, 0, FILE_TYPE_DATA, NULL);

      }



    }

    // Close the temporary file. This will delete it too.
    if(hFile != INVALID_HANDLE_VALUE)
      CloseHandle(hFile);
    return nError;
  }

  //////////////////////////////////////////////////////////////////////////
  
  DWORD SaveAttrFile(TKPMArchive * ha,BOOL bInternal)
  {
    HANDLE  hFile = INVALID_HANDLE_VALUE;
    DWORD  dwToWrite;
    DWORD  dwWritten;

    int nError = ERROR_SUCCESS;


    DWORD  dwFileSavedLength = sizeof(DWORD) + sizeof(DWORD);
    dwFileSavedLength+=sizeof(TKPMCRC32) * ha->pHeader->dwBlockTableSize;
    dwFileSavedLength+=sizeof(TKPMFileTime) * ha->pHeader->dwBlockTableSize;
    dwFileSavedLength+=sizeof(TKPMMD5) * ha->pHeader->dwBlockTableSize;;
    
    
    // If there are no attributes, do nothing
    if(ha->pFileAttr == NULL)
      return ERROR_SUCCESS;

    // Create the local attributes file
    if(nError == ERROR_SUCCESS)
    {
      hFile = CreateFile(ATTRIBUTES_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_DELETE_ON_CLOSE, NULL);
      if(hFile == INVALID_HANDLE_VALUE)
        nError = GetLastError();
    }


    BYTE* pAttrFileCache = KPM_ALLOC(BYTE, dwFileSavedLength);
    assert(pAttrFileCache);
    if(pAttrFileCache==NULL)
    {
      nError = ERROR_NOT_ENOUGH_MEMORY;
    }
    else
    {
      memset(pAttrFileCache,0xFF,dwFileSavedLength);
    }

    BYTE* pAttrFileCompress = KPM_ALLOC(BYTE, dwFileSavedLength+8);
    assert(pAttrFileCompress);
    if(pAttrFileCompress==NULL)
    {
      nError =ERROR_NOT_ENOUGH_MEMORY;
    }
    else
    {
      memset(pAttrFileCompress,0xFF,dwFileSavedLength+8);
    }
    
    *(DWORD*)pAttrFileCompress = dwFileSavedLength;

    DWORD dwMemCpyed = 0;
    BYTE* pCopyBuffer = pAttrFileCache;
    if(nError == ERROR_SUCCESS)
    {
      dwToWrite = sizeof(DWORD) + sizeof(DWORD);
      dwMemCpyed+=dwToWrite;
      if(dwMemCpyed>dwFileSavedLength)
        nError = ERROR_INSUFFICIENT_BUFFER;
      else 
      {
        memcpy(pCopyBuffer, ha->pFileAttr, dwToWrite);
        pCopyBuffer+=dwToWrite;

      }
    }

    if(nError == ERROR_SUCCESS&& ha->pFileAttr->pCrc32 != NULL)
    {
      dwToWrite = sizeof(TKPMCRC32) * ha->pHeader->dwBlockTableSize;
      dwMemCpyed += dwToWrite;
      if(dwMemCpyed>dwFileSavedLength)
        nError = ERROR_INSUFFICIENT_BUFFER;
      else 
      {
        memcpy(pCopyBuffer,ha->pFileAttr->pCrc32, dwToWrite);
        pCopyBuffer+=dwToWrite;
      }
    }

    if(nError == ERROR_SUCCESS&& ha->pFileAttr->pFileTime != NULL)
    {
      dwToWrite = sizeof(TKPMFileTime) * ha->pHeader->dwBlockTableSize;
      dwMemCpyed += dwToWrite;
      if(dwMemCpyed>dwFileSavedLength)
        nError = ERROR_INSUFFICIENT_BUFFER;
      else 
      {
        memcpy(pCopyBuffer,ha->pFileAttr->pFileTime, dwToWrite);
        pCopyBuffer+=dwToWrite;
      }
    }
    
    if(nError == ERROR_SUCCESS && ha->pFileAttr->pMd5 != NULL)
    {
      dwToWrite = sizeof(TKPMMD5) * ha->pHeader->dwBlockTableSize;
      dwMemCpyed += dwToWrite;
      if(dwMemCpyed>dwFileSavedLength)
        nError = ERROR_INSUFFICIENT_BUFFER;
      else 
      {
        memcpy(pCopyBuffer,ha->pFileAttr->pMd5, dwToWrite);
        pCopyBuffer+=dwToWrite;
      }
    }

    if(nError == ERROR_SUCCESS)
    {
      int nOutLength;
      int nCmpType = 0;
      Compress_zlib((char *)(pAttrFileCompress)+8, &nOutLength, (char *)pAttrFileCache, dwMemCpyed, &nCmpType, 0);
      *((DWORD*)pAttrFileCompress+1) = nOutLength;
    }

    
    
    if(nError == ERROR_SUCCESS)
    {
      WriteFile(hFile,pAttrFileCompress, dwFileSavedLength+8, &dwWritten, NULL);
      if((dwFileSavedLength+8) != dwWritten)
        nError = ERROR_DISK_FULL;
    }


    KPM_FREE(pAttrFileCache);
    KPM_FREE(pAttrFileCompress);

    // Add the attributes into MPQ
    if(nError == ERROR_SUCCESS)
    {
      if(bInternal)
      {
        DWORD R;
        TKPMFile* pFile = KPM_FileOpen(ha,ATTRIBUTES_NAME,R);
        if(pFile && (pFile->pBlock->dwFSize>0))
          nError = InternalUpdateFile(ha, hFile,pFile, ATTRIBUTES_NAME, KPM_FILE_REPLACEEXISTING, 0, FILE_TYPE_DATA, NULL);
        KPM_FileClose(pFile);
      }
      else
        nError = AddFileToArchive(ha, hFile, ATTRIBUTES_NAME, KPM_FILE_REPLACEEXISTING, 0, FILE_TYPE_DATA, NULL);

    }

    // Close the temporary file. This will delete it too.
    if(hFile != INVALID_HANDLE_VALUE)
      CloseHandle(hFile);
    return nError;
    
  }


  DWORD SaveAttrFile(TKPMArchive * ha)
  {
    HANDLE  hFile = INVALID_HANDLE_VALUE;
    DWORD  dwToWrite;
    DWORD  dwWritten;

    int nError = ERROR_SUCCESS;

    // If there are no attributes, do nothing
    if(ha->pFileAttr == NULL)
      return ERROR_SUCCESS;

    // Create the local attributes file
    if(nError == ERROR_SUCCESS)
    {
      hFile = CreateFile(ATTRIBUTES_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_DELETE_ON_CLOSE, NULL);
      if(hFile == INVALID_HANDLE_VALUE)
        nError = GetLastError();
    }

    // Write the content of the attributes to the file
    if(nError == ERROR_SUCCESS)
    {
      // Write the header of the attributes file
      dwToWrite = sizeof(DWORD) + sizeof(DWORD);
      WriteFile(hFile, ha->pFileAttr, dwToWrite, &dwWritten, NULL);
      if(dwWritten != dwToWrite)
        nError = ERROR_DISK_FULL;
    }

    // Write the array of CRC32
    if(nError == ERROR_SUCCESS && ha->pFileAttr->pCrc32 != NULL)
    {
      dwToWrite = sizeof(TKPMCRC32) * ha->pHeader->dwBlockTableSize;
      WriteFile(hFile, ha->pFileAttr->pCrc32, dwToWrite, &dwWritten, NULL);
      if(dwWritten != dwToWrite)
        nError = ERROR_DISK_FULL;
    }

    // Write the array of FILETIMEs
    if(nError == ERROR_SUCCESS && ha->pFileAttr->pFileTime != NULL)
    {
      dwToWrite = sizeof(TKPMFileTime) * ha->pHeader->dwBlockTableSize;
      WriteFile(hFile, ha->pFileAttr->pFileTime, dwToWrite, &dwWritten, NULL);
      if(dwWritten != dwToWrite)
        nError = ERROR_DISK_FULL;
    }

    // Write the array of MD5s
    if(nError == ERROR_SUCCESS && ha->pFileAttr->pMd5 != NULL)
    {
      dwToWrite = sizeof(TKPMMD5) * ha->pHeader->dwBlockTableSize;
      WriteFile(hFile, ha->pFileAttr->pMd5, dwToWrite, &dwWritten, NULL);
      if(dwWritten != dwToWrite)
        nError = ERROR_DISK_FULL;
    }

    // Add the attributes into MPQ
    if(nError == ERROR_SUCCESS)
    {
      nError = AddFileToArchive(ha, hFile, ATTRIBUTES_NAME, KPM_FILE_COMPRESS | KPM_FILE_REPLACEEXISTING, 0, FILE_TYPE_DATA, NULL);

    }

    // Close the temporary file. This will delete it too.
    if(hFile != INVALID_HANDLE_VALUE)
      CloseHandle(hFile);
    return nError;
  }

  //////////////////////////////////////////////////////////////////////////



  int AddFileToArchive(TKPMArchive * ha,
    const char* lpszFile,
    DWORD dwFileSize,
    const char * szArchivedName, 
    DWORD dwFlags, 
    DWORD dwQuality, 
    int nFileType, 
    BOOL * pbReplaced)
  {
    int nError = ERROR_SUCCESS;
    int nCmpFirst = KPM_GetDataCompression();           // Compression for the first data block
    int nCmpNext  = KPM_GetDataCompression();           // Compression for the next data blocks
    int nCmp      = KPM_GetDataCompression();           // Current compression
    int nCmpLevel = -1;                         // Compression level

    TKPMBlock * pBlockEnd;                      // Pointer to end of the block table
    TKPMFile *  hf  =   NULL;                  // File structure for newly added file
    BOOL    bReplaced = FALSE;              // TRUE if replaced, FALSE if added
    LONGLONG  TempFilePos;              // For various file offset calculations
    BYTE *    pbToWrite = NULL;               // Data to write to the file
    BYTE *    pbCompressed = NULL;            // Compressed (target) data
    BYTE *    pbFileCont = NULL;
    DWORD    dwTransferred = 0;              // Number of bytes read or written
    DWORD    dwFileSizeLeft = dwFileSize;

    pBlockEnd = ha->pBlockTable + ha->pHeader->dwBlockTableSize;

    // Set the correct compression types
    if(dwFlags & KPM_FILE_IMPLODE)
      nCmpFirst = nCmpNext = KPM_COMPRESSION_PKWARE;
    

    if(dwFlags & KPM_FILE_COMPRESS)
    {
      if(nFileType == FILE_TYPE_DATA)
        nCmpFirst = nCmpNext = nDataCmp;

      if(nFileType == FILE_TYPE_WAVE)
      {
        nCmpNext  = uWaveCmpType[dwQuality];
        nCmpLevel = uWaveCmpLevel[dwQuality];
      }
    }

    // Get the size of the file to be added
    if(nError == ERROR_SUCCESS)
    {
      // Adjust file flags for too-small files
      if(dwFileSize < 0x04)
        dwFlags &= ~(KPM_FILE_ENCRYPTED | KPM_FILE_FIXSEED);
      if(dwFileSize < 0x20)
        dwFlags &= ~KPM_FILE_COMPRESSED;
    }

    // Allocate the TKPMFile entry for newly added file
    if(nError == ERROR_SUCCESS)
    {
      hf = (TKPMFile *)KPM_ALLOC(BYTE, sizeof(TKPMFile) + strlen(szArchivedName));
      assert(hf);
      if(hf == NULL)
        nError = ERROR_NOT_ENOUGH_MEMORY;
      else
        memset(hf,0,sizeof(TKPMFile)+strlen(szArchivedName));
    }

    // Reset the TMPQFile structure
    if(nError == ERROR_SUCCESS)
    {
      memset(hf, 0, sizeof(TKPMFile));
      strcpy(hf->szFileName, szArchivedName);
      hf->ha = ha;

      // Check if the file already exists in the archive
      if((hf->pHash = GetHashEntry(ha, szArchivedName)) != NULL)
      {
        if((dwFlags & KPM_FILE_REPLACEEXISTING) == 0)
        {
          nError = ERROR_ALREADY_EXISTS;
          hf->pHash = NULL;
        }
        else
        {
          hf->pBlock = ha->pBlockTable + hf->pHash->dwBlockIndex;
          bReplaced = TRUE;
        }
      }

      if(nError == ERROR_SUCCESS && hf->pHash == NULL)
      {
        hf->pHash = FindFreeHashEntry(ha, szArchivedName);
        if(hf->pHash == NULL)
          nError = ERROR_HANDLE_DISK_FULL;
      }

      // Set the hash index
      hf->dwHashIndex = (DWORD)(hf->pHash - ha->pHashKeyTable);
    }


    // Find a block table entry for the file
    if(nError == ERROR_SUCCESS)
    {
      TKPMBlock * pBlock = NULL;          // Entry in the block table

      // Get the position of the first file
      hf->MpqFileOffSet.QuadPart = ha->pHeader->dwBlockTablePos+ha->pHeader->dwBlockTableSize*sizeof(TKPMBlock);

      // Search the entire block table and find a free block.
      // Also find KPM offset at which the file data will be stored
      LONGLONG  EmptyDiskSpace =0;
      LONGLONG  EmptyDiskBegin =0;
      
      for(pBlock = ha->pBlockTable; pBlock < pBlockEnd; pBlock++)
      {
        if(pBlock->dwFlags & KPM_FILE_EXISTS)
        {
          if(EmptyDiskBegin>0) 
            EmptyDiskSpace = pBlock->dwFilePos - EmptyDiskBegin;
        
          TempFilePos   = pBlock->dwFilePos;
          TempFilePos  += pBlock->dwCSize;
          
          if(TempFilePos> hf->MpqFileOffSet.QuadPart)
            hf->MpqFileOffSet.QuadPart = TempFilePos;
        }
        else
        {
          if(hf->pBlock == NULL)
          {
                        hf->pBlock = pBlock;
          }
          
          if(EmptyDiskBegin == 0)
            EmptyDiskBegin  =  TempFilePos;
        }
      }

      // Calculate the raw file offset
      hf->RawFileOffSet.QuadPart = hf->MpqFileOffSet.QuadPart + ha->HeaderOffset;

      // If no free block in the middle of the block table,
      // use the one after last used block
      if(hf->pBlock == NULL)
      {
        hf->pBlock = pBlock;
      }
      // If the block offset exceeds number of hash entries,
      // we cannot add new file to the KPM
      hf->dwBlockIndex = (DWORD)(hf->pBlock - ha->pBlockTable);
      if(hf->dwBlockIndex >= ha->pHeader->dwHashTableSize)
        nError = ERROR_HANDLE_DISK_FULL;

    }


    // Create seed1 for file encryption
    if(nError == ERROR_SUCCESS && (dwFlags & KPM_FILE_ENCRYPTED))
    {
      const char * szTemp = strrchr(szArchivedName, '\\');

      // Create seed1 for file encryption
      if(szTemp != NULL)
        szArchivedName = szTemp + 1;

      hf->dwSeed1 = DecryptFileSeed(szArchivedName);
      if(dwFlags & KPM_FILE_FIXSEED)
      {
        hf->dwSeed1 = (hf->dwSeed1 + hf->MpqFileOffSet.LowPart) ^ dwFileSize;
      }
    }

    // Resolve CRC32 and MD5 entry for the file
    // Only do it when the MPQ archive has attributes
    if(nError == ERROR_SUCCESS && ha->pFileAttr != NULL)
    {
      if(ha->pFileAttr->pCrc32 != NULL)
        hf->pCrc32 = ha->pFileAttr->pCrc32 + hf->dwBlockIndex;
      if(ha->pFileAttr->pFileTime != NULL)
        hf->pFileTime = ha->pFileAttr->pFileTime + hf->dwBlockIndex;
      if(ha->pFileAttr->pMd5 != NULL)
        hf->pMD5 = ha->pFileAttr->pMd5 + hf->dwBlockIndex;
    }

    // Allocate buffers for the compressed data
    if(nError == ERROR_SUCCESS)
    {
      hf->nBlocks = (dwFileSize / ha->dwBlockSize) + 1;
      if(dwFileSize % ha->dwBlockSize)
        hf->nBlocks++;

      if((hf->pFileBuffer = KPM_ALLOC(BYTE, ha->dwBlockSize)) == NULL)
        nError = ERROR_NOT_ENOUGH_MEMORY;
      else      
        memset(hf->pFileBuffer,0,ha->dwBlockSize);      
      pbToWrite = hf->pFileBuffer;
    }
    
    // For compressed files, allocate buffer for block positions and for the compressed data
    if(nError == ERROR_SUCCESS && (dwFlags & KPM_FILE_COMPRESSED))
    {
      hf->pllBlockOffSet = KPM_ALLOC(LARGE_INTEGER, hf->nBlocks + 1);
      assert(hf->pllBlockOffSet);
      pbCompressed = KPM_ALLOC(BYTE, ha->dwBlockSize * 2);
      assert(pbCompressed);
      if(hf->pllBlockOffSet == NULL || pbCompressed == NULL)
        nError = ERROR_NOT_ENOUGH_MEMORY;
      if(hf->pllBlockOffSet)
        memset(hf->pllBlockOffSet,0,sizeof(LARGE_INTEGER)*(hf->nBlocks + 1));
      if(pbCompressed)
        memset(pbCompressed,0,ha->dwBlockSize * 2);
      pbToWrite = pbCompressed;
    }


    // Set the file position to the point where the file will be stored
    if(nError == ERROR_SUCCESS)
    {
      // Set the file pointer to file data position
      if(hf->RawFileOffSet.QuadPart != ha->CurrentOffset)
      {
        SetFilePointer(ha->hArchiveFile, hf->RawFileOffSet.LowPart, &hf->RawFileOffSet.HighPart, FILE_BEGIN);
        ha->CurrentOffset = hf->RawFileOffSet.QuadPart;
      }

      // Initialize the hash entry for the file
      hf->pHash->dwBlockIndex = hf->dwBlockIndex;

      // Initialize the block table entry for the file
      
      hf->pBlock->dwFilePos = hf->MpqFileOffSet.QuadPart;
      hf->pBlock->dwFSize   = dwFileSize;
      hf->pBlock->dwCSize   = 0;
      hf->pBlock->dwFlags   = dwFlags | KPM_FILE_EXISTS;
    }
    

    DWORD dwBlockPosLen = 0; // Length of the file block offset (in bytes)

    // Write block positions (if the file will be compressed)
    if(nError == ERROR_SUCCESS && (dwFlags & KPM_FILE_COMPRESSED))
    {
      dwBlockPosLen = hf->nBlocks * sizeof(LARGE_INTEGER);
    
      memset(hf->pllBlockOffSet, 0, dwBlockPosLen);
      hf->pllBlockOffSet[0].LowPart = dwBlockPosLen;

      // Write the block positions. Only swap the first item, rest is zeros.
      WriteFile(ha->hArchiveFile, hf->pllBlockOffSet, dwBlockPosLen, &dwTransferred, NULL);

      // From this position, the archive is considered changed,
      // so the hash table and block table will be written when the archive is closed.
      ha->CurrentOffset += dwTransferred;
      //hf->pBlock->dwCSize = dwTransferred;
      ha->dwFlags |= KPM_FLAG_CHANGED;

      if(dwTransferred == dwBlockPosLen)
        hf->pBlock->dwCSize += dwBlockPosLen;
      else
        nError = GetLastError();
    }


    // Write all file blocks
    if(nError == ERROR_SUCCESS)
    {
      crc32_context crc32_ctx;
      md5_context md5_ctx;
      DWORD nBlock;       

      // Initialize CRC32 and MD5 processing
      CRC32_Init(&crc32_ctx);
      MD5_Init(&md5_ctx);
      nCmp = nCmpFirst;

      // Move the file pointer to the begin of the file
      pbFileCont = (BYTE *)lpszFile;
      for(nBlock = 0; nBlock < hf->nBlocks-1; nBlock++)
      {
        DWORD dwInLength  = ha->dwBlockSize < dwFileSizeLeft ? ha->dwBlockSize : dwFileSizeLeft;
        DWORD dwOutLength = ha->dwBlockSize;

        if(dwInLength == 0)
          break;

        // Load the block from the file
        memcpy(hf->pFileBuffer, pbFileCont, dwInLength);
        pbFileCont += dwInLength;
        dwFileSizeLeft -= dwInLength;

        // Update CRC32 and MD5 for the file
        if(hf->pCrc32 != NULL)
          CRC32_Update(&crc32_ctx, hf->pFileBuffer, dwInLength);
        if(hf->pMD5 != NULL)
          MD5_Update(&md5_ctx, hf->pFileBuffer, dwInLength);

        // Compress the block, if necessary
        dwOutLength = dwInLength;
        if(hf->pBlock->dwFlags & KPM_FILE_COMPRESSED)
        {
          // Should be enough for compression
          int nOutLength = ha->dwBlockSize * 2;
          int nCmpType = 0;

          if(hf->pBlock->dwFlags & KPM_FILE_IMPLODE)
            Compress_pklib((char *)pbCompressed, &nOutLength, (char *)hf->pFileBuffer, dwInLength, &nCmpType, 0);

          if(hf->pBlock->dwFlags & KPM_FILE_COMPRESS)
            SCompCompress((char *)pbCompressed, &nOutLength, (char *)hf->pFileBuffer, dwInLength, nCmp, 0, nCmpLevel);

          // The compressed block size must NOT be the same or greater like
          // the original block size. If yes, do not compress the block
          // and store the data as-is.
          if(nOutLength >= (int)dwInLength)
          {
            memcpy(pbCompressed, hf->pFileBuffer, dwInLength);
            nOutLength = dwInLength;
          }

          // Update block positions
          dwOutLength = nOutLength;
          hf->pllBlockOffSet[nBlock+1].QuadPart = hf->pllBlockOffSet[nBlock].QuadPart + dwOutLength;
          nCmp = nCmpNext;
        }

        // Encrypt the block, if necessary
        if(hf->pBlock->dwFlags & KPM_FILE_ENCRYPTED)
        {
          EncryptBlock((DWORD *)pbToWrite, dwOutLength, hf->dwSeed1 + nBlock);
        }

        // Write the block
        WriteFile(ha->hArchiveFile, pbToWrite, dwOutLength, &dwTransferred, NULL);
        if(dwTransferred != dwOutLength)
        {
          nError = ERROR_DISK_FULL;
          break;
        }

        // Update the hash table position and the compressed file size
        ha->CurrentOffset += dwTransferred;
        hf->pBlock->dwCSize += dwTransferred;
        ha->dwFlags |= KPM_FLAG_CHANGED;
      }

      // Finish calculating of CRC32 and MD5
      if(hf->pCrc32 != NULL)
        CRC32_Finish(&crc32_ctx, (unsigned long *)&hf->pCrc32->dwValue);
      if(hf->pMD5 != NULL)
        MD5_Finish(&md5_ctx, hf->pMD5->Value);
    }


    // Now save the block positions
    if(nError == ERROR_SUCCESS && (hf->pBlock->dwFlags & KPM_FILE_COMPRESSED))
    {
      // If file is encrypted, block positions are also encrypted
      if(dwFlags & KPM_FILE_ENCRYPTED)
        EncryptBlock((DWORD*)hf->pllBlockOffSet, dwBlockPosLen, hf->dwSeed1 - 1);

      // Set the position back to the block table
      SetFilePointer(ha->hArchiveFile, hf->RawFileOffSet.LowPart, &hf->RawFileOffSet.HighPart, FILE_BEGIN);

      // Write block positions to the archive
      
      WriteFile(ha->hArchiveFile, hf->pllBlockOffSet, dwBlockPosLen, &dwTransferred, NULL);
      if(dwTransferred != dwBlockPosLen)
        nError = ERROR_DISK_FULL;

      // Update the file position in the archive
      ha->CurrentOffset = hf->RawFileOffSet.QuadPart + dwTransferred;
    }


    // If success, we have to change the settings
    // in kpm header. If failed, we have to clean hash entry
    if(nError == ERROR_SUCCESS)
    {
      //DWORD dwTableSize;

      ha->BlockCache.pLastFile  = NULL;
      ha->BlockCache.dwblockOffSet  = 0;
      ha->BlockCache.BufferOffSet  = 0;
      


      // Add new entry to the block table (if needed)
      if(hf->dwBlockIndex >= ha->pHeader->dwBlockTableSize)
        ha->pHeader->dwBlockTableSize++;

      // Calculate positions of all tables
      //ha->HashTableOffset.QuadPart = hf->RawFileOffSet.QuadPart + hf->pBlock->dwCSize;没有必要了
      TempFilePos = hf->MpqFileOffSet.QuadPart + hf->pBlock->dwCSize;

      // Set the position of hash table of the archive
      //ha->pHeader->dwHashTablePos = TempFilePos;//没有必要了,固定位置
      
      //dwTableSize = ha->pHeader->dwHashTableSize * sizeof(TKPMHashKey);

      // Update block table pos
      //TempFilePos += dwTableSize;
      //ha->BlockTableOffset.QuadPart = ha->HashTableOffset.QuadPart   + dwTableSize;没有必要了
      //ha->pHeader->dwBlockTablePos = TempFilePos; //没有必要了,固定位置
      //dwTableSize = ha->pHeader->dwBlockTableSize * sizeof(TKPMBlock);

      // If the archive size exceeded 4GB, we have to use extended block table
      //TempFilePos += dwTableSize;
      
      // Update archive size (only valid for version V1)
      ha->KpmSize = TempFilePos;
      ha->pHeader->dwArchiveSize = TempFilePos;
    }
    else
    {
      // Clear the hash table entry
      if(hf != NULL && hf->pHash != NULL)
        memset(hf->pHash, 0xFF, sizeof(TKPMHashKey));
    }

    // Cleanup
    if(pbCompressed != NULL)
      KPM_FREE(pbCompressed);
    if(pbReplaced != NULL)
      *pbReplaced = bReplaced;
    
    FreeFile(hf);

    return nError;

  }

  int AddFileToArchive(TKPMArchive * ha,
    HANDLE hFile, 
    const char * szArchivedName, 
    DWORD dwFlags, 
    DWORD dwQuality, 
    int nFileType, 
    BOOL * pbReplaced)
  {
    int nError = ERROR_SUCCESS;
    int nCmpFirst = KPM_GetDataCompression();           // Compression for the first data block
    int nCmpNext  = KPM_GetDataCompression();           // Compression for the next data blocks
    int nCmp      = KPM_GetDataCompression();           // Current compression
    int nCmpLevel = -1;                         // Compression level

    TKPMBlock * pBlockEnd;                      // Pointer to end of the block table
    DWORD    dwFileSize = 0;                 // Low 32-bits of the file size
    DWORD    dwFileSizeHigh = 0;             // High 32 bits of the file size
    
    TKPMFile *  hf  =   NULL;                  // File structure for newly added file
    BOOL    bReplaced = FALSE;              // TRUE if replaced, FALSE if added
    LONGLONG  TempFilePos;              // For various file offset calculations
    BYTE *    pbToWrite = NULL;               // Data to write to the file
    BYTE *    pbCompressed = NULL;            // Compressed (target) data
    DWORD    dwTransferred = 0;              // Number of bytes read or written

    pBlockEnd = ha->pBlockTable + ha->pHeader->dwBlockTableSize;

    // Set the correct compression types
    if(dwFlags & KPM_FILE_IMPLODE)
      nCmpFirst = nCmpNext = KPM_COMPRESSION_PKWARE;
    

    if(dwFlags & KPM_FILE_COMPRESS)
    {
      if(nFileType == FILE_TYPE_DATA)
        nCmpFirst = nCmpNext = nDataCmp;

      if(nFileType == FILE_TYPE_WAVE)
      {
        nCmpNext  = uWaveCmpType[dwQuality];
        nCmpLevel = uWaveCmpLevel[dwQuality];
      }
    }

    // Get the size of the file to be added
    if(nError == ERROR_SUCCESS)
    {
      dwFileSize = GetFileSize(hFile, &dwFileSizeHigh);

      // Adjust file flags for too-small files
      if(dwFileSize < 0x04)
        dwFlags &= ~(KPM_FILE_ENCRYPTED | KPM_FILE_FIXSEED);
      if(dwFileSize < 0x20)
        dwFlags &= ~KPM_FILE_COMPRESSED;

      // File in KPM cannot be greater than 4GB
      if(dwFileSizeHigh != 0)
        nError = ERROR_PARAMETER_QUOTA_EXCEEDED;
    }

    // Allocate the TKPMFile entry for newly added file
    if(nError == ERROR_SUCCESS)
    {
      hf = (TKPMFile *)KPM_ALLOC(BYTE, sizeof(TKPMFile) + strlen(szArchivedName));
      assert(hf);
      if(hf == NULL)
        nError = ERROR_NOT_ENOUGH_MEMORY;
      else
        memset(hf,0,sizeof(TKPMFile)+strlen(szArchivedName));
    }

    // Reset the TMPQFile structure
    if(nError == ERROR_SUCCESS)
    {
      memset(hf, 0, sizeof(TKPMFile));
      strcpy(hf->szFileName, szArchivedName);
      hf->ha = ha;

      // Check if the file already exists in the archive
      if((hf->pHash = GetHashEntry(ha, szArchivedName)) != NULL)
      {
        if((dwFlags & KPM_FILE_REPLACEEXISTING) == 0)
        {
          nError = ERROR_ALREADY_EXISTS;
          hf->pHash = NULL;
        }
        else
        {
          hf->pBlock = ha->pBlockTable + hf->pHash->dwBlockIndex;
          bReplaced = TRUE;
        }
      }

      if(nError == ERROR_SUCCESS && hf->pHash == NULL)
      {
        hf->pHash = FindFreeHashEntry(ha, szArchivedName);
        if(hf->pHash == NULL)
          nError = ERROR_HANDLE_DISK_FULL;
      }

      // Set the hash index
      hf->dwHashIndex = (DWORD)(hf->pHash - ha->pHashKeyTable);
    }


    // Find a block table entry for the file
    if(nError == ERROR_SUCCESS)
    {
      TKPMBlock * pBlock = NULL;          // Entry in the block table

      // Get the position of the first file
      hf->MpqFileOffSet.QuadPart = ha->pHeader->dwBlockTablePos+ha->pHeader->dwBlockTableSize*sizeof(TKPMBlock);

      // Search the entire block table and find a free block.
      // Also find KPM offset at which the file data will be stored
      
      for(pBlock = ha->pBlockTable; pBlock < pBlockEnd; pBlock++)
      {
        if(pBlock->dwFlags & KPM_FILE_EXISTS)
        {
          
          TempFilePos   = pBlock->dwFilePos;
          TempFilePos  += pBlock->dwCSize;

          if(TempFilePos> hf->MpqFileOffSet.QuadPart)
            hf->MpqFileOffSet.QuadPart = TempFilePos;
        }
        else
        {
          if(hf->pBlock == NULL)
          {
            hf->pBlock = pBlock;
          }
        }
      }

      // Calculate the raw file offset
      hf->RawFileOffSet.QuadPart = hf->MpqFileOffSet.QuadPart + ha->HeaderOffset;

      // If no free block in the middle of the block table,
      // use the one after last used block
      if(hf->pBlock == NULL)
      {
        hf->pBlock = pBlock;
      }
      // If the block offset exceeds number of hash entries,
      // we cannot add new file to the KPM
      hf->dwBlockIndex = (DWORD)(hf->pBlock - ha->pBlockTable);
      if(hf->dwBlockIndex >= ha->pHeader->dwHashTableSize)
        nError = ERROR_HANDLE_DISK_FULL;

    }


    // Create seed1 for file encryption
    if(nError == ERROR_SUCCESS && (dwFlags & KPM_FILE_ENCRYPTED))
    {
      const char * szTemp = strrchr(szArchivedName, '\\');

      // Create seed1 for file encryption
      if(szTemp != NULL)
        szArchivedName = szTemp + 1;

      hf->dwSeed1 = DecryptFileSeed(szArchivedName);
      if(dwFlags & KPM_FILE_FIXSEED)
      {
        hf->dwSeed1 = (hf->dwSeed1 + hf->MpqFileOffSet.LowPart) ^ dwFileSize;
      }
    }

    // Resolve CRC32 and MD5 entry for the file
    // Only do it when the MPQ archive has attributes
    if(nError == ERROR_SUCCESS && ha->pFileAttr != NULL)
    {
      if(ha->pFileAttr->pCrc32 != NULL)
        hf->pCrc32 = ha->pFileAttr->pCrc32 + hf->dwBlockIndex;
      if(ha->pFileAttr->pFileTime != NULL)
        hf->pFileTime = ha->pFileAttr->pFileTime + hf->dwBlockIndex;
      if(ha->pFileAttr->pMd5 != NULL)
        hf->pMD5 = ha->pFileAttr->pMd5 + hf->dwBlockIndex;
    }

    // Allocate buffers for the compressed data
    if(nError == ERROR_SUCCESS)
    {
      hf->nBlocks = (dwFileSize / ha->dwBlockSize) + 1;
      if(dwFileSize % ha->dwBlockSize)
        hf->nBlocks++;

      if((hf->pFileBuffer = KPM_ALLOC(BYTE, ha->dwBlockSize)) == NULL)
        nError = ERROR_NOT_ENOUGH_MEMORY;
      else      
        memset(hf->pFileBuffer,0,ha->dwBlockSize);      
      pbToWrite = hf->pFileBuffer;
    }
    
    // For compressed files, allocate buffer for block positions and for the compressed data
    if(nError == ERROR_SUCCESS && (dwFlags & KPM_FILE_COMPRESSED))
    {
      hf->pllBlockOffSet = KPM_ALLOC(LARGE_INTEGER, hf->nBlocks + 1);
      assert(hf->pllBlockOffSet);
      pbCompressed = KPM_ALLOC(BYTE, ha->dwBlockSize * 2);
      assert(pbCompressed);
      if(hf->pllBlockOffSet == NULL || pbCompressed == NULL)
        nError = ERROR_NOT_ENOUGH_MEMORY;
      if(hf->pllBlockOffSet)
        memset(hf->pllBlockOffSet,0,sizeof(LARGE_INTEGER)*(hf->nBlocks + 1));
      if(pbCompressed)
        memset(pbCompressed,0,ha->dwBlockSize * 2);
      pbToWrite = pbCompressed;
    }


    // Set the file position to the point where the file will be stored
    if(nError == ERROR_SUCCESS)
    {
      // Set the file pointer to file data position
      if(hf->RawFileOffSet.QuadPart != ha->CurrentOffset)
      {
        SetFilePointer(ha->hArchiveFile, hf->RawFileOffSet.LowPart, &hf->RawFileOffSet.HighPart, FILE_BEGIN);
        ha->CurrentOffset = hf->RawFileOffSet.QuadPart;
      }

      // Initialize the hash entry for the file
      hf->pHash->dwBlockIndex = hf->dwBlockIndex;

      // Initialize the block table entry for the file
      
      hf->pBlock->dwFilePos = hf->MpqFileOffSet.QuadPart;
      hf->pBlock->dwFSize   = dwFileSize;
      hf->pBlock->dwCSize   = 0;
      hf->pBlock->dwFlags   = dwFlags | KPM_FILE_EXISTS;
    }
    

    DWORD dwBlockPosLen = 0; // Length of the file block offset (in bytes)

    // Write block positions (if the file will be compressed)
    if(nError == ERROR_SUCCESS && (dwFlags & KPM_FILE_COMPRESSED))
    {
      dwBlockPosLen = hf->nBlocks * sizeof(LARGE_INTEGER);
    
      memset(hf->pllBlockOffSet, 0, dwBlockPosLen);
      hf->pllBlockOffSet[0].LowPart = dwBlockPosLen;

      // Write the block positions. Only swap the first item, rest is zeros.
      WriteFile(ha->hArchiveFile, hf->pllBlockOffSet, dwBlockPosLen, &dwTransferred, NULL);

      // From this position, the archive is considered changed,
      // so the hash table and block table will be written when the archive is closed.
      ha->CurrentOffset += dwTransferred;
      //hf->pBlock->dwCSize = dwTransferred;
      ha->dwFlags |= KPM_FLAG_CHANGED;

      if(dwTransferred == dwBlockPosLen)
        hf->pBlock->dwCSize += dwBlockPosLen;
      else
        nError = GetLastError();
    }


    // Write all file blocks
    if(nError == ERROR_SUCCESS)
    {
      crc32_context crc32_ctx;
      md5_context md5_ctx;
      DWORD nBlock;       

      // Initialize CRC32 and MD5 processing
      CRC32_Init(&crc32_ctx);
      MD5_Init(&md5_ctx);
      nCmp = nCmpFirst;

      // Move the file pointer to the begin of the file
      SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
      for(nBlock = 0; nBlock < hf->nBlocks-1; nBlock++)
      {
        DWORD dwInLength  = ha->dwBlockSize;
        DWORD dwOutLength = ha->dwBlockSize;

        // Load the block from the file
        ReadFile(hFile, hf->pFileBuffer, ha->dwBlockSize, &dwInLength, NULL);
        if(dwInLength == 0)
          break;

        // Update CRC32 and MD5 for the file
        if(hf->pCrc32 != NULL)
          CRC32_Update(&crc32_ctx, hf->pFileBuffer, dwInLength);
        if(hf->pMD5 != NULL)
          MD5_Update(&md5_ctx, hf->pFileBuffer, dwInLength);

        // Compress the block, if necessary
        dwOutLength = dwInLength;
        if(hf->pBlock->dwFlags & KPM_FILE_COMPRESSED)
        {
          // Should be enough for compression
          int nOutLength = ha->dwBlockSize * 2;
          int nCmpType = 0;

          if(hf->pBlock->dwFlags & KPM_FILE_IMPLODE)
            Compress_pklib((char *)pbCompressed, &nOutLength, (char *)hf->pFileBuffer, dwInLength, &nCmpType, 0);

          if(hf->pBlock->dwFlags & KPM_FILE_COMPRESS)
            SCompCompress((char *)pbCompressed, &nOutLength, (char *)hf->pFileBuffer, dwInLength, nCmp, 0, nCmpLevel);

          // The compressed block size must NOT be the same or greater like
          // the original block size. If yes, do not compress the block
          // and store the data as-is.
          if(nOutLength >= (int)dwInLength)
          {
            memcpy(pbCompressed, hf->pFileBuffer, dwInLength);
            nOutLength = dwInLength;
          }

          // Update block positions
          dwOutLength = nOutLength;
          hf->pllBlockOffSet[nBlock+1].QuadPart = hf->pllBlockOffSet[nBlock].QuadPart + dwOutLength;
          nCmp = nCmpNext;
        }

        // Encrypt the block, if necessary
        if(hf->pBlock->dwFlags & KPM_FILE_ENCRYPTED)
        {
          EncryptBlock((DWORD *)pbToWrite, dwOutLength, hf->dwSeed1 + nBlock);
        }

        // Write the block
        WriteFile(ha->hArchiveFile, pbToWrite, dwOutLength, &dwTransferred, NULL);
        if(dwTransferred != dwOutLength)
        {
          nError = ERROR_DISK_FULL;
          break;
        }

        // Update the hash table position and the compressed file size
        ha->CurrentOffset += dwTransferred;
        hf->pBlock->dwCSize += dwTransferred;
        ha->dwFlags |= KPM_FLAG_CHANGED;
      }

      // Finish calculating of CRC32 and MD5
      if(hf->pCrc32 != NULL)
        CRC32_Finish(&crc32_ctx, (unsigned long *)&hf->pCrc32->dwValue);
      if(hf->pMD5 != NULL)
        MD5_Finish(&md5_ctx, hf->pMD5->Value);
    }


    // Now save the block positions
    if(nError == ERROR_SUCCESS && (hf->pBlock->dwFlags & KPM_FILE_COMPRESSED))
    {
      // If file is encrypted, block positions are also encrypted
      if(dwFlags & KPM_FILE_ENCRYPTED)
        EncryptBlock((DWORD*)hf->pllBlockOffSet, dwBlockPosLen, hf->dwSeed1 - 1);

      // Set the position back to the block table
      SetFilePointer(ha->hArchiveFile, hf->RawFileOffSet.LowPart, &hf->RawFileOffSet.HighPart, FILE_BEGIN);

      // Write block positions to the archive
      
      WriteFile(ha->hArchiveFile, hf->pllBlockOffSet, dwBlockPosLen, &dwTransferred, NULL);
      if(dwTransferred != dwBlockPosLen)
        nError = ERROR_DISK_FULL;

      // Update the file position in the archive
      ha->CurrentOffset = hf->RawFileOffSet.QuadPart + dwTransferred;
    }


    // If success, we have to change the settings
    // in kpm header. If failed, we have to clean hash entry
    if(nError == ERROR_SUCCESS)
    {
      //DWORD dwTableSize;

      ha->BlockCache.pLastFile  = NULL;
      ha->BlockCache.dwblockOffSet  = 0;
      ha->BlockCache.BufferOffSet  = 0;
      


      // Add new entry to the block table (if needed)
      if(hf->dwBlockIndex >= ha->pHeader->dwBlockTableSize)
        ha->pHeader->dwBlockTableSize++;

      // Calculate positions of all tables
      //ha->HashTableOffset.QuadPart = hf->RawFileOffSet.QuadPart + hf->pBlock->dwCSize;没有必要了
      TempFilePos = hf->MpqFileOffSet.QuadPart + hf->pBlock->dwCSize;

      // Set the position of hash table of the archive
      //ha->pHeader->dwHashTablePos = TempFilePos;//没有必要了,固定位置
      
      //dwTableSize = ha->pHeader->dwHashTableSize * sizeof(TKPMHashKey);

      // Update block table pos
      //TempFilePos += dwTableSize;
      //ha->BlockTableOffset.QuadPart = ha->HashTableOffset.QuadPart   + dwTableSize;没有必要了
      //ha->pHeader->dwBlockTablePos = TempFilePos; //没有必要了,固定位置
      //dwTableSize = ha->pHeader->dwBlockTableSize * sizeof(TKPMBlock);

      // If the archive size exceeded 4GB, we have to use extended block table
      //TempFilePos += dwTableSize;
      
      // Update archive size (only valid for version V1)
      ha->KpmSize = TempFilePos;
      ha->pHeader->dwArchiveSize = TempFilePos;
    }
    else
    {
      // Clear the hash table entry
      if(hf != NULL && hf->pHash != NULL)
        memset(hf->pHash, 0xFF, sizeof(TKPMHashKey));
    }

    // Cleanup
    if(pbCompressed != NULL)
      KPM_FREE(pbCompressed);
    if(pbReplaced != NULL)
      *pbReplaced = bReplaced;
    
    FreeFile(hf);

    return nError;

  }


  int InternalUpdateFile(TKPMArchive * ha,
               HANDLE hFile,
               TKPMFile* pFile,
               const char * szArchivedName, 
               DWORD dwFlags, 
               DWORD dwQuality, 
               int   nFileType, 
               BOOL * pbReplaced)
  {
    int nError = ERROR_SUCCESS;
    TKPMBlock * pBlockEnd;                      // Pointer to end of the block table
    DWORD    dwFileSize = 0;                 // Low 32-bits of the file size
    DWORD    dwFileSizeHigh = 0;             // High 32 bits of the file size

    TKPMFile *  hf  =   pFile;                  // File structure for newly added file
    BOOL    bReplaced = FALSE;              // TRUE if replaced, FALSE if added
    BYTE *    pbToWrite = NULL;               // Data to write to the file
    DWORD    dwTransferred = 0;              // Number of bytes read or written

    pBlockEnd = ha->pBlockTable + ha->pHeader->dwBlockTableSize;

    // Set the correct compression types
    if(dwFlags & KPM_FILE_IMPLODE)
    {
      nError = ERROR_INVALID_PARAMETER;
    }


    if(dwFlags & KPM_FILE_COMPRESS)
    {
      nError = ERROR_INVALID_PARAMETER;
    }

    // Get the size of the file to be added
    if(nError == ERROR_SUCCESS)
    {
      
      dwFileSize = GetFileSize(hFile, &dwFileSizeHigh);

      // File in KPM cannot be greater than 4GB
      if(dwFileSizeHigh != 0)
        nError = ERROR_PARAMETER_QUOTA_EXCEEDED;
    }

  
    // Reset the TMPQFile structure
    if(nError == ERROR_SUCCESS)
    {
      // Check if the file already exists in the archive
      if((hf->pHash = GetHashEntry(ha, szArchivedName)) != NULL)
      {
        if((dwFlags & KPM_FILE_REPLACEEXISTING) == 0)
        {
          nError = ERROR_ALREADY_EXISTS;
          hf->pHash = NULL;
        }
        else
        {
          hf->pBlock = ha->pBlockTable + hf->pHash->dwBlockIndex;
          bReplaced = TRUE;
        }
      }

      if(nError == ERROR_SUCCESS && hf->pHash == NULL)
      {
        assert(false);
        if(hf->pHash == NULL)
          nError = ERROR_HANDLE_DISK_FULL;
      }
    }

    // Allocate buffers for the compressed data
    if(nError == ERROR_SUCCESS)
    {

      hf->nBlocks = (dwFileSize / ha->dwBlockSize) + 1;
      if(dwFileSize % ha->dwBlockSize)
        hf->nBlocks++;


      if((hf->pFileBuffer = KPM_ALLOC(BYTE, ha->dwBlockSize)) == NULL)
        nError = ERROR_NOT_ENOUGH_MEMORY;
      else      
        memset(hf->pFileBuffer,0,ha->dwBlockSize);      
      pbToWrite = hf->pFileBuffer;
    }
  
    // Set the file position to the point where the file will be stored
    if(nError == ERROR_SUCCESS)
    {
      // Set the file pointer to file data position
      if(hf->RawFileOffSet.QuadPart != ha->CurrentOffset)
      {
        SetFilePointer(ha->hArchiveFile, hf->RawFileOffSet.LowPart, &hf->RawFileOffSet.HighPart, FILE_BEGIN);
        ha->CurrentOffset = hf->RawFileOffSet.QuadPart;
      }
    }

    // Write all file blocks
    if(nError == ERROR_SUCCESS)
    {
      crc32_context crc32_ctx;
      md5_context md5_ctx;
      DWORD nBlock;       

      // Initialize CRC32 and MD5 processing
      CRC32_Init(&crc32_ctx);
      MD5_Init(&md5_ctx);
    
      // Move the file pointer to the begin of the file
      SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
      DWORD dbWrited = 0;
      for(nBlock = 0; nBlock < hf->nBlocks-1; nBlock++)
      {
        DWORD dwInLength  = ha->dwBlockSize;
        DWORD dwOutLength = ha->dwBlockSize;

        // Load the block from the file
        ReadFile(hFile, hf->pFileBuffer, ha->dwBlockSize, &dwInLength, NULL);
        if(dwInLength == 0)
          break;

        // Update CRC32 and MD5 for the file
        if(hf->pCrc32 != NULL)
          CRC32_Update(&crc32_ctx, hf->pFileBuffer, dwInLength);
        if(hf->pMD5 != NULL)
          MD5_Update(&md5_ctx, hf->pFileBuffer, dwInLength);

        // Compress the block, if necessary
        dwOutLength = dwInLength;
      
        if(dbWrited+dwOutLength>hf->pBlock->dwCSize)
        {
          assert(false);//应该不会出现
        }

        // Write the block
        WriteFile(ha->hArchiveFile, pbToWrite, dwOutLength, &dwTransferred, NULL);
        if(dwTransferred != dwOutLength)
        {
          nError = ERROR_DISK_FULL;
          break;
        }
        // Update the hash table position and the compressed file size
        ha->CurrentOffset += dwTransferred;
        dbWrited += dwTransferred;
        ha->dwFlags |= KPM_FLAG_CHANGED;
      }

      // Finish calculating of CRC32 and MD5
      if(hf->pCrc32 != NULL)
        CRC32_Finish(&crc32_ctx, (unsigned long *)&hf->pCrc32->dwValue);
      if(hf->pMD5 != NULL)
        MD5_Finish(&md5_ctx, hf->pMD5->Value);
    }


    // If success, we have to change the settings
    // in kpm header. If failed, we have to clean hash entry
    if(nError == ERROR_SUCCESS)
    {
      ha->BlockCache.pLastFile  = NULL;
      ha->BlockCache.dwblockOffSet  = 0;
      ha->BlockCache.BufferOffSet  = 0;
    
    }
    else
    {
      // Clear the hash table entry
      if(hf != NULL && hf->pHash != NULL)
        memset(hf->pHash, 0xFF, sizeof(TKPMHashKey));
    }

    if(pbReplaced != NULL)
      *pbReplaced = bReplaced;

    return nError;

  }

  

}







