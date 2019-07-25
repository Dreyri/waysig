#pragma once

#include "ws/detail/signal_base.hpp"
#include "ws/detail/slot_access.hpp"
#include "ws/detail/util.hpp"
#include "ws/slot.hpp"

namespace ws
{
namespace detail
{
class signal_access;
}

template<typename Signature>
class signal;

template<typename Ret, typename... Args>
class signal<Ret(Args...)> : private ws::detail::signal_base<Ret>
{
    friend ws::detail::signal_access;

public:
    using result_type = Ret;

public:
    constexpr void emit(Args... args) noexcept
    {
        // this creates an instance of packaged_args.
        // packaged_args automatically handles required conversions
        auto package =
            ws::detail::make_packaged_args(std::forward<Args>(args)...);

        auto& sig_base =
            *static_cast<ws::detail::signal_base<result_type>*>(this);
        sig_base.emit(package.void_ptr());
    }

    template<typename OutputIt>
    constexpr void emit(OutputIt it, Args... args) noexcept
    {
        auto package =
            ws::detail::make_packaged_args(std::forward<Args>(args)...);

        auto& sig_base =
            *static_cast<ws::detail::signal_base<result_type>*>(this);

        sig_base.emit(std::move(it), package.void_ptr());
    }

    void connect(ws::slot<result_type(Args...)>& s) noexcept
    {
        auto& base_slot = ws::detail::slot_access::base(s);
        auto& base_sig =
            *static_cast<ws::detail::signal_base<result_type>*>(this);
        base_sig.connect(base_slot);
    }

    constexpr void operator()(Args... args) noexcept
    {
        emit(std::forward<Args>(args)...);
    }

    template<typename OutputIt>
    constexpr void operator()(OutputIt it, Args... args) noexcept
    {
        emit(std::move(it), std::forward<Args>(args)...);
    }
};

template<typename Res, typename... Args>
constexpr void emit(ws::signal<Res(Args...)>& sig, Args&&... args) noexcept
{
    sig(std::forward<Args>(args)...);
}

template<typename Res, typename... Args>
void connect(ws::signal<Res(Args...)>& sig,
             ws::slot<Res(Args...)>&   slot) noexcept
{
    sig.connect(slot);
}
} // namespace ws
