#include "TFormatReaderHtml.h"
#include "TString.h"
#include "TPrimitiveVariable.h"
#include <cassert>

#define WEB_VOID_ELEMENT_COUNT 14

TString htmlExt[] = {
	TString(L".html"),
	TString(L".htm"),
};

static TString VoidElements[WEB_VOID_ELEMENT_COUNT] = {
	TString(L"area"),
	TString(L"base"),
	TString(L"br"),
	TString(L"col"),
	TString(L"embed"),
	TString(L"hr"),
	TString(L"img"),
	TString(L"input"),
	TString(L"link"),
	TString(L"meta"),
	TString(L"param"),
	TString(L"source"),
	TString(L"track"),
	TString(L"wbr")//,
	// TString(L"colgroup")
};

TrecPointer<TVariable> htmlTrue = TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TPrimitiveVariable>(true);


TString TFormatReaderHtml::Read()
{
	file.Open(fileShell, TFile::t_file_open_existing | TFile::t_file_read);
	if (!file.IsOpen())
		return L"Failed to Open HTML file!";

	TrecPointer<TJsonVariable> ret = TrecPointerKey::ConvertPointer<TVariable, TJsonVariable>( TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TJsonVariable>(true));
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

TrecPointer<TVariable> TFormatReaderHtml::GetData()
{
	return variable;
}

TFormatReaderHtml::TFormatReaderHtml(TrecPointer<TFileShell> file)
{
	this->fileShell = file;
}

TrecPointer<TVariable> TFormatReaderHtml::ProcessObject(TString& worked, TString& name)
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
		bool voidFound = false;
		for (UINT Rust = 0; !voidFound && Rust < ARRAYSIZE(VoidElements); Rust++)
		{
			if (!name.CompareNoCase(VoidElements[Rust]))
				voidFound = true;
		}
		if (!voidFound)
			ProcessSubObjects(ret, worked, name);
		return worked.GetSize() ? TrecPointer<TVariable>() : TrecPointerKey::ConvertPointer<TJsonVariable, TVariable>(ret);
	}

	if (readable.EndsWith(L' '))
	{
		TrecPointer<TJsonVariable> meta = TrecPointerKey::ConvertPointer<TVariable, TJsonVariable>(TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TJsonVariable>(true));
		readable.Empty();
		do
		{
			readable.Trim();
			if (readable.GetSize())
			{
				meta->SetField(readable, htmlTrue);
			}

			file.ReadString(readable, L"=/>", 0b00000111);
			readable.Trim();

			if (readable.EndsWith(L'='))
			{
				int preEq = 0;
				readable.Trim();
				while ((preEq = readable.FindOneOfOutOfQuotes(L" \n\r\t", 0)) != -1)
				{
					TString att(readable.SubString(0, preEq));
					while ((att.StartsWith(L"\'") && att.EndsWith(L"\'")) ||
						(att.StartsWith(L"\"") && att.EndsWith(L"\"")))
					{
						att.Set(att.SubString(1, att.GetSize() - 1).GetTrim());
					}

					if (att.GetSize())
						meta->SetField(att, htmlTrue);
					readable.Delete(0, preEq);
					readable.Trim();
				}

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

void TFormatReaderHtml::ProcessSubObjects(TrecPointer<TJsonVariable>& ret, TString& worked, TString& name)
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
		if (textValue.StartsWith(L'/'))
			return;


		ret->SetField(textValue, var);
	}
	assert(false);
	// We should never reach this point
}

TrecPointer<TFormatReader> TFormatReaderHtml::TFormatReaderBuilderHtml::GetReader(TrecPointer<TFileShell> file)
{
	if (!file.Get())
		return TrecPointer<TFormatReader>();

	TString fileName(file->GetName());

	for (UINT Rust = 0; Rust < ARRAYSIZE(htmlExt); Rust++)
	{
		if (fileName.EndsWith(L".xml", true))
			return TrecPointerKey::GetNewTrecPointerAlt<TFormatReader, TFormatReaderHtml>(file);
	}

	return TrecPointer<TFormatReader>();
}
