#include <fstream>
#include <functional>
#include <utility>
#include <vector>

#include "benchmark/MathBench.hpp"
#include "utils/GitHelper.hpp"
#include "utils/Measure.hpp"
#define BENCH(x) BenchHelper::bench(x, #x)
using namespace std;
namespace BenchHelper {
vector<pair<string, int64_t>> bench_result;
int64_t bench(std::function<void()> func, std::string func_name) {
    std::cout << "BENCHING " << func_name << "()...\n";
    uint64_t ms = measure(func);
    std::cout << "BENCHRESULT: " << func_name << "() costs " << ms << "ms\n";
    bench_result.push_back({func_name, ms});
    return ms;
}
void writeToFile(const std::string& filename) {
    ofstream ofs(filename);
    for (auto& r : bench_result) {
        ofs << r.first << ": " << r.second << "ms\n";
    }
    ofs.close();
}
};  // namespace BenchHelper
int main() {
    BENCH(Mat4xMat4_100000);
    BENCH(Mat4xMat4_10000000);
    BENCH(Mat4xMat4_100000000);

    BENCH(Mat4Transpose_100000);
    BENCH(Mat4Transpose_10000000);
    BENCH(Mat4Transpose_100000000);

    BENCH(Mat4Inv_10000);
    BENCH(Mat4Inv_1000000);

    std::string filename("../benchmark/results/" + GIT_BRANCH + "-" + GIT_HASH +
                         ".txt");
    BenchHelper::writeToFile(filename);
    return 0;
}
