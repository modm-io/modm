/*
 * Copyright (c) 2021, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef MODM_INPLACE_ANY_HPP
#define MODM_INPLACE_ANY_HPP

#include <concepts>
#include <cstddef>
#include <optional>
#include <type_traits>
#include <utility>

#include <modm/utils/aligned_storage.hpp>
#include <modm/architecture/interface/assert.hpp>

namespace modm
{

/// @cond
template <std::size_t Size>
class inplace_any;

namespace inplace_any_impl
{
    enum class Action
    {
        Move,
        Copy,
        Destruct
    };

    using HandlerFunc = void(*)(Action, std::byte*, std::byte*, void (**)());

    template<typename T>
    struct is_any_type : std::false_type {};

    template<std::size_t Size>
    struct is_any_type<inplace_any<Size>> : std::true_type {};

    template<typename T>
    struct is_inplace_type : std::false_type {};

    template<typename T>
    struct is_inplace_type<std::in_place_type_t<T>> : std::true_type {};

    template<typename T>
    concept compatible_value_type = !is_any_type<std::remove_cvref_t<T>>::value &&
                                    !is_inplace_type<std::remove_cvref_t<T>>::value &&
                                    std::copy_constructible<T> &&
                                    std::is_nothrow_move_constructible_v<T>;

    template<typename T>
    struct inplace_any_handler
    {
        static_assert(!std::is_const_v<T> && !std::is_reference_v<T> && !std::is_volatile_v<T>);

        static void execute(Action action, std::byte* data,
                            std::byte* other_data = nullptr,
                            void (**other_handler_ptr)() = nullptr);

        template<typename... Args>
        static void construct(std::byte* data, HandlerFunc* handler_ptr, Args&&... args);

    private:
        static void move(std::byte* data, std::byte* other_data,
                         HandlerFunc* other_handler_ptr) noexcept;

        static void copy(const std::byte* data, std::byte* other_data,
                         HandlerFunc* other_handler_ptr);
    };

    template<typename T>
    HandlerFunc get_handler_function() noexcept
    {
        return &inplace_any_handler<std::remove_cvref_t<T>>::execute;
    }
}
/// @endcond

/**
 * Type-safe container for single values of any copy constructible type
 * of size less than the capacity.
 * The objects are stored in place, no allocation takes place.
 *
 * All stored objects must by copy-constructible and nothrow move-constructible.
 *
 * \warning As this class is optimized for use without exceptions it only
 *          provides the basic exception safety guarantee for copy-assignment
 *          operations. Contrary to std::any, if copy-assignment throws a
 *          currently held value will be discarded.
 *
 * @ingroup modm_utils
 * @author Christopher Durand
 */
template <std::size_t Size = 16>
class inplace_any final
{
public:
    /// Construct empty inplace_any
    inplace_any() noexcept = default;

    inplace_any(const inplace_any& other);

    inplace_any(inplace_any&& other) noexcept;

    inplace_any& operator=(const inplace_any& other);

    inplace_any& operator=(inplace_any&& other) noexcept;

    ~inplace_any();

    /**
     * Copy-construct from inplace_any with smaller capacity
     * \pre OtherSize <= Size
     */
    template<size_t OtherSize>
    inplace_any(const inplace_any<OtherSize>& other);

    /**
     * Move-construct from inplace_any with smaller capacity
     * \pre OtherSize <= Size
     */
    template<size_t OtherSize>
    inplace_any(inplace_any<OtherSize>&& other) noexcept;

    /**
     * Copy-assign from inplace_any with smaller capacity
     * \pre OtherSize <= Size
     */
    template<size_t OtherSize>
    inplace_any& operator=(const inplace_any<OtherSize>& other);

    /**
     * Move-assign from inplace_any with smaller capacity
     * \pre OtherSize <= Size
     */
    template<size_t OtherSize>
    inplace_any& operator=(inplace_any<OtherSize>&& other) noexcept;

    /// In-place construct object
    template<typename T, typename... Args>
        requires inplace_any_impl::compatible_value_type<T>
    inplace_any(std::in_place_type_t<T>, Args&&... args);

    /// Initialize from object
    template<typename T>
        requires inplace_any_impl::compatible_value_type<T>
    inplace_any(T&& value);

    /// Assign from object
    template<typename T>
        requires inplace_any_impl::compatible_value_type<T>
    inplace_any& operator=(T&& value);

    /// Change the contained object, constructing the new object directly
    template<typename T, typename... Args>
        requires inplace_any_impl::compatible_value_type<T>
    T& emplace(Args&&... args);

    /// Destroy the contained object
    void reset() noexcept { destruct(); }

    /// Test if object has a value
    bool has_value() const noexcept { return handler_ != nullptr; }

    /// Test if object is empty
    bool empty() const noexcept { return !handler_; }

    /// Test if object has a value
    explicit operator bool() const noexcept { return handler_ != nullptr; }

    void swap(inplace_any& other) noexcept;

private:
    template<typename T, std::size_t S>
    friend auto any_cast(const inplace_any<S>* any) noexcept
        -> std::add_pointer_t<std::add_const_t<std::remove_cvref_t<T>>>;

    template<typename T, std::size_t S>
    friend auto any_cast(inplace_any<S>* any) noexcept
        -> std::add_pointer_t<std::remove_cvref_t<T>>;

        template<typename T, std::size_t S>
    friend auto any_cast(const inplace_any<S>& any) noexcept
        -> std::add_const_t<std::remove_cvref_t<T>>&;

    template<typename T, std::size_t S>
    friend auto any_cast(inplace_any<S>& any) noexcept
        -> std::remove_cvref_t<T>&;

    template<typename T, typename... Args>
    T& construct(Args&&... args);

    void destruct() noexcept;

    template<std::size_t S>
    void copy_from(const inplace_any<S>& other);

    template<std::size_t S>
    void move_from(inplace_any<S>&& other) noexcept;

private:
    modm::aligned_storage<Size>::type storage_;
    inplace_any_impl::HandlerFunc handler_{nullptr};
};

/**
 * Type-safe access to the contained value
 *
 * \param any pointer to const inplace_any object
 * \tparam T type of the object to access
 * \returns pointer to contained object or nullptr if empty or on type mismatch
 */
template<typename T, std::size_t Size>
auto any_cast(const inplace_any<Size>* any) noexcept
    -> std::add_pointer_t<std::add_const_t<std::remove_cvref_t<T>>>;

/**
 * Type-safe access to the contained value
 *
 * \param any pointer to inplace_any object
 * \tparam T type of the object to access
 * \returns pointer to contained object or nullptr if empty or on type mismatch
 */
template<typename T, std::size_t Size>
auto any_cast(inplace_any<Size>* any) noexcept
    -> std::add_pointer_t<std::remove_cvref_t<T>>;

/**
 * Type-safe access to the contained value
 *
 * \warning asserts if inplace any is empty or on type mismatch
 *
 * \param any reference to const inplace_any object
 * \tparam T type of the object to access
 * \returns reference to contained object
 */
template<typename T, std::size_t Size>
auto any_cast(const inplace_any<Size>& any) noexcept
    -> std::add_const_t<std::remove_cvref_t<T>>&;

/**
 * Type-safe access to the contained value
 *
 * \warning asserts if inplace any is empty or on type mismatch
 *
 * \param any reference to inplace_any object
 * \tparam T type of the object to access
 * \returns reference to contained object
 */
template<typename T, std::size_t Size>
auto any_cast(inplace_any<Size>& any) noexcept
    -> std::remove_cvref_t<T>&;

/// Swap contained values of two inplace_any objects
template <std::size_t Size>
void swap(inplace_any<Size>& first, inplace_any<Size>& second) noexcept;


template<typename T, std::size_t Size>
auto any_cast(const inplace_any<Size>* any) noexcept
    -> std::add_pointer_t<std::add_const_t<std::remove_cvref_t<T>>>
{
    using Type = std::remove_cvref_t<T>;
    if (inplace_any_impl::get_handler_function<Type>() == any->handler_) {
        return reinterpret_cast<const Type*>(&any->storage_);
    }

    return nullptr;
}

template<typename T, std::size_t Size>
auto any_cast(inplace_any<Size>* any) noexcept
    -> std::add_pointer_t<std::remove_cvref_t<T>>
{
    using Type = std::remove_cvref_t<T>;
    if (inplace_any_impl::get_handler_function<Type>() == any->handler_) {
        return reinterpret_cast<Type*>(&any->storage_);
    }

    return nullptr;
}

template<typename T, std::size_t Size>
auto any_cast(const inplace_any<Size>& any) noexcept
    -> std::add_const_t<std::remove_cvref_t<T>>&
{
    using Type = std::remove_cvref_t<T>;
    if (inplace_any_impl::get_handler_function<Type>() == any.handler_) {
        return *reinterpret_cast<const Type*>(&any.storage_);
    } else {
        modm_assert(0, "bad_any_cast", "can't cast modm::inplace_any to type", &any);
        __builtin_unreachable();
    }
}

template<typename T, std::size_t Size>
auto any_cast(inplace_any<Size>& any) noexcept
    -> std::remove_cvref_t<T>&
{
    using Type = std::remove_cvref_t<T>;
    if (inplace_any_impl::get_handler_function<Type>() == any.handler_) {
        return *reinterpret_cast<Type*>(&any.storage_);
    } else {
        modm_assert(0, "bad_any_cast", "can't cast modm::inplace_any to type", &any);
        __builtin_unreachable();
    }
}

template <std::size_t Size>
void swap(inplace_any<Size>& first, inplace_any<Size>& second) noexcept
{
    first.swap(second);
}

template<std::size_t Size>
inplace_any<Size>::inplace_any(const inplace_any& other)
{
    copy_from(other);
}

template<std::size_t Size>
inplace_any<Size>::inplace_any(inplace_any&& other) noexcept
{
    move_from(std::move(other));
}

template<std::size_t Size>
auto inplace_any<Size>::operator=(const inplace_any& other) -> inplace_any&
{
    if(this != &other) {
        destruct();
        copy_from(other);
    }
    return *this;
}

template<std::size_t Size>
auto inplace_any<Size>::operator=(inplace_any&& other) noexcept
    -> inplace_any&
{
    if(this != &other) {
        destruct();
        move_from(std::move(other));
    }
    return *this;
}

template<std::size_t Size>
template<std::size_t OtherSize>
inplace_any<Size>::inplace_any(const inplace_any<OtherSize>& other)
{
    static_assert(OtherSize <= Size, "cannot construct from larger inplace_any");
    copy_from(other);
}

template<std::size_t Size>
template<std::size_t OtherSize>
inplace_any<Size>::inplace_any(inplace_any<OtherSize>&& other) noexcept
{
   static_assert(OtherSize <= Size, "cannot construct from larger inplace_any");
   move_from(std::move(other));
}

template<std::size_t Size>
template<std::size_t OtherSize>
auto inplace_any<Size>::operator=(const inplace_any<OtherSize>& other) -> inplace_any&
{
    static_assert(OtherSize <= Size, "cannot assign from larger inplace_any");
    destruct();
    copy_from(other);
    return *this;
}

template<std::size_t Size>
template<std::size_t OtherSize>
auto inplace_any<Size>::operator=(inplace_any<OtherSize>&& other) noexcept
    -> inplace_any&
{
    static_assert(OtherSize <= Size, "cannot assign from larger inplace_any");
    destruct();
    move_from(std::move(other));
    return *this;
}

template<std::size_t Size>
inplace_any<Size>::~inplace_any()
{
    destruct();
}

template<std::size_t Size>
template<typename T, typename... Args>
    requires inplace_any_impl::compatible_value_type<T>
inplace_any<Size>::inplace_any(std::in_place_type_t<T>, Args&&... args)
{
    static_assert(sizeof(T) <= Size, "type exceeds capacity");
    construct<T>(std::forward<Args>(args)...);
}

template<std::size_t Size>
template<typename T>
    requires inplace_any_impl::compatible_value_type<T>
inplace_any<Size>::inplace_any(T&& value)
{
    static_assert(sizeof(std::decay_t<T>) <= Size, "type exceeds capacity");
    construct<std::decay_t<T>>(std::forward<T>(value));
}

template<std::size_t Size>
template<typename T>
    requires inplace_any_impl::compatible_value_type<T>
auto inplace_any<Size>::operator=(T&& value)
    -> inplace_any&
{
    static_assert(sizeof(std::decay_t<T>) <= Size, "type exceeds capacity");
    destruct();
    construct<std::decay_t<T>>(std::forward<T>(value));
    return *this;
}

template<std::size_t Size>
template<typename T, typename... Args>
    requires inplace_any_impl::compatible_value_type<T>
T& inplace_any<Size>::emplace(Args&&... args)
{
    static_assert(sizeof(T) <= Size, "type exceeds capacity");
    destruct();
    return construct<T>(std::forward<Args>(args)...);
}

template<std::size_t Size>
template<typename T, typename... Args>
T& inplace_any<Size>::construct(Args&&... args)
{
    using handler_t = inplace_any_impl::inplace_any_handler<T>;
    handler_t::construct(reinterpret_cast<std::byte*>(&storage_), &handler_, std::forward<Args>(args)...);
    return *reinterpret_cast<T*>(&storage_);
}

template<std::size_t Size>
template<std::size_t OtherSize>
void inplace_any<Size>::copy_from(const inplace_any<OtherSize>& other)
{
    if (other.handler_) {
        other.handler_(inplace_any_impl::Action::Copy,
                       const_cast<std::byte*>(reinterpret_cast<const std::byte*>(&other.storage_)),
                       reinterpret_cast<std::byte*>(&storage_), reinterpret_cast<void(**)()>(&handler_));
    }
}

template<std::size_t Size>
template<std::size_t OtherSize>
void inplace_any<Size>::move_from(inplace_any<OtherSize>&& other) noexcept
{
    if (other.handler_) {
        other.handler_(inplace_any_impl::Action::Move, reinterpret_cast<std::byte*>(&other.storage_),
                       reinterpret_cast<std::byte*>(&storage_), reinterpret_cast<void(**)()>(&handler_));
    }
}

template<std::size_t Size>
void inplace_any<Size>::destruct() noexcept
{
    if(handler_) {
        handler_(inplace_any_impl::Action::Destruct,
                 reinterpret_cast<std::byte*>(&storage_), nullptr, nullptr);
        handler_ = nullptr;
    }
}

template<std::size_t Size>
void inplace_any<Size>::swap(inplace_any& other) noexcept
{
    if (this == &other) {
        return;
    }

    if (handler_ && other.handler_) {
        inplace_any<Size> tmp;
        tmp.move_from(std::move(other));
        other.move_from(std::move(*this));
        move_from(std::move(tmp));
    } else if(handler_) {
        other.move_from(std::move(*this));
    } else {
        move_from(std::move(other));
    }
}

namespace inplace_any_impl
{

template<typename T>
void inplace_any_handler<T>::execute(Action action, std::byte* data,
                                     std::byte* other_data, void (**other_handler_ptr)())
{
    auto* handler_ptr = reinterpret_cast<HandlerFunc*>(other_handler_ptr);

    switch (action) {
    case Action::Copy:
        copy(data, other_data, handler_ptr);
        break;
    case Action::Move:
        move(data, other_data, handler_ptr);
        break;
    default: // Destruct
        reinterpret_cast<T*>(data)->~T();
    }
}

template<typename T>
template<typename... Args>
void inplace_any_handler<T>::construct(std::byte* data, HandlerFunc* handler_ptr, Args&&... args)
{
    new (static_cast<void*>(data)) T(std::forward<Args>(args)...);
    *handler_ptr = &inplace_any_handler<T>::execute;
}

template<typename T>
void inplace_any_handler<T>::move(std::byte* data, std::byte* other_data,
                                  HandlerFunc* other_handler_ptr) noexcept
{
    if (*other_handler_ptr) {
        (*other_handler_ptr)(Action::Destruct, other_data, nullptr, nullptr);
        *other_handler_ptr = nullptr;
    }
    construct(other_data, other_handler_ptr, std::move(*reinterpret_cast<T*>(data)));
}

template<typename T>
void inplace_any_handler<T>::copy(const std::byte* data, std::byte* other_data,
                                  HandlerFunc* other_handler_ptr)
{
    if (*other_handler_ptr) {
        (*other_handler_ptr)(Action::Destruct, other_data, nullptr, nullptr);
        *other_handler_ptr = nullptr;
    }
    construct(other_data, other_handler_ptr, *reinterpret_cast<const T*>(data));
}

} // namespace inplace_any_impl
} // namespace modm

#endif // MODM_INPLACE_ANY_HPP
