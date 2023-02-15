#pragma once
#include <Windows.h>
#include "GeneralLib.h"
#define EOC 0x0FFFFFFF // ky hieu ket thuc cluster trong bang FAT
#define DELETE_EN 0xE5
#define FILE_EN 0x20
#define FOLDER_EN 0x10
#define SUB_EN 0x0F 
#define DOT_EN 0x2E
#define HIDDEN_EN 0x02
using namespace std;



#pragma pack(push,1)
class MAIN_ENTRY
{
public:
	BYTE	Name[11];
	BYTE	Attr;
	BYTE	Resever;
	BYTE	CrtTimeTenth;
	WORD	CrtTime;
	WORD	CrtDate;
	WORD	LstAccDate;
	WORD	FstClusHi;
	WORD	ModTime;
	WORD	ModDate;
	WORD	FstClusLo;
	UINT32	Size;
public:
	MAIN_ENTRY();
	void read(BYTE sec[32]);
	string getName();
	string getAttribute();
	string getCreateTime();
	string getCreateDate();
	string getLastAccessDate();
	string getModifiedDate();
	string getModifiedTime();
	DWORD  getFstCluster();
	void   setName(string Name);
	UINT32 getSizeFile();

};

class SUB_ENTRY
{
public:
	BYTE LOrd;
	WORD LName1[5];
	BYTE LATT;
	BYTE LType;
	BYTE LChksum;
	WORD LName2[6];
	WORD LFstClusLO;
	WORD LName3[2];
public:
	void read(BYTE sec[32]);
	wstring getFullName();
};
#pragma pack(pop)



//directory tree.
// Component trong design pattern
class DIR_ENTRY {
protected:
	MAIN_ENTRY mEntry; //main entry
	vector <SUB_ENTRY> sEntry; //sub entry
	
public:
	virtual void readMAIN_ENTRY(BYTE[32]);
	virtual void pushSUB_ENTRY(SUB_ENTRY s);
	virtual wstring getLongName();
	virtual DWORD getFstCluster();
	virtual DWORD getSize();

	virtual void AddEntry(DIR_ENTRY* en) {}
	virtual void PrintEntryDeTail(int x = 0);
	virtual void PrintEntryTree(int x=0){}
	virtual DIR_ENTRY * SearchEntry(wstring name){
		return nullptr;
	}

	//virtual DIR_ENTRY* FindFile(wstring filename);
	virtual ~DIR_ENTRY(){}
};
class FILE_ENTRY:public DIR_ENTRY
{
public:

	void PrintEntryTree(int x);
	DIR_ENTRY* SearchEntry(wstring name);
};
class FOLDER_ENTRY :public DIR_ENTRY
{
protected:
	vector <DIR_ENTRY*> listEntry;
public:
	void AddEntry(DIR_ENTRY* en);
	void PrintEntryTree(int x = 0);
	void PrintEntryDeTail(int x = 0);
	DIR_ENTRY* SearchEntry(wstring name);
	~FOLDER_ENTRY();
};
//Root ENTRYectory DIR_ENTRY
class ROOT_ENTRYECTORY :public FOLDER_ENTRY
{

public:
	ROOT_ENTRYECTORY(string name, WORD FstClus);
};


