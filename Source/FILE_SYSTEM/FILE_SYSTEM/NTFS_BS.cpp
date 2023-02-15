#include "NTFS_BS.h"

void NTFS_BS::Read(BYTE sec[512])
{
    /*stringstream ss = ByteToFile(sec, 512);
    ss.read((char*)&bsec, 512);*/
    readFile((char*)&bsec, sec, sizeof(FAT32_BS));
}


void NTFS_BS::PrintBS()
{

    cout << char(218) << string(40, char(196)) << char(191) << endl;
    cout << char(179) << "            Boot Sector NTFS            " << char(179) << endl;
    cout << char(192) << std::string(40, char(196)) << char(217) << endl;
    string b;
    
    cout << "Ten OEM: " << bsec.oemID << endl;
    //So  byte tren mot sector
    cout << "Byte/Sector: " << bsec.bytePerSec << endl;
    //So  byte tren mot sector
    cout << "Sector/Cluster: " << (int)bsec.secPerClus << endl;
    // Số sector vùng Bootsector
    cout << "Sector/Track: " << (int)bsec.secPerTrk << endl;

    cout << "So luong Head: " << (int)bsec.headNum << endl;

    //Kich thuoc volume
    cout << "Tong Sector Volume: " << bsec.totSec << DataSizeFomat(bsec.totSec*512) << endl;

    cout << "First Cluster MTF: " << bsec.MFTClus << endl;
    cout << "First Cluster MFTMirror : " << bsec.MFTMirrClus << endl;
    cout << "Kich thuoc MTF Entry: " << this->getClusPerRecord() <<" byte"<< endl;
    cout << "So byte cua Index Block:" << bsec.clusPerBlock * bsec.secPerClus * bsec.bytePerSec << endl;
    cout  << std::string(42, char(196)) << endl;
}
