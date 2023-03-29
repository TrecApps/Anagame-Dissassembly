#pragma once
#include "TFormatReader.h"
#include "TFile.h"

/**
 * Class: TFormatReaderTml
 * Purpose: Implements the TFormatReader for TML files
 */
class TFormatReaderTml :
    public TFormatReader
{
    friend class TrecPointerKey;
public:
    /**
     * Class: TFormatReaderBuilderTML
     * Purpose: Enables third Parties to provide their own Builders and Readers for formats not directly supported by Anagame
     */
    class TFormatReaderBuilderTml : public TFormatReaderBuilder
    {
    public:
        /**
         * Method: TFormatReaderBuilderTML::GetReader
         * Purpose: Yields a Reader that supports the File (or Null if not supported)
         * Parameters: TrecPointer<TFileShell> - file to read
         * Returns: TrecPointer<TFormatReader> - the reader reading this file
         *
         * Attributes: override
         */
        virtual TrecPointer<TFormatReader> GetReader(TrecPointer<TFileShell>) override;
    };

    /**
     * Method: TFormatReaderTml::Read
     * Purpose: Begins reading the file
     * Parameters: void
     * Returns: TString - error information (empty string means success)
     *
     * Attributes: override
     */
    virtual TString Read() override;

    /**
     * Method: TFormatReaderTml::GetData
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
     * Method: TFormatReaderTml::TFormatReaderTml
     * Purpose: Constructor
     * Parameters: TrecPointer<TFileShell> - the file to read
     * Returns: new Reader object
     */
    TFormatReaderTml(TrecPointer<TFileShell> file);
};

