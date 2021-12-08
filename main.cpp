#include "graphics/Viewer.hpp"

using namespace Rasterizer;
using namespace std;

int main(int argc, char const* argv[]) {
    if (argc < 2) {
        std::cerr << "no enough arguments\n";
        exit(-1);
    }
    Viewer viewer;
    viewer.load(argv[1]);
    viewer.show();
    return 0;
}
