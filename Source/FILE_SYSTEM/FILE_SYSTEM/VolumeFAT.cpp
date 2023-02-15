#include "VolumeFat.h"
int v = 0;
VOLUME_FAT::VOLUME_FAT()
{
    Device = NULL;
    RootENTRY = nullptr;
}
VOLUME_FAT::~VOLUME_FAT()
{
    CloseHandle(Device);
    if (RootENTRY)
        delete RootENTRY;
}
int VOLUME_FAT::Init(LPCWSTR drive)
{
    BYTE sec[512];
    int status = this->OpenVolume(drive);
    if (status !=0) return -1;

    this->ReadSector(0, sec);
    BSector.Read(sec);
    if (BSector.getFileSystem().find("FAT32") == string::npos)
        return 0;

    SecPerClus = BSector.getSecPerClus();
    BytePerSec = BSector.getBytePerSec();
    FstFAT1 = BSector.getFstFAT();
    FstRDET = BSector.getFstRDET();
    WORD fst = BSector.getClusRoot();

    string name;
    name = int(drive[4]);
    RootENTRY = new ROOT_ENTRYECTORY(name, fst);
    readDirectoryTree(RootENTRY);
    return 1;
}
int VOLUME_FAT::OpenVolume(LPCWSTR drive)
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

int VOLUME_FAT::ReadSector(UINT64 readPoint, BYTE sector[], UINT nByte)
{
    int retCode = 0;
    DWORD bytesRead;
    LONG HightOffset = readPoint >>32;
    SetFilePointer(Device, (DWORD)readPoint, NULL, FILE_BEGIN);//Set a Point to Read
    if (!ReadFile(Device, sector, nByte, &bytesRead, NULL))
    {
        printf("ReadFile: %u\n", GetLastError());
        exit(1);
    }
    else
    {
        return bytesRead;
    }
}


void VOLUME_FAT::readDirectoryTree(DIR_ENTRY* Folder)
{
    if (dynamic_cast<FOLDER_ENTRY*>(Folder) == nullptr) return;
    BYTE *sec=new BYTE [BytePerSec];
    UINT32 fstClus = Folder->getFstCluster();
    vector <UINT32> listSector = this->readListSectorFAT(fstClus);
  
    int n = listSector.size();

    stack <SUB_ENTRY> st;
    for (int i = 0; i < n; i++) {

            UINT32 sectorIndex = listSector[i];
            DWORD posByte = BytePerSec * (sectorIndex);
            this->ReadSector(posByte, sec,BytePerSec);

            for (int k = 0; k < BytePerSec; k+=32)
            {
                if ((int)sec[k + 0] == 0x00 || isDelete(sec + k) || isDot(sec + k)) { continue; }
                if (isSUB_ENTRY(sec +k))
                {
                    SUB_ENTRY l;
                    l.read(sec + k);
                    st.push(l);
                }
                else
                {

                    DIR_ENTRY* Entry = nullptr;
                    //read folder and file
                    if (isFile(sec +k))
                    {
                        Entry = new  FILE_ENTRY;
                    }
                    else  if (isFolder(sec + k))
                    {
                        Entry = new FOLDER_ENTRY;
                    }
                    else if (Entry == nullptr)
                        {
                            while (!st.empty())   st.pop();
                            continue;
                        }
                    Entry->readMAIN_ENTRY(sec + k);

                    while (!st.empty())
                    {
                        Entry->pushSUB_ENTRY(st.top());
                        st.pop();
                    }
                    readDirectoryTree(Entry); // de qui
                    Folder->AddEntry(Entry);

                }
            }
    }
    delete[] sec;
}



void VOLUME_FAT::PrintBootSecter()
{
    if (!RootENTRY) return;
    BSector.PrintBS();
}

void VOLUME_FAT::PrintDirTree()
{
    if (!RootENTRY) return;
    cout <<char(218)<<string(40, char(196)) <<char(191)<< endl;
    cout <<char(179) <<"               Cay Thu Muc              "<< char(179) << endl;
    cout << char(192) << std::string(40, char(196)) << char(217)<<endl;
    RootENTRY->PrintEntryTree();

    cout << string(80, '-') << endl;
}

void VOLUME_FAT::PrintDirTreeDetail()
{
    if (!RootENTRY) return;
    cout << char(218) << string(40, char(196)) << char(191) << endl;
    cout << char(179) << "            Cay Thu Muc Chi Tiet        " << char(179) << endl;
    cout << char(192) << std::string(40, char(196)) << char(217) << endl;
    RootENTRY->PrintEntryDeTail();
}
void VOLUME_FAT::PrintDataEntry(wstring name)

{
    if (!RootENTRY) return;
    DIR_ENTRY* entry = RootENTRY->SearchEntry(name);
    if (entry == nullptr)
    {
        cout << "File Not Found!" << endl;
        return;
    }
    // check if file
    if (dynamic_cast <FILE_ENTRY*>(entry) != nullptr)
    {
        entry->PrintEntryDeTail();

        cout << "---------------------DATA---------------------" << endl;
        transform(name.begin(), name.end(), name.begin(), ::tolower);
        if (name.find(L".txt") == std::string::npos) {
            cout << "Dung phan mem tuong thich de doc noi dung!" << endl;
            return;
        }

        DWORD fstClus = entry->getFstCluster();
        DWORD size = entry->getSize();
        if (size == 0) return;
        BYTE* sec = new BYTE[BytePerSec];
        for (int i = 0; i < 8; i++)
        {
            if (i * 512 > size) break;
            UINT64 posByte = (FstRDET + i + (fstClus - 2) * SecPerClus) * BytePerSec;
            this->ReadSector(posByte, sec, BytePerSec);
            cout << ByteToStr(sec, 512);
        }
        //}
        delete[] sec;
    }
    else
    {
        //For Folder

        cout << char(218) << string(40, char(196)) << char(191) << endl;
        cout << char(179) << "             Cay Thu Muc Con            " << char(179) << endl;
        cout << char(192) << std::string(40, char(196)) << char(217) << endl;
        entry->PrintEntryTree();

        cout << string(40, '-') << endl;
        cout << char(218) << string(40, char(196)) << char(191) << endl;
        cout << char(179) << "        Cay Thu Muc Con Chi Tiet        " << char(179) << endl;
        cout << char(192) << std::string(40, char(196)) << char(217) << endl;
        entry->PrintEntryDeTail();
    }
}

bool isDelete(BYTE entry[32])
{
    if (  entry[0x00]== DELETE_EN) return true;
    return false;
}
bool isFolder(BYTE entry[32])
{
   /* if ((entry[0x0B] & 0x10) != 0) return true;*/
    if ((entry[0x0B]  & FOLDER_EN) == FOLDER_EN) return true;
    else return false;
   /* if ((entry[0x0B] == FOLDER_EN)) return true;
    else return false;*/
    
}
bool isFile(BYTE entry[32])
{
    if ((entry[0x0B] & FILE_EN) == FILE_EN) return true;
    else return false;
}
bool isSUB_ENTRY(BYTE entry[32])
{
    if ((entry[0x0B] &SUB_EN)== SUB_EN) return true;
    else return false;
}
bool isDot(BYTE entry[32])
{
    if (entry[0x00] == DOT_EN) return true;
    return false;
}
bool isVOLUME_ENTRY(BYTE entry[32])
{
    
        if ((entry[0x0B] & 0x08) == 0x08) return true;
        else return false;
    
}
//bool VOLUME_FAT::isHiddenEntry(BYTE entry[32])
//{
//    if ((entry[0x0B] & FILE_ENTRY) != 0) return true;
//    else return false;
//}
//
vector<UINT32> VOLUME_FAT::readListSectorFAT(UINT32 FstClus)
{

    vector<UINT32> listClus;
    if (FstClus <2) return listClus;
    UINT32 d= FstClus;
    BYTE *sec=new BYTE[BytePerSec];
    while (d <= EOC-7 &&d>=2)
    {
       
        listClus.push_back(d);
        UINT32 x= FstClus * 4 / BytePerSec;
        UINT32 y= (FstClus * 4)% BytePerSec;
        DWORD posByte = (FstFAT1 + x) * BytePerSec;
        this->ReadSector(posByte, sec,BytePerSec);
        readFile((char*)&d, sec + y, 4);
        FstClus = d;
    }
    delete[] sec;

    vector<UINT32> listSec;
    size_t  size = listClus.size();
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < SecPerClus; j++)
             listSec.push_back(FstRDET + (listClus[i] -2)* SecPerClus + j);
    }

    return listSec;
}
