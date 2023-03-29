#include "TFile.h"

/**
 * Marker to tell if the directory entries have been initialized or not
 */
static bool initialized = false;

/**
 * Collection of default directories
 */
static TString directories[16];

/**
 * Collection of shadow directories (which Anagame can use to hold temporary data
 */
static TString shadowDirectories[16];


#ifdef _WINDOWS
#include <ShlObj.h>

WCHAR slasher = L'\\';

#define CREATE_TIME ftCreationTime
#define LAST_ACCESS ftLastAccessTime
#define LAST_WRITE ftLastWriteTime

#define IS_ARCHIVE fileInfo.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE
#define IS_ENCRYPTED fileInfo.dwFileAttributes & FILE_ATTRIBUTE_ENCRYPTED

void InitializeDirectories()
{
	WCHAR filepath[300];

	GetModuleFileNameW(nullptr, filepath, 299);
	UINT c = wcslen(filepath) - 1;
	for (; filepath[c] != L'\\' && filepath[c] != L'/'; c--);
	filepath[c] = L'\0';

	TString tempString;

	tempString.Set(filepath);
	directories[0].Set(tempString);

	PWSTR folderString = nullptr;
	SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &folderString);
	tempString.Set(folderString);
	directories[1].Set(tempString);

	SHGetKnownFolderPath(FOLDERID_Desktop, 0, NULL, &folderString);
	tempString.Set(folderString);
	directories[2].Set(tempString);

	SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &folderString);
	tempString.Set(folderString);
	directories[3].Set(tempString);

	SHGetKnownFolderPath(FOLDERID_Music, 0, NULL, &folderString);
	tempString.Set(folderString);
	directories[4].Set(tempString);

	SHGetKnownFolderPath(FOLDERID_Pictures, 0, NULL, &folderString);
	tempString.Set(folderString);
	directories[5].Set(tempString);

	SHGetKnownFolderPath(FOLDERID_Videos, 0, NULL, &folderString);
	tempString.Set(folderString);
	directories[6].Set(tempString);

	SHGetKnownFolderPath(FOLDERID_Downloads, 0, NULL, &folderString);
	tempString.Set(folderString);
	directories[7].Set(tempString);


	SHGetKnownFolderPath(FOLDERID_Public, 0, NULL, &folderString);
	tempString.Set(folderString);
	directories[8].Set(tempString);

	SHGetKnownFolderPath(FOLDERID_PublicDesktop, 0, NULL, &folderString);
	tempString.Set(folderString);
	directories[9].Set(tempString);

	SHGetKnownFolderPath(FOLDERID_PublicDocuments, 0, NULL, &folderString);
	tempString.Set(folderString);
	directories[10].Set(tempString);

	SHGetKnownFolderPath(FOLDERID_PublicDownloads, 0, NULL, &folderString);
	tempString.Set(folderString);
	directories[11].Set(tempString);

	SHGetKnownFolderPath(FOLDERID_PublicMusic, 0, NULL, &folderString);
	tempString.Set(folderString);
	directories[12].Set(tempString);

	SHGetKnownFolderPath(FOLDERID_PublicPictures, 0, NULL, &folderString);
	tempString.Set(folderString);
	directories[13].Set(tempString);

	SHGetKnownFolderPath(FOLDERID_PublicVideos, 0, NULL, &folderString);
	tempString.Set(folderString);
	directories[14].Set(tempString);

	directories[15].Set(directories[7]);
	directories[15].Replace(TString(L"\\Downloads"), TString(L""));

	initialized = true;

	TString baseShadow = GetDirectoryWithSlash(CentralDirectories::cd_AppData) + TString(L"AnaGame\\ShadowFiles");
	ForgeDirectory(baseShadow);

	initialized = false;

	shadowDirectories[0].Set(baseShadow + TString(L"\\AnaGameExe"));
	shadowDirectories[1].Set(baseShadow + TString(L"\\AppData"));
	shadowDirectories[2].Set(baseShadow + TString(L"\\Desktop"));
	shadowDirectories[3].Set(baseShadow + TString(L"\\Documents"));
	shadowDirectories[4].Set(baseShadow + TString(L"\\Music"));
	shadowDirectories[5].Set(baseShadow + TString(L"\\Pictures"));
	shadowDirectories[6].Set(baseShadow + TString(L"\\Videos"));
	shadowDirectories[7].Set(baseShadow + TString(L"\\Downloads"));
	// New Public Shadow Directories
	shadowDirectories[8].Set(baseShadow + TString(L"\\Public"));
	shadowDirectories[9].Set(baseShadow + TString(L"\\Public\\Desktop"));
	shadowDirectories[10].Set(baseShadow + TString(L"\\Public\\Documents"));
	shadowDirectories[11].Set(baseShadow + TString(L"\\Public\\Downloads"));
	shadowDirectories[12].Set(baseShadow + TString(L"\\Public\\Music"));
	shadowDirectories[13].Set(baseShadow + TString(L"\\Public\\Pictures"));
	shadowDirectories[14].Set(baseShadow + TString(L"\\Public\\Videos"));

	// User
	shadowDirectories[15].Set(baseShadow);

	for (UINT C = 0; C < 9; C++)
	{
		CreateDirectoryW(shadowDirectories[C].GetConstantBuffer().getBuffer(), 0);
	}

	initialized = true;
}


#elif defined(__linux__) || (defined (__APPLE__) && defined (__MACH__))
#include <cstring>
WCHAR slasher = L'/';

#ifdef __linix__

#define CREATE_TIME st_ctim
#define LAST_ACCESS st_atim
#define LAST_WRITE st_mtim

#elif (defined (__APPLE__) && defined (__MACH__))

#define CREATE_TIME st_birthtimespec
#define LAST_ACCESS st_atimespec
#define LAST_WRITE st_mtimespec

#endif

#define IS_ARCHIVE false
#define IS_ENCRYPTED false

TString PrepKnownDirectory(CentralDirectories cd, TString& home)
{
    if (!home.EndsWith(L'/'))
        home.AppendChar(L'/');

    switch (cd)
    {
    case CentralDirectories::cd_AppData:               // 1
        return home + L".Anagame";
    case CentralDirectories::cd_Desktop:               // 2
    case CentralDirectories::cd_public_Desktop:        // 9
        return home + L"Desktop";
    case CentralDirectories::cd_Documents:           // 3
    case CentralDirectories::cd_public_Documents:    // 10
        return home + L"Documents";
    case CentralDirectories::cd_Music:                // 4
    case CentralDirectories::cd_public_Music:        // 12
        return home + L"Music";
    case CentralDirectories::cd_Pictures:            // 5
    case CentralDirectories::cd_public_pictures:       // 13
        return home + L"Pictures";
    case CentralDirectories::cd_Videos:                // 6
    case CentralDirectories::cd_public_videos:        // 14
        return home + L"Videos";
    case CentralDirectories::cd_Downloads:            // 7
    case CentralDirectories::cd_public_Downloads:    // 11
        return L"Downloads";
    case CentralDirectories::cd_public:
    case CentralDirectories::cd_User:                   // 15
        return home;
    }
    return L"";
}

/*
* InitializeDirectories
* Purpose: Sets up the Directory list for the Get Directory functions to use
* Parameters: void
* Returns: void
*/
void InitializeDirectories()
{
    uint32_t pathSize = 300;
    char filepath[300] = { 0 };

#if __linux__
    readlink("/proc/self/exe", filepath, 299);
#elif __APPLE__
    _NSGetExecutablePath(filepath, &pathSize);
#endif


    TString tempString;

    tempString.Set(filepath);
    directories[0].Set(tempString);

    TString home(std::getenv("HOME"));

    directories[static_cast<UINT>(CentralDirectories::cd_User)].Set(home);

    // First set up the basic
    for (UINT Rust = 1; Rust < 8; Rust++)
    {
        TString direct(PrepKnownDirectory(static_cast<CentralDirectories>(Rust), home));
        ForgeDirectory(direct);
        directories[Rust].Set(direct);
    }

    // Set up the Public Directories
    TrecPointer<TFileShell> homeShell = TFile::TFileShell::GetFileInfo(home);
    home.Set(homeShell->GetParent()->GetPath());
    if (home.EndsWith(L'/'))
        home.AppendChar(L'/');
    home.Append(L"Public/");
    for (UINT Rust = 8; Rust < 15; Rust++)
    {
        TString direct(PrepKnownDirectory(static_cast<CentralDirectories>(Rust), home));
        ForgeDirectory(direct);
        directories[Rust].Set(direct);
    }


    initialized = true;

    TString baseShadow = GetDirectoryWithSlash(CentralDirectories::cd_AppData) + TString(L"ShadowFiles");
    ForgeDirectory(baseShadow);

    initialized = false;

    shadowDirectories[0].Set(baseShadow + TString(L"\\AnaGameExe"));
    shadowDirectories[1].Set(baseShadow + TString(L"\\AppData"));
    shadowDirectories[2].Set(baseShadow + TString(L"\\Desktop"));
    shadowDirectories[3].Set(baseShadow + TString(L"\\Documents"));
    shadowDirectories[4].Set(baseShadow + TString(L"\\Music"));
    shadowDirectories[5].Set(baseShadow + TString(L"\\Pictures"));
    shadowDirectories[6].Set(baseShadow + TString(L"\\Videos"));
    shadowDirectories[7].Set(baseShadow + TString(L"\\Downloads"));
    // New Public Shadow Directories
    shadowDirectories[8].Set(baseShadow + TString(L"\\Public"));
    shadowDirectories[9].Set(baseShadow + TString(L"\\Public\\Desktop"));
    shadowDirectories[10].Set(baseShadow + TString(L"\\Public\\Documents"));
    shadowDirectories[11].Set(baseShadow + TString(L"\\Public\\Downloads"));
    shadowDirectories[12].Set(baseShadow + TString(L"\\Public\\Music"));
    shadowDirectories[13].Set(baseShadow + TString(L"\\Public\\Pictures"));
    shadowDirectories[14].Set(baseShadow + TString(L"\\Public\\Videos"));

    // User
    shadowDirectories[15].Set(baseShadow);

    for (UINT C = 0; C < 9; C++)
    {
        mkdir(shadowDirectories[C].GetRegString().c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }

    initialized = true;
}

#endif

void ForgeDirectory(const TString& dir)
{
	auto pieces = dir.split(TString(L"/\\"));
	TString bDir;
	for (UINT rust = 0; rust < pieces->Size(); rust++)
	{
		TString adder = pieces->at(rust) + L'\\';
		bDir.Append(adder);

		TcMakeDirectory(bDir);
	}
}


TString GetDirectory(CentralDirectories cd)
{
	if (!initialized)
		InitializeDirectories();
	return directories[static_cast<UINT>(cd)];
}

TString GetDirectoryWithSlash(CentralDirectories cd)
{
	if (!initialized)
		InitializeDirectories();

	return directories[static_cast<UINT>(cd)] + slasher;
}

TString GetShadowDirectoryWithSlash(CentralDirectories cd)
{
	if (!initialized)
		InitializeDirectories();
	return  shadowDirectories[static_cast<UINT>(cd)] + slasher;
}

TString GetShadowFilePath(const TFile& f)
{
	if (!f.IsOpen())
		return TString();

	UINT dirNum = 0;
	TString returnable = f.GetFilePath();
	while (dirNum < 9)
	{
		if (returnable.Find(directories[dirNum]) != -1)
		{
			returnable.Replace(directories[dirNum], TString(L""));
			break;
		}
		dirNum++;
	}

	if (dirNum > 8)
		return TString();

	return shadowDirectories[dirNum] + TString(L"\\") + returnable;
}

TString _TREC_LIB_DLL GetShadowFilePath(TrecPointer<TFile::TFileShell> f)
{
	if (!f.Get())
		return TString();

	UINT dirNum = 0;
	TString returnable = f->GetPath();
	while (dirNum < 9)
	{
		if (returnable.Find(directories[dirNum]) != -1)
		{
			returnable.Replace(directories[dirNum], TString(L""));
			break;
		}
		dirNum++;
	}

	if (dirNum > 8)
	{
		TString directory(f->GetParent()->GetPath());
		directory.AppendChar(L'\\');
		directory.Replace(L"\\\\", L"\\");
		directory.Append(L"ShadowDir\\");
		ForgeDirectory(directory);
		return directory + f->GetName();
	}
	return shadowDirectories[dirNum] + TString(L"\\") + returnable;
}

///
/// TFileShell
///



bool TFile::TFileShell::IsDirectory()
{
	return false;
}

bool TFile::TFileShell::IsValid()
{
	TObjectLocker lock(&thread);
		bool ret = path.GetSize() && !deleted;
	return ret;
}

TString TFile::TFileShell::GetPath()
{
	TObjectLocker lock(&thread);
		TString ret(path);
	return ret;
}

TString TFile::TFileShell::GetName()
{
	TObjectLocker lock(&thread);
		int slashLoc = path.FindLastOneOf(TString(L"/\\"));
	TString ret((slashLoc == -1 ? path : path.SubString(slashLoc + 1)));
	return ret;
}

#ifdef _WINDOWS
TrecPointer<TFile::TFileShell> TFile::TFileShell::GetFileInfo(const TString& path)
{
	TString newPath(path);

	newPath.Replace(L'/', L'\\');
	while (newPath.Replace(L"\\\\", L"\\"));

	while ((newPath.StartsWith(L"\"") && newPath.EndsWith(L"\""))
		|| (newPath.StartsWith(L"\'") && newPath.EndsWith(L"\'")))
		newPath.Set(newPath.SubString(1, newPath.GetSize() - 1));

	// Make sure path is canonical
	WCHAR newPathBuff[300];
	for (UINT Rust = 0; Rust < 300; Rust++)
	{
		newPathBuff[Rust] = L'\0';
	}

	if (FAILED(PathCchCanonicalizeEx(newPathBuff, 300, newPath.GetConstantBuffer().getBuffer(), 1)))
		return TrecPointer<TFileShell>();

	newPath.Set(newPathBuff);

	DWORD ftyp = GetFileAttributesW(newPath.GetConstantBuffer().getBuffer());

	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return TrecPointer<TFileShell>(); // if Invalid, simply return a Null


	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return TrecPointerKey::GetNewTrecPointerAlt<TFileShell, TDirectory>(newPath);
	else
		return TrecPointerKey::GetNewTrecPointer<TFileShell>(newPath);
}

#elif  defined(__linux__) || (defined (__APPLE__) && defined (__MACH__))

TrecPointer<TFile::TFileShell> TFile::TFileShell::GetFileInfo(const TString& path)
{
	TString newPath(path);

	newPath.Replace(L'\\', L'/');
	while (newPath.Replace(L"//", L"/"));

	while (newPath.Replace(L"/./", L"/"));

	for (int upIndex = newPath.Find(L"../"); upIndex != -1; upIndex = newPath.Find(L"../"))
	{
		TString beg(newPath.SubString(0, upIndex));
		int chop = beg.FindLast(L'/');
		if (chop == -1)
			return TrecPointer<TFileShell>();
		beg.Set(beg.SubString(0, chop));
		newPath.Set(beg + newPath.SubString(upIndex + 2));
	}

	while ((newPath.StartsWith(L"\"") && newPath.EndsWith(L"\""))
		|| (newPath.StartsWith(L"\'") && newPath.EndsWith(L"\'")))
		newPath.Set(newPath.SubString(1, newPath.GetSize() - 1));
	struct stat tempStat;
	DWORD ftyp = stat(newPath.GetRegString().c_str(), &tempStat);

	if (ftyp < 0)
		return TrecPointer<TFileShell>(); // if Invalid, simply return a Null


	if (S_ISDIR(tempStat.st_mode))
		return TrecPointerKey::GetNewTrecPointerAlt<TFileShell, TDirectory>(newPath);
	else
		return TrecPointerKey::GetNewTrecPointer<TFileShell>(newPath);
}

#endif

TrecPointer<TFile::TFileShell> TFile::TFileShell::GetParent()
{
	auto pieces = path.split(L'\\');

	TString newPath;

	for (UINT Rust = 0; Rust < pieces->Size() - 1; Rust++)
	{
		if (newPath.GetSize())
			newPath.AppendChar(L'\\');
		newPath.Append(pieces->at(Rust));
	}

	newPath.AppendChar(L'\\');
	return TFile::TFileShell::GetFileInfo(newPath);
}

FILETIME TFile::TFileShell::GetCreationDate()
{
	TObjectLocker lock(&thread);
		Refresh();
	FILETIME ret = fileInfo.CREATE_TIME;
	return ret;
}

FILETIME TFile::TFileShell::GetLastAccessed()
{
	TObjectLocker lock(&thread);
		Refresh();
	FILETIME ret = fileInfo.LAST_ACCESS;
	return ret;
}

FILETIME TFile::TFileShell::GetLastWritten()
{
	TObjectLocker lock(&thread);
		Refresh();
	FILETIME ret = fileInfo.LAST_WRITE;
	return ret;
}

ULONG64 TFile::TFileShell::GetSize()
{
	TObjectLocker lock(&thread);
		Refresh();
#ifdef _WINDOWS
	ULONG64 ret = (static_cast<ULONG64>(fileInfo.nFileSizeHigh) << 32) + static_cast<ULONG64>(fileInfo.nFileSizeLow);
#elif  defined(__linux__) || (defined (__APPLE__) && defined (__MACH__))
	ULONG64 ret = fileInfo.st_size;
#endif
	return ret;
}

bool TFile::TFileShell::GetRelativePath(TString& relativePath, TrecPointer<TFileShell> directory, bool allowOutside)
{
	if (!directory.Get() || !directory->IsDirectory())
		return false;

	TString d(directory->GetPath());
	if (!d.EndsWith(L'\\'))
		d.AppendChar(L'\\');
	TString f(path);

	bool done = false;

	if (f.Find(d) == 0)
	{
		relativePath.Set(f.SubString(d.GetSize()));
		done = true;
	}

	if (!done && allowOutside)
	{
		int index = 0;

		while (index != -1)
		{
			int dIndex = d.Find(L'\\', index);

			int fIndex = f.Find(L'\\', index);

			if (dIndex == fIndex && dIndex != -1 && d.SubString(0, dIndex).CompareNoCase(f.SubString(0, fIndex)))
				index = dIndex;
			else break;
		}

		TString remainingDirectory(d.SubString(index + 1));

		relativePath.Empty();

		UINT slashCount = remainingDirectory.CountFinds(L'\\');

		while (slashCount--)
		{
			relativePath.Append(L"..\\");
		}

		relativePath.Append(f.SubString(index + 1));
		done = true;
	}

	return done;
}

TString TFile::TFileShell::GetDirectoryName()
{
	auto pieces = path.split(L'\\');

	UINT pSize = pieces->Size();

	return pSize > 1 ? pieces->at(pSize - 2) : L"";
}

bool TFile::TFileShell::IsArchive()
{
	TObjectLocker lock(&thread);
		Refresh();
	return IS_ARCHIVE;
}

bool TFile::TFileShell::IsEncrypted()
{
	TObjectLocker lock(&thread);
		Refresh();
	return IS_ENCRYPTED;
}

bool TFile::TFileShell::IsHidden()
{
	TObjectLocker lock(&thread);
	Refresh();
	TString name(GetName());
	return name.StartsWith(L'.');
}

bool TFile::TFileShell::IsReadOnly()
{
	TObjectLocker lock(&thread);
	Refresh();
#ifdef _WINDOWS
	bool ret = fileInfo.dwFileAttributes & FILE_ATTRIBUTE_READONLY;
#elif defined(__linux__) || (defined (__APPLE__) && defined (__MACH__))
	if (deleted)
		return false;
	bool ret = access(path.GetRegString().c_str(), W_OK);
#endif
	return ret;
}



#ifdef _WINDOWS
TFile::TFileShell::TFileShell(const TString& path)
{
	TObjectLocker lock(&thread);
		TString newPath(path);
	if (GetFileAttributesExW(newPath.GetConstantBuffer().getBuffer(), GET_FILEEX_INFO_LEVELS::GetFileExInfoStandard, &this->fileInfo))
		this->path.Set(path);


	deleted = false;
}


void TFile::TFileShell::Refresh()
{
	TObjectLocker lock(&thread);
		if (!GetFileAttributesExW(path.GetConstantBuffer().getBuffer(), GET_FILEEX_INFO_LEVELS::GetFileExInfoStandard, &this->fileInfo))
		{
			if (path.GetSize())
				deleted = true;
		}
}

void TDirectory::GetFileListing(TDataArray<TrecPointer<TFileShell>>& files)
{
	TObjectLocker lock(&thread);
		files.RemoveAll();
	HANDLE fileBrowser = 0;
	WIN32_FIND_DATAW data;

	TString searchPath(path + L"\\*");

	fileBrowser = FindFirstFileExW(searchPath.GetConstantBuffer().getBuffer(),
		FindExInfoStandard,
		&data,
		FindExSearchNameMatch,
		nullptr,
		0);

	if (fileBrowser == INVALID_HANDLE_VALUE)
	{
		return;
	}


	TString newPath;

	do
	{
		if (!this->GetName().Compare(data.cFileName))
			continue;
		newPath.Set(this->path + L"\\" + data.cFileName);
		files.push_back(TFileShell::GetFileInfo(newPath));
	} while (FindNextFileW(fileBrowser, &data));

	FindClose(fileBrowser);
	fileBrowser = 0;
}

#elif  defined(__linux__) || (defined (__APPLE__) && defined (__MACH__))

TFile::TFileShell::TFileShell(const TString& path)
{
	TObjectLocker lock(&thread);
	this->path.Set(path);
	Refresh();
	return;
}

void TFile::TFileShell::Refresh()
{
	TObjectLocker lock(&thread);
	if (deleted) return;
	int res = stat(path.GetRegString().c_str(), &(this->fileInfo));

	deleted = res < 0;
	if (deleted) path.Empty();
}

void TDirectory::GetFileListing(TDataArray<TrecPointer<TFileShell>>& files)
{
	TObjectLocker lock(&thread);
	files.RemoveAll();

	DIR* dp = nullptr;
	struct dirent* ep = nullptr;
	dp = opendir(path.GetRegString().c_str());

	if (dp)
	{
		while (ep = readdir(dp))
		{
			TString entName(ep->d_name);
			if (entName.Find(L'/'))
				entName.Set(path + L'/' + entName);
			TrecPointer<TFileShell> entry = TFileShell::GetFileInfo(entName);
			if (entry.Get())
				files.push_back(entry);

			// Documentation says not to free the ep structure
		}

		closedir(dp);
		dp = nullptr;
	}

}

#endif


///
/// TDirectory
///

TDirectory::TDirectory(const TString& path) : TFileShell(path)
{

}


bool TDirectory::IsDirectory()
{
	return true;
}

TDataArray<TrecPointer<TFile::TFileShell>> TDirectory::GetFileListing()
{
	TObjectLocker lock(&thread);
	TDataArray<TrecPointer<TFileShell>> ret;

	GetFileListing(ret);

	return ret;
}

///
/// TFile
///

TFile::TFile(): fileHandle(0), position(0ULL)
{
	fileEncode = FileEncodingType::fet_unknown;
}

TFile::TFile(TrecPointer<TFileShell> lpszFileName, UINT nOpenFlags)
{
	Open(lpszFileName, nOpenFlags);
}

TFile::TFile(TrecPointer<TFileShell> file, const TString& name, UINT nOpenFlags)
{
	Open(file, name, nOpenFlags);
}

TFile::~TFile()
{
	Close();
}

bool TFile::Open(TrecPointer<TFileShell> file, UINT nOpenFlags)
{
	return Open(file, L"", nOpenFlags);
}

bool TFile::Open(TrecPointer<TFileShell> file, const TString& name, UINT nOpenFlags)
{
	if (!file.Get()) return false;
	
	if (!name.GetSize() && file->IsDirectory())
		return false;

	TObjectLocker lock(&thread);
	fileEncode = FileEncodingType::fet_unknown;

	UINT readWrite = 0, sharing = 0, atts = 0;
	ConvertFlags(nOpenFlags, readWrite, sharing, atts);

	// If no attribute for opening is specified, use the value most likely to succeed
	WCHAR sep = L'\0';

#ifdef _WINDOWS
		atts = OPEN_ALWAYS;
		sep = L'\\';
#elif defined(__linux__) || (defined (__APPLE__) && defined (__MACH__))
		atts = O_CREAT;
		sep = L'/';
#endif
	TString newFileName(file->GetPath());

	if (file->IsDirectory())
	{
		if (!newFileName.EndsWith(L'\\') && !newFileName.EndsWith(L'/'))
			newFileName.AppendChar(sep);
		newFileName.Append(name);
	}

	fileHandle = TcFileOpen(newFileName, readWrite, sharing, atts);


	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		fileHandle = 0;
		//int err = GetLastError();
		return false;
	}

	// Here, the file is open, try to deduce the file type
	fileEncode = DeduceEncodingType();
	filePath.Set(newFileName);
	return true;
}

BOOL TFile::ReadString(TString& rString)
{
	TObjectLocker lock(&thread);
		bool success = false;
	rString.Empty();
	char letter[1];
	UCHAR letter2[2];
	WCHAR cLetter;
	UCHAR temp;
	WCHAR wLetter;

	char uni8[4];
	UCHAR bytes;

	WCHAR* wideString = nullptr;

	std::string charHolder;

	switch (fileEncode)
	{
	case FileEncodingType::fet_acsii:

		while (Read(&letter[0], 1))
		{
			success = true;
			if (letter[0] == '\n')
				break;
			rString.AppendChar(ReturnWCharType(letter[0]));
		}

		break;
	case FileEncodingType::fet_unicode:

		while (Read(letter2, 2))
		{
			success = true;
			temp = letter2[0];
			letter2[0] = letter2[1];
			letter2[1] = temp;
			memcpy(&cLetter, letter2, 2);
			if (cLetter == L'\n')
				break;
			rString.AppendChar(cLetter);
		}

		break;
	case FileEncodingType::fet_unicode_little:

		while (Read(&wLetter, 2))
		{
			success = true;
			if (wLetter == L'\n')
				break;
			rString.AppendChar(wLetter);
		}
		break;
	case FileEncodingType::fet_unicode8:
		ZeroMemory(uni8, sizeof(char) * 4);
		while (bytes = ReadUnicode8Char(uni8))
		{
			success = true;
			if (uni8[0] == '\n')
				break;

			for (UINT Rust = 0; bytes; bytes--, Rust++)
			{
				charHolder.push_back(uni8[Rust]);
				uni8[Rust] = '\0';
			}
		}

		wideString = new WCHAR[charHolder.size() * 2];
		ZeroMemory(wideString, sizeof(WCHAR) * charHolder.size() * 2);

		TcMultiByteToWideChar(charHolder.c_str(), charHolder.size(), wideString);
		rString.Set(wideString);
		delete[] wideString;
		wideString = nullptr;
	}
	return success;
}

ULONGLONG TFile::ReadString(TString& rString, ULONGLONG nMax)
{
	TObjectLocker lock(&thread);
		rString.Empty();
	ULONGLONG rust = 0;

	if (fileEncode == FileEncodingType::fet_unknown)
		DeduceEncodingType();

	char uni8[4];
	UCHAR bytes;

	WCHAR* wideString = nullptr;

	std::string charHolder;

	switch (fileEncode)
	{
	case FileEncodingType::fet_acsii:
		char letter[1];
		for (; rust <= nMax && Read(&letter, 1); rust += 1)
		{
			rString.AppendChar(ReturnWCharType(letter[0]));
		}

		break;
	case FileEncodingType::fet_unicode:
		UCHAR letter2[2];

		for (; rust <= nMax && Read(&letter2, 2); rust += 2)
		{
			WCHAR cLetter;
			UCHAR temp = letter2[0];
			letter2[0] = letter2[1];
			letter2[1] = temp;
			memcpy(&cLetter, letter2, 2);

			rString.AppendChar(cLetter);
		}

		break;
	case FileEncodingType::fet_unicode_little:
		WCHAR wLetter;

		for (; rust <= nMax && Read(&wLetter, 2); rust += 2)
		{
			rString.AppendChar(wLetter);
		}
		break;
	case FileEncodingType::fet_unicode8:
		ZeroMemory(uni8, sizeof(char) * 4);
		while ((bytes = ReadUnicode8Char(uni8)) && rust <= nMax)
		{
			rust += bytes;

			for (UINT Rust = 0; bytes; bytes--, Rust++)
			{
				charHolder.push_back(uni8[Rust]);
				uni8[Rust] = '\0';
			}
		}

		wideString = new WCHAR[charHolder.size() * 2];
		ZeroMemory(wideString, sizeof(WCHAR) * charHolder.size() * 2);

		TcMultiByteToWideChar(charHolder.c_str(), charHolder.size(), wideString);
		rString.Set(wideString);
		delete[] wideString;
		wideString = nullptr;
	}
	return rust;
}


ULONGLONG TFile::ReadStringLine(TString& rString, ULONGLONG nMax)
{
	TObjectLocker lock(&thread);
		rString.Empty();
	ULONGLONG rust = 0;

	if (fileEncode == FileEncodingType::fet_unknown)
		DeduceEncodingType();

	char uni8[4];
	UCHAR bytes;

	WCHAR* wideString = nullptr;

	std::string charHolder;

	switch (fileEncode)
	{
	case FileEncodingType::fet_acsii:
		char letter[1];
		for (; rust <= nMax && Read(&letter, 1) && letter[0] != '\n'; rust++)
		{
			rString.AppendChar(ReturnWCharType(letter[0]));
		}

		break;
	case FileEncodingType::fet_unicode:
		UCHAR letter2[2];

		for (; rust <= nMax && Read(&letter2, 2); rust += 2)
		{
			WCHAR cLetter;
			UCHAR temp = letter2[0];
			letter2[0] = letter2[1];
			letter2[1] = temp;
			memcpy(&cLetter, letter2, 2);

			if (cLetter == L'\n')
			{
				rust += 2;
				break;
			}
			rString.AppendChar(cLetter);
		}

		break;
	case FileEncodingType::fet_unicode_little:
		WCHAR wLetter;

		for (; rust < nMax && Read(&wLetter, 2); rust += 2)
		{
			if (wLetter == L'\n')
			{
				rust += 2;
				break;
			}
			rString.AppendChar(wLetter);
		}
		break;
	case FileEncodingType::fet_unicode8:
		ZeroMemory(uni8, sizeof(char) * 4);
		while ((bytes = ReadUnicode8Char(uni8)) && rust <= nMax)
		{
			rust += bytes;
			if (uni8[0] == '\n')
				break;

			for (UINT Rust = 0; bytes; bytes--, Rust++)
			{
				charHolder.push_back(uni8[Rust]);
				uni8[Rust] = '\0';
			}
		}

		wideString = new WCHAR[charHolder.size() * 2];
		ZeroMemory(wideString, sizeof(WCHAR) * charHolder.size() * 2);

		TcMultiByteToWideChar(charHolder.c_str(), charHolder.size(), wideString);
		rString.Set(wideString);
		delete[] wideString;
		wideString = nullptr;
	}
	return rust;
}

UINT TFile::ReadString(TString& rString, WCHAR chara)
{
	TObjectLocker lock(&thread);
		bool success = false;
	rString.Empty();

	char uni8[4];
	UCHAR bytes;

	WCHAR* wideString = nullptr;

	WCHAR charaComp;

	std::string charHolder;

	switch (fileEncode)
	{
	case FileEncodingType::fet_acsii:
		char letter[1];
		while (Read(&letter, 1))
		{
			if (letter[0] == chara)
				break;
			rString.AppendChar(ReturnWCharType(letter[0]));
			success = true;
		}

		break;
	case FileEncodingType::fet_unicode:
		UCHAR letter2[2];
		while (Read(&letter2, 2))
		{
			WCHAR cLetter;
			UCHAR temp = letter2[0];
			letter2[0] = letter2[1];
			letter2[1] = temp;
			memcpy(&cLetter, letter2, 2);
			if (cLetter == chara)
				break;
			rString.AppendChar(cLetter);
			success = true;
		}

		break;
	case FileEncodingType::fet_unicode_little:
		WCHAR wLetter;
		while (Read(&wLetter, 2))
		{
			if (wLetter == chara)
				break;
			rString.AppendChar(wLetter);
			success = true;
		}
		break;
	case FileEncodingType::fet_unicode8:
		ZeroMemory(uni8, sizeof(char) * 4);
		while (bytes = ReadUnicode8Char(uni8))
		{

			if (TcMultiByteToWideChar(uni8, bytes, &charaComp) && chara == charaComp)
				break;

			for (UINT Rust = 0; bytes; bytes--, Rust++)
			{
				charHolder.push_back(uni8[Rust]);
				uni8[Rust] = '\0';
			}
		}

		wideString = new WCHAR[charHolder.size() * 2];
		ZeroMemory(wideString, sizeof(WCHAR) * charHolder.size() * 2);

		TcMultiByteToWideChar(charHolder.c_str(), charHolder.size(), wideString);
		rString.Set(wideString);
		delete[] wideString;
		wideString = nullptr;
	}
	return rString.GetSize();
}

UINT TFile::ReadString(TString& rString, const TString& chars, UCHAR flags, UINT max)
{
	TObjectLocker lock(&thread);
		bool success = false;
	rString.Empty();
	char letter[1] = { '\0' };

	WCHAR quote = L'\0';

	UINT backslashes = 0;

	bool maxSet = max > 0;
	WCHAR charaComp;

	char uni8[4];
	UCHAR bytes;

	WCHAR* wideString = nullptr;


	std::string charHolder;

	switch (fileEncode)
	{
	case FileEncodingType::fet_acsii:

		while (Read(&letter, 1))
		{
			// If we care about backslashes and we encounter one on this character, increase the count
			if (flags & 0b00000100)
			{
				if (letter[0] == '\\')
					backslashes++;
				// else backslashes = 0;
			}

			// If we don't care about backslashes or we have an od number of them, they they don't bother us for these operations
			if ((flags & 0b00000100) == 0 || backslashes % 2 == 0)
			{
				// If we care about making sure the final character is out of quotes, check to make sure we are, in fact, out of them
				if (flags & 0b00000010)
				{
					if (letter[0] == '\'')
					{
						if (quote == L'\'')
							quote = 0;
						else if (!quote)
							quote = L'\'';
					}
					else if (letter[0] == '\"')
					{
						if (quote == L'\"')
							quote = 0;
						else if (!quote)
							quote = L'\"';
					}
				}

				// if we don't care about quotes or we are out of them, then check to see if we reached a terminating character
				if ((!(flags & 0b00000010) || !quote) && chars.Find(letter[0]) != -1)
				{
					// If we want to add the terminating character, then do so here
					if (flags & 0b00000001)
						rString.AppendChar(ReturnWCharType(letter[0]));
					break;
				}
			}

			// Odd # of backslashes will affect the first non-backslash character, so process it first before resetting the backslash count
			if (flags & 0b00000100)
			{
				if (letter[0] != '\\')
					backslashes = 0;
			}

			rString.AppendChar(ReturnWCharType(letter[0]));
			success = true;

			if (maxSet && !(--max))
			{
				break;
			}

		}

		break;
	case FileEncodingType::fet_unicode:
		UCHAR letter2[2];
		while (Read(&letter2, 2))
		{
			WCHAR cLetter;
			UCHAR temp = letter2[0];
			letter2[0] = letter2[1];
			letter2[1] = temp;
			memcpy(&cLetter, letter2, 2);



			// If we care about backslashes and we encounter one on this character, increase the count
			if (flags & 0b00000100)
			{
				if (cLetter == '\\')
					backslashes++;
				// else backslashes = 0;
			}

			// If we don't care about backslashes or we have an od number of them, they they don't bother us for these operations
			if ((flags & 0b00000100) == 0 || backslashes % 2 == 0)
			{
				// If we care about making sure the final character is out of quotes, check to make sure we are, in fact, out of them
				if (flags & 0b00000010)
				{
					if (cLetter == '\'')
					{
						if (quote == L'\'')
							quote = 0;
						else if (!quote)
							quote = L'\'';
					}
					else if (cLetter == '\"')
					{
						if (quote == L'\"')
							quote = 0;
						else if (!quote)
							quote = L'\"';
					}
				}

				// if we don't care about quotes or we are out of them, then check to see if we reached a terminating character
				if ((!(flags & 0b00000010) || !quote) && chars.Find(cLetter) != -1)
				{



					if (chars.Find(cLetter) != -1)
					{
						if (flags & 0b00000001)
							rString.AppendChar(cLetter);
						break;
					}
				}
			}

			// Odd # of backslashes will affect the first non-backslash character, so process it first before resetting the backslash count
			if (flags & 0b00000100)
			{
				if (cLetter != '\\')
					backslashes = 0;
			}

			rString.AppendChar(cLetter);
			success = true;

			if (maxSet)
			{
				max -= 2;
				if (!max) break;
			}

		}

		break;
	case FileEncodingType::fet_unicode_little:
		WCHAR wLetter;
		while (Read(&wLetter, 2))
		{
			// If we care about backslashes and we encounter one on this character, increase the count
			if (flags & 0b00000100)
			{
				if (wLetter == '\\')
					backslashes++;
				// else backslashes = 0;
			}

			// If we don't care about backslashes or we have an od number of them, they they don't bother us for these operations
			if ((flags & 0b00000100) == 0 || backslashes % 2 == 0)
			{
				// If we care about making sure the final character is out of quotes, check to make sure we are, in fact, out of them
				if (flags & 0b00000010)
				{
					if (wLetter == '\'')
					{
						if (quote == L'\'')
							quote = 0;
						else if (!quote)
							quote = L'\'';
					}
					else if (wLetter == '\"')
					{
						if (quote == L'\"')
							quote = 0;
						else if (!quote)
							quote = L'\"';
					}
				}

				// if we don't care about quotes or we are out of them, then check to see if we reached a terminating character
				if ((!(flags & 0b00000010) || quote == 0) && chars.Find(wLetter) != -1)
				{


					if (chars.Find(wLetter) != -1)
					{
						if (flags & 0b00000001)
							rString.AppendChar(wLetter);
						break;
					}
				}
			}

			// Odd # of backslashes will affect the first non-backslash character, so process it first before resetting the backslash count
			if (flags & 0b00000100)
			{
				if (wLetter != '\\')
					backslashes = 0;
			}

			rString.AppendChar(wLetter);
			success = true;
			if (maxSet)
			{
				max -= 2;
				if (!max || max == UINT32_MAX) break;
			}
		}
		break;
	case FileEncodingType::fet_unicode8:
		ZeroMemory(uni8, sizeof(char) * 4);
		while (bytes = ReadUnicode8Char(uni8))
		{

			UINT worked = TcMultiByteToWideChar(uni8, bytes, &charaComp);

			// If we care about backslashes and we encounter one on this character, increase the count
			if (worked)
			{
				if (flags & 0b00000100)
				{
					if (charaComp == '\\')
						backslashes++;
					// else backslashes = 0;
				}

				// If we don't care about backslashes or we have an od number of them, they they don't bother us for these operations
				if ((flags & 0b00000100) == 0 || backslashes % 2 == 0)
				{
					// If we care about making sure the final character is out of quotes, check to make sure we are, in fact, out of them
					if (flags & 0b00000010)
					{
						if (charaComp == '\'')
						{
							if (quote == L'\'')
								quote = 0;
							else if (!quote)
								quote = L'\'';
						}
						else if (charaComp == '\"')
						{
							if (quote == L'\"')
								quote = 0;
							else if (!quote)
								quote = L'\"';
						}
					}

					// if we don't care about quotes or we are out of them, then check to see if we reached a terminating character
					if ((!(flags & 0b00000010) || quote == 0) && chars.Find(charaComp) != -1)
					{


						if (chars.Find(charaComp) != -1)
						{
							if (flags & 0b00000001)
								for (UINT Rust = 0; bytes; bytes--, Rust++)
									charHolder.push_back(uni8[Rust]);
							break;
						}
					}
				}

				// Odd # of backslashes will affect the first non-backslash character, so process it first before resetting the backslash count
				if (flags & 0b00000100)
				{
					if (charaComp != '\\')
						backslashes = 0;
				}

			} // if worked
			success = true;

			bool reachedMax = false;

			if (maxSet)
			{
				max -= bytes;
				if (!max || max == UINT32_MAX)reachedMax = true;
			}

			for (UINT Rust = 0; bytes; bytes--, Rust++)
			{
				charHolder.push_back(uni8[Rust]);
				uni8[Rust] = '\0';
			}

			if (reachedMax)
				break;
		}

		wideString = new WCHAR[charHolder.size() * 2];
		ZeroMemory(wideString, sizeof(WCHAR) * charHolder.size() * 2);

		TcMultiByteToWideChar(charHolder.c_str(), charHolder.size(), wideString);
		rString.Set(wideString);
		delete[] wideString;
		wideString = nullptr;

	}
	return rString.GetSize();
}


void TFile::WriteString(const TString& lpsz)
{
	TObjectLocker lock(&thread);
		UINT size = 0;
	CHAR* acsiiText = nullptr;
	UINT wBytes = 0;
	WCHAR cLetter = L'\0';
	UCHAR bytes[2];
	UCHAR temp = 0;

	TString newParam(lpsz);

	if (fileEncode == FileEncodingType::fet_unknown)
		fileEncode = FileEncodingType::fet_unicode_little;
	switch (fileEncode)
	{
	case FileEncodingType::fet_acsii:
	case FileEncodingType::fet_unicode8:
		size = lpsz.GetSize();
		
		wBytes = TcWideCharToMultiByte(newParam.GetConstantBuffer().getBuffer(),
			0, acsiiText);
		acsiiText = new CHAR[wBytes];
		wBytes = TcWideCharToMultiByte(newParam.GetConstantBuffer().getBuffer(),
			wBytes, acsiiText);
		if(wBytes)
			Write(acsiiText, wBytes - 1);
		delete[] acsiiText;
		break;
	case FileEncodingType::fet_unicode:
		for (UINT c = 0; lpsz[c] != L'\0'; c++)
		{
			cLetter = lpsz[c];

			memcpy(bytes, &cLetter, 2);
			temp = bytes[0];
			bytes[0] = bytes[1];
			bytes[1] = temp;
			Write(bytes, 2);
		}
		break;
	case FileEncodingType::fet_unicode_little:

		Write(newParam.GetConstantBuffer().getBuffer(), lpsz.GetSize() * sizeof(WCHAR));

	}
	return;
}

bool TFile::IsOpen() const
{
	TObjectLocker lock(&thread);
		bool ret = ! INVALID_FILE_HANDLE(fileHandle);
	return ret;
}


bool TFile::SetEncoding(FileEncodingType fet)
{
	TObjectLocker lock(&thread);
		if (fileEncode == FileEncodingType::fet_unknown && fet != FileEncodingType::fet_unknown)
		{
			fileEncode = fet;
			return true;
		}
	return false;
}

TString TFile::GetFileDirectory()
{
	TObjectLocker lock(&thread);
		TString sep(L"/\\");
	int seperate = filePath.FindLastOneOf(sep);

	if (seperate == -1)
	{
		return TString();
	}
	return filePath.SubString(0, seperate + 1);
}


TString TFile::GetFileExtension()
{
	TObjectLocker lock(&thread);
		TString ext = GetFileName();
	if (ext.Find(L'.') == -1)
	{
		return ext;
	}
	for (int c = ext.GetSize() - 1; c >= 0; c--)
	{
		if (ext[c] == L'.')
		{
			return ext.SubString(c + 1);
		}
	}
	return ext;
}

void TFile::Close()
{
	TObjectLocker lock(&thread);
	TcFileClose(fileHandle);
}

void TFile::Flush()
{
	TObjectLocker lock(&thread);
	TcFileFlush(fileHandle);
}

void TFile::Write(const void* buffer, UINT count)
{
	TObjectLocker lock(&thread);
	if (!fileHandle)
	{
		return;
	}
	DWORD size = 0;

	TcFileWrite(fileHandle, (UCHAR*)buffer, static_cast<DWORD>(count), size);

	position += size;

	return;
}

FileEncodingType TFile::GetEncodingType()const
{
	return fileEncode;
}

FileEncodingType TFile::DeduceEncodingType()
{
	if (!IsOpen())
		return FileEncodingType::fet_unknown;
	if (GetLength() < 2)
		return FileEncodingType::fet_unknown;
	TObjectLocker lock(&thread);
		UCHAR twoBytes[30];
	UINT bytes = Read(&twoBytes, 30);

	SeekToBegin();
#ifdef _WINDOWS
	int value = IS_TEXT_UNICODE_UNICODE_MASK |
		IS_TEXT_UNICODE_REVERSE_MASK |
		IS_TEXT_UNICODE_NOT_UNICODE_MASK |
		IS_TEXT_UNICODE_NOT_ASCII_MASK;



	if (!IsTextUnicode(&twoBytes, bytes, &value))
	{
		// Tests have failed, so we have to rely on our own assessment

#endif
		if (bytes >= 5)
		{
			// Test for UTF-7
			if (twoBytes[0] == 0x2b &&
				twoBytes[1] == 0x2f &&
				twoBytes[2] == 0x76 &&
				twoBytes[3] == 0x38 &&
				twoBytes[4] == 0x2d)
			{
				return FileEncodingType::fet_unicode7;
			}
			// Now check for UTF-8
			if (twoBytes[0] == 0xef &&
				twoBytes[1] == 0xbb &&
				twoBytes[2] == 0xbf)
			{
				return FileEncodingType::fet_unicode8;
			}
			// Check for big endian unicode
			if (twoBytes[0] == 0xfe &&
				twoBytes[1] == 0xff)
			{
				return FileEncodingType::fet_unicode;
			}
			// Check for little-endian unicode
			if (twoBytes[0] == 0xff &&
				twoBytes[1] == 0xfe)
			{
				return FileEncodingType::fet_unicode_little;
			}

			// There is no BOM to tell us the encoding, need to guess
			// Check to see if it is big-endian the old fashioned way
			if (twoBytes[0] == 0x00 &&
				twoBytes[2] == 0x00 &&
				twoBytes[4] == 0x00)
			{
				return FileEncodingType::fet_unicode;
			}
			if (twoBytes[1] == 0x00 &&
				twoBytes[3] == 0x00)
			{
				return FileEncodingType::fet_unicode_little;
			}
			return FileEncodingType::fet_acsii;
		}
		else
		{
			return FileEncodingType::fet_unknown;
		}
#ifdef _WINDOWS
	}

	else
	{
		if (value & IS_TEXT_UNICODE_STATISTICS ||
			value & IS_TEXT_UNICODE_CONTROLS ||
			value & IS_TEXT_UNICODE_ASCII16)
		{
			return FileEncodingType::fet_unicode_little;
		}
		if (value & IS_TEXT_UNICODE_REVERSE_STATISTICS ||
			value & IS_TEXT_UNICODE_REVERSE_CONTROLS ||
			value & IS_TEXT_UNICODE_REVERSE_ASCII16)
		{
			return FileEncodingType::fet_unicode;
		}
		if (value & IS_TEXT_UNICODE_NOT_UNICODE_MASK)
		{
			return FileEncodingType::fet_acsii;
		}
	}
#endif
	return FileEncodingType::fet_unknown;
}

void TFile::ConvertFlags(UINT& input, UINT& open, UINT& security, UINT& creation)
{
	open = input & 0xff000000;

	security = (input >> 8) & 0x000000ff;
	creation = (input >> 16) & 0x000000ff;
}

UCHAR TFile::ReadUnicode8Char(char* seq4)
{
	if (!Read(seq4, 1))
		return 0;
	TObjectLocker lock(&thread);
		UINT ret = 0;
	if ((seq4[0] & 0b11110000) == 0b11110000)
	{
		// we are dealing with a 4 byte sequence in UTF-8
		// Already have the first byte, now get the other three
		ret = 1 + Read(&seq4[1], 3);
	}
	else
		if ((seq4[0] & 0b11100000) == 0b11100000)
		{
			// At this point, we are dealing with a 3 byte sequence,
			// Get the other two
			ret = 1 + Read(&seq4[1], 2);
		}
		else
			if ((seq4[0] & 0b11000000) == 0b11000000)
			{
				// Just a two byte sequence
				// Get the second byte
				ret = 1 + Read(&seq4[1], 1);
			}
	// just one byte
	return static_cast<UCHAR>(ret);
}

TString TFile::GetFileName() const
{
	TObjectLocker lock(&thread);
		TString sep(L"/\\");
	int seperate = filePath.FindLastOneOf(sep);

	if (seperate == -1)
	{
		return TString();
	}
	return filePath.SubString(seperate + 1);
}

TString TFile::GetFilePath() const
{
	return filePath;
}

#ifdef _WINDOWS

TString TFile::GetFileTitle() const
{
	TObjectLocker lock(&thread);
		WCHAR* cTitle = new WCHAR[filePath.GetSize() + 1];
	ZeroMemory(cTitle, sizeof(WCHAR) * (filePath.GetSize() + 1));

	TString ret;
	TString newPath(filePath);
	if (!::GetFileTitleW(newPath.GetConstantBuffer().getBuffer(), cTitle, filePath.GetSize()))
	{
		ret.Set(cTitle);
	}

	delete[] cTitle;
	return ret;
}

ULONGLONG TFile::GetLength() const
{
	TObjectLocker lock(&thread);
		LARGE_INTEGER  len_li;
	GetFileSizeEx((HANDLE)fileHandle, &len_li);
	LONGLONG  len_ll = len_li.QuadPart;
	return len_ll;
}

#elif defined(__linux__) || (defined (__APPLE__) && defined (__MACH__))

TString TFile::GetFileTitle() const
{
	return GetFileName();
}

ULONGLONG TFile::GetLength() const
{
	TObjectLocker lock(&thread);
	off_t ret = lseek(fileHandle, 0, SEEK_END);
	lseek(fileHandle, position, SEEK_SET);
	return static_cast<ULONGLONG>(ret);
}

#endif

ULONGLONG TFile::GetPosition() const
{
	return position;
}

UINT TFile::Read(void* buffer, UINT count)
{
	TObjectLocker lock(&thread);
	if (!fileHandle)
	{
		return 0;
	}
	DWORD ret = 0;
	TcFileRead(this->fileHandle, (UCHAR*)buffer, count, ret);

	position += ret;

	return static_cast<UINT>(ret);
}

#ifdef _WINDOWS

ULONGLONG TFile::Seek(LONGLONG offset, UINT from)
{
	TObjectLocker lock(&thread);
		if (!fileHandle) {
			return 0;
		}
	PLONG hZero = new LONG;
	*hZero = 0;
	PLONG store = hZero;
	DWORD res = SetFilePointer((HANDLE)fileHandle, offset, hZero, from);

	if (res != INVALID_SET_FILE_POINTER)
	{
		ULONGLONG upper = static_cast<ULONGLONG>(*hZero) << 32;

		position = upper + static_cast<ULONGLONG>(res);
	}
	else
	{
		// To-Do:
	}
	delete store;
	return position;
}

void TFile::SeekToBegin()
{
	TObjectLocker lock(&thread);
		if (!fileHandle)
		{
			return;
		}
	PLONG hZero = new LONG;
	*hZero = 0;
	PLONG store = hZero;
	DWORD res = SetFilePointer((HANDLE)fileHandle, 0, hZero, FILE_BEGIN);

	if (res != INVALID_SET_FILE_POINTER)
	{
		position = 0;
	}
	else
	{
		// To-Do:
	}

	delete store;
	return;
}

ULONGLONG TFile::SeekToEnd()
{
	TObjectLocker lock(&thread);
		if (!fileHandle)
		{
			return 0;
		}
	PLONG hZero = new LONG;
	*hZero = 0;
	PLONG store = hZero;
	DWORD res = SetFilePointer((HANDLE)fileHandle, 0, hZero, FILE_END);

	if (res != INVALID_SET_FILE_POINTER)
	{
		ULONGLONG upper = static_cast<ULONGLONG>(*hZero) << 32;

		position = upper + static_cast<ULONGLONG>(res);
	}
	else
	{
		// To-Do:
	}

	delete store;

	return position;
}

#elif defined(__linux__) || (defined (__APPLE__) && defined (__MACH__))

ULONGLONG TFile::Seek(LONGLONG offset, UINT from)
{
	TObjectLocker lock(&thread);
	if (fileHandle < 0) {
		return 0;
	}
	int whence = 0;
	if (from == 1) whence = SEEK_CUR;
	else if (from == 2) whence = SEEK_END;

	return position = static_cast<ULONGLONG>(lseek(fileHandle, offset, whence));
}

void TFile::SeekToBegin()
{
	TObjectLocker lock(&thread);
	if (fileHandle < 0)
	{
		return;
	}
	position = static_cast<ULONGLONG>(lseek(fileHandle, 0, SEEK_SET));
}

ULONGLONG TFile::SeekToEnd()
{
	TObjectLocker lock(&thread);
	if (fileHandle < 0)
	{
		return 0;
	}
	return position = static_cast<ULONGLONG>(lseek(fileHandle, 0, SEEK_END));
}
#endif

///
/// TFileNode
///

TFileNode::TFileNode(UINT l) : TObjectNode(l), showAllFiles(false)
{
	filter_mode = file_node_filter_mode::fnfm_blank;
}

TString TFileNode::GetContent()
{
	TObjectLocker lock(&thread);

	return data.Get() ? data->GetName() : L"";
}

TString TFileNode::GetCommand(const TString& info)
{
	TObjectLocker lock(&thread);
	if (!info.CompareNoCase(L"dbl_ck") && data.Get())
	{
		return TString(L"win_OpenFile \"") + data->GetPath() + L"\"";
	}
	return L"";
}

bool TFileNode::IsExtendable()
{
	TObjectLocker lock(&thread);
		if (!data.Get())
		{
			return false;
		}
	return data->IsDirectory();
}

bool TFileNode::IsExtended()
{
	return files.Size();
}

TrecPointer<TObjectNode> TFileNode::GetNodeAt(UINT target, UINT current)
{
	if (target == current)
		return TrecPointerKey::TrecFromSoft<TObjectNode>(self);

	if (target < current)
		return TrecPointer<TObjectNode>();

	TrecPointer<TObjectNode> ret;

	current++;
	TObjectLocker lock(&thread);
		for (UINT rust = 0; rust < files.Size(); rust++)
		{
			ret = files[rust]->GetNodeAt(target, current);
			if (ret.Get())
			{
				return ret;
			}
			current += files[rust]->TotalChildren() + 1;
		}
	return TrecPointer<TObjectNode>();
}

UINT TFileNode::TotalChildren()
{
	UINT ret = 0;
	TObjectLocker lock(&thread);
		for (UINT rust = 0; rust < files.Size(); rust++)
		{
			ret += files[rust]->TotalChildren() + 1;
		}
	return ret;
}

bool TFileNode::Initialize()
{
	return false;
}

bool TFileNode::Initialize(TString& value)
{
	TObjectLocker lock(&thread);
		data = TFileShell::GetFileInfo(value);
	return data.Get() != nullptr;
}

void TFileNode::Extend()
{
	TObjectLocker lock(&thread);
		files.RemoveAll();
	if (data.Get() && data->IsDirectory())
	{
		TDataArray<TrecPointer<TFileShell>> shellFiles;
		dynamic_cast<TDirectory*>(data.Get())->GetFileListing(shellFiles);

		for (UINT Rust = 0; Rust < shellFiles.Size(); Rust++)
		{
			TrecPointer<TFileShell> f = shellFiles[Rust];
			if (ShouldShow(f))
			{
				TrecPointer<TObjectNode> n = TrecPointerKey::GetNewSelfTrecPointerAlt<TObjectNode, TFileNode>(level + 1);
				files.push_back(n);
				auto fileNode = dynamic_cast<TFileNode*>(n.Get());
				fileNode->SetFile(f);
				fileNode->SetFilterMode(filter_mode);
				fileNode->SetShowAllFiles(showAllFiles);

				for (UINT Rust = 0; Rust < extensions.Size(); Rust++)
				{
					fileNode->AddExtension(extensions[Rust]);
				}
			}
		}
	}
	return;
}

TrecPointer<TObjectNode> TFileNode::GetChildNodes(UINT index)
{
	TObjectLocker lock(&thread);
		if (index < files.Size())
		{
			auto ret = files[index];
			return ret;
		}
	return TrecPointer<TObjectNode>();
}

void TFileNode::DropChildNodes()
{
	TObjectLocker lock(&thread);
		files.RemoveAll();
	return;
}

void TFileNode::SetFile(const TrecPointer<TFileShell>& d)
{
	TObjectLocker lock(&thread);
		data = d;
	DropChildNodes();
	return;
}

TrecPointer<TFileShell> TFileNode::GetData()
{
	TObjectLocker lock(&thread);
		auto ret = data;
	return ret;
}

void TFileNode::SetShowAllFiles(bool show)
{
	TObjectLocker lock(&thread);
		for (UINT Rust = 0; Rust < files.Size(); Rust++)
		{
			auto fileNode = dynamic_cast<TFileNode*>(files[Rust].Get());

			if (fileNode)
			{
				fileNode->SetShowAllFiles(show);
			}
		}

	showAllFiles = show;
	return;
}

bool TFileNode::GetShowAllFiles()
{
	TObjectLocker lock(&thread);
		bool ret = showAllFiles || !extensions.Size();
	return ret;
}

bool TFileNode::AddExtension(const TString& extension)
{
	TObjectLocker lock(&thread);
		for (UINT Rust = 0; Rust < extensions.Size(); Rust++)
		{
			if (!extensions[Rust].CompareNoCase(extension))
			{
				return true;
			}
		}

	if (extension.GetSize() < 2)
	{
		return false;
	}
	if (extension[0] == L'.')
	{
		for (UINT Rust = 1; Rust < extension.GetSize(); Rust++)
		{
			WCHAR letter = extension[Rust];

			if ((letter >= L'A' && letter <= L'Z') ||
				(letter >= L'a' && letter <= L'z'))
				continue;
			return false;
		}

		extensions.push_back(extension);

		for (UINT Rust = 0; Rust < files.Size(); Rust++)
		{
			auto fileNode = dynamic_cast<TFileNode*>(files[Rust].Get());

			if (fileNode)
			{
				fileNode->AddExtension(extension);
			}

		}

		return true;
	}
	return false;
}

void TFileNode::SetFilterMode(file_node_filter_mode mode)
{
	TObjectLocker lock(&thread);
		filter_mode = mode;

	for (UINT Rust = 0; Rust < files.Size(); Rust++)
	{
		auto fileNode = dynamic_cast<TFileNode*>(files[Rust].Get());
		if (fileNode)
			fileNode->SetFilterMode(mode);
	}
	return;
}

file_node_filter_mode TFileNode::GetFilterMode()
{
	return filter_mode;
}

bool TFileNode::RemoveNode(TrecPointer<TObjectNode> obj)
{
	TObjectLocker lock(&thread);
		if (!obj.Get())
		{
			return false;
		}
	for (UINT Rust = 0; Rust < files.Size(); Rust++)
	{
		auto f = files[Rust].Get();
		if (!f)
			continue;

		if (f == obj.Get())
		{
			files.RemoveAt(Rust);
			return true;
		}

		if (f->RemoveNode(obj))
		{
			return true;
		}
	}
	return false;
}

bool TFileNode::ShouldShow(TrecPointer<TFileShell> node)
{
	if (!node.Get())
		return false;

	TString nodeName(node->GetName());
	TObjectLocker lock(&thread);
		if (node->IsDirectory())
		{
			switch (filter_mode)
			{
			case file_node_filter_mode::fnfm_blank:
			case file_node_filter_mode::fnfm_block_files:
				return true;
			case file_node_filter_mode::fnfm_block_current:
			case file_node_filter_mode::fnfm_block_current_and_files:
				return nodeName.Compare(L".");
			case file_node_filter_mode::fnfm_block_upper:
			case file_node_filter_mode::fnfm_block_upper_and_files:
				return nodeName.Compare(L"..");
			case file_node_filter_mode::fnfm_block_both:
			case file_node_filter_mode::fnfm_block_both_and_files:
				return nodeName.Compare(L".") && nodeName.Compare(L"..");
			default:
				return false;
			}
		}
		else
		{
			// We have a regular file
			switch (filter_mode)
			{
			case file_node_filter_mode::fnfm_block_current_and_files:
			case file_node_filter_mode::fnfm_block_upper_and_files:
			case file_node_filter_mode::fnfm_block_both_and_files:
			case file_node_filter_mode::fnfm_block_files:
				return false;
			default:
				if (showAllFiles && extensions.Size())
				{
					for (UINT Rust = 0; Rust < extensions.Size(); Rust++)
					{
						if (nodeName.FindLast(extensions[Rust]) == nodeName.GetSize() - extensions[Rust].GetSize())
						{
							return true;
						}
					}
					return false;
				}
				else
				{
					return true;
				}
			}
		}
}
