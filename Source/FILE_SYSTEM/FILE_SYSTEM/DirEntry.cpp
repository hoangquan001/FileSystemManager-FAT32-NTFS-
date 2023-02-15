#include "DirEntry.h"

void DIR_ENTRY::readMAIN_ENTRY(BYTE sec[32])
{
	mEntry.read(sec);
}

//void DIR_ENTRY::pushSUB_ENTRY(BYTE sec[32])
//{
//	SUB_ENTRY l;
//	stringstream ss = ByteToFile(sec, 32);
//	ss.read((char*)&l, sizeof(SUB_ENTRY));
//}
void DIR_ENTRY::pushSUB_ENTRY(SUB_ENTRY s)
{
	sEntry.push_back(s);
}



wstring DIR_ENTRY::getLongName()
{
	if (sEntry.size() == 0)
	{
		string a = mEntry.getName();
		wstring s(a.begin(),a.end());
		return s;
	}
	wstring s;
	size_t size = sEntry.size();
	for (int i = 0; i < size; i++)
		s += sEntry[i].getFullName();
	return s;
}

void DIR_ENTRY::PrintEntryDeTail(int x)
{
	wcout << setw(x) << " " << L"Name: " << getLongName() << endl;
	cout << setw(x) << " " << "Attribute: " << mEntry.getAttribute() << endl;
	cout << setw(x) << " " << "Create:	-Date:" << mEntry.getCreateDate() << "  -Time:" << mEntry.getCreateTime() << endl;
	cout << setw(x) << " " << "Access:	-Date: " << mEntry.getLastAccessDate() << endl;
	cout << setw(x) << " " << "Modified:	-Date: " << mEntry.getModifiedDate() << "  -Time:" << mEntry.getModifiedTime() << endl;
	cout << setw(x) << " " << "FileSize: " << mEntry.getSizeFile() << " Byte" << endl;
	cout << setw(x) << " " << "First Cluster: " << this->getFstCluster()<< endl;;
	cout << setw(x) << " " << string(40,'-') << endl;
}

DWORD DIR_ENTRY::getFstCluster()
{
	return mEntry.getFstCluster();
}

DWORD DIR_ENTRY::getSize()
{
	return mEntry.getSizeFile();
}

void FILE_ENTRY::PrintEntryTree(int x)

{
	wcout << setw(x) << char(195) <<  char(196) << " " << getLongName();
	cout<<DataSizeFomat(mEntry.getSizeFile()) << endl;
}

DIR_ENTRY* FILE_ENTRY::SearchEntry(wstring name)
{
	if (IsEquals(name, this->getLongName()))
	{
		return this;
	}
	return nullptr;
}
MAIN_ENTRY::MAIN_ENTRY()
{
	FstClusLo = 0;
	FstClusHi = 0;
}

void MAIN_ENTRY::read(BYTE sec[32])
{
	readFile((char*)this, sec, sizeof(MAIN_ENTRY));
}

string MAIN_ENTRY::getName()
{
	string str="";
	for (int i = 0; i < 8; i++)
			str += Name[i];
	if((Attr & 0x20)==0x20 )str += ".";
	for (int i = 8; i < 11; i++)
		str += Name[i];
	str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
	
	return str;
}

string MAIN_ENTRY::getAttribute()
{
	string buf = "";
	if (((Attr >> 0) & 1))
		buf += "ReadOnly ";
	if (((Attr >> 1) & 1))
		buf += "Hidden ";
	if (((Attr >> 2) & 1))
		buf += "System ";
	if (((Attr >> 3) & 1))
		buf += "Volume ";
	if (((Attr >> 4) & 1))
		buf += "Folder ";
	if (((Attr >> 5) & 1))
		buf += "Flie ";
	if (((Attr >> 6) & 1))
		buf += "Device ";
	return buf;
}

string MAIN_ENTRY::getCreateTime()
{
	return DectoTime(CrtTime);
}

string MAIN_ENTRY::getCreateDate()
{
	return DectoDate(CrtDate);
}

string MAIN_ENTRY::getLastAccessDate()
{
	return DectoDate(LstAccDate);
}

string MAIN_ENTRY::getModifiedDate()
{
	return DectoDate(ModDate);
}

string MAIN_ENTRY::getModifiedTime()
{
	return DectoTime(ModTime);
}

DWORD MAIN_ENTRY::getFstCluster()
{
	return FstClusHi*65536 +  FstClusLo;//
}





void MAIN_ENTRY::setName(string Name)
{
	size_t n = Name.size() < 11 ? Name.size() : 11;
	for (int i = 0; i < n; i++)
	{
		this->Name[i] = Name[i];
	}
}

UINT32 MAIN_ENTRY::getSizeFile()
{
	return  Size;
}

void SUB_ENTRY::read(BYTE sec[32])
{
	readFile((char*)this, sec,sizeof(SUB_ENTRY));
}

wstring SUB_ENTRY::getFullName()
{
	wstring s;
	UINT d = 0;
	for (int i = 0; i < 5; i++)
	{
		d = LName1[i];
		if (d == 65535|| d==0) break;
		s += d;
	}
	for (int i = 0; i < 6; i++)
	{
		d = LName2[i];
		if (d == 65535 || d == 0) break;
		s += d;
	}
	for (int i = 0; i < 2; i++)
	{
		d = LName3[i];
		if (d == 65535 || d == 0) break;
		s += d;
	}
	return s;
}



void FOLDER_ENTRY::AddEntry(DIR_ENTRY* en)
{
	listEntry.push_back(en);
}

void FOLDER_ENTRY::PrintEntryTree(int x)

{

	size_t n = listEntry.size();
	wcout << setw(x) << char(192) << getLongName() << L":" << endl;
	for (int i = 0; i < n; i++) {
		listEntry[i]->PrintEntryTree(x + 3);
	}
}

void FOLDER_ENTRY::PrintEntryDeTail(int x)
{

	cout << setw(x-2) << char(195) << char(196)<<" " << string(35, 196) << endl;
	wcout << setw(x) << " " << getLongName() << endl;
	cout << setw(x) << " " << "Attribute: " << mEntry.getAttribute() << endl;
	cout << setw(x) << " " << "First Cluster: " << this->getFstCluster() << endl;;
	cout << setw(x)<<" " << string(35, 196) << endl;

	size_t n = listEntry.size();
	for (int i = 0; i < n; i++) {
		listEntry[i]->PrintEntryDeTail(x + 5);
	}
}
DIR_ENTRY* FOLDER_ENTRY::SearchEntry(wstring name)

{
	if (IsEquals(name, this->getLongName()))
	{
		return this;
	}
	int n = listEntry.size();
	for (int i = 0; i < n; i++)
	{
		DIR_ENTRY* en = listEntry[i]->SearchEntry(name);
		if (en != nullptr)
			return en;
	}
	return nullptr;
}

FOLDER_ENTRY::~FOLDER_ENTRY(){
		size_t n = listEntry.size();
		for (int i = 0; i < n; i++)
		{
			delete listEntry[i];
		}
	
}

ROOT_ENTRYECTORY::ROOT_ENTRYECTORY(string name, WORD FstClus)
{
	mEntry.setName(name);
	mEntry.FstClusLo = FstClus;
}
