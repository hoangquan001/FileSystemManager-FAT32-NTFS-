#include "MFT_Record.h"


#include "MFT_Record.h"



void INDEX_ROOT_ATTRI::read(BYTE sec[])
{

	readFile((char*)&headAttr, sec, 24);
	readFile((char*)&indexRoot, sec + headAttr.Res.Offset, sizeof MFT_ATTR_INDEX_ROOT);
	DWORD pos = headAttr.Res.Offset + sizeof(MFT_ATTR_INDEX_ROOT);
	while (pos + 84 < headAttr.RecordLength)
	{
		MFT_INDEX_ENTRY ie;
		readFile((char*)&ie, sec + pos, 16);
		FileName f;
		f.read(sec + pos + 16);
		BLOCKFILE block{ f,ie.DataOffset };
		list.push_back(block);
		pos += ie.Length;
	}
}


MFT_RECORD::MFT_RECORD()
{
	mft_head = new MFT_ENTRY_HEADER;
}
void MFT_RECORD::read(BYTE sec[])
{


	MFT_HEADER_ATTRIBUTE headAttr, headAttr2;
	readFile((char*)mft_head, sec, sizeof(MFT_ENTRY_HEADER));
	if (this->mft_head->magic != FILE_RECORD_MAGIC)
	{
		return;
	}
	int o = mft_head->updateOffset;
	sec[510] = sec[o + 3];
	sec[511] = sec[o + 4];
	sec[1022] = sec[o + 5];
	sec[1023] = sec[o + 6];

	DWORD curOffset = mft_head->attributeOffset;
	//print();
	ATTRIBUTE* attr = nullptr;
	while (true) {
		readFile((char*)&headAttr, sec + curOffset, 16);
		attr = nullptr;

		 if (headAttr.TypeCode == $FILE_NAME) {
			attr = new FILENAME_ATTRI;
		}
		else
			if (headAttr.TypeCode == $INDEX_ROOT) {
				attr = new INDEX_ROOT_ATTRI;
			}
			else if (headAttr.TypeCode == $INDEX_ALLOCATION) {
				attr = new INDEX_ALLOC_ATTRI;
			}
			else if (headAttr.TypeCode == $DATA) {
				attr = new DATA_ATTRIBUTE;
			}
		if (attr) {
			attr->read(sec + curOffset);
			listAttri.push_back(attr);

		}
		curOffset += headAttr.RecordLength;
		if (headAttr.TypeCode == $END || curOffset > 1024)
			break;

	}
}



LISTDATARUN MFT_RECORD::getDataRun_INDEX()
{
	INDEX_ALLOC_ATTRI* ia = nullptr;
	for (int i = 0; i < listAttri.size(); i++)
	{
		ia = dynamic_cast<INDEX_ALLOC_ATTRI*> (listAttri[i]);
		if (ia != nullptr)
			return ia->getDatRun();
	}
	return LISTDATARUN();
}

LISTDATARUN MFT_RECORD::getDataRun_DATA()
{

	LISTDATARUN datarun;
	DATA_ATTRIBUTE* data = nullptr;
	for (int i = 0; i < listAttri.size(); i++)
	{
		data = dynamic_cast<DATA_ATTRIBUTE*> (listAttri[i]);
		if (data != nullptr)
		{
			LISTDATARUN  run = data->getDataRun();
			datarun.insert(datarun.end(), run.begin(), run.end());
		}
	}
	return datarun;


}
//LISTDATARUN MFT_RECORD::getDataRun_DATA()
//{
//	LISTDATARUN datarun;
//	DATA_ATTRIBUTE* data = nullptr;
//	for (int i = 0; i < listAttri.size(); i++)
//	{
//		data = dynamic_cast<DATA_ATTRIBUTE*> (listAttri[i]);
//		if (data != nullptr)
//		{
//			LISTDATARUN  run = data->getDatRun();
//			datarun.insert(datarun.end(), run.begin(), run.end());
//		}
//	}
//	return datarun;
//
//}

LIST_INDEXT MFT_RECORD::getIndexEntrys()
{
	INDEX_ROOT_ATTRI* id = nullptr;
	for (int i = 0; i < listAttri.size(); i++)
	{
		id = dynamic_cast<INDEX_ROOT_ATTRI*> (listAttri[i]);
		if (id != nullptr)
		{
			return id->getListIndexEntry();
		}
	}
	return LIST_INDEXT();
}

DATA_ATTRIBUTE* MFT_RECORD::getDataAttr()

{
	DATA_ATTRIBUTE* data = nullptr;
	for (int i = 0; i < listAttri.size(); i++)
	{
		data = dynamic_cast<DATA_ATTRIBUTE*> (listAttri[i]);
		if (data != nullptr)
			return data;
	}
	return nullptr;
}


MFT_RECORD::~MFT_RECORD()
{
	for (int i = 0; i < listAttri.size(); i++)
	{
		if (listAttri[i] != nullptr)
			delete listAttri[i];
	}
	delete mft_head;
}


vector <MFT_DATARUN> readDataRun(BYTE datarun[], int size)
{
	int i = 0;
	vector < MFT_DATARUN> list;
	while (datarun[i] != 0x00)
	{
		int offset_len = datarun[i] >> 4;
		int length_len = datarun[i] & 0xf;
		ULONGLONG length = 0;
		LONGLONG offset = 0;
		MFT_DATARUN drun;
		readFile((char*)&length, datarun + i + 1, length_len);
		readFile((char*)&offset, datarun + i + 1 + length_len, offset_len);
		drun.length = length;
		drun.offset = offset;
		i += offset_len + length_len + 1;
		list.push_back(drun);
	}
	return list;
}

void INDEX_ALLOC_ATTRI::read(BYTE sec[])
{
	readFile((char*)&headAttr, sec, 64);
	USHORT fstData = headAttr.N_Res.MappingPairsOffset;
	DWORD lengRecord = headAttr.RecordLength;
	_list = readDataRun(sec + fstData, lengRecord - fstData);
}



LISTDATARUN INDEX_ALLOC_ATTRI::getDatRun()
{
	return _list;
}

void FileName::read(BYTE sec[])
{
	wchar_t *buf=new wchar_t[256];
	readFile((char*)&fName, sec, sizeof MFT_ATTR_FILENAME);
		
	WORD length = fName.NameLength;
	readFile((char*)buf, sec + 66, length *2);
	buf[length] = NULL;
	NameEntry = L"";
	for (int i = 0; i < length; i++)
	{
		if (buf[i] < 32||buf[i]>128) continue;
		NameEntry += buf[i];
	}

	

}

ULONGLONG FileName::getSize()
{
	return fName.DataSize;
}


string FileName::getAttribute() {
	string buf;
	DWORD Attr = fName.FileAttributes;
	if (Attr & ATTR_FILENAME_FLAG_READONLY)
		buf += "ReadOnly ";
	if (Attr & ATTR_FILENAME_FLAG_HIDDEN)
		buf += "Hidden ";
	if (Attr & ATTR_FILENAME_FLAG_SYSTEM)
		buf += "System ";
	if (Attr & ATTR_FILENAME_FLAG_ENTRYECTORY)
		buf += "Folder ";
	if (Attr & ATTR_FILENAME_FLAG_ARCHIVE)
		buf += "Flie ";
	if (Attr & ATTR_FILENAME_FLAG_DEVICE)
		buf += "Device ";
	return buf;
}

wstring FileName::getName()
{
	
	//wstring buf(NameEntry);
	//for (int i = 0; i < wcslen(NameEntry); i++) {
	//	buf[i] += NameEntry[i];
	//}
	return NameEntry;
}

FileName FileName::clone()
{
	return *this;
}

void DATA_ATTRIBUTE::read(BYTE sec[])
{
	readFile((char*)&headAttr, sec, 24);
	if (headAttr.FormCode == RESIDENT)
	{
		isDataRun = false;
		Data= ByteToStr(sec+ headAttr.Res.Offset, headAttr.Res.Length);
	}
	else 
	{
		isDataRun = true;
		readFile((char*)&headAttr, sec, 64);
		USHORT fstData = headAttr.N_Res.MappingPairsOffset;
		DWORD lengRecord = headAttr.RecordLength;
		_list = readDataRun(sec + fstData, lengRecord - fstData);
	}


}

LISTDATARUN DATA_ATTRIBUTE::getDataRun()
{
	return _list;
}

