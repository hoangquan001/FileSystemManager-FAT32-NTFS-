Cấu trúc source code:

+FILE_SYSTEM.sln : phần mở rộng của visual studio, để mở code và biên dịch chương trình

+Release/FILE_SYSTEM.exe: Chương trình đọc cây  thư mục và tập tin đã được biên dịch (có thể ko chạy được tùy máy:D)

+FILE_SYSTEM:
	FILE header (.h) thuộc FAT32:
		+DirEntry.h:Mô tả cấu trúc Entry  và đóc cây thư mục
		+FAT32_BS: Mô tả cấu trúc của boot sector và đọc BS
		+VolumeFAT.h: kết hợp với 2 file trên để mở volume và đọc nội dung trên volume
	FILE header thuộc NTFS:
		+EntryType.h: Mô tả cấu trúc của hệ thống tập tin NTFS vd index block, header,...
		+MFT_Record.h: Tổng hợp các cấu trúc trên để đọc dữ liệu cho MFT entry
		+NTFS_BS.h: Mô tả cấu trúc của patiction boot sector và đọc BS
		++VolumeNTFS.h: Kết hợp các file trên để mở và đọc nội dung của volume 
	
	generalLib.h: Tổng hợp các thư viện dùng chung cho FAT và NTFS
	main.cpp: Chạy chương trình