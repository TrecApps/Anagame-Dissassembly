#include "TFormatReaderXml.h"
#include "TString.h"
#include <cassert>

TString TFormatReaderXml::Read()
{
	file.Open(fileShell, TFile::t_file_open_existing | TFile::t_file_read);
	if (!file.IsOpen())
		return L"Failed to Open XML file!";

	TrecPointer<TJsonVariable> ret = TrecPointerKey::ConvertPointer<TVariable, TJsonVariable>(TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TJsonVariable>(true));
	TString readable;
	while (file.ReadString(readable, L"<", 1))
	{
		if (!readable.EndsWith(L'<'))
			return L"Unexpected End of File Detected!";
		TString worked, name;

		auto var = ProcessObject(worked, name);
		if (worked.GetSize())
			return worked;
		ret->SetField(name, var);
	}

	variable = TrecPointerKey::ConvertPointer<TJsonVariable, TVariable>(ret);
}

TrecPointer<TVariable> TFormatReaderXml::GetData()
{
	return variable;
}

TFormatReaderXml::TFormatReaderXml(TrecPointer<TFileShell> file)
{
	this->fileShell = file;
}

TrecPointer<TVariable> TFormatReaderXml::ProcessObject(TString& worked, TString& name)
{
	TString readable;
	file.ReadString(readable, L" />", 1);

	readable.Trim();
	if (readable.EndsWith(L'/'))
	{
		if (readable.GetSize() > 1)
		{
			name.Set(readable.SubString(0, readable.GetSize() - 1).GetTrim());
			file.ReadString(readable, L">", 0);
			if (readable.GetSize() > 1)
			{
				worked.Format(L"Unexpected Tokens detected when attmpting to end tag '%ws'", name.GetConstantBuffer().getBuffer());
			}
			if (!readable.GetSize())
				worked.Format(L"Unexpected End of File detected for tag '%ws'", name.GetConstantBuffer().getBuffer());

			return TrecPointer<TVariable>();
		}
		name.Set(L'/');
		file.ReadString(readable, L">", 1);

		if (!readable.EndsWith(L'>') || !readable.GetSize())
		{
			worked.Format(L"Unexpected End of File detected for tag '%ws'", name.GetConstantBuffer().getBuffer());
			return TrecPointer<TVariable>();
		}

		name.Append(readable.SubString(0, readable.GetSize() - 1));
		return TrecPointer<TVariable>();
	}

	name.Set(readable.SubString(0, readable.GetSize() - 1).GetTrim());


	TrecPointer<TJsonVariable> ret = TrecPointerKey::ConvertPointer<TVariable, TJsonVariable>(TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TJsonVariable>(true));

	if (readable.EndsWith(L'>'))
	{
		ProcessSubObjects(ret, worked, name);
		return worked.GetSize() ? TrecPointer<TVariable>() : TrecPointerKey::ConvertPointer<TJsonVariable, TVariable>(ret);
	}

	if (readable.EndsWith(L' '))
	{
		TrecPointer<TJsonVariable> meta = TrecPointerKey::ConvertPointer<TVariable, TJsonVariable>(TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TJsonVariable>());

		do
		{
			file.ReadString(readable, L"=/>", 1);
			readable.Trim();

			if (readable.EndsWith(L'='))
			{
				readable.Set(readable.SubString(0, readable.GetSize() - 1).GetTrim());

				while ((readable.StartsWith(L"\'") && readable.EndsWith(L"\'")) ||
					(readable.StartsWith(L"\"") && readable.EndsWith(L"\"")))
				{
					readable.Set(readable.SubString(1, readable.GetSize() - 1).GetTrim());
				}

				if (!readable.GetSize())
				{
					worked.Format(L"Malformed Attribute name detected for tag '%ws'", name.GetConstantBuffer().getBuffer());
					return TrecPointer<TVariable>();
				}
				TString subName(readable);

				file.ReadString(readable, L" />", 0b00000111);
				readable.GetTrim();

				while ((readable.StartsWith(L"\'") && readable.EndsWith(L"\'")) ||
					(readable.StartsWith(L"\"") && readable.EndsWith(L"\"")))
				{
					readable.Set(readable.SubString(1, readable.GetSize() - 1).GetTrim());
				}

				meta->SetField(subName, TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(readable));
			}

		} while (!readable.EndsWith(L'/') && !readable.EndsWith(L'>'));

		if (meta->GetSize())
		{
			ret->SetField(L"_attributes", TrecPointerKey::ConvertPointer<TJsonVariable, TVariable>(meta));
			ret->SetField(L"_metadata", TrecPointerKey::ConvertPointer<TJsonVariable, TVariable>(meta));
		}

		if (readable.EndsWith(L'/'))
		{
			file.ReadString(readable, L">", 0);
			if (readable.GetSize() > 1)
			{
				worked.Format(L"Unexpected Tokens detected when attmpting to end tag '%ws'", name.GetConstantBuffer().getBuffer());
			}
			if (!readable.GetSize())
				worked.Format(L"Unexpected End of File detected for tag '%ws'", name.GetConstantBuffer().getBuffer());

			return worked.GetSize() ? TrecPointer<TVariable>() : TrecPointerKey::ConvertPointer<TJsonVariable, TVariable>(ret);
		}

		ProcessSubObjects(ret, worked, name);

		return worked.GetSize() ? TrecPointer<TVariable>() : TrecPointerKey::ConvertPointer<TJsonVariable, TVariable>(ret);
	}


	//if()
}

void TFormatReaderXml::ProcessSubObjects(TrecPointer<TJsonVariable>& ret, TString& worked, TString& name)
{
	UINT textCount = 1;
	TString readable;
	while (true)
	{
		file.ReadString(readable, L"<", 0);

		if (!readable.EndsWith(L'<') || !readable.GetSize())
		{
			worked.Format(L"Unexpected End of File detected for tag '%ws'", name.GetConstantBuffer().getBuffer());
			return;
		}
		readable.Set(readable.SubString(0, readable.GetSize() - 1).GetTrim());
		TString textValue;
		if (readable.GetSize())
		{

			textValue.Format(L"_text%d", textCount++);
			ret->SetField(textValue, TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(readable));
		}
		textValue.Empty();

		auto var = ProcessObject(worked, textValue);
		if (worked.GetSize())
			return;

		// If this is true, then we have reached the closing tag and should go ahead and return what we have collected
		if (textValue.StartsWith(L'/') && !name.Compare(textValue.SubString(1).GetTrim()))
			return;


		ret->SetField(textValue, var);
	}
	assert(false);
	// We should never reach this point
}


TrecPointer<TFormatReader> TFormatReaderXml::TFormatReaderBuilderXml::GetReader(TrecPointer<TFileShell> file)
{
	if (file.Get() && file->GetName().EndsWith(L".xml", true))
		return TrecPointerKey::GetNewTrecPointerAlt<TFormatReader, TFormatReaderXml>(file);
	return TrecPointer<TFormatReader>();
}
