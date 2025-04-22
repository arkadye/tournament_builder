#pragma once

#include "tournament_builder/name.hpp"

#include <cassert>
#include <memory>
#include <vector>

namespace tournament_builder
{
	class ReferenceCopyOptions
	{
		// Current nothing.
	};

	class IReferencable
	{
	public:
		// This should return the reference key for this element. Normally the same as get_name().
		virtual Name get_reference_key() const = 0;

		// Copy this as through a reference.
		virtual std::shared_ptr<IReferencable> copy_ref(const ReferenceCopyOptions&) const = 0;

		// Get a list of all the possible inner targets for use when parsing a reference.
		virtual std::vector<IReferencable*> get_next_locations() = 0;

		// Returns 'true' if this item matches with a particular token (e.g. same name, or a tag match).
		virtual bool matches_token(const Token&) const;

		// Test whether this holds a particular type.
		template <typename T>
		bool is_a() const;

		// Get as a particular type. This will assert if it holds the incorrect type!
		template <typename T>
		const T* get_as() const;
		template <typename T>
		T* get_as();
	};

	template<typename T>
	inline bool IReferencable::is_a() const
	{
		return dynamic_cast<const T*>(this) != nullptr;
	}

	template <typename T>
	const T* IReferencable::get_as() const
	{
		assert(is_a<T>());
		return dynamic_cast<const T*>(this);
	}

	template <typename T>
	T* IReferencable::get_as()
	{
		assert(is_a<T>());
		return dynamic_cast<T*>(this);
	}
}