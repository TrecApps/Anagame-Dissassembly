#pragma once
#include "TObject.h"
#include "TFile.h"


/**
 * Class: TFormatReader
 * Purpose: Provides means for reading Certain Text File type, such as JSON, XML, TML (currently exclusive to Anagame)
 */
class _TREC_LIB_DLL TFormatReader :
    public TObject
{
public:

    typedef enum class write_mode
    {
        formatted,
        line_formated,
        condensed
    }write_mode;


    /**
     * Class: TFormatReaderBuilder
     * Purpose: Enables third Parties to provide their own Builders and Readers for formats not directly supported by Anagame
     */
    class _TREC_LIB_DLL TFormatReaderBuilder: public TCoreObject
    {
    public:
        /**
         * Method: TFormatReaderBuilder::GetReader
         * Purpose: Yields a Reader that supports the File (or Null if not supported)
         * Parameters: TrecPointer<TFileShell> - file to read
         * Returns: TrecPointer<TFormatReader> - the reader reading this file
         *
         * Attributes: abstract
         */
        virtual TrecPointer<TFormatReader> GetReader(TrecPointer<TFileShell>) = 0;
    };

    /**
     * Method: TFormatReader::Read
     * Purpose: Begins reading the file
     * Parameters: void
     * Returns: TString - error information (empty string means success)
     *
     * Attributes: abstract
     */
    virtual TString Read() = 0;

    /**
     * Method: TFormatReader::GetData
     * Purpose: Retrieves the Data from the File after a successful read
     * Parameters: void
     * Returns: TrecPointer<TVariable> - the data held by the file (null if failed or no data)
     *
     * Attributes: abstract
     */
    virtual TrecPointer<TVariable> GetData() = 0;

    virtual TString Write(TrecPointer<TVariable> data, write_mode = write_mode::formatted);

    static void EstablishDefaultBuilders();

    /**
     * Method: TFormatReader::SubmitBuilder
     * Purpose: Adds a Builder to the List of builders held
     * Parameters: TrecPointer<TFormatReaderBuilder> builder - the builder to supply
     * Returns: UINT - index of the builder in the list (use this to later remove the builder)
     *
     * Attributes: static
     */
    static UINT SubmitBuilder(TrecPointer<TFormatReaderBuilder> builder);

    /**
     * Method: TFormatReader::GetReader
     * Purpose: retrieves a reader capable of parsing the provieded file (or null of builder has not been registered)
     * Parameters: TrecPointer<TFileShell> file - the file to read
     * Returns: TrecPointer<TFileReader> - the reader to read the file (or null if type is not supported)
     *
     * Attributes: static
     */
    static TrecPointer<TFormatReader> GetReader(TrecPointer<TFileShell>);

    static bool RemoveBuilder(UINT index);
};
