#include "CoSaveSerialization.h"
#include "EquipEventSink.h"
#include "pch.h"

extern "C" DLLEXPORT bool F4SEAPI F4SEPlugin_Query(const F4SE::QueryInterface* a_f4se, F4SE::PluginInfo* a_info)
{
	// Safety checks for the F4SE pointers
	if (!a_f4se || !a_info) {
		return false;
	}

	// Set up logging
	auto path = logger::log_directory();
	if (!path) {
		return false;
	}
	*path /= fmt::format(FMT_STRING("{}.log"), Version::PROJECT);
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
	auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));
	log->set_level(spdlog::level::trace);
	log->flush_on(spdlog::level::trace);
	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("%g(%#): [%^%l%$] %v"s);

	logger::info("{} v{}", Version::PROJECT, Version::NAME);

	// Fill out plugin info
	a_info->infoVersion = F4SE::PluginInfo::kVersion;
	a_info->name = Version::PROJECT.data();
	a_info->version = Version::MAJOR;

	// Prevent use in Creation Kit
	if (a_f4se->IsEditor()) {
		logger::critical("Loaded in editor. This plugin is for the game only.");
		return false;
	}

	// Check runtime version compatibility
	const auto ver = a_f4se->RuntimeVersion();
	if (ver < F4SE::RUNTIME_1_10_162) {
		logger::critical("Unsupported runtime version: {}", ver.string());
		return false;
	}

	return true;
}

void MessageCallback(F4SE::MessagingInterface::Message* msg)
{
	// Always check if message is valid
	if (!msg) {
		logger::error("MessageCallback: Received a null message.");
		return;
	}
	switch (msg->type) {
	case F4SE::MessagingInterface::kGameDataReady:
		{
			// Register the equip event sink
			auto* equipSource = RE::EquipEventSource::GetSingleton();
			if (equipSource) {
				equipSource->RegisterSink(&g_equipEventSink);
				logger::info("Registered TESEquipEvent sink (Fallout 4).");
			} else {
				logger::error("Failed to get TESEquipEvent source (Fallout 4).");
			}

			// Register serialization callbacks
			if (auto* serialization = F4SE::GetSerializationInterface()) {
				serialization->SetUniqueID(0x4B4E424B);  // 'KNBK' in ASCII
				serialization->SetSaveCallback(SaveCallback);
				serialization->SetLoadCallback(LoadCallback);
				serialization->SetRevertCallback(RevertCallback);
			}
		}
		break;
	case F4SE::MessagingInterface::kNewGame:
		break;
	case F4SE::MessagingInterface::kPostLoadGame:
		break;
	default:
		break;
	}
}

extern "C" DLLEXPORT bool F4SEAPI F4SEPlugin_Load(const F4SE::LoadInterface* a_f4se)
{
	// Safety check for F4SE pointer
	if (!a_f4se) {
		logger::error("F4SEPlugin_Load: a_f4se is null.");
		return false;
	}

	// Initialize the F4SE library
	F4SE::Init(a_f4se);

	// Register for F4SE messaging
	auto* messaging = F4SE::GetMessagingInterface();
	if (!messaging || !messaging->RegisterListener(MessageCallback)) {
		logger::error("Cannot register F4SE message listener.");
		return false;
	}

	logger::info("F4SEPlugin_Load: Plugin loaded successfully.");
	return true;
}