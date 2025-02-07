#include "RenameUtils.h"
#include "GlobalData.h"
#include "pch.h"

// deprecated
/* void RemoveKnownPrefix()
{
	// Acquire the TESDataHandler (contains global game data)
	auto* dataHandler = RE::TESDataHandler::GetSingleton();
	if (!dataHandler) {
		logger::error("RemoveKnownPrefix: TESDataHandler is not available!");
		return;
	}

	// Retrieve the list of all registered book forms
	auto& bookArray = dataHandler->GetFormArray<RE::TESObjectBOOK>();
	if (bookArray.empty()) {
		logger::warn("RemoveKnownPrefix: No books found in data handler.");
		return;
	}

	// The prefix we want to remove
	constexpr std::string_view kKnownPrefix = "(Known) - ";
	int modifiedCount = 0;

	// Iterate through every book in the array
	for (auto* book : bookArray) {
		if (!book) {
			continue;
		}
		if (auto* fullName = book->As<RE::TESFullName>()) {
			const char* currentName = fullName->GetFullName();
			if (!currentName || *currentName == '\0') {
				continue;
			}

			std::string currentStr(currentName);

			// Check if the name starts with our known prefix
			if (currentStr.rfind(kKnownPrefix.data(), 0) == 0) {
				// Remove the prefix portion
				std::string newName = currentStr.substr(kKnownPrefix.size());
				fullName->SetFullName(*book, newName.c_str());

				logger::info(
					"RemoveKnownPrefix: Removed prefix from book. Original: '{}', New: '{}'",
					currentStr,
					newName);
				modifiedCount++;
			}
		}
	}

	logger::info("RemoveKnownPrefix: Removed prefix from {} books.", modifiedCount);
} */

void ReApplyKnownState()
{
	constexpr std::string_view kKnownPrefix = "(Known) - ";
	int appliedCount = 0;

	// Check all known books in our global map
	for (const auto& [formID, data] : g_bookData) {
		// If it's not marked known, skip it
		if (!data.isKnown) {
			continue;
		}

		RE::TESForm* form = RE::TESForm::GetFormByID(formID);
		if (!form) {
			continue;
		}

		auto* book = form->As<RE::TESObjectBOOK>();
		if (!book) {
			continue;
		}

		if (auto* fullName = book->As<RE::TESFullName>()) {
			const char* currentName = fullName->GetFullName();
			if (!currentName || *currentName == '\0') {
				continue;
			}

			std::string currentStr(currentName);

			// If the prefix isn't at the start, add it
			if (currentStr.rfind(kKnownPrefix.data(), 0) != 0) {
				std::string newName = std::string(kKnownPrefix) + currentStr;
				fullName->SetFullName(*book, newName.c_str());

				logger::info(
					"ReApplyKnownState: Re-applied prefix for FormID: '{:X}', New Name: '{}'",
					formID,
					newName);
				appliedCount++;
			}
		}
	}

	logger::info("ReApplyKnownState: Applied prefix to {} known books.", appliedCount);
}

bool SetObjectNameByEditorID(std::monostate, RE::BSFixedString a_editorID, RE::BSFixedString a_newName)
{
	// Attempt to find the form by its EditorID
	RE::TESForm* form = RE::TESForm::GetFormByEditorID(a_editorID);
	if (!form) {
		logger::error("SetObjectNameByEditorID: No form found for EditorID '{}'", a_editorID.c_str());
		return false;
	}

	// Verify the form is a TESObjectBOOK
	auto* book = form->As<RE::TESObjectBOOK>();
	if (!book) {
		logger::error(
			"SetObjectNameByEditorID: Form '{}' is not a TESObjectBOOK. Cannot set name.",
			a_editorID.c_str());
		return false;
	}

	// Set the new name
	book->SetFullName(*form, a_newName.c_str());

	logger::info(
		"SetObjectNameByEditorID: Successfully set name of form with EditorID '{}' to '{}'",
		a_editorID.c_str(),
		a_newName.c_str());

	return true;
}