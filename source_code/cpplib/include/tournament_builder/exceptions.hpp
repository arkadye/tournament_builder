#pragma once

#include "nlohmann/json_fwd.hpp"

#include <string>
#include <format>
#include <initializer_list>
#include <cstdint>
#include <vector>

// All the exceptions that can be thrown are here.

namespace tournament_builder
{
	class NamedElement;
	class SoftReference;
	namespace event
	{
		class EventHandle;
		class EventImpl;
	}
}

namespace tournament_builder::exception
{
	class TournamentBuilderException
	{
		std::string msg;
	public:
		explicit TournamentBuilderException(std::string in) : msg{ std::move(in) } {}
		TournamentBuilderException(const TournamentBuilderException&) = default;
		TournamentBuilderException(TournamentBuilderException&&) = default;
		TournamentBuilderException& operator=(const TournamentBuilderException&) = default;
		TournamentBuilderException& operator=(TournamentBuilderException&&) = default;
		std::string_view what() const { return msg; }
		void add_context(std::string_view message);
		void add_context(const tournament_builder::NamedElement& named_element);
	};

	class IllegalName : public TournamentBuilderException
	{
	public:
		IllegalName(std::string_view name, std::size_t pos, char c)
			: TournamentBuilderException{ std::format("Illegal character '{}' found in position {} in name '{}'", c, pos, name) } {
		}
	};

	class InvalidArgument : public TournamentBuilderException
	{
	public:
		InvalidArgument(std::string_view message)
			: TournamentBuilderException{ std::format("Invalid argument: {}", message) } {
		}
	};

	class ReferenceException : public TournamentBuilderException
	{
	public:
		explicit ReferenceException(std::string_view msg) : TournamentBuilderException{ std::format("Reference error: {}", msg) } {}
	};

	class ReferenceParseException : public ReferenceException
	{
	public:
		explicit ReferenceParseException(std::string_view msg) : ReferenceException{ std::format("Could not parse '{}' as a reference", msg) } {}
	};

	class InvalidReferenceToken : public ReferenceException
	{
	public:
		InvalidReferenceToken(std::string_view reference, std::string_view token, std::size_t pos, std::string_view explanation);
	};

	class InvalidDereference : public ReferenceException
	{
	public:
		explicit InvalidDereference(const SoftReference& reference);
	};

	class ExpectedSingleResult : public ReferenceException
	{
	public:
		ExpectedSingleResult(const SoftReference& reference, std::size_t num_results);
	};

	class TagException : public TournamentBuilderException
	{
	public:
		explicit TagException(std::string_view msg) : TournamentBuilderException{ std::format("Tag error: {}", msg) } {}
	};

	class InvalidTagToken : public TagException
	{
	public:
		InvalidTagToken(std::string_view tag, std::string_view token, std::size_t pos, std::string_view explanation);
	};

	class EventExecutionException : public TournamentBuilderException
	{
	public:
		EventExecutionException(const event::EventImpl& event, std::string_view explanation);
	};

	class JsonParseException : public TournamentBuilderException
	{
	public:
		JsonParseException(const nlohmann::json& object, std::string_view message);
	};

	class JsonParseInvalidType : public JsonParseException
	{
	public:
		JsonParseInvalidType(const nlohmann::json& object, const std::vector<uint8_t>& allowed_types);
	};

	class JsonInvalidDescriptor : public JsonParseException
	{
	public:
		JsonInvalidDescriptor(const nlohmann::json& object, std::string_view type);
	};

	class JsonInvalidEvent : public JsonParseException
	{
	public:
		JsonInvalidEvent(const nlohmann::json& object, std::string_view type);
	};
}