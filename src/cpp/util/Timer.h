#pragma once

#include <chrono>
#include <string>

class ScopedTimer
{
public:
    ScopedTimer(const std::string& label);
    ~ScopedTimer();

private:
    const std::string label_;
    const std::chrono::steady_clock::time_point start_time_;
};