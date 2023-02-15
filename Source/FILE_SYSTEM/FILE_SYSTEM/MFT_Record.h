#pragma once

#include "EntryType.h"
typedef vector <MFT_DATARUN> LISTDATARUN;

/////////////////////////////////////
// use for FileName and IndexEntry//
///////////////////////////////////
class FileName {
	public :
	MFT_ATTR_FILENAME fName;
	wstring NameEntry;
public:
	void read(BYTE sec[]);
	ULONGLONG getSize();
	DWORD getFileAttr() {	return fName.FileAttributes;}
	string getModifiedTime() { 	return DecToDateTime(fName.ChangeTime); }
	string getCreateTime(){ return DecToDateTime(fName.CreateTime);}
	string getAccessTime(){		return DecToDateTime(fName.AccessTime);}
	string getAttribute();
	wstring getName();
	FileName clone();

};


//class inheritance for attribute;
class ATTRIBUTE {
protected:
	MFT_HEADER_ATTRIBUTE headAttr;
public:
	virtual void read(BYTE[]){}
};

  /////////////////////////////////
 //STANDARD_INFOMATION_ATTRIBUTE//
/////////////////////////////////
class STANDARD_INFO_ATTRI :public ATTRIBUTE {
private:
	MFT_ATTR_STANDARD_INFORMATION standInfo;
public:
	void read(BYTE sec[]) {

		readFile((char*)&headAttr, sec, sizeof(MFT_HEADER_ATTRIBUTE));
		readFile((char*)&standInfo, sec+ 24, 48);
	}
};

  /////////////////////////////////
 //		FILENAME_ATTRI			//
/////////////////////////////////
class FILENAME_ATTRI :public ATTRIBUTE,public FileName{

public:
	void read(BYTE sec[]) {

		readFile((char*)&headAttr, sec, (24));
		FileName::read(sec+24);
	}
};


  /////////////////////////////////
 //		INDEX_ROOT_ATTRI		//
/////////////////////////////////

struct BLOCKFILE {
	FileName fname;
	DWORD curIndex;
};

typedef vector<BLOCKFILE> LIST_INDEXT;
class INDEX_ROOT_ATTRI : public ATTRIBUTE {

private:
	MFT_ATTR_INDEX_ROOT indexRoot;
	LIST_INDEXT list;
public:
	void read(BYTE sec[]);
	LIST_INDEXT getListIndexEntry()
	{
		return this->list;
	}
}; 




/////////////////////////////////
//		INDEX_ALLOC_ATTRI		//
/////////////////////////////////
LISTDATARUN readDataRun(BYTE datarun[], int size);

class INDEX_ALLOC_ATTRI :public ATTRIBUTE {
private:
	LISTDATARUN _list;
public:
	void read(BYTE sec[]);
	LISTDATARUN getDatRun();
};


////////////////////////////////
//		DATA ARRTIBUTE      	/
////////////////////////////////		
class DATA_ATTRIBUTE :public ATTRIBUTE {
public:
	bool isDataRun;
	LISTDATARUN _list;
	string Data;
public:
	void read(BYTE sec[]);
	LISTDATARUN getDataRun();
};


/////////////////////////////////
//		MFT_RECORD				//
/////////////////////////////////
class MFT_RECORD {
private:
	MFT_ENTRY_HEADER *mft_head;
	vector <ATTRIBUTE*> listAttri;
	
public:

	void read(BYTE sec[]);
	//Data run of INDEXT block for Allocation Attr
	LISTDATARUN getDataRun_INDEX();
	LISTDATARUN getDataRun_DATA();
	//
	LIST_INDEXT getIndexEntrys();
	DATA_ATTRIBUTE* getDataAttr();
	MFT_RECORD();
	~MFT_RECORD();
};