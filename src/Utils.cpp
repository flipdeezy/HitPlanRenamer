#include "pch.h"
#include "Utils.h"

//Checks whether 'substr' occurs in 'str' (case-insensitive).
bool containsCaseInsensitive(std::string_view str, std::string_view substr)
{
	// Perform a case-insensitive search using std::search
	auto it = std::search(
		str.begin(), str.end(),
		substr.begin(), substr.end(),
		[](char a, char b) {
			return static_cast<char>(std::tolower(a)) == static_cast<char>(std::tolower(b));
		}
	);

	return (it != str.end());
}