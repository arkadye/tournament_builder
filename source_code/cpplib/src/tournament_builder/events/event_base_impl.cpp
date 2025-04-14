#include "tournament_builder/events/event_base_impl.hpp"

#include "tournament_builder/json/json_helpers.hpp"

#include <map>
#include <cassert>

namespace tournament_builder::event
{
	inline std::vector<std::pair<Name, EventHandle>> m_derived_events_register;

	auto find_event(Name uid)
	{
		return std::ranges::find(m_derived_events_register, uid, &std::pair<Name, EventHandle>::first);
	}

	bool contains_event(Name uid)
	{
		return find_event(uid) == end(m_derived_events_register);
	}

	namespace internal_event
	{

		void EventRegister::register_impl(EventHandle new_element, Name uid)
		{
			assert(new_element != nullptr);
			const Name descriptor_uid = new_element->get_event_uid();
			assert(descriptor_uid == uid);
			assert(contains_event(uid) && "Duplicate event detected");
			m_derived_events_register.emplace_back(uid, std::move(new_element));
		}

		EventHandle EventRegister::parse_event(const nlohmann::json& input)
		{
			const std::optional<std::string> descriptor_type = json_helper::get_optional_string(input, "event_type");
			if (!descriptor_type.has_value())
			{
				return nullptr;
			}
			const auto find_it = find_event(Name{ *descriptor_type });
			if (find_it == end(m_derived_events_register))
			{
				throw exception::JsonInvalidEvent{ input, *descriptor_type };
			}

			const auto& exemplar = find_it->second;
			return exemplar->parse(input);
		}
	}

		EventHandle EventImpl::parse(const nlohmann::json& input) const
		{
			return internal_event::EventRegister::parse_event(input);
		}

		void EventImpl::throw_execution_exception(std::string_view explanation)
		{
			throw exception::EventExecutionException(*this, explanation);
		}

		void EventImpl::verify_input(const nlohmann::json& input) const
		{
			const std::string uid = json_helper::get_string(input, "event_type");
			assert(std::string_view{ uid } == get_event_uid());
		}

		EventHandle::EventHandle(const EventHandle& other) : EventHandle{ other->copy() } {}

		EventHandle& EventHandle::operator=(const EventHandle& other)
		{
			*this = other->copy();
			return *this;
		}
}

tournament_builder::exception::EventExecutionException::EventExecutionException(const event::EventImpl& event, std::string_view explanation)
	: TournamentBuilderException{ std::format("While executing event type {}: {}",event.get_event_uid(), explanation) }
{
}