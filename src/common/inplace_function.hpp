#pragma once

#include <concepts>
#include <new>
#include <type_traits>

#include "numtypes.hpp"

template<typename S> class InplaceFunction;

template<typename R, typename... P> class InplaceFunction<R(P...)> {
    static constexpr size_t kCapacity = 16;

    template<typename F> static R invoke(void const* storage, P... params)
    {
        return (*static_cast<F const*>(storage))(params...);
    }

    alignas(16) u8 storage_[kCapacity];
    R (*invoke_)(void const*, P...);

public:
    template<typename F>
    InplaceFunction(F&& f)
        requires(sizeof(F) <= kCapacity && std::invocable<F, P...>)
      : invoke_{ invoke<std::remove_reference_t<F>> }
    {
        ::new (static_cast<void*>(storage_)) std::remove_reference_t<F>(f);
    }

    R operator()(P... params) const { return invoke_(storage_, params...); }
};
