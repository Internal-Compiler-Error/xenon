#include <catch2/catch.hpp>
#include <chrono>
#include <misc/hack_jthread.hpp>
#include <thread>

TEST_CASE("Hack jthreads behave like an automatically joining thread")
{
    using xenon::misc::hack_jthread;
    SECTION("jthreads join automatically")
    {
        using namespace std::literals;
        hack_jthread thread{[] {
            std::this_thread::sleep_for(0.2s);
        }};
        REQUIRE_NOTHROW(thread.~hack_jthread());
    }

    SECTION("jthread can be moved")
    {
        hack_jthread thread;
        thread = hack_jthread{[]{}};
    }
}