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
    return timeOut;
}

// ------ SimpleMetrics ------
SimpleMetrics::SimpleMetrics() : maxDepth(0), nodesExpanded(0), tt_hit(0), tt_miss(0) {}

void SimpleMetrics::incrementNodesExpanded() {
    std::lock_guard<std::mutex> lock(mtx);
    nodesExpanded++;
}
void SimpleMetrics::reset() {
    std::lock_guard<std::mutex> lock(mtx);
    maxDepth = 0;
    nodesExpanded = 0;
    tt_hit = 0;
    tt_miss = 0;
}
void SimpleMetrics::updateMaxDepth(u_int depth) {
    std::lock_guard<std::mutex> lock(mtx);
    if (depth > maxDepth)
        maxDepth = depth;
}
u_int SimpleMetrics::getMaxDepth() const {
    return maxDepth;
}
u_long SimpleMetrics::getNodesExpanded() const {
    return nodesExpanded;
}
void SimpleMetrics::incrementTTMiss() {
    std::lock_guard<std::mutex> lock(mtx);
    tt_miss++;
}
void SimpleMetrics::incrementTTHit() {
    std::lock_guard<std::mutex> lock(mtx);
    tt_hit++;
}
u_int SimpleMetrics::getTTMiss() const {
    return tt_miss;
}
u_int SimpleMetrics::getTTHit() const {
    return tt_hit;
}
std::string SimpleMetrics::toString() const {
    return "Max Depth: " + std::to_string(maxDepth) + ", Nodes Expanded: " + std::to_string(nodesExpanded) +
           ", TT Miss: " + std::to_string(tt_miss) + ", TT Hit: " + std::to_string(tt_hit);
}