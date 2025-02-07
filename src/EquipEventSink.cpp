#include "EquipEventSink.h"
#include "GlobalData.h"
#include "Utils.h"
#include "pch.h"

// Global instance of our event sink
EquipEventSink g_equipEventSink;

RE::BSEventNotifyControl EquipEventSink::ProcessEvent(
	const RE::TESEquipEvent& a_event,
	RE::BSTEventSource<RE::TESEquipEvent>* /*a_eventSource*/)
{
	// We only care about equip events (not unequip)
	if (!a_event.isEquip) {
		return RE::BSEventNotifyControl::kContinue;
	}

	// Validate actor and check if it is the player
	RE::Actor* actor = a_event.a;
	if (!actor || !actor->IsPlayerRef()) {
		return RE::BSEventNotifyControl::kContinue;
	}

	// Lookup the base form from its formID
	const std::uint32_t baseFID = a_event.formId;
	RE::TESForm* baseForm = RE::TESForm::GetFormByID(baseFID);
	if (!baseForm) {
		logger::info("EquipEvent: No base form found for form ID {:#X}", baseFID);
		return RE::BSEventNotifyControl::kContinue;
	}

	// Check if the equipped item is a book
	auto* bookForm = baseForm->As<RE::TESObjectBOOK>();
	if (!bookForm) {
		return RE::BSEventNotifyControl::kContinue;
	}

	// Gather EditorID and FullName for logging and checks
	const char* edid = baseForm->GetFormEditorID();
	std::string fullNameStr = "N/A";
	if (auto* fullNameInterface = baseForm->As<RE::TESFullName>()) {
		if (const char* name = fullNameInterface->GetFullName()) {
			fullNameStr = name;
		}
	}
	const std::uint32_t fid = baseForm->GetFormID();

	// Log details about the equipped book
	logger::info(
		"EquipEventSink: Player equipped a book -> EditorID='{}', FullName='{}', FormID={:#X}",
		edid ? edid : "N/A",
		fullNameStr,
		fid);

	// Check if this is one of the "Hit76Plan" books we want to mark as known
	bool markKnown = false;
	if (containsCaseInsensitive(edid ? edid : "", "Hit76Plan") ||
		containsCaseInsensitive(fullNameStr, "Hit76Plan")) {
		markKnown = true;
		logger::info("Found 'Hit76Plan' in EditorID or FullName for FormID={:#X}.", fid);
	}

	if (markKnown) {
		// Update global data to mark this plan as known
		if (g_bookData.find(fid) == g_bookData.end()) {
			g_bookData[fid] = { true };
			logger::info("Marking as known plan for FormID={:#X}.", fid);
		} else {
			logger::info("FormID={:#X} is already known plan.", fid);
		}

		// Rename the book on the spot if not already prefixed
		static const std::string prefix = "(Known) - ";
		if (auto* fullName = bookForm->As<RE::TESFullName>()) {
			if (const char* currentName = fullName->GetFullName()) {
				std::string currentStr(currentName);
				// Only rename if it doesn't already have the prefix
				if (currentStr.rfind(prefix, 0) != 0) {
					std::string newName = prefix + currentStr;
					fullName->SetFullName(*bookForm, newName.c_str());

					logger::info(
						"Renamed on the spot (FormID={:#X}) from '{}' to '{}'",
						fid,
						currentStr,
						newName);

					// Force-refresh UI by removing and re-adding the item
					auto* player = RE::PlayerCharacter::GetSingleton();
					if (player) {
						std::uint32_t count = 0;
						bool hasItem = player->GetItemCount(count, baseForm, false);
						if (hasItem && count > 0) {
							logger::info("Removing {} of FormID={:#X} to refresh UI.", count, fid);

							RE::TESObjectREFR::RemoveItemData removeData(bookForm, count);
							removeData.reason = RE::ITEM_REMOVE_REASON::kNone;
							removeData.a_otherContainer = nullptr;
							removeData.dropLoc = nullptr;
							removeData.rotate = nullptr;

							player->RemoveItem(removeData);

							logger::info("Re-adding {} of FormID={:#X} with updated name.", count, fid);
							player->AddInventoryItem(bookForm, nullptr, count, nullptr, nullptr, nullptr);
						}
					}
				}
			}
		}
	}

	// Continue with other event sinks (if any)
	return RE::BSEventNotifyControl::kContinue;
}