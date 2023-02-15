#include "GeneralLib.h"

void readFile(char* buf, BYTE byte[], int size)
{
    stringstream ss;
    for (int i = 0; i < size; i++) {
        ss << byte[i];
    }
    ss.read(buf, size);
}

string ByteToStr(BYTE byte[], int size)
{
    string buf;
    for (int i = 0; i < size; i++)
    {
        if( (int)byte[i]!=0)
            buf += byte[i];
    }
    return buf;
}



//if(mode==true, dec=value(little endian) else big ||defaut=true)
UINT32 HexToDec(string hexSector, bool mode)
{
    long a = 0;
    
    if (mode)
    {
        string LitteIndiaHex;
        int n = hexSector.length();
        for (int i = n - 1; i >= 0; i -= 2)
        {
            LitteIndiaHex.push_back(hexSector[int(i - 1)]);
            LitteIndiaHex.push_back(hexSector[i]);
        }
        a = stoll(LitteIndiaHex, 0, 16);
    }
    else
        a = stoll(hexSector, 0, 16);

    return a;
}

UINT StrToDec(BYTE str[], int size, bool mode)
{
    UINT Dec=0;
    readFile((char*)&Dec, str, size);
    return Dec;
}

string DataSizeFomat(UINT64 size)

{
    double a = 0;
    if (size < 1024)
    {
        return "(" + to_string(size) + "B)";
    }
    else if (size < pow(1024, 2))
    {
        a = round(double(size*100  / 1024))/100 ;
        return "(" + to_string(a).substr(0, 5) + "KB)";
    }
    else if (size < pow(1024, 3))
    {        
        a = round(double(size*100 /  pow(1024,2) ))/100;
        return "(" + to_string(a).substr(0,5) + "MB)";
    }
    else{
        a = round(double(size * 100 / pow(1024,3) ) )/ 100;
        return "(" + to_string(a).substr(0, 5) + "GB)";
    }
  
}

bool IsEquals(wstring str1, wstring str2)
{
    transform(str1.begin(), str1.end(), str1.begin(), ::tolower);
    transform(str2.begin(), str2.end(), str2.begin(), ::tolower);
    //if( str2.find(str1) !=string::npos)return true;
    return str1 == str2;
}



string DecToDateTime(ULONGLONG time)
{
    long long int temp;
    temp = time / 10000000;
    temp = temp - 11644473600LL;
    tm ltm;
    localtime_s(&ltm, &temp);
    string s;
    s = "Date: " + to_string(1900 + ltm.tm_year) + "/" + to_string(1 + ltm.tm_mon);
    s += "/" + to_string(ltm.tm_mday) ;
    s += "\tTime: " + to_string(1 + ltm.tm_hour)+":" +to_string(1 + ltm.tm_min) +":";
    s += to_string(1 + ltm.tm_sec);

    return s;
}

string DectoTime(WORD Dec)
{
    UINT s = 0, m = 0, h = 0;
    bool b;
    for (int i = 0; i < 5; i++)
    {
        b = (Dec >> i) & 1;
        if (b) s = ((1 << i) | s);
    }
    for (int i = 0; i < 6; i++)
    {
        b = (Dec >> i + 5) & 1;
        if (b) m = ((1 << i) | m);
    }
    for (int i = 0; i < 5; i++)
    {
        b = (Dec >> i + 11) & 1;
        if (b) h = ((1 << i) | h);
    }
    return string(to_string(h) + ":" + to_string(m) + ":" + to_string(s * 2));
}

string DectoDate(WORD Dec)
{
    
    UINT d = 0, m = 0, y = 0;
    bool b;
    for (int i = 0; i < 5; i++)
    {
        b = (Dec >> i) & 1;
        if (b) d = ((1 << i) | d);
    }
    for (int i = 0; i < 4; i++)
    {
        b = (Dec >> i + 5) & 1;
        if (b) m = ((1 << i) | m);
    }
    for (int i = 0; i < 7; i++)
    {
        b = (Dec >> i + 9) & 1;
        if (b) y = ((1 << i) | y);
    }
    return string(to_string(y + 1980) + "/" + to_string(m) + "/" + to_string(d));
}

