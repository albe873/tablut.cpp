// utilities.h

#include <chrono>
#include <mutex>

class Timer {
private:
    int maxTimeSeconds;
    bool timeOut;
    bool started;
    std::mutex mtx;

public:
    Timer(int maxTimeSeconds);

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