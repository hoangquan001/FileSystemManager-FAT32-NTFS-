#pragma once
#include "DirEntry.h"

//struct Fat32BootSector;
//class FAT32_BS;

#pragma pack(push,1)
struct Fat32BootSector {
	BYTE	jmpBoot[3];
	BYTE	OEMName[8];
	WORD	BytesPerSec;
	BYTE	SecPerClus;
	WORD	RsvdSecCnt;
	BYTE	NumFATs;
	WORD	RootEntCnt;
	WORD	TotSec16;
	BYTE	Media;
	WORD	FATSz16;
	WORD	SecPerTrk;
	WORD	NumHeads;
	DWORD	HiddSec;
	DWORD	TotSec32;
	DWORD	FATSz32;
	WORD	ExtFlags;
	WORD	FSVer;
	DWORD	RootClus;
	WORD	FSInfo;
	WORD	BkBootSec;
	BYTE	reserved[12];
	BYTE	DrvNum;
	BYTE	Reserved1;
	BYTE	BootSig;
	DWORD	VolID;
	BYTE	VolLab[11];
	BYTE	FilSysType[8];
	BYTE	CodeReserved[420];
	BYTE	Sig[2];
};
#pragma pack(pop)
class FAT32_BS {
public:
	Fat32BootSector bsec;
public:

	void Read(BYTE[512]);
	UINT16 getFstFAT();
	UINT16 getFstRDET();
	UINT16 getBytePerSec();
	UINT16 getSecPerClus();
	UINT16 getClusRoot();
	string getFileSystem()
	{
		return ByteToStr(bsec.FilSysType, 8);
	}
	
	void PrintBS();
};
