#include "CoSaveSerialization.h"
#include "GlobalData.h"
#include "RenameUtils.h"
#include "pch.h"

namespace
{
	// 'KNBK' in ASCII (0x4B4E424B)
	constexpr std::uint32_t kBookSignature = 0x4B4E424B;
	constexpr std::uint32_t kCurrentVersion = 1;
}

void F4SEAPI SaveCallback(const F4SE::SerializationInterface* a_intfc)
{
	// Safety check: ensure the SerializationInterface is valid
	if (!a_intfc) {
		logger::error("SaveCallback: a_intfc is null!");
		return;
	}

	// Write the record header (signature + version)
	a_intfc->WriteRecord(kBookSignature, kCurrentVersion, nullptr, 0);

	// Write the number of book data entries
	const std::uint32_t count = static_cast<std::uint32_t>(g_bookData.size());
	a_intfc->WriteRecordData(&count, sizeof(count));

	// Write each entry (FormID and its known state)
	for (const auto& [formID, bookData] : g_bookData) {
		a_intfc->WriteRecordData(&formID, sizeof(formID));
		const bool isKnown = bookData.isKnown;
		a_intfc->WriteRecordData(&isKnown, sizeof(isKnown));
	}

	logger::info("SaveCallback: wrote {} entries to co-save.", count);
}

void F4SEAPI LoadCallback(const F4SE::SerializationInterface* a_intfc)
{
	// Safety check: ensure the SerializationInterface is valid
	if (!a_intfc) {
		logger::error("LoadCallback: a_intfc is null!");
		return;
	}

	std::uint32_t type = 0;
	std::uint32_t version = 0;
	std::uint32_t length = 0;

	// Read all the records in the co-save
	while (a_intfc->GetNextRecordInfo(type, version, length)) {
		// Check if this is our record signature and expected version
		if (type == kBookSignature && version == kCurrentVersion) {
			// Read how many items we stored
			std::uint32_t count = 0;
			if (!a_intfc->ReadRecordData(&count, sizeof(count))) {
				logger::error("LoadCallback: failed to read 'count'. The co-save may be corrupted.");
				break;
			}

			// Clear and reload our global data
			g_bookData.clear();

			// Read each entry (FormID and known state)
			for (std::uint32_t i = 0; i < count; i++) {
				FormID formID = 0;
				if (!a_intfc->ReadRecordData(&formID, sizeof(formID))) {
					logger::error("LoadCallback: failed to read 'formID'. The co-save may be corrupted.");
					break;
				}

				bool isKnown = false;
				if (!a_intfc->ReadRecordData(&isKnown, sizeof(isKnown))) {
					logger::error("LoadCallback: failed to read 'isKnown'. The co-save may be corrupted.");
					break;
				}

				g_bookData[formID] = { isKnown };
			}

			logger::info("LoadCallback: read {} entries from co-save.", count);
		} else {
			// Unrecognized record signature or version; skip it
			logger::warn(
				"LoadCallback: skipping unknown record type=0x{:X}, version={}, length={}",
				type,
				version,
				length);

			// If there's data to skip, read it into a buffer and discard
			if (length > 0) {
				std::vector<std::uint8_t> skipBuffer(length);
				if (!a_intfc->ReadRecordData(skipBuffer.data(), length)) {
					logger::error("LoadCallback: failed to skip record data. The co-save may be corrupted.");
					break;
				}
			}
		}
	}

	// Restore known states or prefixes (if needed)
	// RemoveKnownPrefix(); - Deprecated
	ReApplyKnownState();
}

void F4SEAPI RevertCallback(const F4SE::SerializationInterface* /*a_intfc*/)
{
	logger::info("RevertCallback: clearing global book data.");
	g_bookData.clear();
}
