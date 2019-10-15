#include <catch2/catch.hpp>

#include <type_traits>

#include "ws/ext/wl.hpp"

TEST_CASE("wayland")
{
    SECTION("abi")
    {
        SECTION("link")
        {
            static_assert(ws::detail::is_link_layout_compatible_v);
        }

        SECTION("list")
        {
            static_assert(ws::detail::is_list_layout_compatible_v);
        }

        SECTION("listener")
        {
            static_assert(ws::detail::is_slot_layout_compatible_v);
        }

        SECTION("signal")
        {
            static_assert(ws::detail::is_signal_layout_compatible_v);
        }
    }

    SECTION("functions")
    {
        SECTION("wl_signal_slot")
        {
            int       i = 0;
            wl_signal sig;
            wl_signal_init(&sig);

            // inserted first so invoked first
            ws::slot<void(int&)> s1{[](auto& self, int& i) {
                (void) self;
                REQUIRE(i == 0);
                ++i;
            }};

            ws::connect(sig, s1);
            ws::emit(sig, i);
            REQUIRE(i == 1);

            {
                i = 0;
                ws::slot<void(int&)> s0{[](auto& self, int& i) {
                    (void) self;
                    REQUIRE(i == 1);
                    ++i;
                }};

                ws::connect(sig, s0);
                ws::emit(sig, i);
                REQUIRE(i == 2);

                i = 0;
            }

            ws::emit(sig, i);
            REQUIRE(i == 1);
        }

        SECTION("signal_wl_listener")
        {
            int                    i = 0;
            ws::signal<void(int&)> sig;

            wl_listener s0;
            s0.notify = [](auto*, void* data) {
                int* i = static_cast<int*>(data);
                REQUIRE(*i == 0);
                ++(*i);
            };

            ws::connect(sig, s0);
            ws::emit(sig, i);
            REQUIRE(i == 1);

            {
                // simulate the scope from above
                i = 0;

                wl_listener s1;
                s1.notify = [](auto*, void* data) {
                    int* i = static_cast<int*>(data);
                    REQUIRE(*i == 1);
                    ++(*i);
                };

                ws::connect(sig, s1);
                ws::emit(sig, i);
                REQUIRE(i == 2);

                i = 0;
                wl_list_remove(&s1.link);
            }

            ws::emit(sig, i);
            REQUIRE(i == 1);
        }
    }
}