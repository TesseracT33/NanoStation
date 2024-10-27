#pragma once

#include "numtypes.hpp"
#include <concepts>

template<typename S> class InplaceFunction;

template<typename R, typename... P> class InplaceFunction<R(P...)> {
    static constexpr size_t kCapacity = 16;

public:
    template<typename F>
    InplaceFunction(F&& f)
        requires(sizeof(F) <= kCapacity) && (std::invocable<F, P...>)
      : invoke_{ invoke<F> }
    {
        ::new (static_cast<void*>(storage_)) F(f);
    }

    R operator()(P... params) const { return invoke_(storage_, params...); }

private:
    template<typename F> static R invoke(void const* storage, P... params)
    {
        return (*static_cast<F const*>(storage))(params...);
    }

    R (*invoke_)(void const*, P...);
    alignas(8) u8 storage_[kCapacity];
};
