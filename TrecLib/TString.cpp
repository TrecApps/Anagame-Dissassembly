#include "TString.h"
#include <cassert>
#include <stdlib.h>
#include "FileFunctions.hpp"


#ifndef _WINDOWS


#include <cstring>

#include <cwchar>
#include <wctype.h>
#include <stdarg.h>

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#endif

#ifdef _WINDOWS

#define TC_WCSTOMBS(size, dest, source, winFlag) WideCharToMultiByte(CP_ACP, 0, source, size, dest, size, NULL, &winFlag)

#elif defined(__linux__) || (defined (__APPLE__) && defined (__MACH__))

#define TC_WCSTOMBS(size, dest, source, winFlag) wcstombs(dest, source, size)

#endif


static TDataArray<WCHAR> whiteChar;


void fillWhiteChar()
{
	
	if (whiteChar.Size())
		return;

	whiteChar.push_back(L' ');
	whiteChar.push_back(L'\n');
	whiteChar.push_back(L'\r');
	whiteChar.push_back(L'\t');

}


/*
* Method: TString::Constructor
* Purpose: Default Constructor for the TString Class
* Parameters: void
* Returns: void
*/

TString::TString()
{
	string = nullptr;
	Empty();
	fillWhiteChar();
}


bool ConvertTStringToBinary(const TString& string, UINT& val)
{
	if (string.GetSize() > 32)
		return false;
	UINT tempVal = 0;
	for (UINT Rust = string.GetSize() - 1, mark = 0x00000001; Rust < string.GetSize() && mark; Rust--, mark = mark << 1)
	{
		WCHAR ch = string.GetAt(Rust);
		switch (ch)
		{
		case L'0':
			break;
		case L'1':
			tempVal = tempVal | mark;
			break;
		default:
			return false;
		}
	}
	val = tempVal;
	return true;
}

bool ConvertTStringToOctal(const TString& string, UINT& val)
{
	UINT stringSize = string.GetSize();

	// Max number is 37777777777.
	if (stringSize > 11)
		return false;
	if (stringSize == 11)
	{
		WCHAR ch = string.GetAt(0);
		if (ch > L'3' || ch < L'0')
			return false;
	}

	UINT tempVal = 0, shift = 0;

	for (UINT Rust = string.GetSize() - 1; Rust < stringSize; Rust--, shift += 3)
	{
		WCHAR ch = string.GetAt(Rust);
		if (ch > L'7' || ch < L'0')
			return false;
		UINT charVal = ch - L'0';

		tempVal += (charVal << shift);
	}
	val = tempVal;
	return true;
}

bool ConvertTStringToHex(const TString& string, UINT& val)
{
	if (string.GetSize() > 8)
		return false;
	UINT tempVal = 0, shift = 0;

	for (UINT Rust = string.GetSize() - 1; Rust < string.GetSize(); Rust--, shift += 4)
	{
		WCHAR ch = string.GetAt(Rust);
		UINT charVal = 0;

		if (ch <= L'9' && ch >= L'0')
			charVal = ch - L'0';
		else if (ch <= L'f' && ch >= L'a')
			charVal = ch - L'a' + 10;
		else
			return false;

		tempVal += (charVal << shift);
	}
	val = tempVal;
	return true;
}

int TString::FindInContainer(TDataArray<TString>& container)
{
	for (UINT Rust = 0; Rust < container.Size(); Rust++)
	{
		if (!Compare(container[Rust]))
			return Rust;
	}
	return -1;
}

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
 *		Also, if 'generic' is set and string has no specifier, then decimal will be considered followed by hex.
 *		Finally, if you dont want octal, then add a binary/hex specifier tot he string, remove all leading zeros, or
 *		don't use 'generic' as the base parameter
 */
bool TString::ConvertStringToUint(const TString& string, UINT& num, number_base base)
{
	number_base assumeBase = number_base::nb_generic;

	// Easier to just deal with lowercase than worry about both lower and upper case
	TString tempString(string.GetLower().GetTrim());
	if (tempString.StartsWith(L"0x", true) || tempString.StartsWith(L"x", true))
	{
		assumeBase = number_base::nb_hexadecimal;
		tempString.Delete(0, tempString.Find(L'x') + 1);
	}
	else if (tempString.StartsWith(L"0b", true) || tempString.StartsWith(L"b", true))
	{
		assumeBase = number_base::nb_binary;
		tempString.Delete(0, tempString.Find(L'b') + 1);
	}
	else if (tempString.StartsWith(L'0'))
	{
		assumeBase = number_base::nb_octal;
	}


	// Now make sure that there is no inconsistancy in what the user is asking for and what the string provides
	bool works = false;
	switch (base)
	{
	case number_base::nb_generic:	// User has no preference, use the assumed
		works = true;
		break;
	case number_base::nb_binary:	// User has a preference. Make sure our assumption is generic or octal or matches the user preference
	case number_base::nb_decimal:
	case number_base::nb_hexadecimal:
	case number_base::nb_octal:
		if (assumeBase == number_base::nb_generic || assumeBase == number_base::nb_octal)
			assumeBase = base;
		works = (assumeBase == base);
	}

	if (!works)
		return false;
	int iNum = 0;

	switch (assumeBase)
	{
	case number_base::nb_binary:
		works = ConvertTStringToBinary(tempString, num);
		break;
	case number_base::nb_decimal:
		works = (tempString.ConvertToInt(iNum) == 0);
		if (works)
			num = static_cast<UINT>(iNum);
		break;
	case number_base::nb_hexadecimal:
		works = ConvertTStringToHex(tempString, num);
		break;
	case number_base::nb_octal:
		works = ConvertTStringToOctal(tempString, num);
		break;
	default: // i.e. Generic
		works = (tempString.ConvertToInt(iNum) == 0);
		if (works)
			num = static_cast<UINT>(iNum);
		else
			works = ConvertTStringToHex(tempString, num);
	}

	return works;
}

TString::~TString()
{
	TObjectLocker threadLock(&thread);
	if (string)
		delete[] string;
	return;
}

TString::TString(UINT num)
{
	this->capacity = this->size = 0;
	this->string = nullptr;
	Format(L"%d", num);
}

TString::TString(float num)
{
	this->capacity = this->size = 0;
	this->string = nullptr;
	Format(L"%f", num);
}

TString::TString(const TString* orig)
{
	fillWhiteChar();
	if (!orig)
	{
		size = capacity = 0;
		string = nullptr;
		return;
	}

	capacity = orig->capacity;
	size = orig->size;

	string = new WCHAR[capacity];
	memcpy(string, (orig->string), capacity * sizeof(WCHAR));
	string[size] = L'\0';
}


TString::TString(const char* cps)
{
	fillWhiteChar();
	if (!cps)
	{
		string = nullptr;
		Empty();
		return;
	}
	
	size_t s = TcMultiByteToWideChar(cps, strlen(cps), nullptr);

	size = s;
	capacity = size + 1;

	string = new WCHAR[capacity];
	
	s = TcMultiByteToWideChar(cps, strlen(cps), string, size);
	string[capacity - 1] = L'\0';
}

TString::TString(const WCHAR* wcps)
{
	fillWhiteChar();
	if (!wcps)
	{
		string = nullptr;
		Empty();
		return;
	}

	size = wcslen(wcps);
	capacity = size + 1;
	string = new WCHAR[capacity];
	memcpy(string, wcps, size * sizeof(WCHAR));
	string[capacity - 1] = L'\0';
}

TString::TString(const TString& c)
{
	fillWhiteChar();
	size = c.size;
	capacity = c.capacity;

	string = new WCHAR[capacity];
	memcpy(string, c.string, capacity * sizeof(WCHAR));
	string[capacity - 1] = L'\0';
}

TString::TString(std::string& str)
{
	fillWhiteChar();

	size_t s = TcMultiByteToWideChar(str.c_str(), str.size(), nullptr);

	size = s;
	capacity = size + 1;

	string = new WCHAR[capacity];

	s = TcMultiByteToWideChar(str.c_str(), str.size(), string, size);
	string[capacity - 1] = L'\0';
}

TString::TString(WCHAR c)
{
	fillWhiteChar();
	size = 1;
	capacity = 5;
	string = new WCHAR[capacity];
	memset(string, 0, capacity * sizeof(WCHAR));
	string[0] = c;
}

short TString::ConvertToInt(int& value)const
{
	TObjectLocker threadLock(&thread);
	if (!size)
	{
		return NOT_NUMB;
	}int temp = value;

	value = 0;
	int hold;
	bool positive = true;

	for (UINT c = 0; c < size; c++)
	{
		if (convertToNumber(this->GetAt(c), &hold))
		{
			value = (value * 10) + hold;
		}
		else if (!c && this->GetAt(c) == L'-')
		{
			positive = false;
		}
		else
		{
			value = temp;
			return NOT_NUMB;
		}
	}
	if (positive)
		value = abs(value);
	else
		value = -(abs(value));
	return T_NO_ERROR;
}

short TString::ConvertToInt(long& value)const
{
	TObjectLocker threadLock(&thread);
	if (!size)
	{
		return NOT_NUMB;
	}
	int temp = value;

	value = 0;
	int hold;
	bool positive = true;

	for (UINT c = 0; c < size; c++)
	{
		if (convertToNumber(this->GetAt(c), &hold))
		{
			value = (value * 10) + hold;
		}
		else if (!c && this->GetAt(c) == L'-')
		{
			positive = false;
		}
		else
		{
			value = temp;
			return NOT_NUMB;
		}
	}
	if (positive)
		value = abs(value);
	else
		value = -(abs(value));
	return T_NO_ERROR;
}

short TString::ConvertToLong(long long& value)const
{
	TObjectLocker threadLock(&thread);
	if (!size)
	{
		return NOT_NUMB;
	}
	long long temp = value;

	value = 0;
	int hold;

	bool positive = true;

	for (UINT c = 0; c < size; c++)
	{
		if (convertToNumber(this->GetAt(c), &hold))
		{
			value = (value * 10) + hold;
		}
		else if (!c && this->GetAt(c) == L'-')
		{
			positive = false;
		}
		else
		{
			value = temp;
			return NOT_NUMB;
		}
	}
	if (positive)
		value = abs(value);
	else
		value = -(abs(value));
	return T_NO_ERROR;

}

short TString::ConvertToDouble(double& value)const
{
	TObjectLocker threadLock(&thread);
	if (!size)
	{
		return NOT_NUMB;
	}
	double temp = value;

	value = 0;
	double dec = 0.1;
	bool fullInt = true;
	int hold;

	bool positive = true;

	for (UINT c = 0; c < size; c++)
	{
		if (fullInt && convertToNumber(this->GetAt(c), &hold))
		{
			value = (value * 10) + hold;
		}
		else if (convertToNumber(this->GetAt(c), &hold)) // moved to decimal portion
		{
			value = value + (double)hold * dec;
			dec = dec / 10;
		}
		else if (this->GetAt(c) == L'.')
		{
			fullInt = false;
		}
		else if (!c && this->GetAt(c) == L'-')
		{
			positive = false;
		}
		else
		{
			value = temp;
			return NOT_NUMB;
		}

	}
	if (positive)
		value = abs(value);
	else
		value = -(abs(value));

	return T_NO_ERROR;
}

short TString::ConvertToFloat(float& value)const
{
	TObjectLocker threadLock(&thread);
	if (!size)
	{
		return NOT_NUMB;
	}
	float temp = value;

	value = 0;
	float dec = 0.1f;
	bool fullInt = true;
	int hold;
	bool positive = true;

	for (UINT c = 0; c < size; c++)
	{
		if (fullInt && convertToNumber(this->GetAt(c), &hold))
		{
			value = (value * 10) + hold;
		}
		else if (convertToNumber(this->GetAt(c), &hold)) // moved to decimal portion
		{
			value = value + (float)hold * dec;
			dec = dec / 10;
		}
		else if (this->GetAt(c) == L'.')
		{
			fullInt = false;
		}
		else if (!c && this->GetAt(c) == L'-')
		{
			positive = false;
		}
		else
		{
			value = temp;
			return NOT_NUMB;
		}

	}

	if (positive)
		value = abs(value);
	else
		value = -(abs(value));

	return T_NO_ERROR;
}

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
*        0b00001000 - t_file_include_empty
*/
TrecPointer<TDataArray<TString>> TString::split(TString str, UCHAR flags, WCHAR exitQuote) const
{
	return splitn(str, 0, flags, exitQuote);
}

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
*        0b00001000 - t_file_include_empty
*
* Attributes: const
*/
TrecPointer<TDataArray<TString>> TString::splitn(TString str, UINT elements, UCHAR flags, WCHAR exitQuote) const
{
	TObjectLocker threadLock(&thread);
	TrecPointer<TDataArray<TString>> ret;
	ret = TrecPointerKey::GetNewTrecPointer<TDataArray<TString>>();

	if (!this->GetSize())
	{
		return ret;
	}


	if (elements == 1)
	{
		ret->push_back(TString(this));
		return ret;
	}
	bool hasLimit = elements > 0;

	TString tok;

	int pos = ((flags & 0b00000110) == 0b00000110) ? Find(exitQuote) : 0;

	bool insertEmpty = flags & 0b00001000;

	int begPos = pos;
	tok.Set(this->Tokenize(str, pos, flags & 0b00000010));
	while ((!tok.IsEmpty() || (insertEmpty && pos != -1)) && begPos != -1 && (!hasLimit || elements--))
	{
		if (flags & 0b00000001)
		{

			while (tok.IsBackSlashChar(tok.GetSize()))
			{
				tok.Set(this->Tokenize(str, pos, flags & 0b00000010));
				tok.Set(this->SubString(begPos, pos));
			}
		}

		for (UINT Rust = 0; Rust < str.size; Rust++)
		{

			tok.Remove(str[Rust], flags & 0b00000010);
		}

		(ret).Get()->push_back(tok);
		begPos = pos;
		if (hasLimit && elements == 1)
		{
			while (pos == ((flags & 2) ? FindOneOf(str, pos) : FindOneOfOutOfQuotes(str, pos)))
				pos++;
			ret->push_back(SubString(pos));
			break;
		}
		else
			tok.Set(this->Tokenize(str, pos, flags & 0b00000010));
	}

	return ret;
}

bool TString::IsBackSlashChar(UINT index)
{
	TObjectLocker threadLock(&thread);
	if (index > size)
	{
		return false;
	}
	UINT comp = 0;

	for (int C = index - 1; C >= 0 && this->string[C] == L'\\'; C--)
		comp++;

	return comp % 2 == 1;
}

WCHAR* TString::GetBufferCopy() const
{
	TObjectLocker threadLock(&thread);
	WCHAR* returnable = new WCHAR[size + 1];
	for (UINT c = 0; c < GetSize(); c++)
		returnable[c] = string[c];
	returnable[size] = L'\0';
	return returnable;
}

TString::TConstBuffer TString::GetConstantBuffer() const
{
	return TConstBuffer(this);
}

TString TString::SubString(UINT beginningIndex, int endIndex) const
{
	TObjectLocker threadLock(&thread);

	TString returnable;
	if (endIndex == -1)
	{
		for (UINT c = beginningIndex; c < GetSize(); c++)
			returnable.AppendChar(GetAt(c));
	}
	else
	{
		endIndex = abs(endIndex);
		if (static_cast<UINT>(endIndex) > GetSize())
			endIndex = GetSize();
		for (UINT c = beginningIndex; c < static_cast<UINT>(endIndex); c++)
			returnable.AppendChar(GetAt(c));
	}
	return returnable;
}

void TString::Trim()
{
	TObjectLocker threadLock(&thread);
	TrimRight();
	TrimLeft();
	return;
}

TString TString::GetTrim() const
{
	TObjectLocker threadLock(&thread);
	TString ret(this);
	ret.Trim();
	return ret;
}

void TString::TrimRight()
{
	TObjectLocker threadLock(&thread);
	int end = size - 1;
	while (end >= 0 && (iswspace(string[end]) || string[end] == 0xFEFF))
	{
		end--;
	}
	end++;
	size = end;
	string[size] = L'\0';
	return;
}

TString TString::GetTrimRight()
{
	TObjectLocker threadLock(&thread);
	TString ret(this);
	ret.TrimRight();
	return ret;
}

void TString::TrimLeft()
{
	TObjectLocker threadLock(&thread);
	UINT end = 0;
	while (end < size && (iswspace(string[end]) || string[end] == 0xFEFF)) end++;
	UINT sEnd = end;
	for (UINT Rust = 0; end < size; Rust++, end++)
	{
		string[Rust] = string[end];
	}
	size -= sEnd;
	string[size] = L'\0';
	return;
}

TString TString::GetTrimLeft()
{
	TObjectLocker threadLock(&thread);
	TString ret(this);
	ret.TrimLeft();
	return ret;
}

void TString::Empty()
{
	TObjectLocker threadLock(&thread);
	if (string)
		delete[] string;
	string = nullptr;
	size = capacity = 0;

	string = new WCHAR[3];
	capacity = 3;
	string[0] = string[1] = string[2] = L'\0';
	return;
}

bool TString::IsEmpty() const 
{
	return size == 0;
}



std::string TString::GetRegString()const
{
	TObjectLocker threadLock(&thread);
	std::string reg;
	char* regString = new char[size + 1];
	BOOL bFlag = FALSE;
	TC_WCSTOMBS(size, regString, string, bFlag);

	regString[size] = '\0';
	reg = regString;
	delete[] regString;
	return reg;
}

int TString::FindOutOfQuotes(const TString& subString, int start, bool ignoreEscape) const
{
	TObjectLocker threadLock(&thread);
	TDataArray<int> possibleIndeces;
	while (start != -1)
	{
		start = Find(subString, start, ignoreEscape);
		if (start != -1)
		{
			possibleIndeces.push_back(start);
			start++;
		}
	}

	TDataArray<IndexRange> quotes;
	WCHAR quoteMode = 0;
	IndexRange range{ -1,-1 };
	for (UINT rust = 0; rust < GetSize() && rust != -1; rust++)
	{
		switch (quoteMode)
		{
		case L'\'':
			range.start = rust - 1;
			rust = Find(L'\'', rust);
			if (rust != -1)
			{
				range.end = rust;
				quotes.push_back(range);
				quoteMode = 0;
			}
			break;
		case L'\"':
			range.start = rust - 1;
			rust = Find(L'\"', rust);
			if (rust != -1)
			{
				range.end = rust;
				quotes.push_back(range);
				quoteMode = 0;
			}
			break;
		default: // should be 0
			if (GetAt(rust) == L'\'')
				quoteMode = L'\'';
			else if (GetAt(rust) == L'\"')
				quoteMode = L'\"';
		}
	}

	for (UINT c = 0; c < possibleIndeces.Size(); c++)
	{
		int ind = possibleIndeces[c];
		bool works = true;
		for (UINT rust = 0; rust < quotes.Size(); rust++)
		{
			if (ind > quotes[rust].start && ind < quotes[rust].end)
			{
				works = false;
				continue;
			}
		}
		if (works) { return ind; }
	}
	return -1;
}

int TString::FindOneOfOutOfQuotes(const TString& chars, int start) const
{
	TObjectLocker threadLock(&thread);
	int min = INT32_MAX;

	bool found = false;

	for (UINT Rust = 0; Rust < chars.GetSize(); Rust++)
	{
		int end = FindOutOfQuotes(chars[Rust], start, false);

		if (end != -1)
		{
			found = true;
			if (end < min)
				min = end;
		}
	}

	return found ? min : -1;
}

void TString::Set(const TString& t)
{
	TObjectLocker threadLock(&thread);

	if (string)
		delete[] string;
	
	string = new WCHAR[t.capacity];
	this->capacity = t.capacity;
	this->size = t.size;
	for (UINT Rust = 0; Rust < capacity; Rust++)
		string[Rust] = t.string[Rust];

	return;
}

void TString::Set(const WCHAR* w)
{
	TObjectLocker threadLock(&thread);
	if (w)
	{
		TString str(w);
		Set(&str);
	}
	return;
}

void TString::Set(WCHAR w)
{
	TObjectLocker threadLock(&thread);
	this->Empty();
	this->AppendChar(w);
	return;
}

TString& TString::operator=(const TString& t)
{
	TObjectLocker threadLock(&thread);
	Set(t);
	return *this;
}

TString TString::operator=(const WCHAR* w)
{
	TObjectLocker threadLock(&thread);
	Set(w);
	return this;
}

TString TString::operator=(WCHAR w)
{
	TObjectLocker threadLock(&thread);
	Set(w);
	return this;
}

TString TString::operator+(const TString& t)const
{
	TObjectLocker threadLock(&thread);
	TString returnstring = this;
	for (UINT c = 0; c < t.GetSize(); c++)
		returnstring.AppendChar(t.GetAt(c));
	return returnstring;
}

TString TString::operator+(const WCHAR* w)const
{
	TObjectLocker threadLock(&thread);
	if (w)
	{
		TString returnstring = this;
		for (int c = 0; w[c] != L'\0'; c++)
			returnstring.AppendChar(w[c]);
		return returnstring;
	}
	return this; // return this string since there was nothing to add
}

TString TString::operator+(WCHAR w) const
{
	TObjectLocker threadLock(&thread);
	TString returnstring = this;
	returnstring.AppendChar(w);
	return returnstring;
}

TString TString::operator+=(const TString& t)
{
	TObjectLocker threadLock(&thread);
	for (UINT c = 0; c < t.GetSize(); c++)
		AppendChar(t.GetAt(c));
	return this;
}

TString TString::operator+=(const WCHAR* w)
{
	TObjectLocker threadLock(&thread);
	if (w)
	{
		for (int c = 0; w[c] != L'\0'; c++)
			this->AppendChar(w[c]);
	}
	return this;
}

TString TString::operator+=(WCHAR w)
{
	TObjectLocker threadLock(&thread);
	AppendChar(w);
	return this;
}

bool TString::operator==(const TString& s)
{
	TObjectLocker threadLock(&thread);
	if (GetSize() != s.GetSize())
	{
		return false;
	}
	for (UINT c = 0; c < GetSize(); c++)
		if (GetAt(c) != s[c])
		{
			return false;
		}
	return true;
}


bool TString::operator==(const WCHAR* s)
{
	TObjectLocker threadLock(&thread);
	if (!s)
	{
		return false;
	}
	for (UINT c = 0; c < GetSize(); c++, s++)
	{
		if (*s == L'\0')
		{
			return false;
		}
		if (*s != GetAt(c))
		{
			return false;
		}
	}
	return *s == L'\0';
}

bool TString::operator!=(const TString& s)
{
	TObjectLocker threadLock(&thread);
	return !(*this == s);
}

bool TString::operator!=(const WCHAR* s)
{
	TObjectLocker threadLock(&thread);
	return !(*this == s);
}

WCHAR& TString::operator[](UINT loc) const
{
	ThreadLock();
	if (loc > size)
	{
		ThreadRelease();
		throw 2;
	}
	ThreadRelease();
	return string[loc];
}

UINT TString::GetSize() const
{
	return size;
}

WCHAR TString::GetAt(UINT c) const
{
	TObjectLocker threadLock(&thread);
	if (c < size)
	{
		return string[c];
	}
	return L'\0';
}

void TString::AppendFormat(const WCHAR* format, ...)
{

	if (!format)
		return;
	va_list vList;
	va_start(vList, format);
	TObjectLocker threadLock(&thread);
	TString tFormat(format);

	WCHAR* formatedString = new WCHAR[tFormat.capacity * 2 + 100];

	int result = vswprintf(formatedString, (tFormat.capacity * 2) + 99, tFormat.string, vList);

	va_end(vList);


	//TString ret;
	if (result > -1)
	{
		formatedString[result] = L'\0';
		Append(formatedString);
	}

	delete[] formatedString;
	return;
}

void TString::AppendFormat(const TString format, ...)
{
	va_list vList;
	va_start(vList, format);
	TObjectLocker threadLock(&thread);;
	WCHAR* formatedString = new WCHAR[format.capacity * 2 + 100];
	int result = vswprintf(formatedString, (format.capacity * 2) + 99, format.string, vList);

	va_end(vList);


	TString ret;
	if (result > -1)
	{
		formatedString[result] = L'\0';
		Append(formatedString);
	}

	delete[] formatedString;
	return;
}

void TString::Format(const WCHAR* format, ...)
{

	if (!format)
		return;
	va_list vList;
	va_start(vList, format);
	TObjectLocker threadLock(&thread);
	TString tFormat(format);

	WCHAR* formatedString = new WCHAR[tFormat.capacity * 2 + 100];


	int result = vswprintf(formatedString, (tFormat.capacity * 2) + 99, tFormat.string, vList);

	va_end(vList);

	TString ret;
	if (result > -1)
	{
		formatedString[result] = L'\0';
		Set(formatedString);
	}

	delete[] formatedString;
	return;
}

void TString::Format(const TString format, ...)
{
	va_list vList;
	va_start(vList, format);
	TObjectLocker threadLock(&thread);
	WCHAR* formatedString = new WCHAR[format.capacity * 2 + 100];



	int result = vswprintf(formatedString, (format.capacity * 2) + 99, format.string, vList);

	va_end(vList);


	TString ret;
	if (result > -1)
	{
		formatedString[result] = L'\0';
		Set(formatedString);
	}

	delete[] formatedString;
	return;
}

void TString::AppendChar(WCHAR ch)
{
	TObjectLocker threadLock(&thread);
	if (size + 1 == capacity)
	{
		WCHAR* newString = new WCHAR[++capacity];

		memcpy(newString, string, (capacity - 1) * sizeof(WCHAR));
		delete[] string;
		string = newString;
	}

	string[size++] = ch;
	string[size] = L'\0'; return;
}

void TString::Append(const TString& app)
{
	TObjectLocker threadLock(&thread);
	capacity += app.capacity;

	WCHAR* newString = new WCHAR[capacity];

	for (UINT c = 0; c < size; c++)
	{
		newString[c] = string[c];
	}

	UINT newSize = size + app.size;

	for (UINT c = size, rust = 0; c < newSize && rust < app.size; c++, rust++)
	{
		newString[c] = app[rust];
	}
	size = newSize;
	newString[size] = L'\0';
	delete[] string;
	string = newString;
	return;
}

bool IsWhitespace(WCHAR ch)
{
	fillWhiteChar();

	for (UINT Rust = 0; Rust < whiteChar.Size(); Rust++)
	{
		if (whiteChar[Rust] == ch)
			return true;
	}

	return false;
}

bool convertToNumber(TCHAR c, int* i)
{
	switch (c)
	{
	case '0':
		*i = 0;
		return true;
	case '1':
		*i = 1;
		return true;
	case '2':
		*i = 2;
		return true;
	case '3':
		*i = 3;
		return true;
	case '4':
		*i = 4;
		return true;
	case '5':
		*i = 5;
		return true;
	case '6':
		*i = 6;
		return true;
	case '7':
		*i = 7;
		return true;
	case '8':
		*i = 8;
		return true;
	case '9':
		*i = 9;
		return true;
	default:
		return false;
	}
}

bool convertToNumberHex(TCHAR c, UINT& i)
{
	switch (c)
	{
	case L'0':
		i = 0;
		return true;
	case L'1':
		i = 1;
		return true;
	case L'2':
		i = 2;
		return true;
	case L'3':
		i = 3;
		return true;
	case L'4':
		i = 4;
		return true;
	case L'5':
		i = 5;
		return true;
	case L'6':
		i = 6;
		return true;
	case L'7':
		i = 7;
		return true;
	case L'8':
		i = 8;
		return true;
	case L'9':
		i = 9;
		return true;
	case L'a':
	case L'A':
		i = 10;
		return true;
	case L'b':
	case L'B':
		i = 11;
		return true;
	case L'c':
	case L'C':
		i = 12;
		return true;
	case L'd':
	case L'D':
		i = 13;
		return true;
	case L'e':
	case L'E':
		i = 14;
		return true;
	case L'f':
	case L'F':
		i = 15;
		return true;
	}
	return false;
}

float ConvertHueToRGB(float p, float q, int hue)
{
	float javaScriptHue = static_cast<float>(hue) / 360.0f;

	if (javaScriptHue < 1.0f / 6.0f)
		return p + (q - p) * 6 * javaScriptHue;
	if (javaScriptHue < 0.5f)
		return q;
	if (javaScriptHue < 2.0f / 3.0f)
		return p + (q - p) * (2.0f / 3.0f - javaScriptHue) * 6.0f;
	return p;

}


WCHAR ReturnCharType(char c)
{
	WCHAR w[] = { L'0',L'\0' };
	char charTo[] = { c, '\0' };
	TcMultiByteToWideChar(charTo, 1, w, 1);
	return w[0];
}

WCHAR ReturnWCharType(char c)
{
	WCHAR w[] = { L'0',L'\0' };
	char charTo[] = { c, '\0' };
	TcMultiByteToWideChar(charTo, 1, w, 1);
	return w[0];
}

bool _TREC_LIB_DLL IndexComesFirst(int focusIndex, int checkIndex)
{
	if (focusIndex == -1)
		return false;
	return checkIndex == -1 || focusIndex < checkIndex;
}

TString formatSpecifiers(L"diuoxXfFeEgGaAcCsSpT");

int TString::Compare(const TString& other) const
{
	return Compare(other.string);
}

/**
 * Method: TString::Compare
 * Purpose: Compares this string with a provided raw string
 * Parameters: const WCHAR* other - the string to compare this string to
 * Returns: int - 0 if they are the same
 */
int TString::Compare(const WCHAR* other) const
{
	TObjectLocker threadLock(&thread);
	int min = min(size, wcslen(other));
	for (int c = 0; c < min; c++)
	{
		if (string[c] < other[c])
		{
			return -(c + 1);
		}
		if (string[c] > other[c])
		{
			return c + 1;
		}
	}

	if (size == wcslen(other)) { return 0; }
	if (size > wcslen(other)) { return size; }
	return static_cast<int>(-1);
}

int TString::CompareNoCase(const TString& other) const
{
	TObjectLocker threadLock(&thread);
	TString _this(this);
	TString _other(other);
	_this.SetLower();
	_other.SetLower();
	return _this.Compare(_other);
}

int TString::Compare(const TString& str1, const TString& str2)
{
	return str1.Compare(str2);
}
int TString::Delete(UINT index, int count)
{
	TObjectLocker threadLock(&thread);
	if (count < 1)
	{
		return size;
	}
	if (index < 0 || index > size)
	{
		return size;
	}
	WCHAR* newString = new WCHAR[capacity];
	for (int c = 0; c < index; c++)
	{
		newString[c] = static_cast<WCHAR>(string[c]);
	}

	for (int c = index, rust = index + count; rust < capacity; c++, rust++)
	{
		newString[c] = string[rust];
	}

	delete[] string;
	string = newString;

	size -= count;
	string[size] = L'\0';
	return size;
}

TString TString::GetDelete(int& ret, int index, int count)
{
	TObjectLocker threadLock(&thread);
	TString retStr(this);
	ret = retStr.Delete(index, count);
	return retStr;
}

bool TString::StartsWith(const TString& seq, bool ignoreCase, bool whitespace) const
{
	return StartsAt(seq, 0, ignoreCase, whitespace);
}

bool TString::StartsAt(const TString& seq, UINT index, bool ignoreCase, bool whitespace) const
{
	TObjectLocker threadLock(&thread);
	if (seq.GetSize() + index > size)
	{
		return false;
	}
	if (ignoreCase)
	{
		TString lSeq(seq.GetLower());
		TString lThis(this->GetLower());

		for (UINT Rust = 0; Rust < lSeq.GetSize(); Rust++)
		{
			if (lThis[Rust + index] != lSeq[Rust]) { return false; }
		}
	}
	else
	{
		for (UINT Rust = 0; Rust < seq.GetSize(); Rust++)
		{
			if (string[Rust + index] != seq[Rust]) { return false; }
		}
	}

	if (whitespace)
	{
		if (seq.GetSize() == size)
		{
			return false;
		}
		WCHAR ch = this->string[seq.GetSize()];

		for (UINT Rust = 0; Rust < whiteChar.Size(); Rust++)
		{
			if (whiteChar[Rust] == ch)
			{
				return true;
			}
		}

		return false;
	}

	return true;
}

bool TString::EndsWith(const TString& seq, bool ignoreCase)const
{
	TObjectLocker threadLock(&thread);
	if (seq.GetSize() > size)
	{
		return false;
	}
	if (ignoreCase)
	{
		TString lSeq(seq.GetLower());
		TString lThis(this->GetLower());
		for (UINT C = lThis.GetSize() - 1, Rust = lSeq.GetSize() - 1; C < size && Rust < size; C--, Rust--)
		{
			if (lThis[C] != lSeq[Rust])
			{
				return false;
			}
		}
	}
	else
	{
		for (UINT C = size - 1, Rust = seq.GetSize() - 1; C < size && Rust < size; C--, Rust--)
		{
			if (string[C] != seq[Rust])
			{
				return false;
			}
		}
	}
	return true;
}

int TString::Find(const TString& sub, int start, bool ignoreEscape, bool notAlphaNum) const
{
	TObjectLocker threadLock(&thread);
	int indexStart = start;

	while ((indexStart = Find(sub[0], indexStart, ignoreEscape)) != -1)
	{
		bool works = true;
		for (int c = 0, rust = indexStart; c < sub.GetSize() || rust < size; c++, rust++)
		{
			if (c == sub.GetSize())
			{
				works = true;
				break;
			}
			if (rust == size)
			{
				works = false;
				break;
			}
			if (sub[c] != string[rust])
			{
				works = false;
				break;
			}
		}

		if (works)
		{
			if (!notAlphaNum) // If we don't care about what surrounds our entry, go ahead and return it
			{
				return indexStart;
			}// Otherwise, check to see if it is isolated from other alpha numeric characters
			// Use this to track the ends. 1 = clear from beginning, 2 means clear from end, need three to return
			UCHAR uWorks = 0;
			if (indexStart == 0 || !isalnum(string[indexStart - 1]))
				uWorks += 1;
			int indexEnd = indexStart + sub.GetSize();
			if (indexEnd >= size || !isalnum(string[indexEnd]))
				uWorks += 2;
			if (uWorks == 3)
			{
				return indexStart;
			}
		}
		indexStart++;
	}

	return -1;
}

int TString::Find(WCHAR sub, int start, bool ignoreEscape) const
{
	TObjectLocker threadLock(&thread);
	if (start < 0)
	{
		return -1;
	}
	for (int c = start; c < size; c++)
	{
		if (string[c] == sub)
		{
			bool cont = false;
			if (!ignoreEscape && c > 0 && string[c - 1] == L'\\')
			{
				UINT slashCount = 1;
				for (int Rust = c - 2; Rust > -1 && string[Rust] == L'\\'; Rust--)
					slashCount++;
				cont = slashCount % 2 != 0;
			}
			if (cont)
				continue;
			return c;
		}
	}
	return -1;
}

int TString::FindOneOf(const TString& chars, int start) const
{
	TObjectLocker threadLock(&thread);
	if (start < 0)
	{
		return -1;
	}
	for (int c = start; c < size; c++)
	{
		for (UINT rust = 0; rust < chars.GetSize(); rust++)
		{
			if (string[c] == chars[rust])
			{
				return c;
			}
		}
	}


	return -1;
}

int TString::FindOneOf(const TDataArray<TString>& strs, UINT& index, int start, bool ignoreEscape)const
{
	int ret = -1;
	for (UINT Rust = 0; Rust < strs.Size(); Rust++)
	{
		int tempRet = Find(strs[Rust], start, ignoreEscape);
		if (tempRet == -1)
			continue;

		if (ret == -1 || tempRet < ret)
		{
			ret = tempRet;
			index = Rust;
		}
	}
	return ret;
}

int TString::FindLast(const TString& sub, int start) const
{
	TObjectLocker threadLock(&thread);
	if (start >= static_cast<int>(size))
	{
		return -1;
	}
	if (start < 0)
		start = size - 1;

	for (int c = start; c >= 0; c--)
	{
		if (sub[0] == string[c])
		{
			bool works = true;
			for (int rust = c, C = 0; rust < size && C < sub.size; rust++, C++)
			{
				if (string[rust] != sub[C])
				{
					works = false;
					break;
				}
			}
			if (works)
			{
				return c;
			}
		}
	}
	return -1;
}

int TString::FindLast(WCHAR sub, int start) const
{
	TObjectLocker threadLock(&thread);
	if (start >= static_cast<int>(size))
	{
		return -1;
	}
	for (int rust = ((start == -1) ? size - 1 : start); rust >= 0; rust--)
	{
		if (string[rust] == sub)
		{
			return rust;
		}
	}
	return -1;
}

int TString::FindLastOneOf(const TString& chars, int start) const
{
	TObjectLocker threadLock(&thread);
	if (start >= static_cast<int>(size))
	{
		return -1;
	}
	if (start < 0)
		start = size - 1;

	for (int c = start; c >= 0; c--)
	{
		for (UINT rust = 0; rust < chars.GetSize(); rust++)
		{
			if (string[c] == chars[rust])
			{
				return c;
			}
		}
	}

	return -1;
}


UINT TString::CountFinds(const TString& query, int stop) const
{
	TObjectLocker threadLock(&thread);
	UINT ret = 0;

	int start = 0;
	if (stop < 0)
		stop = size;

	while ((start = Find(query, start)) != -1 && start < stop)
	{
		ret++;
		start++;
	}

	return ret;
}

UINT TString::CountFinds(WCHAR ch, int stop) const
{
	TObjectLocker threadLock(&thread);
	UINT ret = 0;

	int start = 0;
	if (stop < 0)
		stop = size;

	while ((start = Find(ch, start)) != -1 && start < stop)
	{
		ret++;
		start++;
	}

	return ret;
}

UINT TString::CountOneOfFinds(const TString& query) const
{
	TObjectLocker threadLock(&thread);
	UINT ret = 0;

	int start = 0;

	while ((start = FindOneOf(query, start)) != -1)
	{
		ret++;
		start++;
	}

	return ret;
}

int TString::Insert(int index, const TString& subStr)
{
	TObjectLocker threadLock(&thread);
	if (index < 0 || index > size)
	{
		return size;
	}
	WCHAR* newString = new WCHAR[capacity + subStr.capacity];


	for (int c = 0; c < index; c++)
	{
		newString[c] = string[c];
	}

	for (int c = index, rust = 0; rust < subStr.GetSize(); c++, rust++)
	{
		newString[c] = subStr.GetAt(rust);
	}

	for (int c = index + subStr.GetSize(), rust = index; rust < size; c++, rust++)
	{
		newString[c] = string[rust];
	}

	delete[]string;
	string = newString;
	size = size + subStr.GetSize();

	capacity += subStr.capacity;
	string[size] = L'\0';
	return size;
}

TString TString::GetInsert(int& ret, int index, TString& subStr)
{
	TObjectLocker threadLock(&thread);
	TString retStr(this);
	ret = retStr.Insert(index, subStr);
	return retStr;
}

int TString::Insert(int index, WCHAR ch)
{
	TObjectLocker threadLock(&thread);
	TString sub(ch);
	return Insert(index, sub);
}

TString TString::GetInsert(int& ret, int index, WCHAR ch)
{
	TObjectLocker threadLock(&thread);
	TString retStr(this);
	ret = retStr.Insert(index, ch);
	return retStr;
}

void TString::SetLower()
{
	TObjectLocker threadLock(&thread);
	for (UINT c = 0; c < size; c++)
	{
		string[c] = towlower(string[c]);
	}
}

TString TString::GetLower() const
{
	TObjectLocker threadLock(&thread);
	TString ret(this);
	ret.SetLower();
	return ret;
}

void TString::SetUpper(UINT mode)
{
	TObjectLocker threadLock(&thread);
	bool firstLetter = false;
	bool onSpace = true;
	for (UINT c = 0; c < size; c++)
	{
		if (mode)
		{
			if (IsWhitespace(string[c]))
				onSpace = true;
			else if (onSpace)
			{
				onSpace = false;
				firstLetter = true;
			}
			else firstLetter = false;
		}
		if(!mode || firstLetter)
			string[c] = towupper(string[c]);
	}
}

TString TString::GetUpper(UINT mode)const
{
	TObjectLocker threadLock(&thread);
	TString ret(this);
	ret.SetUpper(mode);
	return ret;
}

void TString::SetReverse()
{
	TObjectLocker threadLock(&thread);
	for (UINT Rust = 0; Rust < size / 2; Rust++)
	{
		WCHAR temp = string[Rust];
		string[Rust] = string[size - (Rust + 1)];
		string[size - (Rust + 1)] = temp;
	}
}

TString TString::GetReverse()
{
	TObjectLocker threadLock(&thread);
	TString ret(this);
	ret.SetReverse();
	return ret;
}

int TString::Remove(WCHAR c, bool outOfQuotes)
{
	TObjectLocker threadLock(&thread);
	WCHAR* newString = new WCHAR[capacity];

	int newCount = 0;

	WCHAR q = L'\0';

	for (int rust = 0; rust < size; rust++)
	{
		if (outOfQuotes)
		{
			if (!q && (string[rust] == L'\'' || string[rust] == L'\"'))
				q = string[rust];
			else if (q == string[rust])
				q = L'\0';
		}

		if (!q && string[rust] == c)
			continue;
		newString[newCount++] = string[rust];
	}
	size = newCount;
	delete[] string;
	string = newString;
	return size;
}

TString TString::GetRemove(int& ret, WCHAR c, bool outOfQuotes)
{
	TObjectLocker threadLock(&thread);
	TString retStr(this);
	ret = retStr.Remove(c, outOfQuotes);
	return retStr;
}

int TString::Replace(const TString& oldStr, const TString& newStr, bool doAll)
{
	TObjectLocker threadLock(&thread);
	TDataArray<int> indices;

	int index = 0;
	while (true)
	{
		index = Find(oldStr, index);
		if (index == -1)
			break;
		indices.push_back(index);
		index += oldStr.GetSize();
		if (!doAll)
			break;
	}

	// If the old string does not appear, make no changes
	if (!indices.Size())
	{
		return 0;
	}
	int difference = (static_cast<int>(newStr.size) - static_cast<int>(oldStr.size)) * indices.Size();
	int newSize = size + difference;

	WCHAR* newString = new WCHAR[capacity + difference];

	int currentIndex = indices[0];
	int indexIndex = 1;
	for (int rust = 0, c = 0; rust < size && c < newSize; rust++, c++)
	{
		if (rust == currentIndex)
		{
			for (UINT C = 0; C < newStr.size && c < newSize; C++, c++)
			{
				newString[c] = newStr[C];
			}
			c--; // So then the outer for loop increments again, it will be where it should
			if (indexIndex >= indices.Size())
			{
				currentIndex = -1;
			}
			else
			{
				currentIndex = indices[indexIndex++];
			}
			rust += (oldStr.size - 1);
			continue;
		}

		newString[c] = string[rust];
	}

	size = newSize;
	newString[size] = L'\0';
	delete[] string;
	string = newString;
	capacity += difference;

	return indices.Size();
}

TString TString::GetReplace(int& ret, const TString& oldStr, const TString& newStr, bool doAll) const
{
	TObjectLocker threadLock(&thread);
	TString retStr(this);
	ret = retStr.Replace(oldStr, newStr, doAll);
	return retStr;
}

int TString::Replace(WCHAR& oldStr, WCHAR& newStr)
{
	TObjectLocker threadLock(&thread);
	int ret = 0;

	for (UINT Rust = 0; Rust < size; Rust++)
	{
		if (string[Rust] == oldStr)
		{
			ret++;
			string[Rust] = newStr;
		}
	}

	return ret;
}

TString TString::GetReplace(int& ret, WCHAR& oldStr, WCHAR& newStr)
{
	TObjectLocker threadLock(&thread);
	TString retStr(this);
	ret = retStr.Replace(oldStr, newStr);
	return retStr;
}

TString TString::Tokenize(TString& tokens, int& start, bool outOfQuotes) const
{
	TObjectLocker threadLock(&thread);
	int end;
	if (!outOfQuotes)
	{
		while ((end = FindOneOf(tokens, start)) == start)
		{
			start++;
		}
	}
	else
	{
		while ((end = FindOneOfOutOfQuotes(tokens, start)) == start)
			start++;
	}

	TString ret = SubString(start, end);

	start = end;


	return ret;
}

TString::TConstBuffer::TConstBuffer(const TString* string)
{
	if (!string)
		throw 0;
	this->string = string;
	string->ThreadLock();
}

TString::TConstBuffer::TConstBuffer(const TConstBuffer& buff)
{
	this->string = buff.string;
	string->ThreadLock();
}

TString::TConstBuffer::~TConstBuffer()
{
	string->ThreadRelease();
}

const WCHAR* TString::TConstBuffer::getBuffer()
{
	return string->string;
}

IndexRange::IndexRange()
{
	start = end = 0;
	active = false;
}

IndexRange::IndexRange(const IndexRange& copy)
{
	this->start = copy.start;
	this->end = copy.end;
	this->active = copy.active;
}

IndexRange::IndexRange(int s, int e)
{
	start = s;
	end = e;
	active = false;
}

TString TStringVariable::Extract(TrecPointer<TVariable> var, const TString& fallback)
{
	if (var.Get())
	{
		auto strVar = TrecPointerKey::ConvertPointer<TVariable, TStringVariable>(var->ToString());

		if (strVar.Get())
			return strVar->GetString();
	}
	return fallback;
}

TrecPointer<TVariable> TStringVariable::Clone()
{
	return TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TStringVariable>(string);
}

TStringVariable::TStringVariable(const TString& string)
{
	this->string.Set(string);
}

var_type TStringVariable::GetVarType()
{
	return var_type::string;
}

TrecPointer<TVariable> TStringVariable::GetIterator()
{
	return TrecPointer<TVariable>();
}

TString& TStringVariable::GetString()
{
	return string;
}

UINT TStringVariable::Get4Value()
{
	return 0;
}

UINT TStringVariable::GetSize()
{
	return string.GetSize();
}

TrecPointer<TVariable> TStringVariable::ToString()
{
	return TrecPointerKey::TrecFromSoft<TVariable>(vSelf);
}

TrecPointer<TVariable> TStringVariable::ToString(TrecPointer<TVariable> detail)
{
	return TrecPointerKey::TrecFromSoft<TVariable>(vSelf);
}

bool TVObject::GetVariable(const TString& name, TrecPointer<TVariable>& var)
{
	return false;
}

bool TVObject::SetVariable(const TString& name, TrecPointer<TVariable> var)
{
	return false;
}
