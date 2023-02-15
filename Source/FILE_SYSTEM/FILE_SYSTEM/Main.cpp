#include "VolumeNTFS.h"
#include "VolumeFAT.h"
#include <fcntl.h> //_O_WTEXT
void Menu(int& choice, wchar_t name);
void NhapVolume(VOLUME*& volume, wchar_t& name);
//<Main>
int main()
{
    VOLUME *volume=nullptr;
    wchar_t DriveName;
    wstring FileName;

    NhapVolume(volume, DriveName);
    int choice = 0;
    do
    {
        system("cls");
        Menu(choice, DriveName);
        switch (choice)
        {
        case 1:
            NhapVolume(volume, DriveName);
            cout << "Thay doi thanh cong!" << endl;
            break;
        case 2:
            volume->PrintBootSecter();
            break;
        case 3:
            volume->PrintDirTree();
            break;
        case 4:
            volume->PrintDirTreeDetail();
            break;
        case 5:
            wcin.ignore();
            cout << " Nhap Ten Entry:";
            getline(wcin,FileName);
            volume->PrintDataEntry(FileName);
            break;
        }
        system("pause");


    } while (choice);

    delete volume;
    return 0;
}




void NhapVolume(VOLUME*& volume, wchar_t& name)
{
    if (volume)
    {
        delete volume;
        volume = nullptr;
    }
    WCHAR  nameDrive[] = L"\\\\.\\E:";

    while (true)
    {

        system("cls");
        cout << "Nhap Ten O Dia(Ex: D,E,F...0:Exit):";
        wcin >> name;
        if (name == '0')
        {
            if (volume) delete volume;
            exit(0);
        }
        nameDrive[4] = name;
        volume = new VOLUME_FAT;
        int status = volume->Init(nameDrive);

        if (status == -1)
        {
            cout << "Khong tim thay o dia, hoac truy cap bi chan." << endl;
            cout<<"Moi nhap lai sau 2s!" << endl;
            Sleep(2500);
            delete volume;
            volume = nullptr;
            continue;
        }
        else if (status == 0)
        {
            delete volume;
            volume = new VOLUME_NTFS;
            volume->Init(nameDrive);
        }
        break;
    }

}
void Menu(int& choice, wchar_t name)
{
    do {
        cout << char(218) << string(40, char(196)) << char(191) << endl;
        cout << char(179) << "        CHUONG TRINH DOC THU MUC        " << char(179) << endl;
        cout << char(192) << std::string(40, char(196)) << char(217) << endl;

        cout << char(218) << string(40, char(196)) << char(191) << endl;
        cout << "\tNhap:" << endl;
        cout << "\t1. Thay Doi Volume (" << (char)name << ":/)." << endl;
        cout << "\t2. In Boot Sector Volume. " << endl;
        cout << "\t3. In Cay Thu Muc." << endl;
        cout << "\t4. In cay Thu Muc Chi Tiet." << endl;
        cout << "\t5. Tim File hoac Folder." << endl;
        cout << "\t0. Thoat." << endl;
        cout << char(192) << std::string(40, char(196)) << char(217) << endl;
        cout << "--Lua chon cua ban:";

        cin >> choice;
    } while (choice < 0 || choice >5);
    cout << string(40, '=') << endl;

}