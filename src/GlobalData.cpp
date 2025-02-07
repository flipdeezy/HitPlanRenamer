#include "GlobalData.h"
#include "pch.h"

// global container holds information about books (plans) for the mod.
// Each entry maps a FormID to a BookData struct to track if a book is 'known'.

std::unordered_map<FormID, BookData> g_bookData;