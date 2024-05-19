#include <iostream>
#include <knncpp.h>

#include "mainWorkFLow.hpp"
#include "mpiFlow.hpp"

typedef Eigen::MatrixXd Matrix;
typedef knncpp::Matrixi Matrixi;


int main(int argc, char* argv[])
{


    MainWorkFlow f;
    auto x = f.makeVectors("data_file");
    
    auto map = x.second;
    auto data = splitOnToTestAndTeaching(0.7, x.first);

    auto& teaching = data.first;
    auto& work = data.second;
    //std::cout << data.first.size() << '\n';

    //std::cout << data.second.size() << '\n';

    Matrix dataPoints(teaching[0].size(), teaching.size());

    for (int i = 0; i < teaching[0].size(); ++i){

        for (int j = 0; j < teaching.size(); ++j){
            dataPoints(i, j) = teaching[i][j];
        }
    }

    knncpp::KDTreeMinkowskiX<double, knncpp::EuclideanDistance<double>> kdtree(dataPoints);

    kdtree.setThreads(1);

    kdtree.build();


    Matrix queryPoints(work[0].size(), 1);

    for (int i = 0; i < work[0].size(); ++i){
        queryPoints(i, 0) = work[i][0];
    }
    
    Matrixi indices;
    Matrix distances;

    kdtree.query(queryPoints, 10, indices, distances);


    //std::cout \
        << "Query points:" << std::endl \
        << queryPoints << std::endl \
        << "Neighbor indices:" << std::endl \
        << indices << std::endl \
        << "Neighbor distances:" << std::endl \
        << distances << std::endl;

    
    int a = 0;
    int b = 0;

    for (int i = 0; i < indices.size(); ++i){
        auto vec = teaching[indices(i)];
        if (map.count(vec) == 0){
            //assert(false);
        }
        else {
            if (map[vec] == 0) ++a;
            else ++b;
        }
    }

    //std::cout << "intit  = " << map[work[0]] << ' ' << a << ' ' << b << '\n';


    MpiFlow flow;
    flow.init(argc, argv);
    flow.start();
    return 0;
}