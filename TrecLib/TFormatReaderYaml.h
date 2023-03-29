#pragma once
#include "TFormatReader.h"
#include "TFile.h"

/**
 * Class: TFormatReaderYaml
 * Purpose: Implements the TFormatReader for TML files
 */
class TFormatReaderYaml :
    public TFormatReader
{
    friend class TrecPointerKey;
public:
    /**
     * Class: TFormatReaderBuilderYaml
     * Purpose: Enables third Parties to provide their own Builders and Readers for formats not directly supported by Anagame
     */
    class TFormatReaderBuilderYaml : public TFormatReaderBuilder
    {
    public:
        /**
         * Method: TFormatReaderBuilderYaml::GetReader
         * Purpose: Yields a Reader that supports the File (or Null if not supported)
         * Parameters: TrecPointer<TFileShell> - file to read
         * Returns: TrecPointer<TFormatReader> - the reader reading this file
         *
         * Attributes: override
         */
        virtual TrecPointer<TFormatReader> GetReader(TrecPointer<TFileShell>) override;
    };

    /**
     * Method: TFormatReaderYaml::Read
     * Purpose: Begins reading the file
     * Parameters: void
     * Returns: TString - error information (empty string means success)
     *
     * Attributes: override
     */
    virtual TString Read() override;

    /**
     * Method: TFormatReaderYaml::GetData
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
     * Method: TFormatReaderYaml::TFormatReaderYaml
     * Purpose: Constructor
     * Parameters: TrecPointer<TFileShell> - the file to read
     * Returns: new Reader object
     */
    TFormatReaderYaml(TrecPointer<TFileShell> file);
};

