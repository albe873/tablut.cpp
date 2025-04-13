// utilities.h

#include <chrono>
#include <mutex>
#include <thread>
#include <condition_variable>

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

template <class A, class U>
struct actionUtility {
    A action;
    U utility;

    bool operator<(const actionUtility<A, U>& other) const {
        return utility > other.utility;
    }
};