#include <knncpp.h>
#include <iostream>

#include "mainWorkFLow.hpp"
#include "mpiFlow.hpp"

typedef Eigen::MatrixXd Matrix;
typedef knncpp::Matrixi Matrixi;

int main(int argc, char* argv[]) {
    MpiFlow flow;
    flow.init(argc, argv);
    flow.start();
    return 0;
}