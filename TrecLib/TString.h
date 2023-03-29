#pragma once
#include "TDataArray.h"
#include "TObject.h"
#include <string>
#include "TrecPointer.h"

constexpr auto T_NO_ERROR = 0;
constexpr auto T_OVERFLOW = 1;
constexpr auto NOT_NUMB = 2;

/*
* Enum Class: ColorFormat
* Purpose: Used to keep track of how the color originally was presented
*/
using ColorFormat = enum class ColorFormat
{
	cform_hex,
	cform_hexa,
	cform_rgb,
	cform_rgba,
	cform_hsl,
	cform_hsla,
	cform_ana,
	cform_ana_a
};

class _TREC_LIB_DLL IndexRange {
public:
	IndexRange();
	IndexRange(const IndexRange& copy);
	IndexRange(int s, int e);
	int start;
	int end;

	bool active;
};

/**
 * Enum Class: number_base
 * Purpose: Signal to which number base to use
 */
using number_class = enum class number_base
{
	nb_generic, // Let the method decide which base to use
	nb_binary,	// assume string is 1's and 0's
	nb_octal,	// assume string is 0-7
	nb_decimal, // Use scale 0-9
	nb_hexadecimal // Use scale 0-f
};

/**
 * Class: TString
 * Purpose: Represents the UNICODE version of the String class, with features such as split,
 *	number extraction, substring, and trim
 *
 * SuperClass: TObject - allows it to be used by Environment Scripts
 */
class _TREC_LIB_DLL TString : public TObject
{
	friend class TConstBuffer;
public:


	/**
	 * Class: TConstBuffer
	 * Purpose: Provides Thread Protection to calls to GetConstBuffer
	 */
	class _TREC_LIB_DLL TConstBuffer
	{
	public:
		TConstBuffer(const TString* string);
		TConstBuffer(const TConstBuffer& buff);
		~TConstBuffer();

		const WCHAR* getBuffer();
	private:
		const TString* string;
	};


	int FindInContainer(TDataArray<TString>& container);


	/**
	 * Method: TString::ConvertStringToUint
	 * Purpose: Allows TString class to convert itself into a numeral representation
	 * Parameters: const TString& string - the string to convert
	 *				UINT& num - the number to hold the data in
	 *				number_base base - the base to assume
	 * Returns: bool - whether the string could be converted into an int
	 *
	 * Note: if the String starts with a number specifier (0x or 0b), then the base param must
	 *		match the specifier or be 'generic'. Otherwise it will fail and false is returned.
	 *		Also, if 'generic' is set, then binary will only be considered if the string specifies binary
	 */
	static bool ConvertStringToUint(const TString& string, UINT& num, number_base base = number_base::nb_generic);

	TString();

	virtual ~TString();

	TString(UINT num);

	TString(float num);

	TString(const TString*);

	TString(const char*);

	TString(const WCHAR*);

	TString(const TString& c);

	TString(std::string& str);

	TString(WCHAR c);

	short ConvertToInt(int&)const;

	short ConvertToInt(long& value)const;

	short ConvertToDouble(double&)const;

	short ConvertToLong(long long&)const;

	short ConvertToFloat(float&)const;

	/*
	* Method: TString::split
	* Purpose: Splits a String by the provided deliniators
	* Parameters: TString str - the TString holding deliniators
	*			UCHAR flags - flags to use to control the behavior of this method
				WCHAR exitQuote - quote to look for if String is believed to begin in quotes
	* Returns: TrecPointer<TArray<TString>> - Array of TStrings holding tokens
	*
	* flags: 0b00000001 - t_file_check_back_slash - ignore a hit if odd number of backslashes are present
	*		 0b00000010 - t_file_out_of_quotes	  - ignore hits found within a quotation string
	*		 0b00000100 - t_file_starts_in_quote  - assume that String starts within a quote
	*
	* Attributes: const
	*/
	TrecPointer<TDataArray<TString>> split(TString, UCHAR flags = 0, WCHAR exitQuote = L'\"') const;

	/*
	* Method: TString::splitn
	* Purpose: Splits a String by the provided deliniators, providing a limit as to how many times the string can be split
	* Parameters: TString str - the TString holding deliniators
	*			UINT elements - the max number of elements to split by (0 for no limit)
	*			UCHAR flags - flags to use to control the behavior of this method
				WCHAR exitQuote - quote to look for if String is believed to begin in quotes
	* Returns: TrecPointer<TArray<TString>> - Array of TStrings holding tokens
	*
	* flags: 0b00000001 - t_file_check_back_slash - ignore a hit if odd number of backslashes are present
	*		 0b00000010 - t_file_out_of_quotes	  - ignore hits found within a quotation string
	*		 0b00000100 - t_file_starts_in_quote  - assume that String starts within a quote
	*
	* Attributes: const
	*/
	TrecPointer<TDataArray<TString>> splitn(TString, UINT elements = 0, UCHAR flags = 0, WCHAR exitQuote = L'\"') const;

	bool IsBackSlashChar(UINT index);

	WCHAR* GetBufferCopy()const;

	TConstBuffer GetConstantBuffer()const;

	TString SubString(UINT beginningIndex, int endIndex = -1) const;

	int FindOutOfQuotes(const TString& subString, int start = 0, bool ignoreEscape = true) const;

	int FindOneOfOutOfQuotes(const TString& chars, int start) const;

	void Set(const TString& s);

	void Set(const WCHAR* w);

	void Set(const WCHAR w);

	TString& operator=(const TString& t);

	TString operator=(const WCHAR* w);

	TString operator=(const WCHAR w);

	TString operator+(const TString&) const;

	TString operator+(const WCHAR*)const;

	TString operator+(const WCHAR w)const;

	TString operator+=(const TString&);

	TString operator+=(const WCHAR*);

	TString operator+=(const WCHAR w);

	bool operator==(const TString& s);


	bool operator==(const WCHAR* s);

	bool operator!=(const TString& s);

	bool operator!=(const WCHAR* s);


	WCHAR& operator[](UINT loc)const;

	UINT GetSize() const;

	WCHAR GetAt(UINT c)const;

	void AppendFormat(const WCHAR* format, ...);

	void AppendFormat(const TString format, ...);

	void Format(const WCHAR* format, ...);

	void Format(const TString format, ...);

	void AppendChar(const WCHAR ch);

	void Append(const TString&);

	int Compare(const TString& other)const;

	int Compare(const WCHAR* other)const;

	int CompareNoCase(const TString& other) const;

	static int Compare(const TString& str1, const TString& str2);

	virtual int Delete(UINT index, int count = 1);

	TString GetDelete(int& ret, int index, int count = 1);

	bool StartsWith(const TString& seq, bool ignoreCase = false, bool whitespace = false) const;

	bool StartsAt(const TString& seq, UINT index, bool ignoreCase = false, bool whitespace = false) const;

	bool EndsWith(const TString& seq, bool ignoreCase = false) const;

	int Find(const TString& sub, int start = 0, bool ignoreEscape = true, bool notAlphaNum = false) const;

	int Find(WCHAR sub, int start = 0, bool ignoreEscape = true)const;

	int FindOneOf(const TString& chars, int start = 0)const;

	int FindOneOf(const TDataArray<TString>& strs, UINT& index, int start = 0, bool ignoreEscape = true)const;

	int FindLast(const TString& sub, int start = -1) const;

	int FindLast(WCHAR sub, int start = -1)const;

	int FindLastOneOf(const TString& chars, int start = -1) const;


	UINT CountFinds(const TString& query, int stop = -1) const;

	UINT CountFinds(WCHAR ch, int stop = -1) const;

	UINT CountOneOfFinds(const TString& query) const;

	virtual int Insert(int index, const TString& subStr);

	TString GetInsert(int& ret, int index, TString& subStr);

	int Insert(int index, WCHAR ch);

	TString GetInsert(int& ret, int index, WCHAR ch);

	void SetLower();

	TString GetLower()const;

	void SetUpper(UINT mode = 0);

	TString GetUpper(UINT mode = 0)const;

	void SetReverse();

	TString GetReverse();

	int Remove(WCHAR c, bool outOfQuotes = false);

	TString GetRemove(int& ret, WCHAR c, bool outOfQuotes = false);

	int Replace(const TString& oldStr, const TString& newStr, bool doAll = true);

	TString GetReplace(int& ret, const TString& oldStr, const TString& newStr, bool doAll = true) const;
	
	int Replace(WCHAR& oldStr, WCHAR& newStr);

	TString GetReplace(int& ret, WCHAR& oldStr, WCHAR& newStr);


	/**
	 * Method: TString::Tokenize
	 * Purpose: Allows the Traditional Tokenization routine to be performed in the TString
	 * Parameters: TString& tokens - characters to split by
	 *				 int& start - the location to begin at
	 * Returns:
	 *
	 * Attributes: const
	 */
	TString Tokenize(TString& tokens, int& start, bool outOfQuotes) const;

	void Trim();

	TString GetTrim() const;

	void TrimRight();

	TString GetTrimRight();

	void TrimLeft();

	TString GetTrimLeft();

	void Empty();

	bool IsEmpty() const;

	std::string GetRegString()const;

private:
	UINT size;			// Note: these values focus on the number of WCHARs
	UINT capacity;
	WCHAR* string;
};

class _TREC_LIB_DLL TStringVariable : public TVariable
{
public:

	static TString Extract(TrecPointer<TVariable> var, const TString& fallback);

	virtual TrecPointer<TVariable> Clone()override;

	TStringVariable(const TString& string);

	virtual var_type GetVarType() override;

	virtual TrecPointer<TVariable> GetIterator();

	virtual TString& GetString();

	virtual UINT Get4Value()override;

	virtual UINT GetSize()override;

	virtual TrecPointer<TVariable> ToString();

	virtual TrecPointer<TVariable> ToString(TrecPointer<TVariable> detail);

private:
	/**
	 * The String held by the variable
	 */
	TString string;
};


class _TREC_LIB_DLL TVObject : public TObject
{
public:

	virtual bool GetVariable(const TString& name, TrecPointer<TVariable>& var);

	virtual bool SetVariable(const TString& name, TrecPointer<TVariable> var);
};



bool _TREC_LIB_DLL IsWhitespace(WCHAR ch);

bool convertToNumber(TCHAR c, int* i);

bool convertToNumberHex(TCHAR c, UINT& i);

float ConvertHueToRGB(float p, float q, int hue);

WCHAR ReturnWCharType(char c);

bool _TREC_LIB_DLL IndexComesFirst(int focusIndex, int checkIndex);