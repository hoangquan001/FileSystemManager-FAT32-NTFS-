#include "VolumeNTFS.h"

VOLUME_NTFS::VOLUME_NTFS()
{
	Device = NULL;
	RootEntry = nullptr;
}

VOLUME_NTFS::~VOLUME_NTFS()
{
	CloseHandle(Device);
	delete RootEntry;
}

int VOLUME_NTFS::Init(LPCWSTR drive)
{

		BYTE sec[512];
		//mo dirve, neu khac 1 la that bai
		int status = this->OpenVolume(drive);
		if (status != 0) return -1;

		this->ReadSector(0, sec);
		BSector.Read(sec);
		BSector.getFileSystem();
		if (BSector.getFileSystem().find("NTFS") == string::npos)
			return 0;

		SecPerClus = BSector.getSecPerClus();
		BytePerSec = BSector.getBytePerSec();
		FstMTF = BSector.getFstMTFSec()* SecPerClus;
		BytePerRecord = BSector.getClusPerRecord();
		RootEntry = new FOLDER_NTFS(ROOT_FILE_NAME_INDEX);
		RootEntry->Sector = FstMTF+ ROOT_FILE_NAME_INDEX*2;
		readDirectoryTree(RootEntry);
		return 1;
}

int VOLUME_NTFS::OpenVolume(LPCWSTR drive)
{
	Device = CreateFile(drive,    // Drive to open
		GENERIC_READ,           // Access mode
		FILE_SHARE_READ | FILE_SHARE_WRITE,        // Share Mode
		NULL,                   // Security Descriptor
		OPEN_EXISTING,          // How to create
		0,                      // File attributes
		NULL);                  // Handle to template

	if (Device == INVALID_HANDLE_VALUE) // Open Error
		return GetLastError();
	else
		return 0;
}

int VOLUME_NTFS::ReadSector(UINT64 readPoint, BYTE sector[], UINT nByte)
{
	int retCode = 0;
	DWORD bytesRead;
	LONG Hightoffset = readPoint >> 32;
	SetFilePointer(Device, (DWORD)readPoint, &Hightoffset, FILE_BEGIN);//Set a Point to Read
	if (!ReadFile(Device, sector, nByte, &bytesRead, NULL))
	{
		printf("ReadFile: %u\n", GetLastError());
		exit(1);

	}
	else
		return bytesRead;
}

void VOLUME_NTFS::PrintBootSecter()
{
	BSector.PrintBS();
}
void VOLUME_NTFS::readDirectoryTree(ENTRY_NTFS* folder)
{
	BYTE* byte = new BYTE[2048];
	UINT64 pos = (FstMTF * 512 + 1024 * folder->IndexEntry);
	ENTRY_NTFS* entry = nullptr;
	FileName fName;
	ReadSector(pos, byte, 1024);

	//Read mtf entry
	MFT_RECORD _mftRecord;
	_mftRecord.read(byte);
	//read entry in root indext folder attribute.
	LIST_INDEXT list = _mftRecord.getIndexEntrys();
	for (int i = 0; i < list.size(); i++)
	{
		if (list[i].fname.getFileAttr() & ATTR_FILENAME_FLAG_ENTRYECTORY)
		{
			entry = new FOLDER_NTFS(list[i].curIndex);
			//de quy/////////////////////////////
			readDirectoryTree(entry);
		}
		else if (list[i].fname.getFileAttr() & ATTR_FILENAME_FLAG_ARCHIVE)
		{
			entry = new   FILE_NTFS(list[i].curIndex);
		}
		if (entry != nullptr)
		{
			entry->addFileName(list[i].fname);
			entry->Sector = FstMTF + list[i].curIndex * 2;
			folder->addEntry(entry);
		}
	}


	//read if folder have indext Allocation attribute:
	LISTDATARUN run = _mftRecord.getDataRun_INDEX();
	INT64 offset = 0;
	BYTE LIST_US[20];
	//read index block from data run, readed MFT_RECORD.

	for (int i = 0; i < run.size(); i++)
	{
		int US = 0;
		offset += run[i].offset;
		pos = offset * SecPerClus * BytePerSec;
		ReadSector(pos, byte, 1024);
		INDEX_BLOCK idb;
		readFile((char*)&idb, byte, sizeof INDEX_BLOCK);

		readFile((char*)LIST_US, byte+ idb.OffsetOfUS+2,(idb.SizeOfUS-1)*2);



		byte[BytePerSec - 2] = LIST_US[US++];
		byte[BytePerSec - 1] = LIST_US[US++];
		//cout << ByteToStr(byte  , 512);

		UINT64 pos2 = idb.EntryOffset + 0x18;
		if (idb.Magic != INDEX_BLOCK_MAGIC)
		{
			cout << "\nMagic number error!\n";
			exit(1);
		}
		UINT64 total = 0;
		while (total + 66 <= idb.TotalEntrySize)
		{
			if (pos2 > 512)
			{
				pos += 512;
				pos2 -= 512;
				ReadSector(pos, byte, 1024);

				byte[BytePerSec - 2] = LIST_US[US++];
				byte[BytePerSec - 1] = LIST_US[US++];
			}


			MFT_INDEX_ENTRY ie;
			readFile((char*)&ie, byte + pos2, 84);
			if (ie.Flags == INDEX_ENTRY_FLAG_LAST) {
				break;
			}


			fName.read(byte + pos2 + 16);
	
			DWORD idex = ie.DataOffset;
			entry = nullptr;
			if (fName.getFileAttr() == ATTR_FILENAME_FLAG_ENTRYECTORY)
			{
				entry = new FOLDER_NTFS(idex);
				readDirectoryTree(entry); //de quy/////////////////////
			}

			else if ((fName.getFileAttr() & ATTR_FILENAME_FLAG_ARCHIVE)!= 0)
			{
				entry = new   FILE_NTFS(idex);
			}
			if (entry != nullptr)
			{
				entry->addFileName(fName);
				entry->Sector = FstMTF + idex * 2;
				folder->addEntry(entry);
			}
			pos2 += ie.Length;
			total += ie.Length;
		}
	}
	delete[]byte;
}

void VOLUME_NTFS::PrintDirTree()
{
	cout << char(218) << string(40, char(196)) << char(191) << endl;
	cout << char(179) << "               Cay Thu Muc              " << char(179) << endl;
	cout << char(192) << std::string(40, char(196)) << char(217) << endl;
	if(RootEntry)
		RootEntry->printDirectoryTree();
}

void VOLUME_NTFS::PrintDirTreeDetail()
{

	if (!RootEntry) return;
	cout << char(218) << string(40, char(196)) << char(191) << endl;
	cout << char(179) << "           Cay Thu Muc Chi Tiet         " << char(179) << endl;
	cout << char(192) << std::string(40, char(196)) << char(217) << endl;
	RootEntry->PrintInfoEntry();
}

void VOLUME_NTFS::PrintDataEntry(wstring NameFile)
{


	if (!RootEntry) return;
	ENTRY_NTFS* entry = RootEntry->SearchEntry(NameFile);

	if (entry == nullptr)
	{
		cout << "File Not Found!" << endl;
		return;
	}



	if (dynamic_cast <FILE_NTFS*>(entry) != nullptr) //For File
	{
		cout << " ---Thong Tin File---" << endl;

		entry->PrintInfoEntry(0);
		BYTE* sec = new BYTE[1024];
		ULONGLONG sizeFile = entry->Name.getSize();
		UINT64 pos = (FstMTF * 512 + 1024 * entry->IndexEntry);
		ReadSector(pos, sec, 1024);
		MFT_RECORD mft_record;
		mft_record.read(sec);
		DATA_ATTRIBUTE* dataAtr = mft_record.getDataAttr();

		transform(NameFile.begin(), NameFile.end(), NameFile.begin(), ::tolower);
		if (NameFile.find(L".txt") == std::string::npos) {
			cout << "Dung phan mem tuong thich de doc noi dung!" << endl;
			return;
		}

		if (dataAtr == nullptr)
		{
			cout << "No Data!";
		}
		else if (dataAtr->isDataRun == false)
		{
			cout << dataAtr->Data;
		}
		else
		{

			cout << "---------------------DATA---------------------" << endl;
			LISTDATARUN run = mft_record.getDataRun_DATA();
			int size = run.size();
			BYTE* byte = new BYTE[BytePerSec];
			ULONGLONG total = 0;
			for (int i = 0; i < size; i++)
			{

				for (int j = 0; j < SecPerClus * run[i].length; j++)
				{
					ReadSector(((run[i].offset) * SecPerClus + j) * BytePerSec, byte, BytePerSec);

					cout << ByteToStr(byte, BytePerSec);

					if (total > sizeFile)
					{
						delete[] sec;
						delete[] byte;
						return;
					}
					total += BytePerSec;

				}
			}
			delete[] byte;
		}

		delete[] sec;

	}
	else
	{
		//For folder
		cout << char(218) << string(40, char(196)) << char(191) << endl;
		cout << char(179) << "             Cay Thu Muc Con            " << char(179) << endl;
		cout << char(192) << std::string(40, char(196)) << char(217) << endl;
		entry->printDirectoryTree();
		cout << string(40, char(196))<<endl;
		cout << char(218) << string(40, char(196)) << char(191) << endl;
		cout << char(179) << "           Cay Thu Muc Chi Tiet         " << char(179) << endl;
		cout << char(192) << std::string(40, char(196)) << char(217) << endl;
		entry->PrintInfoEntry();
	}

}

ENTRY_NTFS::ENTRY_NTFS()

{
	//this->Name = new FileName;
	IndexEntry = -1;
}

ENTRY_NTFS::ENTRY_NTFS(DWORD index)
{
	IndexEntry = index;
}

void ENTRY_NTFS::addFileName( FileName  Name)
{

	this->Name = Name;
}

void ENTRY_NTFS::PrintInfoEntry(int x)

{
	wcout << setw(x) << " " << L"Name: " << Name.getName() << endl;
	cout << setw(x) << " " << "Attribute: " << Name.getAttribute() << endl;
	cout << setw(x) << " " << "Create:	-" << Name.getCreateTime() << endl;
	cout << setw(x) << " " << "Access:	-" << Name.getAccessTime() << endl;
	cout << setw(x) << " " << "Modified:	-" << Name.getModifiedTime() << endl;
	cout << setw(x) << " " << "FileSize:	" << Name.getSize() << " Byte" << endl;
	cout << setw(x) << " " << "IndexEntry: " << IndexEntry << endl;
	cout << setw(x) << " " << "Sector: " << Sector<< endl;
	cout << setw(x) << " " << string(40, '-') << endl;
}

void FILE_NTFS::printDirectoryTree(int x)
{
	wstring a = Name.getName();
	wcout << setw(x) << char(195) << char(196) << " " <<a;
	//cout << setw(x) << char(195) << char(196) << " "<<a.size();
	cout<< DataSizeFomat(Name.fName.DataSize) << endl;

}
ENTRY_NTFS* FILE_NTFS::SearchEntry(wstring name)

{
	if (IsEquals(name, this->Name.getName()))
	{
		return this;
	}
	return nullptr;
}

void FOLDER_NTFS::addEntry(ENTRY_NTFS* e)
{

	list.push_back(e);
}

void FOLDER_NTFS::printDirectoryTree(int x)
{

	wcout << setw(x) << char(192) << Name.getName() << L":" << endl;
	size_t n = list.size();
	for (int i = 0; i < n; i++) {

		list[i]->printDirectoryTree(x + 3);
	}
}
void FOLDER_NTFS::PrintInfoEntry(int x)
{
	cout << setw(x) << " " << string(35, 196) << endl;
	wcout << setw(x - 1) << char(195) << " " << Name.getName() << endl;
	cout << setw(x) << " " << "Attribute: " << Name.getAttribute() << endl;
	cout << setw(x) << " " << "First Index: " << this->IndexEntry << endl;
	cout << setw(x) << " " << "Sector MFT entry: " << Sector << endl;
	cout << setw(x) << " " << string(35, 196) << endl;

	size_t n = list.size();
	for (int i = 0; i < n; i++) {
		list[i]->PrintInfoEntry(x + 5);
	}
}

ENTRY_NTFS* FOLDER_NTFS::SearchEntry(wstring name)

{

	if (IsEquals(name, this->Name.getName()))
	{
		return this;
	}
	int n = list.size();
	for (int i = 0; i < n; i++)
	{
		ENTRY_NTFS* en = list[i]->SearchEntry(name);
		if (en != nullptr)
			return en;
	}
	return nullptr;
}
FOLDER_NTFS::~FOLDER_NTFS()
{
	int size = list.size();
	for (int i = 0; i < size; i++)
	{
		if(list[i]!=nullptr)
			delete list[i];
			
	}
}
