// utilities.h

#include <chrono>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <string>
#include <cstdint>

#ifndef UTILITIES_H
#define UTILITIES_H

class Timer {
private:
    int maxTimeSeconds;
    bool timeOut;
    bool started;
    bool stopReq;
    std::mutex mtx;
    std::thread timerTh;
    std::condition_variable cv;

public:
    Timer(int maxTimeSeconds);
    ~Timer();

    bool start();
    void setTimeOut(bool timeOut);
    bool isTimeOut();
};

class SimpleMetrics {
private:
    std::mutex mtx;
    uint32_t maxDepth;
    uint64_t nodesExpanded;
    uint32_t tt_miss;
    uint32_t tt_hit;
public:
    SimpleMetrics();
    void reset();
    void incrementNodesExpanded();
    void updateMaxDepth(uint32_t depth);
    uint32_t getMaxDepth() const;
    uint64_t getNodesExpanded() const;
    
    // transposition table metrics
    uint32_t getTTMiss() const;
    uint32_t getTTHit() const;
    void incrementTTMiss();
    void incrementTTHit();
    std::string toString() const;
};

template <class A, class U>
struct actionUtility {
    A action;
    U utility;
    bool completed = true; // default value

    bool operator<(const actionUtility<A, U>& other) const {
        return utility > other.utility;
    }
};

#endif // UTILITIES_H