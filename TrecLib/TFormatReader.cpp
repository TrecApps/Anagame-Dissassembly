#include "TFormatReader.h"

#include "TFormatReaderTml.h"
#include "TFormatReaderJson.h"
#include "TFormatReaderXml.h"
#include "TFormatReaderHtml.h"
#include "TFormatReaderProps.h"
#include "TFormatReaderYaml.h"

TDataArray <TrecPointer<TFormatReader::TFormatReaderBuilder>> builders;

TString TFormatReader::Write(TrecPointer<TVariable> data, write_mode)
{
    return L"Not Implemented";
}

void TFormatReader::EstablishDefaultBuilders()
{
    static bool defaultBuildersAdded = false;

    if (defaultBuildersAdded)
        return;

    defaultBuildersAdded = true;

    SubmitBuilder(TrecPointerKey::GetNewTrecPointerAlt<TFormatReaderBuilder, TFormatReaderTml::TFormatReaderBuilderTml>());
    SubmitBuilder(TrecPointerKey::GetNewTrecPointerAlt<TFormatReaderBuilder, TFormatReaderJson::TFormatReaderBuilderJson>());
    SubmitBuilder(TrecPointerKey::GetNewTrecPointerAlt<TFormatReaderBuilder, TFormatReaderXml::TFormatReaderBuilderXml>());
    SubmitBuilder(TrecPointerKey::GetNewTrecPointerAlt<TFormatReaderBuilder, TFormatReaderHtml::TFormatReaderBuilderHtml>());
    SubmitBuilder(TrecPointerKey::GetNewTrecPointerAlt<TFormatReaderBuilder, TFormatReaderProps::TFormatReaderBuilderProps>());
    SubmitBuilder(TrecPointerKey::GetNewTrecPointerAlt<TFormatReaderBuilder, TFormatReaderYaml::TFormatReaderBuilderYaml>());
}

UINT TFormatReader::SubmitBuilder(TrecPointer<TFormatReaderBuilder> builder)
{
    if (!builder.Get())
        return 0;

    EstablishDefaultBuilders();

    for (UINT Rust = 0; Rust < builders.Size(); Rust++)
    {
        if (!builders[Rust].Get())
        {
            builders[Rust] = builder;
            return Rust;
        }
    }

    return builders.push_back(builder) - 1;
}

TrecPointer<TFormatReader> TFormatReader::GetReader(TrecPointer<TFileShell> file)
{
    if (!file.Get())
        return TrecPointer<TFormatReader>();
    TString path(file->GetPath());

    int period = path.FindLast(L'.');
    if (period == -1)
        return TrecPointer<TFormatReader>();

    path.Set(path.SubString(period + 1));

    EstablishDefaultBuilders();

    for (UINT Rust = 0; Rust < builders.Size(); Rust++)
    {
        if (builders[Rust].Get())
        {
            TrecPointer<TFormatReader> reader = builders[Rust]->GetReader(file);
            if (reader.Get())
                return reader;
        }
    }
    return TrecPointer<TFormatReader>();
}

bool TFormatReader::RemoveBuilder(UINT index)
{
    if (index < 6 || index >= builders.Size())
    {
        return false;
    }
    builders[index].Nullify();
    return true;
}