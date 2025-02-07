#pragma once

namespace F4SE
{
	class SerializationInterface;
}

void F4SEAPI SaveCallback(const F4SE::SerializationInterface* a_intfc);
void F4SEAPI LoadCallback(const F4SE::SerializationInterface* a_intfc);
void F4SEAPI RevertCallback(const F4SE::SerializationInterface* a_intfc);