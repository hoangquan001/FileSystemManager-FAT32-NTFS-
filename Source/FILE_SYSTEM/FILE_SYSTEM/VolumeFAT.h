#pragma once
#include "generalLib.h"
#include "DirEntry.h"
#include "FAT32_BS.h"

class  VOLUME_FAT :public VOLUME
{
private:
	HANDLE			Device;
	FAT32_BS		BSector;
	DIR_ENTRY*		RootENTRY;
	UINT16			SecPerClus;
	UINT16			BytePerSec;
	UINT16			FstFAT1;
	UINT16			FstRDET;

public:
	VOLUME_FAT();
	~VOLUME_FAT();
	int Init(LPCWSTR  drive);
	int OpenVolume(LPCWSTR  drive);
	int ReadSector(UINT64 readPoint, BYTE sector[],UINT nByte = 512);
	void readDirectoryTree(DIR_ENTRY* Folder=nullptr);
	void PrintBootSecter();
	void PrintDirTree();
	void PrintDirTreeDetail ();
	void PrintDataEntry(wstring name);

	vector<UINT32>readListSectorFAT(UINT32 FstClus);
};
