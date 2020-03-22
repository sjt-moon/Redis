#include "view.h"

std::string view::centered_str(int output_width, const std::string & str, char filled_char, std::string border)
{
	if (output_width < (int)str.length()) {
		return str;
	}
	int diff = output_width - str.length();
	return border + std::string(diff / 2, filled_char) + str + std::string(diff - diff / 2, filled_char) + border;
}