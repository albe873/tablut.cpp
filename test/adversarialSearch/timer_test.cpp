// timer_test.cpp

#include "gtest/gtest.h"
#include "adversarialSearch/utilities.h"

int main(int argc, char **argv) {
    std::cout << "timer test" << std::endl;
    Timer timer(5);
    std::cout << "timer created" << std::endl;
    timer.start();
    std::cout << "timer started" << std::endl;
    while (!timer.isTimeOut()) {
        std::cout << "Waiting for timer to timeout..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cout << "Timer timed out!" << std::endl;
    timer.~Timer();
    std::cout << "Timer destructed" << std::endl;
}