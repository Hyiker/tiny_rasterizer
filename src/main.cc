#include <iostream>

#include <Eigen/Core>

int main(int argc, char const *argv[]) {
    std::cout << "Eigen Version: ";
    std::cout << EIGEN_WORLD_VERSION << "." << EIGEN_MAJOR_VERSION << "."
              << EIGEN_MINOR_VERSION;

    return 0;
}
