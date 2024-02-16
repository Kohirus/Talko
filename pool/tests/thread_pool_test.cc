#include <future>
#include <iostream>
#include <pool/thread_pool.h>
using namespace talko;

using ullong = unsigned long long;

ullong sum(ullong begin, ullong end) {
    ullong res = 0;
    for (ullong i = begin; i <= end; ++i) {
        res += i;
    }
    return res;
}

int main() {
    tp::ThreadPool::instance().start();

    auto r1 = tp::submitTask(sum, 1, 100000000);
    auto r2 = tp::submitTask(sum, 100000001, 200000000);
    auto r3 = tp::submitTask(sum, 200000001, 300000000);

    ullong a = r1.get();
    ullong b = r2.get();
    ullong c = r3.get();

    std::cout << a << std::endl;
    std::cout << b << std::endl;
    std::cout << c << std::endl;

    std::cout << a + b + c << std::endl;

    return 0;
}