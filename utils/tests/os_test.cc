#include <iostream>
#include <mutex>
#include <thread>
#include <utils/os.h>

std::mutex mutex;

void threadFunction() {
    std::lock_guard<std::mutex> lock(mutex);
    std::cout << "Thread Id: " << talko::utils::os::threadId() << std::endl;
}

int main() {
    std::cout << "Process Id: " << talko::utils::os::processId() << std::endl;

    std::thread myThread1(threadFunction);
    std::thread myThread2(threadFunction);

    std::cout << __FUNCTION__ << std::endl;
    std::cout << __LINE__ << std::endl;
    std::cout << __FILE__ << std::endl;

    myThread1.join();
    myThread2.join();
}