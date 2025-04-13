// utilities.cpp

#include "adversarialSearch/utilities.h"


// ------ Timer ------

Timer::Timer(int maxTimeSeconds) : maxTimeSeconds(maxTimeSeconds) {
    timeOut = false;
    started = false;
}

Timer::~Timer() {
    {
        std::lock_guard<std::mutex> lock(mtx);
        stopReq = true;
    }
    cv.notify_all();
    if (timerTh.joinable())
        timerTh.join();
}

bool Timer::start() {
    if (maxTimeSeconds <= 0)
        return false; // Invalid timer duration

    std::lock_guard<std::mutex> lock(mtx);
    if (started)
        return false; // Timer already started

    timeOut = false;
    started = true;
    stopReq = false;

    timerTh = std::thread([this]() {
        std::unique_lock<std::mutex> lk(mtx);
        cv.wait_for(lk, std::chrono::seconds(maxTimeSeconds), [this]() {
            return stopReq;
        });

        if (!stopReq && started) {
            timeOut = true;
            started = false;
        }
    });

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

