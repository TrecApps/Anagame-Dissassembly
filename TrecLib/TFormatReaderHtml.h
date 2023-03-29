#pragma once
#include "TFormatReader.h"
#include "TContainerVariable.h"
#include "TFile.h"

/**
 * Class: TFormatReaderHtml
 * Purpose: Implements the TFormatReader for Html files
 */
class TFormatReaderHtml :
    public TFormatReader
{
    friend class TrecPointerKey;
public:
    /**
     * Class: TFormatReaderBuilderHtml
     * Purpose: Enables third Parties to provide their own Builders and Readers for formats not directly supported by Anagame
     */
    class TFormatReaderBuilderHtml : public TFormatReaderBuilder
    {
    public:
        /**
         * Method: TFormatReaderBuilderHtml::GetReader
         * Purpose: Yields a Reader that supports the File (or Null if not supported)
         * Parameters: TrecPointer<TFileShell> - file to read
         * Returns: TrecPointer<TFormatReader> - the reader reading this file
         *
         * Attributes: override
         */
        virtual TrecPointer<TFormatReader> GetReader(TrecPointer<TFileShell>) override;
    };

    /**
     * Method: TFormatReaderHtml::Read
     * Purpose: Begins reading the file
     * Parameters: void
     * Returns: TString - error information (empty string means success)
     *
     * Attributes: override
     */
    virtual TString Read() override;

    /**
     * Method: TFormatReaderHtml::GetData
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
     * Method: TFormatReaderHtml::TFormatReaderHtml
     * Purpose: Constructor
     * Parameters: TrecPointer<TFileShell> - the file to read
     * Returns: new Reader object
     */
    TFormatReaderHtml(TrecPointer<TFileShell> file);

    TrecPointer<TVariable> ProcessObject(TString& worked, TString& name);

    void ProcessSubObjects(TrecPointer<TJsonVariable>& ret, TString& worked, TString& name);
};

