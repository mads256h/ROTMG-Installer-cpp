#ifndef CONVERTER_H
#define CONVERTER_H

#include <string>

//Converts things to other things.
class Converter {
public:
	//Convert std::wstring to std::string.
	static std::string ToString(const std::wstring& input)
	{
		std::string str(input.begin(), input.end());

		return str;
	}

	//Convert std::string to std::wstring.
	static std::wstring ToWString(const std::string& input)
	{
		std::wstring str(input.begin(), input.end());

		return str;
	}

private:
	Converter() = default;
};

#endif // CONVERTER_H
