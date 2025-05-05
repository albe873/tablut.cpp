// utilities.h

#include <chrono>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <string>

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
    u_int maxDepth;
    u_long nodesExpanded;
    u_int tt_miss;
    u_int tt_hit;
public:
    SimpleMetrics();
    void reset();
    void incrementNodesExpanded();
    void updateMaxDepth(u_int depth);
    u_int getMaxDepth() const;
    u_long getNodesExpanded() const;
    
    // transposition table metrics
    u_int getTTMiss() const;
    u_int getTTHit() const;
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