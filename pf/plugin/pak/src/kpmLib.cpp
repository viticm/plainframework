#include "kpmLib.h"
#include "kpmListFile.h"
#include "kpmEncrypt.h"
#include "Util.h"
#include "kpmCompress.h"


namespace KPM
{
#define ID_WAVE     0x46464952          // Signature of WAVes for name breaking
#define ID_EXE      0x00005A4D          // Signature of executable files
#define HASH_TABLE_SIZE_MIN    0x00004
#define HASH_TABLE_SIZE_MAX    0x40000


extern int nDataCmp;
  

  static BOOL IsValidSearchHandle(TKPMSearch * hs)
  {
    if(hs == NULL)
      return FALSE;

    if(!IsValidArchiveHandle(hs->ha))
      return FALSE;

    return TRUE;
  }

  // This function compares a string with a wildcard search string.
  // returns TRUE, when the string matches with the wildcard.
  BOOL CheckWildCard(const char * szString, const char * szWildCard)
  {
    char * szTemp;                      // Temporary helper pointer
    int nResult = 0;                    // For memcmp return values
    int nMustNotMatch = 0;              // Number of following chars int szString,
    // which must not match with szWildCard
    int nMustMatch = 0;                 // Number of the following characters,
    // which must match

    // When the string is empty, it does not match with every wildcard
    if(*szString == 0)
      return FALSE;

    // When the mask is empty, it matches to every wildcard
    if(szWildCard == NULL || *szWildCard == 0)
      return FALSE;

    // Do normal test
    for(;;)
    {
      switch(*szWildCard)
      {
      case '*': // Means "every number of characters"
        // Skip all asterisks
        while(*szWildCard == '*')
          szWildCard++;

        // When no more characters in wildcard, it means that the strings match
        if(*szWildCard == 0)
          return TRUE;

        // The next N characters must not agree
        nMustNotMatch |= 0x70000000;
        break;

      case '?':  // Means "One or no character"
        while(*szWildCard == '?')
        {
          nMustNotMatch++;
          szWildCard++;
        }
        break;

      default:
        // If the two characters match
        if(toupper(*szString) == toupper(*szWildCard))
        {
          // When end of string, they agree
          if(*szString == 0)
            return TRUE;

          nMustNotMatch = 0;
          szWildCard++;
          szString++;
          break;
        }

        // If the next character must match, the string does not match
        if(nMustNotMatch == 0)
          return FALSE;

        // Count the characters which must match after characters
        // that must not match
        szTemp = (char *)szWildCard;
        nMustMatch = 0;
        while(*szTemp != 0 && *szTemp != '*' && *szTemp != '?')
        {
          nMustMatch++;
          szTemp++;
        }

        // Now skip characters from szString up to number of chars
        // that must not match
        nResult = -1;
        while(nMustNotMatch > 0 && *szString != 0)
        {
          if((nResult = _strnicmp(szString, szWildCard, nMustMatch)) == 0)
            break;

          szString++;
          nMustNotMatch--;
        }

        // Make one more comparison
        if(nMustNotMatch == 0)
          nResult = _strnicmp(szString, szWildCard, nMustMatch);

        // If a match has been found, continue the search
        if(nResult == 0)
        {
          nMustNotMatch = 0;
          szWildCard += nMustMatch;
          szString   += nMustMatch;
          break;
        }
        return FALSE;
      }
    }
  }
  
  // Performs one KPM search
  static int DoKPMSearch(TKPMSearch * hs,FILE_FIND_DATA * lpFindFileData)
  {
    TKPMArchive * ha = hs->ha;
    TFileNode * pNode;
    TKPMHashKey * pHashEnd  =  ha->pHashKeyTable + ha->pHeader->dwHashTableSize;
    TKPMHashKey * pHash    =  ha->pHashKeyTable + hs->dwNextIndex;
    
    // Do until some file found or no more files
    while(pHash < pHashEnd)
    {
      pNode = ha->pListFile[hs->dwNextIndex++];
      // If this entry is free, do nothing
      if(pHash->dwBlockIndex < HASH_ENTRY_FREE && (DWORD_PTR)pNode < HASH_ENTRY_FREE)
      {
        
        // Check the file name.
        if(CheckWildCard(pNode->szFileName, hs->szSearchMask))
        {
          TKPMBlock * pBlock = ha->pBlockTable + pHash->dwBlockIndex;
          lpFindFileData->dwFileSize   = pBlock->dwFSize;
          lpFindFileData->dwFileFlags  = pBlock->dwFlags;
          lpFindFileData->dwBlockIndex = pHash->dwBlockIndex;
          lpFindFileData->dwCompSize   = pBlock->dwCSize;

          // Fill the file name and plain file name
          strncpy(lpFindFileData->cFileName, pNode->szFileName, MAX_PATH);
          lpFindFileData->szPlainName = strrchr(lpFindFileData->cFileName, '\\');
          if(lpFindFileData->szPlainName == NULL)
            lpFindFileData->szPlainName = lpFindFileData->cFileName;
          else
            lpFindFileData->szPlainName++;
          // Fill the next entry
          return ERROR_SUCCESS;
        }
      }

      pHash++;
    }
    // No more files found, return error
    return ERROR_NO_MORE_FILES;
  }

  static void FreeSearch(TKPMSearch *& hs)
  {
    if(hs != NULL)
    {
      KPM_FREE(hs);
      hs = NULL;
    }
  }
  // This function gets the right positions of the hash table and the block table.
  static int RelocateTablePositions(TKPMArchive * ha)
  {
    TKPMHeader * pHeader = ha->pHeader;
    LARGE_INTEGER FileSize;
    LARGE_INTEGER TempSize;

    // Get the size of the file
    FileSize.LowPart = GetFileSize(ha->hArchiveFile, (LPDWORD)&FileSize.HighPart);

    // Set the proper hash table position
  
    ha->HashTableOffset.QuadPart = pHeader->dwHashTablePos;
    ha->HashTableOffset.QuadPart += ha->HeaderOffset;
    if(ha->HashTableOffset.QuadPart > FileSize.QuadPart)
      return ERROR_BAD_FORMAT;

    // Set the proper block table position
    
    ha->BlockTableOffset.QuadPart = pHeader->dwBlockTablePos;
    ha->BlockTableOffset.QuadPart    += ha->HeaderOffset;
    if(ha->BlockTableOffset.QuadPart > FileSize.QuadPart)
      return ERROR_BAD_FORMAT;

    // Size of KPM archive is computed as the biggest of
    // (EndOfBlockTable, EndOfHashTable, EndOfExtBlockTable)
    TempSize.QuadPart = ha->HashTableOffset.QuadPart + (pHeader->dwHashTableSize * sizeof(TKPMHashKey));
    if(TempSize.QuadPart > ha->KpmSize)
      ha->KpmSize = TempSize.QuadPart;
    TempSize.QuadPart = ha->BlockTableOffset.QuadPart + (pHeader->dwBlockTableSize * sizeof(TKPMBlock));
    if(TempSize.QuadPart > ha->KpmSize)
      ha->KpmSize = TempSize.QuadPart;
        

    // KPM size does not include the bytes before KPM header
    //ha->KpmSize    -= ha->HeaderOffset;
    return ERROR_SUCCESS;
  }

  TKPMArchive*  KPM_OpenArchive(const char* szFilename,DWORD& nError,bool WriteFlag)
  {
    
    TKPMArchive * ha = NULL;        // Archive handle
    HANDLE hFile = INVALID_HANDLE_VALUE;  // Opened archive file handle
    DWORD dwMaxBlockIndex = 0;          // Maximum value of block entry
    DWORD dwBlockTableSize = 0;         // Block table size.
    DWORD dwTransferred;                // Number of bytes read
    DWORD dwBytes = 0;                  // Number of bytes to read
    LARGE_INTEGER TempPos;
    
    nError = ERROR_SUCCESS;
    
    // Check the right parameters
    if(nError == ERROR_SUCCESS)
    {
      if(szFilename == NULL || *szFilename == 0 )
        nError = ERROR_INVALID_PARAMETER;
    }

    // Ensure that StormBuffer is allocated
    if(nError == ERROR_SUCCESS)
      nError =  PerpareBuffers();

    // Open the MPQ archive file
    if(nError == ERROR_SUCCESS)
    {
      if(WriteFlag)
                hFile = CreateFile(szFilename, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
      else
                hFile = CreateFile(szFilename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
      if(hFile == INVALID_HANDLE_VALUE)
        nError = GetLastError();
    }

    // Allocate the KPMhandle
    if(nError == ERROR_SUCCESS)
    {
      if((ha = KPM_ALLOC(TKPMArchive, 1)) == NULL)
      {
        nError = ERROR_NOT_ENOUGH_MEMORY;
      }
      else
      {
        memset(ha,0,sizeof(TKPMArchive));
      }
    }

    // Initialize handle structure and allocate structure for MPQ header
    if(nError == ERROR_SUCCESS)
    {
      memset(ha, 0, sizeof(TKPMArchive));
      strncpy(ha->szFileName, szFilename, strlen(szFilename));
      ha->hArchiveFile      = hFile;
      ha->pHeader    = &ha->Header;
      ha->BlockCache.pLastFile  = NULL;
      hFile = INVALID_HANDLE_VALUE;
    }


    // Find the offset of KPM header within the file
    if(nError == ERROR_SUCCESS)
    {
      LARGE_INTEGER SearchPos = {0};
      LARGE_INTEGER MpqPos = {0};
      DWORD dwHeaderID;

      for(;;)
      {
        // Invalidate the KPM ID and read the eventual header
        SetFilePointer(ha->hArchiveFile, MpqPos.LowPart, &MpqPos.HighPart, FILE_BEGIN);
        ReadFile(ha->hArchiveFile, ha->pHeader, sizeof(TKPMHeader), &dwTransferred, NULL);
        dwHeaderID = ha->pHeader->dwMagic;
        ha->KpmSize = ha->Header.dwArchiveSize;
        // If different number of bytes read, break the loop
        if(dwTransferred != sizeof(TKPMHeader))
        {
          nError = ERROR_BAD_FORMAT;
          break;
        }

        // There must be MPQ header signature
        if(dwHeaderID == KPM_MAGIC)
        {
          

          // Save the position where the MPQ header has been found
          ha->HeaderOffset = ((sizeof(TKPMHeader)-1)/512 +1)*512;;

          if(ha->pHeader->dwVersion == 1400)
          {
            break;
          }

          nError = ERROR_NOT_SUPPORTED;
          break;
        }
        nError = ERROR_BAD_FORMAT;
        return NULL;
      }
    }
    
    // Relocate tables position
    if(nError == ERROR_SUCCESS)
    {
      ha->dwBlockSize = (0x200 << ha->pHeader->wBlockSize);
      nError = RelocateTablePositions(ha);
    }

    // Allocate buffers
    if(nError == ERROR_SUCCESS)
    {
      //
      // Note that the block table should be as large as the hash table
      // (For later file additions).
      //
      // I have found a KPM which has the block table larger than
      // the hash table. We should avoid buffer overruns caused by that.
      //

      if(ha->pHeader->dwBlockTableSize > ha->pHeader->dwHashTableSize)
        ha->pHeader->dwBlockTableSize = ha->pHeader->dwHashTableSize;
      dwBlockTableSize   = ha->pHeader->dwHashTableSize;

      ha->pHashKeyTable      = KPM_ALLOC(TKPMHashKey, ha->pHeader->dwHashTableSize);
      ha->pBlockTable        = KPM_ALLOC(TKPMBlock, dwBlockTableSize);
      ha->BlockCache.pbBuffer    = KPM_ALLOC(BYTE, ha->dwBlockSize);
      assert(ha->pHashKeyTable);
      memset(ha->pHashKeyTable,0,sizeof(TKPMHashKey)*(ha->pHeader->dwHashTableSize));
      assert(ha->pBlockTable);
      memset(ha->pBlockTable,0,sizeof(TKPMBlock)*dwBlockTableSize);
      assert(ha->BlockCache.pbBuffer);
      memset(ha->BlockCache.pbBuffer,0,ha->dwBlockSize);

      if(!ha->pHashKeyTable || !ha->pBlockTable || !ha->BlockCache.pbBuffer)
        nError = ERROR_NOT_ENOUGH_MEMORY;
    }

    // Read the hash table into memory
    if(nError == ERROR_SUCCESS)
    {
      dwBytes = ha->pHeader->dwHashTableSize * sizeof(TKPMHashKey);
      SetFilePointer(ha->hArchiveFile, ha->HashTableOffset.LowPart, &ha->HashTableOffset.HighPart, FILE_BEGIN);
      ReadFile(ha->hArchiveFile, ha->pHashKeyTable, dwBytes, &dwTransferred, NULL);

      if(dwTransferred != dwBytes)
        nError = ERROR_FILE_CORRUPT;
    }

    // Decrypt hash table and check if it is correctly decrypted
    if(nError == ERROR_SUCCESS)
    {
      // We have to convert the hash table from LittleEndian
      Decrypt((DWORD *)ha->pHashKeyTable, (BYTE *)"(hashkey table)", (dwBytes>>2));
    
    }

    // Now, read the block table
    if(nError == ERROR_SUCCESS)
    {
      memset(ha->pBlockTable, 0, dwBlockTableSize * sizeof(TKPMBlock));

      // Carefully check the block table size
      dwBytes = ha->pHeader->dwBlockTableSize * sizeof(TKPMBlock);
      SetFilePointer(ha->hArchiveFile, ha->BlockTableOffset.LowPart, &ha->BlockTableOffset.HighPart, FILE_BEGIN);
      ReadFile(ha->hArchiveFile, ha->pBlockTable, dwBytes, &dwTransferred, NULL);

      // I have found a MPQ which claimed 0x200 entries in the block table,
      // but the file was cut and there was only 0x1A0 entries.
      // We will handle this case properly, even if that means 
      // omiting another integrity check of the MPQ
      if(dwTransferred < dwBytes)
        dwBytes = dwTransferred;

      // If nothing was read, we assume the file is corrupt.
      if(dwTransferred == 0)
        nError = ERROR_FILE_CORRUPT;
    }

    // Decrypt block table.
    // Some KPMs don't have Decrypted block table, e.g. cracked Diablo version
    // We have to check if block table is really encrypted
    if(nError == ERROR_SUCCESS)
    {
      TKPMBlock * pBlockEnd = ha->pBlockTable + ha->pHeader->dwBlockTableSize;
      TKPMBlock * pBlock = ha->pBlockTable;
      BOOL bBlockTableEncrypted = FALSE;

      // Verify all blocks entries in the table
      // The loop usually stops at the first entry
      while(pBlock < pBlockEnd)
      {
        // The lower 8 bits of the KPM flags are always zero.
        // Note that this may change in next KPM versions
        if(pBlock->dwFlags & 0x000000FF)
        {
          bBlockTableEncrypted = TRUE;
          break;
        }

        // Move to the next block table entry
        pBlock++;
      }

      if(bBlockTableEncrypted)
      {
        dwBytes = ha->pHeader->dwBlockTableSize * sizeof(TKPMBlock);

        Decrypt((DWORD *)ha->pBlockTable,
          (BYTE *)"(block table)",
          (dwBytes>>2));
      }
    }
    
    // Verify the both block tables (If the KPM file is not protected)
    if(nError == ERROR_SUCCESS && (ha->dwFlags & KPM_FLAG_PROTECTED) == 0)
    {
      
      TKPMBlock * pBlockEnd = ha->pBlockTable + dwMaxBlockIndex + 1;
      TKPMBlock * pBlock   = ha->pBlockTable;

      // If the KPM file is not protected,
      // we will check if all sizes in the block table is correct.
      // Note that we will not relocate the block table (change from previous versions)
      for(; pBlock < pBlockEnd; pBlock++)
      {
        if(pBlock->dwFlags & KPM_FILE_EXISTS)
        {
          // Get the 64-bit file position
          TempPos.QuadPart = pBlock->dwFilePos;

          if(TempPos.QuadPart > ha->KpmSize || pBlock->dwCSize > ha->KpmSize)
          {
            nError = ERROR_BAD_FORMAT;
            break;
          }
        }
      }
    }

    // If the caller didn't specified otherwise, 
    // include the internal listfile to the TMPQArchive structure
    if(nError == ERROR_SUCCESS)
    {
      nError = KPM_CreateListFile(ha);

      if(ha)
      {
        InitializeCriticalSection(&ha->section);
      }

      // Add the internal listfile
      if(nError == ERROR_SUCCESS)
      {
        if(ha->Header.wUseType == USE_TYPE_CLIENT)  
          KPM_AddListFileClient(ha, NULL);
        else
          KPM_AddListFilePatch(ha, NULL);
      }
    }
    // If the caller didn't specified otherwise, 
    // load the "(attr)" file
    if(nError == ERROR_SUCCESS)
    {
      // Ignore the result here. Attrobutes are not necessary,
      // if they are not there, we will just ignore them
      if(ha->Header.wUseType == USE_TYPE_CLIENT)
        nError = KPM_LoadAttrFileClient(ha);
      else
        nError = KPM_LoadAttrFilePatch(ha);
    }

    // Cleanup and exit
    if(nError != ERROR_SUCCESS)
    {
      FreeArchive(ha);
      if(hFile != INVALID_HANDLE_VALUE)
        CloseHandle(hFile);
      SetLastError(nError);
      ha = NULL;
    }

    return ha;
  }

  bool        KPM_CloseArchive(TKPMArchive* hArchive)
  {
    if(!IsValidArchiveHandle(hArchive))
    {
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }

    if(hArchive->dwFlags & KPM_FLAG_CHANGED)
    {
      if(hArchive->Header.wUseType == USE_TYPE_CLIENT)
        SaveListFile(hArchive,TRUE);
      else
        SaveListFile(hArchive);
      
      if(hArchive->Header.wUseType == USE_TYPE_CLIENT)
        SaveAttrFile(hArchive,TRUE);
      else
        SaveAttrFile(hArchive);

      SaveKPMTables(hArchive);

      
    }

    DeleteCriticalSection(&hArchive->section);

    FreeArchive(hArchive);
    

    return TRUE;
  }

  TKPMArchive*    KPM_CreateArchive(const char* szFilename,DWORD& oresult,DWORD dwHashTableSize,DWORD UseType)
  {
    oresult = ERROR_SUCCESS;
    TKPMArchive* pArchive = NULL;
    HANDLE pFile;
    BOOL bFileExists = (GetFileAttributes(szFilename) != 0xFFFFFFFF);
    if(bFileExists) //如果文件存在
    {
      
    }

    if(oresult == ERROR_SUCCESS)
      oresult =  PerpareBuffers();


    pFile = CreateFile(szFilename,
               GENERIC_READ | GENERIC_WRITE,
               FILE_SHARE_READ,
               NULL,
               CREATE_ALWAYS,
               0,
               NULL);
    if(pFile == INVALID_HANDLE_VALUE)
    {
      oresult = GetLastError();
      return NULL;
    }
    if(oresult == ERROR_SUCCESS)
    {
      pArchive = (TKPMArchive*)KPM_ALLOC(TKPMArchive,1);
      assert(pArchive);
      memset(pArchive,0,sizeof(TKPMArchive));
      
      if(pArchive == NULL)
      {
        oresult = ERROR_NOT_ENOUGH_MEMORY;
        return NULL;
      }
    }
    LONG dwFilePointerHigh = 0;
    if(SetFilePointer(pFile,0,&dwFilePointerHigh,FILE_BEGIN)==0xFFFFFFFF)
    {
      oresult = GetLastError();
            return NULL;
    }

    if(dwHashTableSize>HASH_TABLE_SIZE_MAX)
        dwHashTableSize =  HASH_TABLE_SIZE_MAX;
    if(dwHashTableSize<HASH_TABLE_SIZE_MIN)
        dwHashTableSize  =  HASH_TABLE_SIZE_MIN;

    // fill archive struct
    if(oresult == ERROR_SUCCESS)
    {
      memset(pArchive,0,sizeof(TKPMArchive));
      strcpy(pArchive->szFileName,szFilename);
      pArchive->hArchiveFile = pFile;
      pArchive->dwBlockSize  = 4096;
      pArchive->HeaderOffset = ((sizeof(TKPMHeader)-1)/512 +1)*512;
      pArchive->HashTableOffset.QuadPart = ((sizeof(TKPMHeader)-1)/512 +1)*512;
      pArchive->CurrentOffset = 0;
      pArchive->pHeader = &pArchive->Header;
      
      pArchive->pHashKeyTable = KPM_ALLOC(TKPMHashKey,dwHashTableSize);
      assert(pArchive->pHashKeyTable);
      memset(pArchive->pHashKeyTable,0xFF,sizeof(TKPMHashKey)*dwHashTableSize);
      pArchive->pBlockTable  = KPM_ALLOC(TKPMBlock,dwHashTableSize);
      assert(pArchive->pBlockTable);
      memset(pArchive->pBlockTable,0,sizeof(TKPMBlock)*dwHashTableSize);
      pArchive->BlockCache.pbBuffer = KPM_ALLOC(BYTE,pArchive->dwBlockSize);
      assert(pArchive->BlockCache.pbBuffer);
      memset(pArchive->BlockCache.pbBuffer,0,pArchive->dwBlockSize);


      pArchive->dwFlags       |= KPM_FLAG_CHANGED;

    }
         
    if(!pArchive->pHashKeyTable
      ||!pArchive->pBlockTable
      ||!pArchive->BlockCache.pbBuffer)
      oresult = GetLastError();
    pFile = NULL;

    //fill all header and buffers
    if(oresult == ERROR_SUCCESS)
    {
      TKPMHeader* pHeader = pArchive->pHeader;
      memset(pHeader,0,sizeof(TKPMHeader));
      pHeader->dwMagic    =  KPM_MAGIC;
      pHeader->dwHeaderSize  =   sizeof(TKPMHeader);
      pHeader->dwArchiveSize  =  0;
      pHeader->dwVersion    =   1400;
      pHeader->dwHashTableSize  = dwHashTableSize;
      pHeader->dwBlockTableSize  = dwHashTableSize;
      pHeader->wBlockSize    =  3;
      pHeader->wUseType    =  UseType;
      pHeader->dwMaxListFileCache = 1024*1024;
      pArchive->KpmSize =pHeader->dwArchiveSize;
      
      //hash pos
      pArchive->HashTableOffset.QuadPart = pArchive->HeaderOffset;
      pHeader->dwHashTablePos = 0;
      
      //block pos 
      pArchive->BlockTableOffset.QuadPart = pArchive->HashTableOffset.QuadPart+pHeader->dwHashTableSize*sizeof(TKPMHashKey);
      pHeader->dwBlockTablePos  = pHeader->dwHashTableSize*sizeof(TKPMHashKey);
    }


    if ( oresult == ERROR_SUCCESS )
    {
      InitializeCriticalSection(&pArchive->section);
    }


    DWORD dwTransferred = 0;
    //write header to KPM files
    if(oresult == ERROR_SUCCESS)
    {
      WriteFile(pArchive->hArchiveFile,pArchive->pHeader , pArchive->pHeader->dwHeaderSize, &dwTransferred, NULL);
      if(dwTransferred != pArchive->pHeader->dwHeaderSize)
        oresult = ERROR_DISK_FULL;

      pArchive->CurrentOffset = pArchive->pHeader->dwHeaderSize;
    }
    
    if(oresult == ERROR_SUCCESS)
    {
      SaveKPMTables(pArchive);
    }

    if(oresult == ERROR_SUCCESS)
    {
      oresult = KPM_CreateListFile(pArchive);
    }

    if(oresult == ERROR_SUCCESS)
    {
      oresult = KPM_AddInternalFile(pArchive,LISTFILE_NAME);
    }

    if(oresult == ERROR_SUCCESS)
    {
      if(pArchive->Header.wUseType == USE_TYPE_CLIENT)
        SaveListFile(pArchive,FALSE);
    }
    
    if(oresult == ERROR_SUCCESS)  
    {
      oresult = KPM_CreateAttrFile(pArchive);
    }

    if(oresult == ERROR_SUCCESS)  
    {
      oresult = KPM_AddInternalFile(pArchive,ATTRIBUTES_NAME);
    }

    if(oresult == ERROR_SUCCESS)
    {
      if(pArchive->Header.wUseType == USE_TYPE_CLIENT)
        SaveAttrFile(pArchive,FALSE);
    }
    
    if(oresult != ERROR_SUCCESS)  
    {
      FreeArchive(pArchive);
    }
    
    return pArchive;
  }

  DWORD  KPM_AddInternalFile(TKPMArchive* ha,const char* filename)
  {
  
    DWORD dwError = ERROR_SUCCESS;

    TKPMHashKey* pHash;
    TKPMBlock * pBlockEnd;
    TKPMBlock * pBlock;
      BOOL bFoundFreeEntry = FALSE;

    pHash = GetHashEntry(ha,filename);

    if(pHash == NULL)
    {
      pHash = FindFreeHashEntry(ha,filename);
    }

    if(pHash!=NULL)
    {
      pBlockEnd = ha->pBlockTable + ha->pHeader->dwBlockTableSize;
      
      for(pBlock = ha->pBlockTable; pBlock < pBlockEnd; pBlock++ )
      {
        if((pBlock->dwFlags & KPM_FILE_EXISTS) == 0)
        {
          bFoundFreeEntry = TRUE;
          break;
        }
      }

      // If the block is out of the available entries, return error
      if(pBlock >= (ha->pBlockTable + ha->pHeader->dwHashTableSize))
        return ERROR_DISK_FULL;    

      // If we had to add the file at the end, increment the block table
      if(bFoundFreeEntry == FALSE)
        ha->pHeader->dwBlockTableSize++;

      // Fill the block entry
      pBlock->dwFilePos = ha->HashTableOffset.QuadPart;
      pBlock->dwFSize   = 0;
      pBlock->dwCSize   = 0;
      pBlock->dwFlags   = KPM_FILE_EXISTS;

      // Add the node for the file name

      return KPM_AddNode2ListFile(ha,filename);
      
    }
    else
    {
      dwError = ERROR_HANDLE_DISK_FULL;
    }
    
    return dwError;
  }

  //-----------------------------------------------------------------------------
  // ReadBlock
  //
  //  hf          - MPQ File handle.
  //  dwBlockPos  - Position of block in the file (relative to file begin)
  //  buffer      - Pointer to target buffer to store blocks.
  //  dwBlockSize - Number of bytes to read. Must be multiplier of block size.
  //
  //  Returns number of bytes read.

  static DWORD internal_ReadBlocks(TKPMFile * hf, DWORD dwBlockPos, BYTE * buffer, DWORD blockBytes)
  {
    LARGE_INTEGER FilePos;
    TKPMArchive * ha = hf->ha;          // Archive handle
    BYTE  * tempBuffer = NULL;          // Buffer for reading compressed data from the file
    LONGLONG   dwFilePos = dwBlockPos;     // Reading position from the file
    DWORD   dwToRead;                   // Number of bytes to read
    DWORD   blockNum;                   // Block number (needed for decrypt)
    DWORD   dwBytesRead = 0;            // Total number of bytes read
    DWORD   bytesRemain = 0;            // Number of data bytes remaining up to the end of the file
    DWORD   nBlocks;                    // Number of blocks to load
    DWORD   i;

    // Test parameters. Block position and block size must be block-aligned, block size nonzero
    if((dwBlockPos & (ha->dwBlockSize - 1)) || blockBytes == 0)
      return 0;

    // Check the end of file
    if((dwBlockPos + blockBytes) > hf->pBlock->dwFSize)
      blockBytes = hf->pBlock->dwFSize - dwBlockPos;

    bytesRemain = hf->pBlock->dwFSize - dwBlockPos;
    blockNum    = dwBlockPos / ha->dwBlockSize;
    nBlocks     = blockBytes / ha->dwBlockSize;
    if(blockBytes % ha->dwBlockSize)
      nBlocks++;

    // If file has variable block positions, we have to load them
    if((hf->pBlock->dwFlags & KPM_FILE_COMPRESSED) && hf->bBlockPosLoaded == FALSE)
    {
      // Move file pointer to the begin of the file in the MPQ
      if(hf->RawFileOffSet.QuadPart != ha->CurrentOffset)
      {
        SetFilePointer(ha->hArchiveFile, hf->RawFileOffSet.LowPart, &hf->RawFileOffSet.HighPart, FILE_BEGIN);
      }

      // Read block positions from begin of file.
      dwToRead = (hf->nBlocks+1) * sizeof(LARGE_INTEGER);
    
      // Read the block pos table and convert the buffer to little endian
      ReadFile(ha->hArchiveFile, hf->pllBlockOffSet, dwToRead, &dwBytesRead, NULL);
      //
      // If the archive if protected some way, perform additional check
      // Sometimes, the file appears not to be encrypted, but it is.
      //
      // Note: In WoW 1.10+, there's a new flag. With this flag present,
      // there's one additional entry in the block table.
      //

      if(hf->pllBlockOffSet[0].QuadPart != dwBytesRead)
        hf->pBlock->dwFlags |= KPM_FILE_ENCRYPTED;

      // Decrypt loaded block positions if necessary
      if(hf->pBlock->dwFlags & KPM_FILE_ENCRYPTED)
      {
        // If we don't know the file seed, try to find it.
        if(hf->dwSeed1 == 0)
          hf->dwSeed1 = DetectFileSeed((DWORD*)hf->pllBlockOffSet, dwBytesRead);

        // If we don't know the file seed, sorry but we cannot extract the file.
        if(hf->dwSeed1 == 0)
          return 0;

        // Decrypt block positions
        DecryptBlock((DWORD*)hf->pllBlockOffSet, dwBytesRead, hf->dwSeed1 - 1);

        // Check if the block positions are correctly decrypted
        // I don't know why, but sometimes it will result invalid block positions on some files
        if(hf->pllBlockOffSet[0].QuadPart != dwBytesRead)
        {
          // Try once again to detect file seed and decrypt the blocks
          SetFilePointer(ha->hArchiveFile, hf->RawFileOffSet.LowPart, &hf->RawFileOffSet.HighPart, FILE_BEGIN);
          ReadFile(ha->hArchiveFile, hf->pllBlockOffSet, dwToRead, &dwBytesRead, NULL);


          hf->dwSeed1 = DetectFileSeed((DWORD*)hf->pllBlockOffSet, dwBytesRead);
          DecryptBlock((DWORD*)hf->pllBlockOffSet, dwBytesRead, hf->dwSeed1 - 1);
          // Check if the block positions are correctly decrypted
          if(hf->pllBlockOffSet[0].QuadPart != dwBytesRead)
            return 0;
        }
      }

      // Update hf's variables
      ha->CurrentOffset = hf->RawFileOffSet.QuadPart + dwBytesRead;
      hf->bBlockPosLoaded = TRUE;
    }

    // Get file position and number of bytes to read
    dwFilePos = dwBlockPos;
    dwToRead  = blockBytes;
    if(hf->pBlock->dwFlags & KPM_FILE_COMPRESSED)
    {
      dwFilePos = hf->pllBlockOffSet[blockNum].QuadPart;
      dwToRead  = hf->pllBlockOffSet[blockNum + nBlocks].QuadPart - dwFilePos;
    }
    FilePos.QuadPart = hf->RawFileOffSet.QuadPart + dwFilePos;

    // Get work buffer for store read data
    tempBuffer = buffer;
    if(hf->pBlock->dwFlags & KPM_FILE_COMPRESSED)
    {
      if((tempBuffer = KPM_ALLOC(BYTE, dwToRead)) == NULL)
      {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return 0;
      }
      else
      {
        memset(tempBuffer,0,dwToRead);
      }
    }

    // Set file pointer, if necessary 
    if(ha->CurrentOffset!= FilePos.QuadPart) 
    {
      SetFilePointer(ha->hArchiveFile, FilePos.LowPart, &FilePos.HighPart, FILE_BEGIN);
    }

    // 15018F87 : Read all requested blocks
    ReadFile(ha->hArchiveFile, tempBuffer, dwToRead, &dwBytesRead, NULL);
    ha->CurrentOffset = FilePos.QuadPart + dwBytesRead;

    // Block processing part.
    DWORD blockStart = 0;               // Index of block start in work buffer
    DWORD blockSize  = min(blockBytes, ha->dwBlockSize);
    DWORD index      = blockNum;        // Current block index

    dwBytesRead = 0;                      // Clear read byte counter

    // Walk through all blocks
    for(i = 0; i < nBlocks; i++, index++)
    {
      BYTE * inputBuffer = tempBuffer + blockStart;
      int    outLength = ha->dwBlockSize;

      if(bytesRemain < (DWORD)outLength)
        outLength = bytesRemain;

      // Get current block length
      if(hf->pBlock->dwFlags & KPM_FILE_COMPRESSED)
        blockSize = hf->pllBlockOffSet[index+1].QuadPart - hf->pllBlockOffSet[index].QuadPart;

      // If block is encrypted, we have to decrypt it.
      if(hf->pBlock->dwFlags & KPM_FILE_ENCRYPTED)
      {
        // If we don't know the seed, try to decode it as WAVE file
        if(hf->dwSeed1 == 0)
          hf->dwSeed1 = DetectFileSeed2((DWORD *)inputBuffer, 3, ID_WAVE, hf->pBlock->dwFSize - 8, 0x45564157);

        // Let's try MSVC's standard EXE or header
        if(hf->dwSeed1 == 0)
          hf->dwSeed1 = DetectFileSeed2((DWORD *)inputBuffer, 2, 0x00905A4D, 0x00000003);

        if(hf->dwSeed1 == 0)
          return 0;

        DecryptBlock((DWORD *)inputBuffer, blockSize, hf->dwSeed1 + index);
      }

      // If the block is really compressed, decompress it.
      // WARNING : Some block may not be compressed, it can be determined only
      // by comparing uncompressed and compressed size !!!
      if(blockSize < (DWORD)outLength)
      {
        // Is the file compressed with PKWARE Data Compression Library ?
        if(hf->pBlock->dwFlags & KPM_FILE_IMPLODE)
          Decompress_pklib((char *)buffer, &outLength, (char *)inputBuffer, (int)blockSize);

        // Is it a file compressed by Blizzard's multiple compression ?
        // Note that Storm.dll v 1.0.9 distributed with Warcraft III
        // passes the full path name of the opened archive as the new last parameter
        if(hf->pBlock->dwFlags & KPM_FILE_COMPRESS)
          SCompDecompress((char *)buffer, &outLength, (char *)inputBuffer, (int)blockSize);
        dwBytesRead += outLength;
        buffer    += outLength;
      }
      else
      {
        if(buffer != inputBuffer)
          memcpy(buffer, inputBuffer, blockSize);

        dwBytesRead += blockSize;
        buffer    += blockSize;
      }
      blockStart  += blockSize;
      bytesRemain -= outLength;
    }

    // Delete input buffer, if necessary
    if(hf->pBlock->dwFlags & KPM_FILE_COMPRESSED)
      KPM_FREE(tempBuffer);

    return dwBytesRead;
  }

  // When this function is called, it is already ensured that the parameters are valid
  // (e.g. the "dwToRead + dwFilePos" is not greater than the file size)
  static DWORD WINAPI ReadFileSingleUnit(TKPMFile * hf, DWORD dwFilePos, BYTE * pbBuffer, DWORD dwToRead)
  {
    TKPMArchive * ha = hf->ha; 
    DWORD dwBytesRead = 0;

    if(ha->CurrentOffset != hf->RawFileOffSet.QuadPart)
    {
      SetFilePointer(ha->hArchiveFile, hf->RawFileOffSet.LowPart, &hf->RawFileOffSet.HighPart, FILE_BEGIN);
      ha->CurrentOffset = hf->RawFileOffSet.QuadPart;
    }

    // If the file is really compressed, decompress it.
    // Otherwise, read the data as-is to the caller.
    if(hf->pBlock->dwCSize < hf->pBlock->dwFSize)
    {
      if(hf->pFileBuffer == NULL)
      {
        BYTE * inputBuffer = NULL;
        int outputBufferSize = (int)hf->pBlock->dwFSize;
        int inputBufferSize = (int)hf->pBlock->dwCSize;

        hf->pFileBuffer = KPM_ALLOC(BYTE, outputBufferSize);
        inputBuffer = KPM_ALLOC(BYTE, inputBufferSize);
        assert(hf->pFileBuffer);
        memset(hf->pFileBuffer,0,outputBufferSize);
        assert(inputBuffer);
        memset(inputBuffer,0,inputBufferSize);

        if(inputBuffer != NULL && hf->pFileBuffer != NULL)
        {
          // Read the compressed file data
          ReadFile(ha->hArchiveFile, inputBuffer, inputBufferSize, &dwBytesRead, NULL);

          // Is the file compressed with PKWARE Data Compression Library ?
          if(hf->pBlock->dwFlags & KPM_FILE_IMPLODE)
            Decompress_pklib((char *)hf->pFileBuffer, &outputBufferSize, (char *)inputBuffer, (int)inputBufferSize);

          // Is it a file compressed by Blizzard's multiple compression ?
          // Note that Storm.dll v 1.0.9 distributed with Warcraft III
          // passes the full path name of the opened archive as the new last parameter
          if(hf->pBlock->dwFlags & KPM_FILE_COMPRESS)
            SCompDecompress((char *)hf->pFileBuffer, &outputBufferSize, (char *)inputBuffer, (int)inputBufferSize);
        }

        // Free the temporary buffer
        if(inputBuffer != NULL)
          KPM_FREE(inputBuffer);
      }

      // Copy the file data, if any there
      if(hf->pFileBuffer != NULL)
      {
        memcpy(pbBuffer, hf->pFileBuffer + dwFilePos, dwToRead);
        dwBytesRead = dwToRead;
      }
    }
    else
    {
      // Read the uncompressed file data
      ReadFile(ha->hArchiveFile, pbBuffer, dwToRead, &dwBytesRead, NULL);
    }

    return dwBytesRead;
  }
  //-----------------------------------------------------------------------------
  // ReadMPQFile

  static DWORD internal_ReadFile(TKPMFile * hf, DWORD dwFilePos, BYTE * pbBuffer, DWORD dwToRead)
  {
    TKPMArchive * ha  = hf->ha; 
    TKPMBlock * pBlock  = hf->pBlock;  // Pointer to file block
    
    DWORD dwBytesRead = 0;        // Number of bytes read from the file
    DWORD dwBlockPos;          // Position in the file aligned to the whole blocks
    DWORD dwLoaded;

    // File position is greater or equal to file size ?
    if(dwFilePos >= pBlock->dwFSize)
      return dwBytesRead;

    // If too few bytes in the file remaining, cut them
    if((pBlock->dwFSize - dwFilePos) < dwToRead)
      dwToRead = (pBlock->dwFSize - dwFilePos);

    // If the file is stored as single unit, handle it separately
    if(pBlock->dwFlags & KPM_FILE_SINGLE_UNIT)
      return ReadFileSingleUnit(hf, dwFilePos, pbBuffer, dwToRead);

    // Block position in the file
    dwBlockPos = dwFilePos & ~(ha->dwBlockSize - 1);  // Position in the block

    // Load the first block, if incomplete. It may be loaded in the cache buffer.
    // We have to check if this block is loaded. If not, load it.
    if((dwFilePos % ha->dwBlockSize) != 0)
    {
      // Number of bytes remaining in the buffer
      DWORD dwToCopy;
      DWORD dwLoaded = ha->dwBlockSize;

      // Check if data are loaded in the cache
      if(hf != ha->BlockCache.pLastFile || dwBlockPos != ha->BlockCache.dwblockOffSet)
      {
        // Load one MPQ block into archive buffer
        dwLoaded = internal_ReadBlocks(hf, dwBlockPos, ha->BlockCache.pbBuffer, ha->dwBlockSize);
        if(dwLoaded == 0)
          return (DWORD)-1;

        // Save lastly accessed file and block position for later use
        ha->BlockCache.pLastFile  = hf;
        ha->BlockCache.dwblockOffSet = dwBlockPos;
        ha->BlockCache.BufferOffSet  = dwFilePos % ha->dwBlockSize;
      }
      dwToCopy = dwLoaded - ha->BlockCache.BufferOffSet;
      if(dwToCopy > dwToRead)
        dwToCopy = dwToRead;

      // Copy data from block buffer into target buffer
      memcpy(pbBuffer, ha->BlockCache.pbBuffer + ha->BlockCache.BufferOffSet, dwToCopy);

      // Update pointers
      dwToRead     -= dwToCopy;
      dwBytesRead     += dwToCopy;
      pbBuffer     += dwToCopy;
      dwBlockPos       += ha->dwBlockSize;
      ha->BlockCache.BufferOffSet += dwToCopy;

      // If all, return.
      if(dwToRead == 0)
        return dwBytesRead;
    }

    // Load the whole ("middle") blocks only if there are more or equal one block
    if(dwToRead > ha->dwBlockSize)
    {                                           
      DWORD dwBlockBytes = dwToRead & ~(ha->dwBlockSize - 1);

      dwLoaded = internal_ReadBlocks(hf, dwBlockPos, pbBuffer, dwBlockBytes);
      if(dwLoaded == 0)
        return (DWORD)-1;

      // Update pointers
      dwToRead    -= dwLoaded;
      dwBytesRead += dwLoaded;
      pbBuffer    += dwLoaded;
      dwBlockPos  += dwLoaded;

      // If all, return.
      if(dwToRead == 0)
        return dwBytesRead;
    }

    // Load the terminating block
    if(dwToRead > 0)
    {
      DWORD dwToCopy = ha->dwBlockSize;

      // Check if data are loaded in the cache
      if(hf != ha->BlockCache.pLastFile || dwBlockPos != ha->BlockCache.dwblockOffSet)
      {
        // Load one MPQ block into archive buffer
        dwToCopy = internal_ReadBlocks(hf, dwBlockPos, ha->BlockCache.pbBuffer, ha->dwBlockSize);
        if(dwToCopy == 0)
          return (DWORD)-1;

        // Save lastly accessed file and block position for later use
        ha->BlockCache.pLastFile  = hf;
        ha->BlockCache.dwblockOffSet = dwBlockPos;
      }
      ha->BlockCache.BufferOffSet  = 0;

      // Check number of bytes read
      if(dwToCopy > dwToRead)
        dwToCopy = dwToRead;

      memcpy(pbBuffer, ha->BlockCache.pbBuffer, dwToCopy);
      dwBytesRead  += dwToCopy;
      ha->BlockCache.BufferOffSet = dwToCopy;
    }

    // Return what we've read
    return dwBytesRead;
  }


  BOOL        KPM_AddFileFromMemory(TKPMArchive* ha, const char * szAliasFileName,const char* lpszFile, DWORD dwFileSize,
                  DWORD dwFlags,DWORD dwQuality, DWORD nFileType)
  {
    BOOL  bReplaced = FALSE;          // TRUE if replacing file in the archive
    int    nError = ERROR_SUCCESS;

    if(nError == ERROR_SUCCESS)
    {
      // Check valid parameters
      if(IsValidArchiveHandle(ha) == FALSE 
        || szAliasFileName == NULL || lpszFile == 0 )
        nError = ERROR_INVALID_PARAMETER;

      // Check the values of dwFlags
      if((dwFlags & KPM_FILE_IMPLODE) && (dwFlags & KPM_FILE_COMPRESS))
        nError = ERROR_INVALID_PARAMETER;
    }

    // If anyone is trying to add listfile, and the archive already has a listfile,
    // deny the operation, but return success.
    if(nError == ERROR_SUCCESS)
    {
      if(ha->pListFile != NULL && !_stricmp(szAliasFileName, LISTFILE_NAME))
        return ERROR_SUCCESS;
    }

    if(nError == ERROR_SUCCESS)
      nError = AddFileToArchive(ha, lpszFile, dwFileSize, szAliasFileName, dwFlags, dwQuality, nFileType, &bReplaced);

    // Add the file into listfile also
    if(nError == ERROR_SUCCESS && bReplaced == FALSE)
      nError = KPM_AddNode2ListFile(ha, szAliasFileName);

    // Cleanup and exit
    if(nError != ERROR_SUCCESS)
      SetLastError(nError);
    return (nError == ERROR_SUCCESS);
  }



  BOOL  KPM_AddFile(TKPMArchive* ha, const char * szRealFileName,const char* szAliasFileName,
              DWORD dwFlags,DWORD dwQuality, DWORD nFileType)
  {

    
    HANDLE  hFile = INVALID_HANDLE_VALUE;
    BOOL  bReplaced = FALSE;          // TRUE if replacing file in the archive
    int    nError = ERROR_SUCCESS;

    if(nError == ERROR_SUCCESS)
    {
      // Check valid parameters
      if(IsValidArchiveHandle(ha) == FALSE 
        || szRealFileName == NULL || *szRealFileName == 0 
        || szAliasFileName == NULL || *szAliasFileName == 0 )
        nError = ERROR_INVALID_PARAMETER;

      // Check the values of dwFlags
      if((dwFlags & KPM_FILE_IMPLODE) && (dwFlags & KPM_FILE_COMPRESS))
        nError = ERROR_INVALID_PARAMETER;
    }

    // If anyone is trying to add listfile, and the archive already has a listfile,
    // deny the operation, but return success.
    if(nError == ERROR_SUCCESS)
    {
      if(ha->pListFile != NULL && !_stricmp(szAliasFileName, LISTFILE_NAME))
        return ERROR_SUCCESS;
    }

    // Open added file
    if(nError == ERROR_SUCCESS)
    {
      hFile = CreateFile(szRealFileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, NULL);
      if(hFile == INVALID_HANDLE_VALUE)
        nError = GetLastError();
    }

    if(nError == ERROR_SUCCESS)
      nError = AddFileToArchive(ha, hFile, szAliasFileName ,dwFlags, dwQuality, nFileType, &bReplaced);

    // Add the file into listfile also
    if(nError == ERROR_SUCCESS && bReplaced == FALSE)
      nError = KPM_AddNode2ListFile(ha, szAliasFileName);

    // Cleanup and exit
    if(hFile != INVALID_HANDLE_VALUE)
      CloseHandle(hFile);
    if(nError != ERROR_SUCCESS)
      SetLastError(nError);
    return (nError == ERROR_SUCCESS);
  }

  BOOL  KPM_RemoveFile(TKPMArchive* ha,const char * szFileName)
  {
  
    TKPMBlock    * pBlock = NULL;    // Block entry of deleted file
    TKPMHashKey     * pHash = NULL;     // Hash entry of deleted file
    DWORD      dwBlockIndex  =  0;

    DWORD nError = ERROR_SUCCESS;

    // Check the parameters
    if(nError == ERROR_SUCCESS)
    {
      if(IsValidArchiveHandle(ha) == FALSE)
        nError = ERROR_INVALID_PARAMETER;
      if(*szFileName == 0||szFileName == NULL)
        nError = ERROR_INVALID_PARAMETER;
    }
    // Do not allow to remove listfile
    if(nError == ERROR_SUCCESS)
    {
      if(!_stricmp(szFileName, LISTFILE_NAME))
        nError = ERROR_ACCESS_DENIED;
    }

    // Get hash entry belonging to this file
    if(nError == ERROR_SUCCESS)
    {
      if((pHash = GetHashEntry(ha, (char *)szFileName)) == NULL)
        nError = ERROR_FILE_NOT_FOUND;
    }

    // If index was not found, or is greater than number of files, exit.
    if(nError == ERROR_SUCCESS)
    {
      if((dwBlockIndex = pHash->dwBlockIndex) > ha->pHeader->dwBlockTableSize)
        nError = ERROR_FILE_NOT_FOUND;
    }

    // Get block and test if the file is not already deleted
    if(nError == ERROR_SUCCESS)
    {
      pBlock = ha->pBlockTable + dwBlockIndex;
      if((pBlock->dwFlags & KPM_FILE_EXISTS) == 0)
        nError = ERROR_FILE_NOT_FOUND;
    }

    

    // Remove the file from the list file
    if(nError == ERROR_SUCCESS)
      nError = KPM_ListFileRemoveNode(ha, szFileName);

    // Now invalidate the block entry and the hash entry. Do not make any
    // relocations and file copying, use SFileCompactArchive for it.
    if(nError == ERROR_SUCCESS)
    {
      pBlock->dwFilePos   = 0;
      pBlock->dwFSize     = 0;
      pBlock->dwCSize     = 0;
      pBlock->dwFlags     = 0;
      pHash->dwName1      = 0xFFFFFFFF;
      pHash->dwName2      = 0xFFFFFFFF;
      pHash->dwBlockIndex = HASH_ENTRY_DELETED;

      // Update KPM archive
      ha->dwFlags |= KPM_FLAG_CHANGED;
    }

    // Resolve error and exit
    if(nError != ERROR_SUCCESS)
      SetLastError(nError);
    return (nError == ERROR_SUCCESS);
  }

  BOOL  KPM_RenameFile(TKPMArchive* ha, const char * szFileName, const char * szNewFileName)
  {
    
    TKPMHashKey *  pOldHash = NULL;         // Hash entry for the original file
    TKPMHashKey *  pNewHash = NULL;         // Hash entry for the renamed file
    DWORD      dwBlockIndex = 0;
    
    DWORD  nError = ERROR_SUCCESS;

    // Test the valid parameters
    if(nError == ERROR_SUCCESS)
    {
      if(ha == NULL || szNewFileName == NULL || *szNewFileName == 0)
        nError = ERROR_INVALID_PARAMETER;
      if(szFileName == NULL || *szFileName == 0)
        nError = ERROR_INVALID_PARAMETER;
    }

    // Do not allow to rename listfile
    if(nError == ERROR_SUCCESS)
    {
      if(!_stricmp(szFileName, LISTFILE_NAME))
        nError = ERROR_ACCESS_DENIED;
    }

    // Test if the file already exists in the archive
    if(nError == ERROR_SUCCESS)
    {
      if((pNewHash = GetHashEntry(ha, szNewFileName)) != NULL)
        nError = ERROR_ALREADY_EXISTS;
    }

    // Get the hash table entry for the original file
    if(nError == ERROR_SUCCESS)
    {
      if((pOldHash = GetHashEntry(ha, szFileName)) == NULL)
        nError = ERROR_FILE_NOT_FOUND;
    }

    // Get the hash table entry for the renamed file
    if(nError == ERROR_SUCCESS)
    {
      // Save block table index and remove the hash table entry
      dwBlockIndex = pOldHash->dwBlockIndex;
      pOldHash->dwName1      = 0xFFFFFFFF;
      pOldHash->dwName2      = 0xFFFFFFFF;
      pOldHash->dwBlockIndex = HASH_ENTRY_DELETED;

      if((pNewHash = FindFreeHashEntry(ha, szNewFileName)) == NULL)
        nError = ERROR_CAN_NOT_COMPLETE;
    }

    // Save the block index and clear the hash entry
    if(nError == ERROR_SUCCESS)
    {
      // Copy the block table index
      pNewHash->dwBlockIndex = dwBlockIndex;
      ha->dwFlags |= KPM_FLAG_CHANGED;
    }

    // Rename the file in the list file
    if(nError == ERROR_SUCCESS)
      nError = KPM_ListFileRenameNode(ha, szFileName, szNewFileName);

    // Resolve error and return
    if(nError != ERROR_SUCCESS)
      SetLastError(nError);
    return (nError == ERROR_SUCCESS);
  }
  


  TKPMFile*  KPM_FileOpen(TKPMArchive* ha, const char * szFileName, DWORD& oresult)
  {
    EnterCriticalSection(&ha->section);

    LARGE_INTEGER FilePos;
    
    TKPMFile    * hf = NULL;
    TKPMHashKey * pHash  = NULL;        // Hash table index
    TKPMBlock   * pBlock = NULL;        //File block
    
    DWORD    dwHashIndex  = 0;             // Hash table index
    DWORD    dwBlockIndex = (DWORD)-1;     // Found table index
    size_t    nHandleSize = 0;              // Memory space necessary to allocate TMPQHandle
    int nError = ERROR_SUCCESS;

#ifdef _DEBUG    
    // Due to increasing numbers of files in KPMs, I had to change the behavior
    // of opening by file index. Now, the SFILE_OPEN_BY_INDEX value of dwSearchScope
    // must be entered. This check will allow to find code places that are incompatible
    // with the new behavior.
    if(szFileName != NULL)
    {
      assert((DWORD_PTR)szFileName > 0x10000);
    }
#endif

    if(nError == ERROR_SUCCESS)
    {
      if(ha == NULL)
        nError = ERROR_INVALID_PARAMETER;
      
      if(szFileName == NULL || *szFileName == 0)
        nError = ERROR_INVALID_PARAMETER;
    }

    // Prepare the file opening
    if(nError == ERROR_SUCCESS)
    {
      // If we have to open a disk file
      nHandleSize = sizeof(TKPMFile) + strlen(szFileName);
      if((pHash = GetHashEntry(ha, szFileName)) != NULL)
      {
        dwHashIndex  = (DWORD)(pHash - ha->pHashKeyTable);
        dwBlockIndex = pHash->dwBlockIndex;
      }
    }

    // Get block index from file name and test it
    if(nError == ERROR_SUCCESS)
    {
      // If index was not found, or is greater than number of files, exit.
      // This also covers the deleted files and free entries
      if(dwBlockIndex > ha->pHeader->dwBlockTableSize)
        nError = ERROR_FILE_NOT_FOUND;
    }

    // Get block and test if the file was not already deleted.
    if(nError == ERROR_SUCCESS)
    {
      // Get both block tables and file position
      
      pBlock = ha->pBlockTable + dwBlockIndex;
      FilePos.QuadPart = pBlock->dwFilePos;

      if(FilePos.QuadPart > ha->KpmSize||
        pBlock->dwCSize > ha->KpmSize)
        nError = ERROR_FILE_CORRUPT;
      if((pBlock->dwFlags & KPM_FILE_EXISTS) == 0)
        nError = ERROR_FILE_NOT_FOUND;
      if(pBlock->dwFlags & ~KPM_FILE_VALID_FLAGS)
        nError = ERROR_NOT_SUPPORTED;
    }

    // Allocate file handle
    if(nError == ERROR_SUCCESS)
    {
      if((hf = (TKPMFile *)KPM_ALLOC(char, nHandleSize)) == NULL)
      {
        nError = ERROR_NOT_ENOUGH_MEMORY;
      }
      else
      {
        memset(hf,0,nHandleSize);
      }
    }

    // Initialize file handle
    if(nError == ERROR_SUCCESS)
    {
      memset(hf, 0, nHandleSize);
      hf->hFile    = INVALID_HANDLE_VALUE;
      hf->ha       = ha;
      hf->pBlock   = pBlock;
      hf->nBlocks  = (hf->pBlock->dwFSize + ha->dwBlockSize - 1) / ha->dwBlockSize;
      hf->pHash    = pHash;

    
      hf->MpqFileOffSet.QuadPart    = pBlock->dwFilePos;
      hf->RawFileOffSet.QuadPart = hf->MpqFileOffSet.QuadPart + ha->HeaderOffset;

      hf->dwHashIndex  = dwHashIndex;
      hf->dwBlockIndex = dwBlockIndex; 

      // Allocate buffers for decompression.
      if(hf->pBlock->dwFlags & KPM_FILE_COMPRESSED)
      {
        // Allocate buffer for block positions. At the begin of file are stored
        // DWORDs holding positions of each block relative from begin of file in the archive
        // As for newer KPMs, there may be one additional entry in the block table
        
        if((hf->pllBlockOffSet = KPM_ALLOC(LARGE_INTEGER, hf->nBlocks + 2)) == NULL)
        {
          nError = ERROR_NOT_ENOUGH_MEMORY;
        }
        else
        {
          memset(hf->pllBlockOffSet,0,sizeof(LARGE_INTEGER)*(hf->nBlocks + 2));
        }
      }

      // Decrypt file seed. Cannot be used if the file is given by index
    
        if(hf->pBlock->dwFlags & KPM_FILE_ENCRYPTED)
        {
          const char * szTemp = strrchr(szFileName, '\\');
          strcpy(hf->szFileName, szFileName);
          if(szTemp != NULL)
            szFileName = szTemp + 1;
          hf->dwSeed1 = DecryptFileSeed((char *)szFileName);

          if(hf->pBlock->dwFlags & KPM_FILE_FIXSEED)
          {
            LARGE_INTEGER TempPos;
            TempPos.QuadPart = hf->pBlock->dwFilePos;
            hf->dwSeed1 = (hf->dwSeed1 + TempPos.LowPart) ^ hf->pBlock->dwFSize;
          }
        }
      }

    // Resolve pointers to file's attributes
    if(nError == ERROR_SUCCESS && ha->pFileAttr != NULL)
    {
      if(ha->pFileAttr->pCrc32 != NULL)
        hf->pCrc32 = ha->pFileAttr->pCrc32 + dwBlockIndex;
      if(ha->pFileAttr->pFileTime != NULL)
        hf->pFileTime = ha->pFileAttr->pFileTime + dwBlockIndex;
      if(ha->pFileAttr->pMd5 != NULL)
        hf->pMD5 = ha->pFileAttr->pMd5 + dwBlockIndex;
    }

    // Cleanup
    if(nError != ERROR_SUCCESS)
    {
      FreeFile(hf);
      SetLastError(nError);
    }
    oresult = nError;

    LeaveCriticalSection(&ha->section);

    return hf;
    
  }


  DWORD KPM_FileClose(TKPMFile* hf)
  {

    if(!IsValidFileHandle(hf))
    {
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }

    // Set the last accessed file in the archive
    if(hf->ha != NULL)
      hf->ha->BlockCache.pLastFile = NULL;

    // Free the structure
    FreeFile(hf);
    return TRUE;
    
  }

  DWORD  KPM_GetFileSize(TKPMFile* hf)
  {
    if(hf == NULL)
    {
      SetLastError(ERROR_INVALID_PARAMETER);
      return (DWORD)-1;
    }

    if(!IsValidFileHandle(hf))
    {
      SetLastError(ERROR_INVALID_PARAMETER);
      return (DWORD)-1;
    }
    // If opened from archive, return file size
    return hf->pBlock->dwFSize;
  }

  DWORD  KPM_SetFilePointer(TKPMFile* hf, LONG lFilePos, DWORD dwMethod)
  {  
    TKPMArchive * ha;

    if(hf == NULL)
    {
      SetLastError(ERROR_INVALID_PARAMETER);
      return (DWORD)-1;
    }
    ha = hf->ha;

    switch(dwMethod)
    {
    case FILE_BEGIN:
      // Cannot set pointer before begin of file
      if(-lFilePos > (LONG)hf->FileOffSet)
        hf->FileOffSet = 0;
      else
        hf->FileOffSet = lFilePos;
      break;

    case FILE_CURRENT:
      // Cannot set pointer before begin of file
      if(-lFilePos > (LONG)hf->FileOffSet)
        hf->FileOffSet = 0;
      else
        hf->FileOffSet += lFilePos;
      break;

    case FILE_END:
      // Cannot set file position before begin of file
      if(-lFilePos >= (LONG)hf->pBlock->dwFSize)
        hf->FileOffSet = 0;
      else
        hf->FileOffSet = hf->pBlock->dwFSize + lFilePos;
      break;

    default:
      return ERROR_INVALID_PARAMETER;
    }

    if(hf == ha->BlockCache.pLastFile && (hf->FileOffSet & ~(ha->dwBlockSize - 1)) == ha->BlockCache.dwblockOffSet)
      ha->BlockCache.BufferOffSet = hf->FileOffSet & (ha->dwBlockSize - 1);
    else
    {
      ha->BlockCache.pLastFile = NULL;
      ha->BlockCache.BufferOffSet = 0;
    }

    return hf->FileOffSet;
  }
  DWORD  KPM_GetFilePos(TKPMFile* hf)
  {
    if(hf == NULL)
    {
      SetLastError(ERROR_INVALID_PARAMETER);
      return (DWORD)-1;
    }
    return hf->FileOffSet;
  }

  DWORD  KPM_FileSkip(TKPMFile* pFile,LONG Count)
  {
    return KPM_SetFilePointer(pFile,Count,FILE_CURRENT);
  }
  DWORD  KPM_FileSeek(TKPMFile* pFile,LONG Count)
  {
    return KPM_SetFilePointer(pFile,Count,FILE_BEGIN);
  }
  DWORD  KPM_FileTell(TKPMFile* pFile)
  {
    return KPM_GetFilePos(pFile);
  }
  DWORD  KPM_FileEof(TKPMFile* pFile)
  {
    return KPM_GetFilePos(pFile)>=KPM_GetFileSize(pFile);
  }

  bool  KPM_ReadFile(TKPMFile* hf, VOID * lpBuffer, DWORD dwToRead, DWORD * pdwRead)
  {
      
    
    DWORD dwBytes = 0;                  // Number of bytes (for everything)
    int nError = ERROR_SUCCESS;

    // Zero the number of bytes read
    if(pdwRead != NULL)
      *pdwRead = 0;

    // Check valid parameters
    if(nError == ERROR_SUCCESS)
    {
      if(hf == NULL || lpBuffer == NULL)
        nError = ERROR_INVALID_PARAMETER;
    }

    // If direct access to the file, use Win32 for reading
    if(nError == ERROR_SUCCESS && hf->hFile != INVALID_HANDLE_VALUE)
    {
      DWORD dwTransferred;

      ReadFile(hf->hFile, lpBuffer, dwToRead, &dwTransferred, NULL);
      if(dwTransferred < dwToRead)
      {
        SetLastError(ERROR_HANDLE_EOF);
        return FALSE;
      }

      if(pdwRead != NULL)
        *pdwRead = dwTransferred;
      return TRUE;
    }

    // Read all the bytes available in the buffer (If any)
    if(nError == ERROR_SUCCESS)
    {
      if(dwToRead > 0)
      {
        dwBytes = internal_ReadFile(hf, hf->FileOffSet, (BYTE *)lpBuffer, dwToRead);
        if(dwBytes == (DWORD)-1)
        {
          SetLastError(ERROR_CAN_NOT_COMPLETE);
          return FALSE;
        }
        hf->ha->BlockCache.pLastFile= hf;
        hf->FileOffSet += dwBytes;
      }
      if(pdwRead != NULL)
        *pdwRead = dwBytes;
    }

    // Check number of bytes read. If not OK, return FALSE.
    if(dwBytes < dwToRead)
    {
      SetLastError(ERROR_HANDLE_EOF);
      return FALSE;
    }
    return TRUE;
  }

  //File Search 
  TKPMSearch*  KPM_FindFirstFile(TKPMArchive* ha, const char * szMask,FILE_FIND_DATA * lpFindFileData)
  {
    
    TKPMSearch * hs    = NULL;             // Search object handle
    size_t     nSize  = 0;
    DWORD     nError = ERROR_SUCCESS;

    // Check for the valid parameters
    if(nError == ERROR_SUCCESS)
    {
      if(!IsValidArchiveHandle(ha))
        nError = ERROR_INVALID_PARAMETER;
      if(szMask == NULL || lpFindFileData == NULL)
        nError = ERROR_INVALID_PARAMETER;
    }


    // Allocate the structure for KPM search
    if(nError == ERROR_SUCCESS)
    {
      nSize = sizeof(TKPMSearch) + strlen(szMask) + 1;
      printf("nSize = %d",nSize);
      if((hs = (TKPMSearch *)KPM_ALLOC(char, nSize)) == NULL)
      {
        nError = ERROR_NOT_ENOUGH_MEMORY;
      }
      else
      {
        memset(hs,0,nSize);
      }
    }

    // Perform the first search
    if(nError == ERROR_SUCCESS)
    {
      memset(hs, 0, nSize);
      hs->ha          = ha;
      hs->dwNextIndex = 0;
      strcpy(hs->szSearchMask, szMask);
      nError = DoKPMSearch(hs, lpFindFileData);
    }

    // Cleanup
    if(nError != ERROR_SUCCESS)
    {
      FreeSearch(hs);
      SetLastError(nError);
    }
    // Return the result value
    return hs;
  }

  bool  KPM_FindNextFile(TKPMSearch* hFind, FILE_FIND_DATA * lpFindFileData)
  {
    TKPMSearch * hs = (TKPMSearch *)hFind;
    int nError = ERROR_SUCCESS;

    // Check the parameters
    if(nError == ERROR_SUCCESS)
    {
      if(!IsValidSearchHandle(hs) || lpFindFileData == NULL)
        nError = ERROR_INVALID_PARAMETER;
    }

    if(nError == ERROR_SUCCESS)
      nError = DoKPMSearch(hs, lpFindFileData);

    if(nError != ERROR_SUCCESS)
    {
      SetLastError(nError);
      return FALSE;
    }
    return TRUE;
  }
  
  bool KPM_SearchClose(TKPMSearch* hFind)
  {
    TKPMSearch * hs = (TKPMSearch *)hFind;

    // Check the parameters
    if(!IsValidSearchHandle(hs))
    {
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }

    FreeSearch(hs);
    return TRUE;
  }

  DWORD  KPM_SetDataCompression(int nDataCompression)
  {
    int    nValidMask = (KPM_COMPRESSION_ZLIB | KPM_COMPRESSION_PKWARE | KPM_COMPRESSION_BZIP2);

    DWORD  nError = ERROR_SUCCESS;

    if((nDataCompression & nValidMask) != nDataCompression)
    {
      nError = ERROR_INVALID_PARAMETER;
      return nError;
    }
    
    nDataCmp = nDataCompression;
    return nError;

  }

  DWORD  KPM_GetDataCompression()
  {
    return nDataCmp;
  }
}



