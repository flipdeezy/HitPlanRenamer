#pragma once

using FormID = std::uint32_t;

struct BookData
{
	bool isKnown;
};

extern std::unordered_map<FormID, BookData> g_bookData;
