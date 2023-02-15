#pragma once
#include <Windows.h>
#include <iostream>
#include <sstream>
#include <string>
#include <stdint.h>
#include <ctime>
#include <math.h>
#include <iomanip>
#include <Vector>
#include <stack>
#include <algorithm>
using namespace std;

//general


class VOLUME //volume interface
{
public:
	virtual int Init(LPCWSTR  drive) = 0;
	virtual int OpenVolume(LPCWSTR  drive) = 0;
	virtual int ReadSector(UINT64 readPoint, BYTE sector[], UINT nByte = 512) = 0;
	virtual void PrintBootSecter() = 0;
	virtual void PrintDirTree() = 0;
	virtual	void PrintDirTreeDetail() = 0;
	virtual void PrintDataEntry(wstring name) = 0;
};



//read size byte from byte to buf
void readFile(char* buf, BYTE byte[], int size);

//convert lib
string DecToDateTime(ULONGLONG time);
string DectoTime(WORD dec);
string DectoDate(WORD dec);
string ByteToStr(BYTE[], int size);
//if(mode==true, dec=value(little endian) else bigEndien ||defaut=true)
UINT32 HexToDec(string hexSector, bool mode = true);
UINT StrToDec(BYTE[], int size, bool mode = true);
string DataSizeFomat(UINT64 size);
bool IsEquals(wstring str1, wstring str2);
//Fat
//CHeck Entry;
bool isDelete(BYTE[32]);
bool isFolder(BYTE[32]);
bool isFile(BYTE[32]);
bool isSUB_ENTRY(BYTE[32]);
bool isDot(BYTE[32]);
bool isHiddenEntry(BYTE[32]);
bool isVOLUME_ENTRY(BYTE[32]);
//NTFS

