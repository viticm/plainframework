#include "kpmEncrypt.h"


namespace KPM
{

  
  BOOL  bBufferCreated = FALSE;
  DWORD  mBuffer[HELPER_BUFFER_SIZE];
  
  int PerpareBuffers()
  {
    DWORD dwSeed = 0x00100001;
    DWORD index1 = 0;
    DWORD index2 = 0;
    int   i;

    // Initialize the decryption buffer.
    // Do nothing if already done.
    if(bBufferCreated == FALSE)
    {
      for(index1 = 0; index1 < 0x100; index1++)
      {
        for(index2 = index1, i = 0; i < 5; i++, index2 += 0x100)
        {
          DWORD temp1, temp2;

          dwSeed = (dwSeed * 125 + 3) % 0x2AAAAB;
          temp1  = (dwSeed & 0xFFFF) << 0x10;

          dwSeed = (dwSeed * 125 + 3) % 0x2AAAAB;
          temp2  = (dwSeed & 0xFFFF);

          mBuffer[index2] = (temp1 | temp2);
        }
      }
      bBufferCreated = TRUE;
    }
    return ERROR_SUCCESS;
  
  }

  void  Encrypt(DWORD * pdwPtr, BYTE * pbKey, DWORD dwLength)
  {
    DWORD dwSeed1 = 0x7FED7FED;
    DWORD dwSeed2 = 0xEEEEEEEE;
    DWORD ch;                           // One key character

    // Prepare seeds
    while(*pbKey != 0)
    {
      ch = toupper(*pbKey++);

      dwSeed1 = mBuffer[0x300 + ch] ^ (dwSeed1 + dwSeed2);
      dwSeed2 = ch + dwSeed1 + dwSeed2 + (dwSeed2 << 5) + 3;
    }

    // Encrypt it
    dwSeed2 = 0xEEEEEEEE;
    while(dwLength-- > 0)
    {
      dwSeed2   += mBuffer[0x400 + (dwSeed1 & 0xFF)];
      ch       = *pdwPtr;
      *pdwPtr++ = ch ^ (dwSeed1 + dwSeed2);

      dwSeed1  = ((~dwSeed1 << 0x15) + 0x11111111) | (dwSeed1 >> 0x0B);
      dwSeed2  = ch + dwSeed2 + (dwSeed2 << 5) + 3;
    }
  }
  void  Decrypt(DWORD * pdwPtr, BYTE * pbKey, DWORD dwLength)
  {
    DWORD dwSeed1 = 0x7FED7FED;
    DWORD dwSeed2 = 0xEEEEEEEE;
    DWORD ch;                           // One key character

    // Prepare seeds
    while(*pbKey != 0)
    {
      ch = toupper(*pbKey++);

      dwSeed1 = mBuffer[0x300 + ch] ^ (dwSeed1 + dwSeed2);
      dwSeed2 = ch + dwSeed1 + dwSeed2 + (dwSeed2 << 5) + 3;
    }

    // Decrypt it
    dwSeed2 = 0xEEEEEEEE;
    while(dwLength-- > 0)
    {
      dwSeed2 += mBuffer[0x400 + (dwSeed1 & 0xFF)];
      ch       = *pdwPtr ^ (dwSeed1 + dwSeed2);

      dwSeed1  = ((~dwSeed1 << 0x15) + 0x11111111) | (dwSeed1 >> 0x0B);
      dwSeed2  = ch + dwSeed2 + (dwSeed2 << 5) + 3;
      *pdwPtr++ = ch;
    }
  }


  //-----------------------------------------------------------------------------
  // Encrypting and decrypting block table

  void EncryptBlockTable(DWORD * pdwTable, BYTE * pbKey, DWORD dwLength)
  {
    DWORD dwSeed1 = 0x7FED7FED;
    DWORD dwSeed2 = 0xEEEEEEEE;
    DWORD ch;                           // One key character

    // Prepare seeds
    while(*pbKey != 0)
    {
      ch = toupper(*pbKey++);

      dwSeed1 = mBuffer[0x300 + ch] ^ (dwSeed1 + dwSeed2);
      dwSeed2 = ch + dwSeed1 + dwSeed2 + (dwSeed2 << 5) + 3;
    }

    // Decrypt it
    dwSeed2 = 0xEEEEEEEE;
    while(dwLength-- > 0)
    {
      dwSeed2   += mBuffer[0x400 + (dwSeed1 & 0xFF)];
      ch       = *pdwTable;
      *pdwTable++ = ch ^ (dwSeed1 + dwSeed2);

      dwSeed1  = ((~dwSeed1 << 0x15) + 0x11111111) | (dwSeed1 >> 0x0B);
      dwSeed2  = ch + dwSeed2 + (dwSeed2 << 5) + 3;
    }
  }

  void DecryptBlockTable(DWORD * pdwTable, BYTE * pbKey, DWORD dwLength)
  {
    DWORD dwSeed1 = 0x7FED7FED;
    DWORD dwSeed2 = 0xEEEEEEEE;
    DWORD ch;                           // One key character

    // Prepare seeds
    while(*pbKey != 0)
    {
      ch = toupper(*pbKey++);

      dwSeed1 = mBuffer[0x300 + ch] ^ (dwSeed1 + dwSeed2);
      dwSeed2 = ch + dwSeed1 + dwSeed2 + (dwSeed2 << 5) + 3;
    }

    // Encrypt it
    dwSeed2 = 0xEEEEEEEE;
    while(dwLength-- > 0)
    {
      dwSeed2 += mBuffer[0x400 + (dwSeed1 & 0xFF)];
      ch     = *pdwTable ^ (dwSeed1 + dwSeed2);

      dwSeed1  = ((~dwSeed1 << 0x15) + 0x11111111) | (dwSeed1 >> 0x0B);
      dwSeed2  = ch + dwSeed2 + (dwSeed2 << 5) + 3;
      *pdwTable++ = ch;
    }
  }

  //-----------------------------------------------------------------------------
  // Functions tries to get file decryption key. The trick comes from block
  // positions which are stored at the begin of each compressed file. We know the
  // file size, that means we know number of blocks that means we know the first
  // DWORD value in block position. And if we know encrypted and decrypted value,
  // we can find the decryption key !!!
  //
  // hf    - KPM file handle
  // block - DWORD array of block positions
  // ch    - Decrypted value of the first block pos

  DWORD DetectFileSeed(DWORD * block, DWORD decrypted)
  {
    DWORD saveSeed1;
    DWORD temp = *block ^ decrypted;    // temp = seed1 + seed2
    temp -= 0xEEEEEEEE;                 // temp = seed1 + mBuffer[0x400 + (seed1 & 0xFF)]

    for(int i = 0; i < 0x100; i++)      // Try all 255 possibilities
    {
      DWORD seed1;
      DWORD seed2 = 0xEEEEEEEE;
      DWORD ch;

      // Try the first DWORD (We exactly know the value)
      seed1  = temp - mBuffer[0x400 + i];
      seed2 += mBuffer[0x400 + (seed1 & 0xFF)];
      ch     = block[0] ^ (seed1 + seed2);

      if(ch != decrypted)
        continue;

      // Add 1 because we are decrypting block positions
      saveSeed1 = seed1 + 1;

      // If OK, continue and test the second value. We don't know exactly the value,
      // but we know that the second one has lower 16 bits set to zero
      // (no compressed block is larger than 0xFFFF bytes)
      seed1  = ((~seed1 << 0x15) + 0x11111111) | (seed1 >> 0x0B);
      seed2  = ch + seed2 + (seed2 << 5) + 3;

      seed2 += mBuffer[0x400 + (seed1 & 0xFF)];
      ch     = block[1] ^ (seed1 + seed2);

      if((ch & 0xFFFF0000) == 0)
        return saveSeed1;
    }
    return 0;
  }

  // Function tries to detect file seed. It expectes at least two uncompressed bytes
  DWORD DetectFileSeed2(DWORD * pdwBlock, UINT nDwords, ...)
  {
    va_list argList;
    DWORD dwDecrypted[0x10];
    DWORD saveSeed1;
    DWORD dwTemp;
    DWORD i, j;

    // We need at least two DWORDS to detect the seed
    if(nDwords < 0x02 || nDwords > 0x10)
      return 0;

    va_start(argList, nDwords);
    for(i = 0; i < nDwords; i++)
      dwDecrypted[i] = va_arg(argList, DWORD);
    va_end(argList);

    dwTemp = (*pdwBlock ^ dwDecrypted[0]) - 0xEEEEEEEE;
    for(i = 0; i < 0x100; i++)      // Try all 255 possibilities
    {
      DWORD seed1;
      DWORD seed2 = 0xEEEEEEEE;
      DWORD ch;

      // Try the first DWORD
      seed1  = dwTemp - mBuffer[0x400 + i];
      seed2 += mBuffer[0x400 + (seed1 & 0xFF)];
      ch     = pdwBlock[0] ^ (seed1 + seed2);

      if(ch != dwDecrypted[0])
        continue;

      saveSeed1 = seed1;

      // If OK, continue and test all bytes.
      for(j = 1; j < nDwords; j++)
      {
        seed1  = ((~seed1 << 0x15) + 0x11111111) | (seed1 >> 0x0B);
        seed2  = ch + seed2 + (seed2 << 5) + 3;

        seed2 += mBuffer[0x400 + (seed1 & 0xFF)];
        ch     = pdwBlock[j] ^ (seed1 + seed2);

        if(ch == dwDecrypted[j] && j == nDwords - 1)
          return saveSeed1;
      }
    }
    return 0;
  }


  //-----------------------------------------------------------------------------
  // Encrypting and decrypting MPQ blocks

  void EncryptBlock(DWORD * block, DWORD dwLength, DWORD dwSeed1)
  {
    DWORD dwSeed2 = 0xEEEEEEEE;
    DWORD ch;

    // Round to DWORDs
    dwLength >>= 2;

    while(dwLength-- > 0)
    {
      dwSeed2 += mBuffer[0x400 + (dwSeed1 & 0xFF)];
      ch     = *block;
      *block++ = ch ^ (dwSeed1 + dwSeed2);

      dwSeed1  = ((~dwSeed1 << 0x15) + 0x11111111) | (dwSeed1 >> 0x0B);
      dwSeed2  = ch + dwSeed2 + (dwSeed2 << 5) + 3;
    }
  }

  void DecryptBlock(DWORD * block, DWORD dwLength, DWORD dwSeed1)
  {
    DWORD dwSeed2 = 0xEEEEEEEE;
    DWORD ch;

    // Round to DWORDs
    dwLength >>= 2;

    while(dwLength-- > 0)
    {
      dwSeed2 += mBuffer[0x400 + (dwSeed1 & 0xFF)];
      ch     = *block ^ (dwSeed1 + dwSeed2);

      dwSeed1  = ((~dwSeed1 << 0x15) + 0x11111111) | (dwSeed1 >> 0x0B);
      dwSeed2  = ch + dwSeed2 + (dwSeed2 << 5) + 3;
      *block++ = ch;
    }
  }


  DWORD DecryptHashIndex(TKPMArchive * hArchive, const char * szFileName)
  {
    BYTE * pbKey   = (BYTE *)szFileName;
    DWORD  dwSeed1 = 0x7FED7FED;
    DWORD  dwSeed2 = 0xEEEEEEEE;
    DWORD  ch;

    while(*pbKey != 0)
    {
      ch = toupper(*pbKey++);

      dwSeed1 = mBuffer[0x000 + ch] ^ (dwSeed1 + dwSeed2);
      dwSeed2 = ch + dwSeed1 + dwSeed2 + (dwSeed2 << 5) + 3;
    }
    return (dwSeed1 & (hArchive->pHeader->dwHashTableSize - 1));
  }

  DWORD DecryptName1(const char * szFileName)
  {
    BYTE * pbKey   = (BYTE *)szFileName;
    DWORD  dwSeed1 = 0x7FED7FED;
    DWORD  dwSeed2 = 0xEEEEEEEE;
    DWORD  ch;

    while(*pbKey != 0)
    {
      ch = toupper(*pbKey++);

      dwSeed1 = mBuffer[0x100 + ch] ^ (dwSeed1 + dwSeed2);
      dwSeed2 = ch + dwSeed1 + dwSeed2 + (dwSeed2 << 5) + 3;
    }
    return dwSeed1;
  }

  DWORD DecryptName2(const char * szFileName)
  {
    BYTE * pbKey   = (BYTE *)szFileName;
    DWORD  dwSeed1 = 0x7FED7FED;
    DWORD  dwSeed2 = 0xEEEEEEEE;
    int    ch;

    while(*pbKey != 0)
    {
      ch = toupper(*pbKey++);

      dwSeed1 = mBuffer[0x200 + ch] ^ (dwSeed1 + dwSeed2);
      dwSeed2 = ch + dwSeed1 + dwSeed2 + (dwSeed2 << 5) + 3;
    }
    return dwSeed1;
  }

  DWORD DecryptFileSeed(const char * szFileName)
  {
    BYTE * pbKey   = (BYTE *)szFileName;
    DWORD  dwSeed1 = 0x7FED7FED;          // EBX
    DWORD  dwSeed2 = 0xEEEEEEEE;          // ESI
    DWORD  ch;

    while(*pbKey != 0)
    {
      ch = toupper(*pbKey++);           // ECX

      dwSeed1 = mBuffer[0x300 + ch] ^ (dwSeed1 + dwSeed2);
      dwSeed2 = ch + dwSeed1 + dwSeed2 + (dwSeed2 << 5) + 3;
    }
    return dwSeed1;
  }


  TKPMHashKey * GetHashEntry(TKPMArchive * ha, const char * szFileName)
  {
    TKPMHashKey * pHashEnd = ha->pHashKeyTable + ha->pHeader->dwHashTableSize;
    TKPMHashKey * pHash0;                      // File hash entry (start)
    TKPMHashKey * pHash;                       // File hash entry (current)
    DWORD dwIndex = (DWORD)(DWORD_PTR)szFileName;
    DWORD dwName1;
    DWORD dwName2;

    // If filename is given by index, we have to search all hash entries for the right index.
    if(dwIndex <= ha->pHeader->dwBlockTableSize)
    {
      // Pass all the hash entries and find the 
      for(pHash = ha->pHashKeyTable; pHash < pHashEnd; pHash++)
      {
        if(pHash->dwBlockIndex == dwIndex)
          return pHash;
      }
      return NULL;
    }

    // Decrypt name and block index
    dwIndex = DecryptHashIndex(ha, szFileName);
    dwName1 = DecryptName1(szFileName);
    dwName2 = DecryptName2(szFileName);
    pHash   = pHash0 = ha->pHashKeyTable + dwIndex;

    // Look for hash index
    while(pHash->dwBlockIndex != HASH_ENTRY_FREE)
    {
      if(pHash->dwName1 == dwName1 && pHash->dwName2 == dwName2 && pHash->dwBlockIndex != HASH_ENTRY_DELETED)
        return pHash;

      // Move to the next hash entry
      if(++pHash >= pHashEnd)
        pHash = ha->pHashKeyTable;
      if(pHash == pHash0)
        break;
    }

    // File was not found
    return NULL;
  }

  // Encrypts file name and gets the hash entry
  // Returns the hash pointer, which is always within the allocated array
  TKPMHashKey * FindFreeHashEntry(TKPMArchive * ha, const char * szFileName)
  {
    TKPMHashKey * pHashEnd = ha->pHashKeyTable + ha->pHeader->dwHashTableSize;
    TKPMHashKey * pHash0;                    // File hash entry (search start)
    TKPMHashKey * pHash;                     // File hash entry
    DWORD dwIndex = DecryptHashIndex(ha, szFileName);
    DWORD dwName1 = DecryptName1(szFileName);
    DWORD dwName2 = DecryptName2(szFileName);
    DWORD dwBlockIndex = 0xFFFFFFFF;

    // Save the starting hash position
    pHash = pHash0 = ha->pHashKeyTable + dwIndex;

    // Look for the first free hash entry. Can be also a deleted entry
    while(pHash->dwBlockIndex < HASH_ENTRY_DELETED)
    {
      if(++pHash >= pHashEnd)
        pHash = ha->pHashKeyTable;
      if(pHash == pHash0)
        return NULL;
    }

    // Fill the hash entry with the informations about the file name
    pHash->dwName1   = dwName1;
    pHash->dwName2   = dwName2;
    // Now we have to find a free block entry
    for(dwIndex = 0; dwIndex < ha->pHeader->dwBlockTableSize; dwIndex++)
    {
      TKPMBlock * pBlock = ha->pBlockTable + dwIndex;

      if((pBlock->dwFlags & KPM_FILE_EXISTS) == 0)
      {
        dwBlockIndex = dwIndex;
        break;
      }
    }

    // If no free block entry found, we have to use the index
    // at the end of the current block table
    if(dwBlockIndex == 0xFFFFFFFF)
      dwBlockIndex = ha->pHeader->dwBlockTableSize;
    pHash->dwBlockIndex = dwBlockIndex;
    return pHash;
  }

}