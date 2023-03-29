#include "TFormatReaderTml.h"
#include "TContainerVariable.h"
#include "TString.h"

TrecPointer<TFormatReader> TFormatReaderTml::TFormatReaderBuilderTml::GetReader(TrecPointer<TFileShell> file)
{
    if (file.Get() && file->GetName().EndsWith(L".tml", true))
        return TrecPointerKey::GetNewTrecPointerAlt<TFormatReader, TFormatReaderTml>(file);
    return TrecPointer<TFormatReader>();
}

TString TFormatReaderTml::Read()
{
    file.Open(fileShell, TFile::t_file_open_existing | TFile::t_file_read);
    if (!file.IsOpen())
        return L"Failed to Open TML file!";

    TString lineStr;
    UINT line = 0;

    variable = TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TJsonVariable>(true);
    dynamic_cast<TJsonVariable*>(variable.Get())->SetField(L"Format", TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(L"TML"));

    TDataArray<TrecPointer<TJsonVariable>> variableStack; // Allows us to keep track of variables

    UINT dashCount = 1;

    TrecPointer<TJsonVariable> currentVariable = TrecPointerKey::ConvertPointer<TVariable, TJsonVariable>(variable);

    while (file.ReadString(lineStr))
    {
        line++;

        // Remove Comment from TML
        int index = lineStr.FindOutOfQuotes(L'#');
        if (index != -1)
            lineStr.Set(lineStr.SubString(0, index));

        int indexArrow = lineStr.Find(L'>'), indexLine = lineStr.Find(L'|');

        if (indexArrow != -1 && (indexArrow < indexLine || indexLine == -1))
        {
            // New Object
            TString name(lineStr.SubString(indexArrow).GetTrim());

            // Detemine relation to established variables
            UINT dashes = lineStr.CountFinds(L'-', indexArrow);

            if (dashes > dashCount + 1)
            {
                TString ret;
                ret.Format(L"TML Format Error. Can Only add one dash at a time! Detected on line %d!", line);
                return ret;
            }

            if (dashes > dashCount)
            {
                dashCount++;
                TrecPointer<TJsonVariable> childVariable = TrecPointerKey::ConvertPointer<TVariable, TJsonVariable>(TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TArrayVariable>());
                currentVariable->SetField(name, TrecPointerKey::ConvertPointer<TJsonVariable, TVariable>( childVariable ));
                variableStack.push_back(childVariable);

                currentVariable = childVariable;
                continue;
            }
            else if (dashes < dashCount)
            {
                while (dashes < dashCount)
                {
                    dashCount--;
                    UINT curStackSize = variableStack.Size();
                    if (curStackSize < 1)
                    {
                        TString ret;
                        ret.Format(L"Internal Error Detected on line %d!", line);
                        return ret;
                    }
                    variableStack.RemoveAt(curStackSize - 1);
                }



                if (variableStack.Size())
                    currentVariable = variableStack[variableStack.Size() - 1];
                else
                    currentVariable = TrecPointerKey::ConvertPointer<TVariable, TJsonVariable>(variable);
            }

            TrecPointer<TJsonVariable> childVariable = TrecPointerKey::ConvertPointer<TVariable, TJsonVariable>(TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TArrayVariable>());
            childVariable->SetField(L"_title", TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(name));
            if (currentVariable.Get() == variable.Get())
                currentVariable->SetField(name, TrecPointerKey::ConvertPointer<TJsonVariable, TVariable>(childVariable));
            else
                currentVariable->SetField(L"", TrecPointerKey::ConvertPointer<TJsonVariable, TVariable>(childVariable));
        }
        else if (indexLine != -1 && (indexLine < indexArrow || indexArrow == -1))
        {
            // New Attribute
            TString data(lineStr.SubString(indexLine + 1));
            auto pair = data.splitn(L":", 2);
            if (pair->Size() != 2)
            {
                TString ret;
                ret.Format(L"Incomplete Pair Detected on line %d!", line);
                return ret;
            }
            currentVariable->SetField(pair->at(0), TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(pair->at(1)));
        }
    }

    return TString();
}

TrecPointer<TVariable> TFormatReaderTml::GetData()
{
    return variable;
}

TFormatReaderTml::TFormatReaderTml(TrecPointer<TFileShell> file)
{
    this->fileShell = file;
}

