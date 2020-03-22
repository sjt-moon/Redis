#pragma once
#include <string>

class view
{
public:
	static std::string centered_str(int output_width, const std::string& str, char filled_char = '  ', std::string border = "");
};

