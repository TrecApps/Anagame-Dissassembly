#include "TFormatReaderYaml.h"

TString TFormatReaderYaml::Read()
{
    return TString();
}

TrecPointer<TVariable> TFormatReaderYaml::GetData()
{
    return TrecPointer<TVariable>();
}

TFormatReaderYaml::TFormatReaderYaml(TrecPointer<TFileShell> file)
{
}

TrecPointer<TFormatReader> TFormatReaderYaml::TFormatReaderBuilderYaml::GetReader(TrecPointer<TFileShell>)
{
    return TrecPointer<TFormatReader>();
}
