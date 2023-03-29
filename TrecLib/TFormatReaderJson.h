#pragma once
#include "TFormatReader.h"
#include "TFile.h"

/**
 * Class: TFormatReaderJson
 * Purpose: Implements the TFormatReader for Json files
 */
class TFormatReaderJson :
    public TFormatReader
{
    friend class TrecPointerKey;
public:
    /**
     * Class: TFormatReaderBuilderJson
     * Purpose: Enables third Parties to provide their own Builders and Readers for formats not directly supported by Anagame
     */
    class TFormatReaderBuilderJson : public TFormatReaderBuilder
    {
    public:
        /**
         * Method: TFormatReaderBuilderJson::GetReader
         * Purpose: Yields a Reader that supports the File (or Null if not supported)
         * Parameters: TrecPointer<TFileShell> - file to read
         * Returns: TrecPointer<TFormatReader> - the reader reading this file
         *
         * Attributes: override
         */
        virtual TrecPointer<TFormatReader> GetReader(TrecPointer<TFileShell>) override;
    };

    /**
     * Method: TFormatReaderJson::Read
     * Purpose: Begins reading the file
     * Parameters: void
     * Returns: TString - error information (empty string means success)
     *
     * Attributes: override
     */
    virtual TString Read() override;

    TString Write(TrecPointer<TVariable> data, write_mode mode)override;

    /**
     * Method: TFormatReaderJson::GetData
     * Purpose: Retrieves the Data from the File after a successful read
     * Parameters: void
     * Returns: TrecPointer<TVariable> - the data held by the file (null if failed or no data)
     *
     * Attributes: override
     */
    virtual TrecPointer<TVariable> GetData() override;

protected:

    /**
     * The Variable Generated
     */
    TrecPointer<TVariable> variable;

    /**
     * File Resources
     */
    TrecPointer<TFileShell> fileShell;
    TFile file;

    /**
     * Method: TFormatReaderJson::TFormatReaderJson
     * Purpose: Constructor
     * Parameters: TrecPointer<TFileShell> - the file to read
     * Returns: new Reader object
     */
    TFormatReaderJson(TrecPointer<TFileShell> file);

    TrecPointer<TVariable> ProcessArray(TString& worked);

    TrecPointer<TVariable> ProcessObject(TString& worked);

    bool ProcessName(TString& name);

    TrecPointer<TVariable> ProccessValue(const TString& v, bool& worked);
};

