// utilities.cpp

#include "adversarialSearch/utilities.h"
#include <thread>


// ------ Timer ------

Timer::Timer(int maxTimeSeconds) : maxTimeSeconds(maxTimeSeconds) {
    timeOut = false;
    started = false;
}

bool Timer::start() {
    if (maxTimeSeconds <= 0)
        return false; // Invalid timer duration

    std::lock_guard<std::mutex> lock(mtx);
    if (started)
        return false; // Timer already started

    timeOut = false;
    started = true;

    std::thread([this]() {
        std::this_thread::sleep_for(std::chrono::seconds(maxTimeSeconds));
        std::lock_guard<std::mutex> lock(mtx);
        if (started) {
            timeOut = true;
            started = false;
        }
    }).detach();

    return true; // Timer started successfully
}

void Timer::setTimeOut(bool timeOut) {
    std::lock_guard<std::mutex> lock(mtx);
    this->timeOut = timeOut;
}
bool Timer::isTimeOut() {
    std::lock_guard<std::mutex> lock(mtx);
    return timeOut;
}

