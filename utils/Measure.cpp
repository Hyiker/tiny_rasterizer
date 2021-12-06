#include "utils/Measure.hpp"

#include <chrono>
using namespace std;
int64_t measure(std::function<void()> func) {
    auto start_ms = chrono::duration_cast<chrono::milliseconds>(
                        chrono::system_clock::now().time_since_epoch())
                        .count();
    func();
    auto end_ms = chrono::duration_cast<chrono::milliseconds>(
                      chrono::system_clock::now().time_since_epoch())
                      .count();
    auto duration = end_ms - start_ms;
    return duration;
}
