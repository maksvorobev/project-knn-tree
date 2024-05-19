#pragma once

#include <mpi.h>
#include "mainWorkFLow.hpp"

typedef Eigen::MatrixXd Matrix;
typedef knncpp::Matrixi Matrixi;

class MpiFlow {

   public:
    MpiFlow() {}

    void init(int argc, char* argv[]) {
        check_rc(MPI_Init(&argc, &argv), "MPI_Init");
        check_rc(MPI_Comm_rank(MPI_COMM_WORLD, &rank), "MPI_Comm_rank");
        check_rc(MPI_Comm_size(MPI_COMM_WORLD, &size), "MPI_Comm_size");
    }

    void start() {
        unsigned int seed;

        if (rank == 0) {
            // Процесс с рангом 0 генерирует случайное seed
            std::random_device rd;
            seed = rd();
            std::cout << "Process 0 generated seed: " << seed << '\n';
        }

        // Рассылка seed от процесса с рангом 0 ко всем остальным процессам
        check_rc(MPI_Bcast(&seed, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD),
                 "MPI_Bcast");

        MainWorkFlow f;
        auto x = f.makeVectors("home/max/KNNAlgorithmDataset.csv");

        auto map = x.second;

        auto data = splitOnToTestAndTeaching(0.7, x.first, seed);

        auto& teaching = data.first;
        auto& work = data.second;

        int z = 0;
        int p = 0;

        if (rank == 0)
            std::cout << teaching[0] << '\n';
        //data borders
        size_t segmentSize = teaching.size() / size;
        size_t startIndex = rank * segmentSize;
        size_t endIndex =
            (rank == size - 1) ? teaching.size() : (rank + 1) * segmentSize;

        size_t len = endIndex - startIndex;
        Matrix dataPoints(teaching[0].size(), len);

        for (int i = startIndex; i < endIndex; ++i) {

            for (int j = 0; j < teaching[0].size(); ++j) {
                dataPoints(j, i % len) = teaching[i][j];
            }

            if (map[teaching[i]] == 0)
                ++z;
            if (map[teaching[i]] == 1) {
                ++p;
            }
        }

        // Build k-n tree
        knncpp::KDTreeMinkowskiX<double, knncpp::EuclideanDistance<double>>
            kdtree(dataPoints);

        kdtree.setThreads(1);

        kdtree.build();

        // construct work point
        Matrix queryPoints(work[0].size(), 1);

        std::vector<double> dd;
        for (int i = 0; i < work[0].size(); ++i) {
            queryPoints(i, 0) = work[0][i];
            dd.push_back(work[0][i]);
        }

        if (rank == 0) {
            std::cout << "Truth is " << map[dd] << '\n';
            std::cout << "target  = " << dd << '\n';
        }

        Matrixi indices;
        Matrix distances;

        kdtree.query(queryPoints, numNeighbors, indices, distances);

        std::vector<int> types;

        for (int i = 0; i < indices.size(); ++i) {
            int ind = indices(i, 0);

            auto vec = teaching[startIndex + ind];
            if (map.count(vec) == 0)
                assert(false);
            types.push_back(map[vec]);
        }

        std::vector<int> allTypes;
        if (rank == 0) {
            allTypes.resize(
                numNeighbors *
                size);  // Подготовка места для данных от всех процессов
        }
        check_rc(
            MPI_Gather(types.data(), numNeighbors, MPI_INT, allTypes.data(),
                       numNeighbors, MPI_INT, 0, MPI_COMM_WORLD),
            "MPI_Gather");

        // Заполните distances результатами

        // Преобразование distances в линейный массив для отправки
        std::vector<double> distancesLinear(numNeighbors);
        Eigen::Map<Eigen::VectorXd>(distancesLinear.data(), numNeighbors) =
            distances;

        // Создание буфера для сбора данных в процессе 0
        std::vector<double> gatheredDistances;
        if (rank == 0) {
            gatheredDistances.resize(
                numNeighbors *
                size);  // Подготовка места для данных от всех процессов
        }

        check_rc(MPI_Gather(distancesLinear.data(), numNeighbors, MPI_DOUBLE,
                            gatheredDistances.data(), numNeighbors, MPI_DOUBLE,
                            0, MPI_COMM_WORLD),
                 "MPI_Gather");

        if (rank == 0) {

            std::vector<std::pair<double, int>> compressAll;
            for (int i = 0; i < numNeighbors * size; ++i)
                compressAll.push_back(
                    std::pair(gatheredDistances[i], allTypes[i]));
            std::sort(compressAll.begin(), compressAll.end(),
                      [](auto pair1, auto pair2) {
                          return (pair1.first <= pair2.first);
                      });

            std::cout << "sort vector = " << compressAll << "\n\n";

            int zeroes = 0;
            int units = 0;
            for (int i = 0; i < numNeighbors; ++i) {
                compressAll[i].second == 0 ? ++zeroes : ++units;
            }

            std::cout << "Program predict that truth is "
                      << (zeroes >= units ? 0 : 1) << '\n';
        }
    }

    ~MpiFlow() { check_rc(MPI_Finalize(), "MPI_Finalize"); }

   private:
    void check_rc(int rc, const char* str) {
        if (rc != MPI_SUCCESS) {
            printf("Error in MPI call %s\n", str);
            MPI_Abort(MPI_COMM_WORLD, rc);
        }
    }

    int rank;
    int size;
    const int numNeighbors = 4;
};