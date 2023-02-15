#include "FAT32_BS.h"

void FAT32_BS::Read(BYTE sec[512])
{
    readFile((char*)&bsec, sec, 512);

}

UINT16 FAT32_BS::getFstFAT()
{
    return bsec.RsvdSecCnt;
}

UINT16 FAT32_BS::getFstRDET()
{
    return bsec.RsvdSecCnt+bsec.NumFATs*bsec.FATSz32;
}

UINT16 FAT32_BS::getBytePerSec()
{
    return bsec.BytesPerSec;
}

UINT16 FAT32_BS::getSecPerClus()
{
    return bsec.SecPerClus;
}

UINT16 FAT32_BS::getClusRoot()
{
    return bsec.RootClus;
}
void FAT32_BS::PrintBS()
{
    cout << char(218) << string(40, char(196)) << char(191) << endl;
    cout << char(179) << "             Boot Sector FAT            " << char(179) << endl;
    cout << char(192) << std::string(40, char(196)) << char(217) << endl;
    string b;
    //So sector trong 1 bang fat
    cout << "Ten Volume: " << ByteToStr(bsec.VolLab,11) << endl;

    //Ten HDH
    cout << "Ten OEM: " << bsec.OEMName << endl;

    //Loai FAT:
    cout << "Loai FAT: " << ByteToStr(bsec.FilSysType, 8) << endl;
    //So  byte tren mot sector
    cout << "Byte/Sector: " << bsec.BytesPerSec << endl;
    //So  byte tren mot sector
    cout << "Sec/Clus(SC): " <<(int)bsec.SecPerClus << endl;
    // Số sector/track;
    cout << "Sector/Track: " << (int)bsec.SecPerTrk << endl;

    cout << "So head: " << bsec.NumHeads << endl;

    // Số sector vùng Bootsector
    cout << "Sector  Bootsector (Sb): " << bsec.RsvdSecCnt << endl;
    //So Bang fat
    cout << "So Bang FAT (NF): " <<(int) bsec.NumFATs << endl;
    //Số sector cho bảng RDET
    cout << "Sector RDET (NR): " << bsec.RootEntCnt << " (FAT32 = 0)" << endl;
    //Kich thuoc volume
    cout << "Sector  VOLUME (SV): " << bsec.TotSec32 << DataSizeFomat(bsec.TotSec32 * 512) <<endl;
    //So sector trong 1 bang fat
    cout << "Sector  FAT (SF): " << bsec.FATSz32 << endl;
    //cluster đầu tiên của RDET
    //
    cout << "First Cluster  RDET: " << bsec.RootClus << endl;
    cout  << string(42, '-')  << endl;
}
