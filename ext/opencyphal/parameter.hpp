/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <algorithm>
#include <bit>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <cstring>
#include <span>
#include <string_view>

namespace modm::canard
{

/**
 * A configuration parameter of a DroneCAN or Cyphal node, which the
 * uavcan.protocol.param.GetSet service (DroneCAN) or the uavcan.register.Access
 * and List services (Cyphal) read and write by name or index.
 *
 * Integer and real parameters have a range, sets outside of it are refused.
 * The application reads the value with integer(), real() or boolean().
 */
class Parameter
{
public:
	enum class
	Type : uint8_t
	{
		Integer,
		Real,
		Boolean,
	};

	/// A value of the type of the parameter
	union Data
	{
		int64_t integer;
		float real;
		bool boolean;
	};

	/// @param name at most 50 characters for both protocols, by convention lowercase
	///        with dots, e.g. "uavcan.node.id"
	template< std::integral T > requires (not std::same_as<T, bool>)
	constexpr Parameter(const char* name, T value, int64_t min, int64_t max)
	:	name_{name}, type_{Type::Integer}, value_{.integer = value}, default_{.integer = value},
		min_{.integer = min}, max_{.integer = max} {}

	constexpr Parameter(const char* name, float value, float min, float max)
	:	name_{name}, type_{Type::Real}, value_{.real = value}, default_{.real = value},
		min_{.real = min}, max_{.real = max} {}

	constexpr Parameter(const char* name, bool value)
	:	name_{name}, type_{Type::Boolean}, value_{.boolean = value}, default_{.boolean = value} {}

	Parameter(const Parameter&) = delete;
	Parameter& operator=(const Parameter&) = delete;

	std::string_view name() const { return name_; }
	Type type() const { return type_; }

	int64_t integer() const { return value_.integer; }
	float real() const { return value_.real; }
	bool boolean() const { return value_.boolean; }

	Data value() const { return value_; }
	Data defaultValue() const { return default_; }
	/// Range of integer and real parameters
	Data min() const { return min_; }
	Data max() const { return max_; }

	/// Sets the value, an integer also of a real parameter.
	/// @return false if the type does not match or the value is out of range
	template< class T >
	bool
	set(T value)
	{
		if constexpr (std::same_as<T, bool>)
		{
			if (type_ != Type::Boolean) return false;
			value_.boolean = value;
			return true;
		}
		else if constexpr (std::integral<T>)
		{
			if (type_ == Type::Real) return set(float(value));
			if constexpr (std::unsigned_integral<T>) if (value > uint64_t(INT64_MAX)) return false;
			if (type_ != Type::Integer or int64_t(value) < min_.integer or int64_t(value) > max_.integer)
				return false;
			value_.integer = value;
			return true;
		}
		else
		{
			// compare in the type of the value, so that a double out of the
			// float range is refused before narrowing
			if (type_ != Type::Real or std::isnan(value) or
				value < T(min_.real) or value > T(max_.real)) return false;
			value_.real = float(value);
			return true;
		}
	}

	/// Sets the default value.
	void
	reset()
	{
		value_ = default_;
	}

	bool
	isDefault() const
	{
		return raw(value_) == raw(default_);
	}

	/// Key of the parameter in a storage: a hash of the name and the type, so
	/// that changing the order of the parameters keeps the stored values.
	uint32_t
	key() const
	{
		// FNV-1a
		uint32_t hash = 2166136261u;
		for (const char c : name()) hash = (hash ^ uint8_t(c)) * 16777619u;
		hash = (hash ^ uint8_t(type_)) * 16777619u;
		// ZMS reserves the ID 0xffff'ffff
		return hash == 0xffff'ffff ? 0 : hash;
	}

	/// The value as 8 bytes for a storage
	uint64_t
	stored() const
	{
		return raw(value_);
	}

	/// Sets a value read from a storage
	/// @return false if it is out of range
	bool
	load(uint64_t data)
	{
		switch (type_)
		{
			case Type::Integer: return set(int64_t(data));
			case Type::Real: return data <= 0xffff'ffff and set(std::bit_cast<float>(uint32_t(data)));
			case Type::Boolean: return data <= 1 and set(data == 1);
		}
		return false;
	}

private:
	uint64_t
	raw(Data data) const
	{
		switch (type_)
		{
			case Type::Integer: return uint64_t(data.integer);
			case Type::Real: return std::bit_cast<uint32_t>(data.real);
			case Type::Boolean: return data.boolean;
		}
		return 0;
	}

	const char* const name_;
	const Type type_;
	Data value_;
	const Data default_;
	const Data min_{};
	const Data max_{};
};

/// Persistent storage of parameters, e.g. modm::canard::ZmsStorage.
/// saveParameters() writes all parameters on every save, so a storage on
/// flash should skip writes of unchanged values, like ZMS does.
class ParameterStorage
{
public:
	/// @return false if there is no stored value
	virtual bool read(uint32_t key, uint64_t& data) = 0;
	virtual bool write(uint32_t key, uint64_t data) = 0;
	/// Removes a stored value, also succeeds if there is none.
	virtual bool remove(uint32_t key) = 0;

protected:
	~ParameterStorage() = default;
};

/// Loads the stored values of parameters, e.g. before initializing a node
/// whose node-ID is a parameter. Values out of range are ignored.
inline void
loadParameters(std::span<Parameter* const> parameters, ParameterStorage& storage)
{
	for (Parameter* parameter : parameters)
	{
		uint64_t data;
		if (storage.read(parameter->key(), data) and not parameter->load(data))
			parameter->reset();
	}
}

/// Writes the parameters with a non-default value to the storage and removes
/// the others, so that new default values take effect.
/// @return false if a write failed
inline bool
saveParameters(std::span<Parameter* const> parameters, ParameterStorage& storage)
{
	bool ok{true};
	for (const Parameter* parameter : parameters)
	{
		if (parameter->isDefault()) ok &= storage.remove(parameter->key());
		else ok &= storage.write(parameter->key(), parameter->stored());
	}
	return ok;
}

/// Removes the parameters from the storage, if any, and resets them to their defaults.
/// @return false if removing failed
inline bool
eraseParameters(std::span<Parameter* const> parameters, ParameterStorage* storage)
{
	bool ok{true};
	for (Parameter* parameter : parameters)
	{
		if (storage) ok &= storage->remove(parameter->key());
		parameter->reset();
	}
	return ok;
}

}	// namespace modm::canard
