#pragma once
#include "generalLib.h"
//Mater file table entry index
									//index
#define MTF0							(0)
#define MTF1							(1)
#define ROOT_FILE_NAME_INDEX			(5)
//attribute of 1 entry				//TypeCode
#define $STANDARD_INFORMATION			(0x10)
#define $FILE_NAME						(0x30)
#define $DATA							(0x80)
#define $INDEX_ROOT						(0x90)
#define $INDEX_ALLOCATION				(0xA0)
#define $END							(0xFFFFFFFF)

#define RESIDENT						0x00
#define NON_RESIDENT					0x01
////attribute of 1 file	
#define	ATTR_FILENAME_FLAG_READONLY		0x00000001
#define	ATTR_FILENAME_FLAG_HIDDEN		0x00000002
#define	ATTR_FILENAME_FLAG_SYSTEM		0x00000004
#define	ATTR_FILENAME_FLAG_ARCHIVE		0x00000020
#define	ATTR_FILENAME_FLAG_DEVICE		0x00000040
#define	ATTR_FILENAME_FLAG_NORMAL		0x00000080
#define	ATTR_FILENAME_FLAG_ENTRYECTORY	0x10000000


#define	ATTR_INDEXROOT_FLAG_SMALL	0x00	// Fits in Index Root File Record
#define	ATTR_INDEXROOT_FLAG_LARGE	0x01	// Index Allocation and Bitmap needed
#define	INDEX_ENTRY_FLAG_SUBNODE	0x01	// Index entry points to a sub-node
#define	INDEX_ENTRY_FLAG_LAST		0x02	// Last index entry in the node, no Stream
#define	INDEX_BLOCK_MAGIC		'XDNI'
#define	FILE_RECORD_MAGIC		'ELIF'
#pragma pack(push,1)
//Header of MTF Emtry
struct MFT_ENTRY_HEADER
{
	DWORD       magic; //0x0 – 0x03
	WORD        updateOffset; //0x04 – 0x05
	WORD        updateNumber;//0x06 – 0x07
	LONGLONG    logFile;//0x08 – 0x0F
	WORD        sequenceNumber; //0x10 – 0x11
	WORD        hardLinkCount; //0x12 – 0x13
	WORD        attributeOffset;//0x14 – 0x15 
	WORD        flag;// 0x16 – 0x17
	DWORD       usedSize; //0x18 – 0x1B
	DWORD       allocatedSize;// 0x1C – 0x1F
	LONGLONG    baseRecord; //0x20 – 0x27
	WORD        nextAttributeID;//0x28 – 0x29
	BYTE        unsed[2];//0x2A – 0x2B
	DWORD       MFTRecordIndex;//2c -2f
	WORD		updateSequenceNumber;
	WORD		updateSequenceArray[1];
};

//header of MTF attriibute
struct MFT_HEADER_ATTRIBUTE
{
	DWORD  TypeCode;
	DWORD  RecordLength;
	UCHAR  FormCode;   //is non-resident or not
	UCHAR  NameLength;
	USHORT NameOffset;
	USHORT Flags;
	USHORT AttributeID;
	union
	{
		// for Resident
		struct
		{
			ULONG  Length;
			USHORT Offset;
			UCHAR  ResidentFlags;
			UCHAR  Reserved;
		} Res;

		// for non-resident
		struct
		{
			LONGLONG LowestVcn;
			LONGLONG HighestVcn;
			USHORT   MappingPairsOffset;
			USHORT   CompressionUnit;
			UCHAR    Reserved[4];
			ULONGLONG AllocatedLength;
			ULONGLONG FileSize;
			ULONGLONG ValidDataLength;
			//ULONGLONG TotalAllocated;
		} N_Res; 
	};
};

struct MFT_DATARUN
{
	LONGLONG    offset;
	ULONGLONG   length;
};

struct MFT_ATTR_STANDARD_INFORMATION
{
	ULONGLONG	CreateTime;
	ULONGLONG	AlterTime;
	ULONGLONG	MFTTime;
	ULONGLONG	ReadTime;
	DWORD		Permission;
	DWORD		MaxVersionNo;
	DWORD		VersionNo;
	DWORD		ClassId;
	DWORD		OwnerId;
	DWORD		SecurityId;
	ULONGLONG	QuotaCharged;
	ULONGLONG	USN;
};


struct MFT_ATTR_FILENAME
{
	struct {
		ULONGLONG RecordNum : 48;	//48 bit for index entry
		ULONGLONG SeqNum : 16;		//16bit for sequence number
	} ParentDir;					// File reference to the parent ENTRYectory
	ULONGLONG	CreateTime;			// File creation time
	ULONGLONG	ChangeTime;			// File altered time
	ULONGLONG	MFTTime;			// MFT changed time
	ULONGLONG	AccessTime;			// File read time
	ULONGLONG	AllocatedSize;		// Allocated size of the file
	ULONGLONG	DataSize;			// data size of the file
	ULONG		FileAttributes;		// Flags attribute
	ULONG		ReparseTag;
	UCHAR		NameLength;			// Filename length in characters
	UCHAR		NameType;			// Filename space
	WCHAR		Name[1];			// Filename
};

struct MFT_ATTR_INDEX_ROOT
{
	// Index Root Header
	DWORD		AttrType; // Attribute type (ATTR_TYPE_FILE_NAME: ENTRYectory, 0: Index View)
	DWORD		CollRule;// Collation rule
	DWORD		IBSize;// Collation rule
	BYTE		ClusPerIB;// Collation rule
	BYTE		Padding1[3];

	// Index Node Header
	DWORD		EntryOffset;// Offset to the first index entry, relative to this address(0x10)
	DWORD		TotalEntrySize;// Total size of the index entries
	DWORD		AllocEntrySize;// Allocated size of the index entries
	BYTE		Flags;
	BYTE		Padding2[3];
};

struct MFT_INDEX_ENTRY
{
	union
	{
		ULONGLONG FileReference;
		struct
		{
			USHORT DataOffset;
			USHORT DataLength;
			ULONG32 ReservedForZero;
		};
	};
	USHORT Length;
	USHORT AttributeLength;
	USHORT Flags;
	USHORT Reserved;
	union {
		union {
			struct {
				ULONGLONG vcn;
			} asNode;
			struct {
				ULONG32 ReparseTag;
				ULONGLONG FileReference;
			} asKeys;
		} reparse;
		MFT_ATTR_FILENAME FileName;
	};
};


struct INDEX_BLOCK
{
	// Index Block Header
	DWORD		Magic;			// "INDX"
	WORD		OffsetOfUS;		// Offset of Update Sequence
	WORD		SizeOfUS;		// Size in words of Update Sequence Number & Array
	ULONGLONG	LSN;			// $LogFile Sequence Number
	ULONGLONG	VCN;			// VCN of this index block in the index allocation
	// Index Header
	DWORD		EntryOffset;	// Offset of the index entries, relative to this address(0x18)
	DWORD		TotalEntrySize;	// Total size of the index entries
	DWORD		AllocEntrySize;	// Allocated size of index entries
	BYTE		NotLeaf;		// 1 if not leaf node (has children)
	BYTE		Padding[3];		// Padding
};










