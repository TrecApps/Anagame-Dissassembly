
#include <TFormatReader.h>
#include "AnagameEnvironment.h"
#include <TFile.h>
#include <TContainerVariable.h>
#include <cassert>


#ifdef _WINDOWS
WCHAR w = L'\\';
#else
WCHAR w = L'/';
#endif

TrecPointer<TObject> BasicAnagameEnvironment::RetrieveResource(const TString& name)
{

	return TrecPointer<TObject>();
}
void BasicAnagameEnvironment::RetrieveResourceListSub(TDataArray<TrecPointer<TVariable>>& resources) {
	// First Resource, Java Script Files
	TrecPointer<TVariable> baseSpecs = TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TArrayVariable>();
	TrecPointer<TArrayVariable> resourceSpecs = TrecPointerKey::ConvertPointer<TVariable, TArrayVariable>(baseSpecs);

	resourceSpecs->Push(TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(L"Anaface Buider"));
	resourceSpecs->Push(TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(L"No Show"));
	resourceSpecs->Push(TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(L"Env-Source: anagame"));
	resourceSpecs->Push(TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(L"Resource-Type: Page"));
	resourceSpecs->Push(TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(L"Resource: anaface"));

	resources.push_back(baseSpecs);

	

	if (project.Get())
	{
		TDataArray<TString> nodeTypes;
		project->GetProjectNodeTypes(nodeTypes);

		for (UINT Rust = 0; Rust < nodeTypes.Size(); Rust++)
		{	
			baseSpecs = TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TArrayVariable>();
			resourceSpecs = TrecPointerKey::ConvertPointer<TVariable, TArrayVariable>(baseSpecs);

			resourceSpecs->Push(TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(L"Project Hierarchy"));
			resourceSpecs->Push(TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(TString(L"Details: ") + nodeTypes[Rust]));
			resourceSpecs->Push(TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(L"Env-Source: anagame"));
			resourceSpecs->Push(TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(L"Resource-Type: Page"));
			resourceSpecs->Push(TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(L"Resource: hierarchy"));
			resourceSpecs->Push(TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(L"IDE_Loc: Hierarchy"));

			resources.push_back(baseSpecs);
		}
		// Check if Project environment is in child environments. If not, then get resources
		bool projectIsChild = false;
		for (UINT Rust = 0; Rust < this->ChildEnvironmentCount(); Rust++)
		{
			if (this->GetChildEnvironmentAt(Rust).Get() == project.Get())
			{
				projectIsChild = true;
				break;
			}
		}
		if (!projectIsChild)
			project->RetrieveResourceList(resources);
	}
}

TrecPointer<TObject> BasicAnagameEnvironment::GetResource(const TString& name)
{
	TrecPointer<TObject> ret;
	
	if (this->anagame.Get())
	{
		ret = anagame->GetResource(name);
	}
	if (!ret.Get() && this->user.Get())
	{
		ret = user->GetResource(name);
	}
	if (!ret.Get() && project.Get())
		project->GetResource(name);

	if (!ret.Get())
		ret = TEnvironment::GetResource(name);
	
	return ret;
}

BasicAnagameEnvironment::BasicAnagameEnvironment()
{
	this->anagame = TrecPointerKey::GetNewTrecPointerAlt<TEnvironment, AppDataEnvironment>();
	this->user = TrecPointerKey::GetNewTrecPointerAlt<TEnvironment, UserProfileEnvironment>();
}

void BasicAnagameEnvironment::SetProject(TrecActivePointer<AGProjectEnvironment> projectEnvironment)
{
	if (this->project.Get())
		this->project->Save();

	this->project = projectEnvironment.GetTrecPointer();
}

void BasicAnagameEnvironment::SetProperty(const TString& name, TrecPointer<TVariable> var, env_target target)
{
	switch (target)
	{
	case env_target::anagame:
		anagame->SetProperty(name, var, true);
		break;
	case env_target::user:
		user->SetProperty(name, var, true);
		user->SaveProperties();
		break;
	case env_target::project:
		if (project.Get())
			project->SetProperty(name, var, true);
		break;
	default:
		TEnvironment::SetProperty(name, var, true);
	}
}

TrecPointer<TVariable> BasicAnagameEnvironment::GetProperty(const TString& name, env_target target)
{
	switch (target)
	{
	case env_target::user:
		return user.Get()->GetProperty(name);
	case env_target::anagame:
		return anagame->GetProperty(name);
	case env_target::project:
		if (project.Get())
			return project->GetProperty(name);
		return TrecPointer<TVariable>();
	default:
		return TEnvironment::GetProperty(name);
	}
}

TrecPointer<TVariable> BasicAnagameEnvironment::GetProperty(const TString& name, const TDataArray<env_target>& targets)
{
	TrecPointer<TVariable> ret;

	for (UINT Rust = 0; !ret.Get() && Rust < targets.Size(); Rust++)
	{
		ret = GetProperty(name, targets[Rust]);
	}
	return ret;
}

TrecPointer<AGProjectEnvironment> BasicAnagameEnvironment::RetrieveProjectEnvironment()
{
	return project;
}


TrecPointer<TObject> UserProfileEnvironment::RetrieveResource(const TString& name)
{
	return TrecPointer<TObject>();
}

void UserProfileEnvironment::RetrieveResourceListSub(TDataArray<TrecPointer<TVariable>>& resources)
{
}

UserProfileEnvironment::UserProfileEnvironment()
{
	TString file(GetDirectoryWithSlash(CentralDirectories::cd_User) + L".Anagame");

	ForgeDirectory(file);

	TrecPointer<TFileShell> targetFile = TFileShell::GetFileInfo(file + w + L"settings.json");

	if (!targetFile.Get())
	{
		TrecPointer<TFileShell> userSettingsDirectory = TFileShell::GetFileInfo(file);
		assert(userSettingsDirectory.Get() && userSettingsDirectory->IsDirectory());

		TFile creat(userSettingsDirectory, L"settings.json", TFile::t_file_create_always | TFile::t_file_write);

	}
	else
	{
		TrecPointer<TFormatReader> reader = TFormatReader::GetReader(targetFile);
		assert(reader.Get());
		TString message(reader->Read());
		if (message.GetSize())
			return;
		TrecPointer<TVariable> data = reader->GetData();

		assert(data.Get() && data->GetVarType() == var_type::json);
		TrecPointer<TJsonVariable> jData = TrecPointerKey::ConvertPointer<TVariable, TJsonVariable>(data);
		TString fieldName;
		
		for (UINT Rust = 0; jData->RetrieveFieldAt(Rust, fieldName, data); Rust++)
		{
			this->SetProperty(fieldName, data);
		}
	}
}

UserProfileEnvironment::~UserProfileEnvironment()
{
	TrecPointer<TVariable> vProps = TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TJsonVariable>();
	TrecPointer<TJsonVariable> props = TrecPointerKey::ConvertPointer<TVariable, TJsonVariable>(vProps);

	TDataEntry<TrecPointer<TVariable>> entry;
	for (UINT Rust = 0; properties.GetEntryAt(Rust, entry); Rust++)
		props->SetField(entry.key, entry.object);


	TString file(GetDirectoryWithSlash(CentralDirectories::cd_User) + L".Anagame" + w + L"settings.json");

	TrecPointer<TFileShell> targetFile = TFileShell::GetFileInfo(file);
	assert(targetFile.Get());
	TrecPointer<TFormatReader> reader = TFormatReader::GetReader(targetFile);
	assert(reader.Get());

	reader->Write(vProps);
	
}

void UserProfileEnvironment::SaveProperties()
{
	TString file(GetDirectoryWithSlash(CentralDirectories::cd_User) + L".Anagame");

	ForgeDirectory(file);

	TrecPointer<TFileShell> targetFile = TFileShell::GetFileInfo(file + w + L"settings.json");

	TrecPointer<TFormatReader> writer = TFormatReader::GetReader(targetFile);

	if (!writer.Get())
		return;
	TrecPointer<TVariable> propsVar = TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TJsonVariable>();
	TrecPointer<TJsonVariable> jsonProps = TrecPointerKey::ConvertPointer<TVariable, TJsonVariable>(propsVar);

	TDataEntry<TrecPointer<TVariable>> varEntry;
	for (UINT Rust = 0; properties.GetEntryAt(Rust, varEntry); Rust++)
	{
		jsonProps->SetField(varEntry.key, varEntry.object);
	}

	writer->Write(propsVar);
}

TrecPointer<TObject> AppDataEnvironment::RetrieveResource(const TString& name)
{
	return TrecPointer<TObject>();
}

void AppDataEnvironment::RetrieveResourceListSub(TDataArray<TrecPointer<TVariable>>& resources)
{
}

AppDataEnvironment::AppDataEnvironment()
{
	TString file(GetDirectoryWithSlash(CentralDirectories::cd_AppData) + L"Anagame");

	ForgeDirectory(file);

	TrecPointer<TFileShell> targetFile = TFileShell::GetFileInfo(file + w + L"settings.json");

	if (!targetFile.Get())
	{
		TrecPointer<TFileShell> userSettingsDirectory = TFileShell::GetFileInfo(file);
		assert(userSettingsDirectory.Get() && userSettingsDirectory->IsDirectory());

		TFile creat(userSettingsDirectory, L"settings.json", TFile::t_file_create_always | TFile::t_file_write);

	}
	else
	{
		TrecPointer<TFormatReader> reader = TFormatReader::GetReader(targetFile);
		assert(reader.Get());
		TString message(reader->Read());
		if (message.GetSize())
			return;
		TrecPointer<TVariable> data = reader->GetData();

		assert(data.Get() && data->GetVarType() == var_type::json);
		TrecPointer<TJsonVariable> jData = TrecPointerKey::ConvertPointer<TVariable, TJsonVariable>(data);
		TString fieldName;

		for (UINT Rust = 0; jData->RetrieveFieldAt(Rust, fieldName, data); Rust++)
		{
			this->SetProperty(fieldName, data);
		}
	}
}

AppDataEnvironment::~AppDataEnvironment()
{
	TrecPointer<TVariable> vProps = TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TJsonVariable>();
	TrecPointer<TJsonVariable> props = TrecPointerKey::ConvertPointer<TVariable, TJsonVariable>(vProps);

	TDataEntry<TrecPointer<TVariable>> entry;
	for (UINT Rust = 0; properties.GetEntryAt(Rust, entry); Rust++)
		props->SetField(entry.key, entry.object);


	TString file(GetDirectoryWithSlash(CentralDirectories::cd_User) + L"Anagame" + w + L"settings.json");

	TrecPointer<TFileShell> targetFile = TFileShell::GetFileInfo(file);
	//assert(targetFile.Get());
	TrecPointer<TFormatReader> reader = TFormatReader::GetReader(targetFile);
	//sassert(reader.Get());

	//reader->Write(vProps);
}

AGProjectEnvironment::AGProjectEnvironment(const TString& name, TrecActivePointer<TFileShell> directory)
{
	this->projectName.Set(name);
	this->directory = directory.GetTrecPointer();
}

TrecPointer<TFileShell> AGProjectEnvironment::GetDirectory()
{
	return directory;
}

TrecPointer<TObjectNode> AGProjectEnvironment::GetProjectNodes(const TString& name)
{
	return TrecPointer<TObjectNode>();
}

void AGProjectEnvironment::GetProjectNodeTypes(TDataArray<TString>& nodeTypes)
{
}
