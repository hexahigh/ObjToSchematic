#include "Timer.h"

#include <stdio.h>

ScopedTimer::ScopedTimer(const std::string& in_label)
    : label_(in_label)
    , start_time_(std::chrono::high_resolution_clock::now())
{
}

ScopedTimer::~ScopedTimer()
{
    const auto end_time = std::chrono::high_resolution_clock::now();
    const auto duration = end_time - start_time_;
    printf("[CPP]: '%s' took %lldns\n", label_.c_str(), duration);
}