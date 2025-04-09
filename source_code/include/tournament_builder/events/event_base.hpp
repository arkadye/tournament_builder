#pragma once

#include "tournament_builder/events/event_base_impl.hpp"
#include "tournament_builder/generic_utils.hpp"

namespace tournament_builder::event
{
	EventHandle parse_event(const nlohmann::json& input);

	template <typename T>
	class EventBase : public EventImpl
	{
	public:
		EventBase();

		// Override this to do the actual work for the event.
		virtual void execute(World& world) const = 0;

		// Return a UNIQUE Name used in the parser to identify which type to parse.
		// This will match up with the "event_type" field in the JSON.
		virtual Name get_event_uid() const = 0;

		// Parse the event here.
		virtual EventHandle parse(const nlohmann::json& input) const = 0;

		[[noreturn]] static void throw_execution_exception(std::string_view explanation);

		EventHandle copy() const final;

	protected:
		// Run some asserts on the input. If this is written correctly
		// it should never fail.

		// void verify_input(const nlohmann::json& input) const;
		using EventImpl::verify_input;

	private:
		inline static internal_event::EventRegistrarObject<T> m_register_object;
	};

	template<typename T>
	inline EventBase<T>::EventBase() : EventImpl()
	{
		auto force_instantiation_of_registrar = &m_register_object;
	}

	template<typename T>
	inline void EventBase<T>::throw_execution_exception(std::string_view explanation)
	{
		T{}.EventImpl::throw_execution_exception(explanation);
	}

	template<typename T>
	inline EventHandle EventBase<T>::copy() const
	{
		return EventHandle{ new T{*static_cast<const T*>(this)} };
	}
}