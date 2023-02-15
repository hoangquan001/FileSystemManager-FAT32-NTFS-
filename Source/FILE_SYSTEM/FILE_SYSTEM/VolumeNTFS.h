#pragma once

#include "NTFS_BS.h"
#include "generalLib.h"
#include "MFT_Record.h"


//Directory tree//
//Tree entry use patten component OOP
class ENTRY_NTFS {
public:
	DWORD		IndexEntry;
	DWORD		Sector;
	FileName	Name;
public:
	//virtual void print();
	ENTRY_NTFS();
	ENTRY_NTFS(DWORD index);
	virtual void addFileName( FileName  Name);
	virtual void addEntry(ENTRY_NTFS* e) {}
	virtual void printDirectoryTree(int x=0) {};
	virtual void PrintInfoEntry(int x = 0);
	virtual ENTRY_NTFS* SearchEntry(wstring name) { return nullptr;}
};

class FILE_NTFS :public ENTRY_NTFS {

public:
	FILE_NTFS(DWORD index) : ENTRY_NTFS(index){
	}
	void printDirectoryTree(int x);
	//void PrintInfoEntry(int x = 0){}
	ENTRY_NTFS* SearchEntry(wstring name);
};

class FOLDER_NTFS :public ENTRY_NTFS {
private:
	vector < ENTRY_NTFS*>	list;
public:
	void addEntry(ENTRY_NTFS* e);
	FOLDER_NTFS(DWORD index) : ENTRY_NTFS(index) {}
	void printDirectoryTree(int x);
	void PrintInfoEntry(int x = 0);
	ENTRY_NTFS* SearchEntry(wstring name);
	~FOLDER_NTFS();
};


//Volume NTFS//
class  VOLUME_NTFS :public VOLUME
{
private:
	HANDLE		Device;
	NTFS_BS		BSector;
	WORD		SecPerClus;
	WORD		BytePerSec;
	DWORD		FstMTF;
	WORD		BytePerRecord;
	ENTRY_NTFS* RootEntry;
public:
	VOLUME_NTFS();
	~VOLUME_NTFS();
	int Init(LPCWSTR  drive);
	int  OpenVolume(LPCWSTR  drive);
	int  ReadSector(UINT64 readPoint, BYTE sector[], UINT nByte = 512);
	void PrintBootSecter();
	void readDirectoryTree(ENTRY_NTFS* folder = nullptr);
	void PrintDirTree();
	void PrintDirTreeDetail();
	void PrintDataEntry(wstring NameFile);
	
};
