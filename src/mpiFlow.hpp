#pragma once 

#include <mpi.h>
#include "mainWorkFLow.hpp"


typedef Eigen::MatrixXd Matrix;
typedef knncpp::Matrixi Matrixi;


class MpiFlow{

public:
    MpiFlow(){

    }

    
    void init(int argc, char* argv[]){
         check_rc(MPI_Init(&argc, &argv), "MPI_Init");
         check_rc(MPI_Comm_rank(MPI_COMM_WORLD, &rank), "MPI_Comm_rank");
         check_rc(MPI_Comm_size(MPI_COMM_WORLD, &size), "MPI_Comm_size");

    }

    void start(){
        unsigned int seed;

        if (rank == 0) {
            // Процесс с рангом 0 генерирует случайное seed
            std::random_device rd;
            seed = rd();
            std::cout << "Process 0 generated seed: " << seed << '\n';
        }

        // Рассылка seed от процесса с рангом 0 ко всем остальным процессам
        check_rc(MPI_Bcast(&seed, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD), "MPI_Bcast");

        
        //std::cout << "I'm process " << rank << " using seed: " << seed << '\n';


        MainWorkFlow f;
        auto x = f.makeVectors("home/max/KNNAlgorithmDataset.csv");
    
        auto map = x.second;

        
        auto data = splitOnToTestAndTeaching(0.7, x.first, seed);

        auto& teaching = data.first;
        auto& work = data.second;

        if (rank == 0) std::cout << teaching[0] << '\n';
        //data borders
        size_t segmentSize = teaching.size() / size;
        size_t startIndex = rank * segmentSize;
        size_t endIndex = (rank == size - 1) ? teaching.size() : (rank + 1) * segmentSize;
        //if (rank == 0) std::cout << teaching[200] << "\n\n\n";
        //if (rank == 3) std::cout << teaching[200] << '\n';

        size_t len = endIndex - startIndex;
        Matrix dataPoints(teaching[0].size(), len);

        for (int i = 0; i < len; ++i){

            for (int j = 0; j < teaching[0].size(); ++j){
                dataPoints(j, i) = teaching[i][j];
            }
        }
        

        //std::cout << len << '\n';
        //std::cout << teaching.size()  << '\n';
        //std::cout << dataPoints;
        if (rank == 0){
            for (int i =0; i < dataPoints.rows(); ++i){
                    std::cout <<  dataPoints(i, 0) << '\n';
            }
        }
        
    }


    ~MpiFlow(){
        check_rc(MPI_Finalize(), "MPI_Finalize");
    }
private:

    void check_rc(int rc, const char *str) {
    if(rc != MPI_SUCCESS) {
        printf("Error in MPI call %s\n", str);
        MPI_Abort(MPI_COMM_WORLD, rc);
    }
    }

    int rank;
    int size;


};