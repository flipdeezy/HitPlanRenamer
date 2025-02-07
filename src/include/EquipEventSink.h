#pragma once
#include "GlobalData.h"
#include "Utils.h"

class EquipEventSink : public RE::BSTEventSink<RE::TESEquipEvent>
{
public:
	virtual RE::BSEventNotifyControl ProcessEvent(
		const RE::TESEquipEvent& a_event,
		RE::BSTEventSource<RE::TESEquipEvent>* a_eventSource) override;
};

extern EquipEventSink g_equipEventSink;