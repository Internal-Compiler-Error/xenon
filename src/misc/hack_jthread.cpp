//
// Created by bill on 2019-12-26.
//

#include "hack_jthread.hpp"
xenon::misc::hack_jthread::~hack_jthread() {
    if (joinable()) {
        join();
    }
}
