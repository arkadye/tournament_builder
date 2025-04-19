#pragma once

#include "tournament_builder/name.hpp"

namespace tournament_builder
{
	class World;
}

namespace tournament_builder::event
{
	// I need to declare this as a class in order to be able to forward declare it.
	class EventImpl;
	class EventHandle
	{
		std::unique_ptr<EventImpl> m_data;
	public:
		explicit EventHandle(EventImpl* init) : m_data{ init } {}
		EventHandle(nullptr_t) : EventHandle{ static_cast<EventImpl*>(nullptr) } {}
		EventHandle() : EventHandle{ nullptr } {}
		EventHandle(const EventHandle&);
		EventHandle(EventHandle&&) noexcept = default;
		EventHandle& operator=(const EventHandle&);
		EventHandle& operator=(EventHandle&&) noexcept = default;
		bool operator==(std::nullptr_t) const { return m_data == nullptr; }
		operator bool() const { return (*this != nullptr); }
		EventImpl& operator*() const { return *m_data; }
		auto operator->() const { return m_data.operator->(); }
	};

	class EventImpl
	{
	public:
		virtual void execute(World& world) const = 0;
		virtual Name get_event_uid() const = 0;
		virtual EventHandle parse(const nlohmann::json& input) const = 0;
		virtual EventHandle copy() const = 0;
		[[noreturn]] void throw_execution_exception(std::string_view explanation);
	protected:
		void verify_input(const nlohmann::json& input) const;
	};

	namespace internal_event
	{

		template <typename T>
		class EventRegistrarObject
		{
		public:
			EventRegistrarObject();
		};

		class EventRegister
		{
		private:
			static void register_impl(EventHandle new_element, Name uid);
		public:
			template <typename T>
			static void register_event();
			static EventHandle parse_event(const nlohmann::json& input);
		};

		template<typename T>
		inline void EventRegister::register_event()
		{
			EventHandle event{ new T };
			Name uid = event->get_event_uid();
			register_impl(std::move(event), uid);
		}

		template <typename T>
		EventRegistrarObject<T>::EventRegistrarObject()
		{
			EventRegister::register_event<T>();
		}
	}
}