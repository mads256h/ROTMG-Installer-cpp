#ifndef CONVERTER_H
#define CONVERTER_H

#include <string>

class Converter {
public:
	static std::string ToString(const std::wstring input)
	{
		std::string str(input.begin(), input.end());

		return str;
	}

	static std::wstring ToWString(const std::string input)
	{
		std::wstring str(input.begin(), input.end());

		return str;
	}

private:
	Converter() = default;
};

#endif // CONVERTER_H
