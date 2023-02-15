#pragma once

#include "FAT32_BS.h"
#include "MFT_Record.h"


#pragma pack(push,1)
typedef struct NTFSBootSector
{
	BYTE        jmpBoot[3];
	BYTE        oemID[8];
	WORD        bytePerSec;
	BYTE        secPerClus;
	BYTE        reserved[2];
	BYTE        zero0[3];
	BYTE        unused1[2];
	BYTE        media;
	BYTE        zero1[2];
	WORD        secPerTrk;
	WORD        headNum;
	DWORD       HiddSec;
	BYTE        unused2[8];
	LONGLONG    totSec;
	LONGLONG    MFTClus;
	LONGLONG    MFTMirrClus;
	INT8		clusPerRecord;
	BYTE        unused4[3];
	INT8		clusPerBlock;
	BYTE        unused5[3];
	LONGLONG    serialNum;
	DWORD       checkSum;
	BYTE        bootCode[426];
	BYTE		endMarker[2];
};


class NTFS_BS {
private:
	NTFSBootSector bsec;
public:

	void Read(BYTE[512]);
	DWORD getFstMTFSec() { return bsec.MFTClus; }
	UINT16 getBytePerSec() { return bsec.bytePerSec; }
	UINT16 getSecPerClus() { return bsec.secPerClus; }
	UINT16 getClusPerRecord() { return pow(2, abs(bsec.clusPerRecord)); }
	string getFileSystem() {
		return ByteToStr(bsec.oemID, 8);
	}
	void PrintBS();
};
#pragma pack(pop)