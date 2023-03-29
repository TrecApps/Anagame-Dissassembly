#pragma once
#include "TFormatReader.h"
#include "TContainerVariable.h"
#include "TFile.h"

/**
 * Class: TFormatReaderXml
 * Purpose: Implements the TFormatReader for Xml files
 */
class TFormatReaderXml :
    public TFormatReader
{
    friend class TrecPointerKey;
public:
    /**
     * Class: TFormatReaderBuilderXml
     * Purpose: Enables third Parties to provide their own Builders and Readers for formats not directly supported by Anagame
     */
    class TFormatReaderBuilderXml : public TFormatReaderBuilder
    {
    public:
        /**
         * Method: TFormatReaderBuilderXml::GetReader
         * Purpose: Yields a Reader that supports the File (or Null if not supported)
         * Parameters: TrecPointer<TFileShell> - file to read
         * Returns: TrecPointer<TFormatReader> - the reader reading this file
         *
         * Attributes: override
         */
        virtual TrecPointer<TFormatReader> GetReader(TrecPointer<TFileShell>) override;
    };

    /**
     * Method: TFormatReaderXml::Read
     * Purpose: Begins reading the file
     * Parameters: void
     * Returns: TString - error information (empty string means success)
     *
     * Attributes: override
     */
    virtual TString Read() override;

    /**
     * Method: TFormatReaderXml::GetData
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
     * Method: TFormatReaderXml::TFormatReaderXml
     * Purpose: Constructor
     * Parameters: TrecPointer<TFileShell> - the file to read
     * Returns: new Reader object
     */
    TFormatReaderXml(TrecPointer<TFileShell> file);

    TrecPointer<TVariable> ProcessObject(TString& worked, TString& name);

    void ProcessSubObjects(TrecPointer<TJsonVariable>& ret, TString& worked, TString& name);
};

