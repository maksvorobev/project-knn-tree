#pragma once 

#include "csvRead.hpp"
#include <vector>
#include "rapidcsv.h"

class MainWorkFlow {
public:

    MainWorkFlow() = default;



    auto makeVectors(char patToFile[]){
        std::vector<std::pair<std::string, std::vector<int>>> three_cols = read_csv("/home/max/Downloads/KNNAlgorithmDataset.csv");

        rapidcsv::Document doc("/home/max/Downloads/KNNAlgorithmDataset.csv");
        std::vector<float> col = doc.GetColumn<float>("radius_mean");
        std::cout << "Read " << col.size() << " values." << std::endl;

        for (int i = 0; i < col.size(); ++i){
            std::cout << col[i] << "\n";
        }
        const int n = three_cols[0].second.size();
        //std::cout << "n =  " << n;
        const int rows_len = three_cols.size();
        //std::cout << "rows =  " << rows_len;
        std::vector<std::vector<double>> vd;

        std::vector<std::vector<double>> table;
        for (int i  =2; i < 32; ++i){
            std::vector<double> col     = doc.GetColumn<double>(i);
        std::cout << "Read " << col.size() << " values." << std::endl;
            table.push_back(col); 
        }

        for (int i = 0  ; i < table[0].size(); ++i){
            std::vector<double> a;
            for (int j = 0; j < table.size(); ++j){
                a.push_back(table[j][i]);
                std::cout << table[j][i] << " " ;
            }
            std::cout << "\n";
            vd.push_back(std::move(a));
        }

        return std::move(vd);


    }
};