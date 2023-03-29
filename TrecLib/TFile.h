#pragma once
#include "TObject.h"
#include "TObjectNode.h"
#include "FileFunctions.hpp"

#ifdef _WINDOWS
#define TC_FILE_SEP L'\\'
#elif defined(__linux__) || (defined (__APPLE__) && defined (__MACH__))
#define TC_FILE_SEP L'/'
#endif

/*
* Enum Class: CentralDirectories
* Represents possible directory types AnaGame and its taps might be interested in
*/
using CentralDirectories = enum class CentralDirectories
{
	cd_Executable,			// 0
	cd_AppData,				// 1
	cd_Desktop,				// 2
	cd_Documents,			// 3
	cd_Music,				// 4
	cd_Pictures,			// 5
	cd_Videos,				// 6
	cd_Downloads,			// 7
	cd_public,				// 8
	cd_public_Desktop,		// 9
	cd_public_Documents,	// 10
	cd_public_Downloads,	// 11
	cd_public_Music,		// 12
	cd_public_pictures,		// 13
	cd_public_videos,		// 14
	cd_User					// 15
};

using FileEncodingType = enum class FileEncodingType
{
	fet_unknown,          // We do not know the encoding of this file
	fet_acsii,            // The encoding is acsii
	//fet_acsii16,          // The encoding is acsii-16
	fet_unicode,          // The encoding is Unicode
	fet_unicode_little,   // The encoding is Unicode - little endian
	fet_unicode8,         // The encoding is Unicode-8
	fet_unicode32,        // The encoding is Unicode-32
	fet_unicode32_little, // The encoding is Unicode-32 - little endian
	fet_unicode7          // The Encoding is UTF-7
};

void _TREC_LIB_DLL ForgeDirectory(const TString& dir);

TString _TREC_LIB_DLL GetDirectoryWithSlash(CentralDirectories cd);

TString _TREC_LIB_DLL GetShadowDirectoryWithSlash(CentralDirectories cd);


class _TREC_LIB_DLL TFile :
    public TObject
{
public:
	class _TREC_LIB_DLL TFileShell : public TObject
	{
		friend class TrecPointerKey;
	public:

		/*
		* Method: TFileShell::IsDirectory
		* Purpose: Reports whether this file represents a directory
		* Parameters: void
		* Returns: bool - false if object is the base class, true, if object is a TDirectory
		*
		* Attributes: virtual - overriden by the TDirectory
		*/
		virtual bool IsDirectory();

		/*
		* Method: TFileShell::IsValid
		* Purpose: Reports whether the object represents an existing file
		* Parameters: void
		* Returns: bool - true if the file exists and have not yet been deleted, false otherwise
		*/
		bool IsValid();

		/*
		* Method: TFileShell::GetPath
		* Purpose: Reports the path that leads to the file
		* Parameters: void
		* Returns: TString - the path of the file
		*
		* Note: if the path is an empty string, you can assume that the TFileShell is invalid
		*/
		TString GetPath();


		/*
		* Method: TFileShell::GetName
		* Purpose: Reports the name of the file minus the path
		* Parameters: void
		* Returns: TString - the name of the file
		*/
		TString GetName();


		/*
		* Method: static TFileShell::GetFileInfo
		* Purpose:Returns an Object representing the file of the specified path
		* Parameters: TString& path - the path of the file
		* Returns: TrecPointer<TFileShell> - the file information regarding the path
		*
		* Note: If the file does not exist, the pointer will be null. If the file is a directory,
		*	the pointer will contain a TDirectory (which extends the TFileShell)
		*/
		TrecPointer<TFileShell> static GetFileInfo(const TString& path);

		/**
		 * Method: TFileShell::GetParent
		 * Purpose: Enables the File Object to return a representation of it's parent folder
		 * Parameters: void
		 * Returns: TrecPointer<TFileShell> - the directory of the current file (if null, then likely this file is as high as it can be)
		 */
		TrecPointer<TFileShell> GetParent();


		/*
		* Method: TFileShell::GetCreationDate
		* Purpose: Reports the time that the file was created
		* Parameters: void
		* Returns: FILETIME - the structure representing the file creation time
		*/
		FILETIME GetCreationDate();

		/*
		* Method: TFileShell::GetLastAccessed
		* Purpose: Reports the time that the file was last accessed
		* Parameters: void
		* Returns: FILETIME - the structure representing the last access time
		*/
		FILETIME GetLastAccessed();

		/*
		* Method: TFileShell::GetLastWritten
		* Purpose: Reports the time that the file was last updated
		* Parameters: void
		* Returns: FILETIME - the structure representing the last update time
		*/
		FILETIME GetLastWritten();


		/*
		* Method: TFileShell::GetSize
		* Purpose: Reports the current size of the file
		* Parameters: void
		* Returns: ULONG64 - the size of the file
		*/
		ULONG64 GetSize();

		/**
		 * Method: TFileShell::GetRelativePath
		 * Purpose: Retrieves the Relative file path for the Provided directory
		 * Parameters: TString& relativePath - the relative path provided if successful
		 *				TrecPointer<TFileShell> directory - the directory to check (must be valid and a TDirectory)
		 *				bool allowOutside - if true, than this file can be outside of the provided directory
		 * Returns: bool - true if the method worked, false otherwise
		 */
		bool GetRelativePath(TString& relativePath, TrecPointer<TFileShell> directory, bool allowOutside);

		/**
		 * Method: TFileShell::GetDirectoryName
		 * Purpose: Retrieves the Name fo the directory containing this file path ("C:\\Users\\John\\Desktop\\file.txt" returns "Desktop")
		 * Parameters: void
		 * Returns: TString - the name of the containing Directory
		 */
		TString GetDirectoryName();

		bool IsArchive();

		bool IsEncrypted();

		bool IsHidden();

		bool IsReadOnly();

	protected:
		TFileShell(const TString& path);

		void Refresh();

		/**
		 * whether the relevant file has been deleted since the object was created
		 */
		bool deleted;

		/**
		 * the Path of the target file
		 */
		TString path;

		/**
		 * information about the file
		 */
		FILE_ATTS fileInfo;
	};

#ifdef _WINDOWS

	static const UINT t_file_read = GENERIC_READ;
	static const UINT t_file_write = GENERIC_WRITE;
	static const UINT t_file_share_delete = 0x00000400;
	static const UINT t_file_share_read = 0x00000100;
	static const UINT t_file_share_write = 0x00000200;
	static const UINT t_file_create_always = (CREATE_ALWAYS << 16);
	static const UINT t_file_create_new = (CREATE_NEW << 16);
	static const UINT t_file_open_always = (OPEN_ALWAYS << 16);
	static const UINT t_file_open_existing = (OPEN_EXISTING << 16);
	static const UINT t_file_truncate_existing = TRUNCATE_EXISTING << 16;
	static const UCHAR t_file_include_terminator = 0b00000001;

#elif defined(__linux__) || (defined (__APPLE__) && defined (__MACH__))

	static const UINT t_file_read = 0x0004;
	static const UINT t_file_write = O_WRONLY;
	static const UINT t_file_share_delete = 0x00004000;
	static const UINT t_file_share_read = 0x00001000;
	static const UINT t_file_share_write = 0x00002000;
	static const UINT t_file_create_always = O_CREAT | O_TRUNC;
	static const UINT t_file_create_new = O_CREAT | O_EXCL;
	static const UINT t_file_open_always = O_CREAT;
	static const UINT t_file_open_existing = 0;
	static const UINT t_file_truncate_existing = O_TRUNC;
	static const UCHAR t_file_include_terminator = 0b00000001;

#endif

	TFile();

	TFile(TrecPointer<TFileShell> file,
		UINT nOpenFlags);

	TFile(TrecPointer<TFileShell> file, const TString& name,
		UINT nOpenFlags);

	~TFile();

	bool Open(TrecPointer<TFileShell> file,
		UINT nOpenFlags);

	bool Open(TrecPointer<TFileShell> file, const TString& name,
		UINT nOpenFlags);


	// Methods Imported from CArchive

	BOOL ReadString(TString& rString);
	/**
	 * Method: TFile::ReadString
	 * Purpose: Reads a line in a file into a String, taking into account the file encoding,
	 *			Or when the specificed number of characters are read
	 * Parameters: TString& rString - the String to read into
	 *				ULONGLONG nMax - max number of characters to read
	 * Returns: bool - success of reading
	 */
	ULONGLONG ReadString(TString& rString, ULONGLONG nMax);


	/**
	 * Method: TFile::ReadStringLine
	 * Purpose: Reads a line in a file into a String, taking into account the file encoding, stopping at the next line
	 *			Or when the specificed number of characters are read
	 * Parameters: TString& rString - the String to read into
	 *				ULONGLONG nMax - max number of characters to read
	 * Returns: bool - success of reading
	 */
	ULONGLONG ReadStringLine(TString& rString, ULONGLONG nMax);

	UINT ReadString(TString& rString, WCHAR chara);

	/**
	 * Method: TFile::ReadString
	 * Purpose: Reads the file up to one of the provided characters
	 * Parameters: TString& rString - the string to retun
	 *				TString& chars - the characters to stop at
	 *				UCHAR flags - flags influence the behavior of this method
	 *				UINT max - max number of bytes to read (0 for no maximum)
	 * Returns: UINT - the size of the resulting string
	 *
	 * Note: Written with Source code interpretation in mind
	 * Flags variable values:
	 *		0b00000001 - TFile::include_end - include the terminating character in the return String
	 *      0b00000010 - TFile::out_of_quotes - makesure that when we do find the characters, they are outside of quotes
	 *      0b00000100 - TFile::watch_backslash - factor backslashes in handling the other flags
	 */
	UINT ReadString(TString& rString, const TString& chars, UCHAR flags, UINT max = 0);

	void WriteString(const TString& lpsz);

	bool IsOpen()const;
	/*
	* Method: TFile::SetEncoding
	* Purpose: Sets the encoding of the File (if not already set)
	* Parameters: FileEncodingType fet - the Encoding type to do
	* Returns: bool - whether the file encoding was set
	*/
	bool SetEncoding(FileEncodingType fet);

	TString GetFileDirectory();

	TString GetFileExtension();

	void Close();

	void Flush();

	TString GetFileName()const;

	TString GetFilePath()const;
	/**
	 * Method: TFile::GetFileTitle
	 * Purpose: Retirevs the "title" of the file
	 * Parameters: void
	 * Returns: TString - the title of the file
	 *
	 * Attributes: const
	 */
	TString GetFileTitle()const;

	ULONGLONG GetLength()const;

	ULONGLONG GetPosition()const;

	/**
	 * Method: TFile::Read
	 * Purpose: Reads a given number of bytes
	 * Parameters: void* buffer - pointer to the buffer
	 *				UINT count - number of bytes to read
	 * Returns: UINT - number of bytes read
	 */
	UINT Read(void* buffer, UINT count);




	/**
	 * Method: TFile::Seek
	 * Purpose: Sets the file's seek point according to the users specification
	 * Parameters: LONGLONG offset - the offset for the seek point
	 *				 UINT from - ofset from where? (0 for beginning, 1 for current point, 2 for end)
	 * Returns: ULONGLONG -  the position of the file's seek point
	 */
	ULONGLONG Seek(LONGLONG offset, UINT from);
	/**
	 * Method: TFile::SeekToBegin
	 * Purpose: Sets the files seek point to the beginning
	 * Parameters: void
	 * Returns: void
	 */
	void SeekToBegin();
	/**
	 * Method: TFile::SeekToEnd
	 * Purpose: Sets the seek to the end of the file
	 * Parameters: void
	 * Returns: ULONGLONG - the current seek of the file
	 */
	ULONGLONG SeekToEnd();
	/**
	 * Method: TFile::Write
	 * Purpose: Writes a set of bytes to the file
	 * Parameters: const void* buffer - pointer to bytes
	 *				UINT count - the number of bytes to write
	 * Returns: void
	 */
	void Write(const void* buffer, UINT count);

	/**
	 * Method: TFile::GetEncodingType
	 * Purpose: Retriev the file encoding mode
	 * Parameters: void
	 * Returns: FileEncodingType - the type of encoding the file is set to use
	 */
	FileEncodingType GetEncodingType() const;

protected:
	/*
	* Method: TFile::DeduceEncodingType
	* Purpose: Attempts to determine the encoding type of the file if possible
	* Parameters: void
	* Returns: FileEncodingType - the File Encoding type deduced
	*/
	FileEncodingType DeduceEncodingType();

	/**
	 * Method: TFile::ConvertFlags
	 * Purpose: Used internally by the class to convert TFile:: flags into Windows Crreate flags
	 * Parameters: UINT& input - the flags to convert
	 *				 UINT& open - basic open flags to send to Windows
	 *				 UINT& security - share attributes to send to windows
	 *				 UINT& creation - create instruction flags for Windows
	 * Returns: void
	 */
	void ConvertFlags(UINT& input, UINT& open, UINT& security, UINT& creation);

private:
	TString filePath;
	FileEncodingType fileEncode;
	FILE_HANDLE fileHandle;
	ULONGLONG position;


	UCHAR ReadUnicode8Char(char* seq4);
};

using TFileShell = TFile::TFileShell;


TString _TREC_LIB_DLL GetShadowFilePath(TrecPointer<TFile::TFileShell> f);

class _TREC_LIB_DLL TDirectory :
	public TFile::TFileShell
{
	friend class TrecPointerKey;
protected:
	TDirectory(const TString& path);
public:


	bool IsDirectory()override;

	TDataArray<TrecPointer<TFile::TFileShell>> GetFileListing();

	void GetFileListing(TDataArray<TrecPointer<TFile::TFileShell>>& files);
};

using file_node_filter_mode = enum class file_node_filter_mode
{
	fnfm_blank,                   // no directory filtering
	fnfm_block_upper,             // block the upper directory
	fnfm_block_current,           // block the current directory
	fnfm_block_both,              // block both the upper and current directory
	fnfm_block_upper_and_files,   // Only show regular and current directories
	fnfm_block_current_and_files, // Only show regular and upper directories
	fnfm_block_both_and_files,    // Only show regular directories
	fnfm_block_directories,       // Only show files
	fnfm_block_files              // Show all Directories and no files
};

/*
 * Class: TFileNode
 * Purpose: Implements the TObjectNode interface for Files found on the local hard drive
 *
 * SuperClass: TObjectNode - Allows TBlankNode to be used by the TTreeDataBind control
 */
class _TREC_LIB_DLL TFileNode :
	public TObjectNode
{
public:

	explicit TFileNode(UINT l);

	TString GetContent() override;

	TString GetCommand(const TString& info)override;

	bool IsExtendable() override;

	bool IsExtended()override;

	TrecPointer<TObjectNode> GetNodeAt(UINT target, UINT current) override;

	UINT TotalChildren() override;

	bool Initialize() override;

	bool Initialize(TString& value) override;

	void Extend() override;

	TrecPointer<TObjectNode> GetChildNodes(UINT index) override;

	void DropChildNodes() override;

	void SetFile(const TrecPointer<TFileShell>& d);

	TrecPointer<TFileShell> GetData();

	void SetShowAllFiles(bool show);

	bool GetShowAllFiles();

	bool AddExtension(const TString& extension);

	void SetFilterMode(file_node_filter_mode mode);

	file_node_filter_mode GetFilterMode();

	bool RemoveNode(TrecPointer<TObjectNode> obj) override;

protected:

	bool ShouldShow(TrecPointer<TFileShell> node);

	/**
	 * the data held by this node
	 */
	TrecPointer<TFileShell> data;

	/**
	 * list of files held by this node (if object is a TDirectory
	 */
	TDataArray<TrecPointer<TObjectNode>> files;

	/**
	 * The Directory filtering mode
	 */
	file_node_filter_mode filter_mode;

	/**
	 * Whether to show all files or only the target ones
	 */
	bool showAllFiles;

	/**
	 * Extension Filter, only report these files
	 */
	TDataArray<TString> extensions;
};