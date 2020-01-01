//
// Created by bill on 2019-12-26.
//

#ifndef XENON_HACK_JTHREAD_HPP
#define XENON_HACK_JTHREAD_HPP

#include <thread>

namespace xenon::misc
{
struct hack_jthread : public std::thread
{
    using std::thread::thread;
    hack_jthread(std::thread&&) noexcept = delete;
    hack_jthread& operator=(std::thread&&) noexcept = delete;
    hack_jthread(hack_jthread&&) noexcept  = default;
    hack_jthread& operator=(hack_jthread&&) noexcept = default;
    ~hack_jthread();
};
}    // namespace xenon::misc


#endif    // XENON_HACK_JTHREAD_HPP
