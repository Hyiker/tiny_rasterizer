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
    viewer.scene->dumpToPNG(string("../images/") + argv[1] + ".png");
    return 0;
}
