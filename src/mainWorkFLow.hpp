#pragma once 

#include "csvRead.hpp"
#include <vector>
#include "rapidcsv.h"
#include <algorithm>
#include <numeric>
#include <random>

enum TYPE {
    M = 0,
    B = 1,
};

template <typename T, typename Y>
std::ostream & operator<<(std::ostream & out, const std::pair<T, Y> & pair) {
    
        out << pair.first << " " << pair.second;
    return out;
}

template <typename T>
std::ostream & operator<<(std::ostream & out, const std::vector<T> & vec) {
    for (const auto & elem : vec) {
        out << elem << " ";
    }
    return out;
}


class MainWorkFlow {
public:

    MainWorkFlow() = default;


    /**
     * @brief Create all neccessery data
     * 
     * @param patToFile 
     * @return auto std::pair of vector of pints and std::maps wich associate each points with its kind respectevly
     */
    auto makeVectors(char patToFile[] = "data_file"){
        /* Возрарщает пару из вектора и std::map этих веторов и принадлжености к классу
        */
        std::vector<std::pair<std::string, std::vector<int>>> three_cols = read_csv("data_file");

        //rapidcsv::Document doc("/home/max/Downloads/KNNAlgorithmDataset.csv");
        rapidcsv::Document doc("data_file");
        std::vector<float> col = doc.GetColumn<float>("radius_mean");
        //std::cout << "Read " << col.size() << " values." << std::endl;

        for (int i = 0; i < col.size(); ++i){
            //std::cout << col[i] << "\n";
        }
        const int n = three_cols[0].second.size();
        //std::cout << "n =  " << n;
        const int rows_len = three_cols.size();
        //std::cout << "rows =  " << rows_len;
        std::vector<std::vector<double>> vd;

        std::vector<std::vector<double>> table;
        for (int i  = 0; i < 32; ++i){
            std::vector<double> col     = doc.GetColumn<double>(i);
        //std::cout << "Read " << col.size() << " values." << std::endl;
            table.push_back(col); 
        }

        std::map<std::vector<double>, int> mapVectorToType;
        for (int i = 0  ; i < table[0].size(); ++i){
            std::vector<double> a;
            for (int j = 0; j < table.size(); ++j){
                a.push_back(table[j][i]);
            }
            int type = a[1];
            a.erase(a.begin());
            a.erase(a.begin());
            mapVectorToType[a] = type;
            vd.push_back(a);
        }

        return std::make_pair(std::move(vd), mapVectorToType);


    }


    
};


/**
 * @brief Return teaching data and work
 * 
 * @param percentOfTeaching percent of teaching data between 0 and 1
 * @param data all entry points
 * @return auto std::pair were first is teaching vector the second is work vector
 */
auto splitOnToTestAndTeaching(double percentOfTeaching, const auto& data, int seed = 40){
         std::mt19937 rng(seed);
        std::vector<int> vec(data.size());
        std::iota(begin(vec), end(vec), 0);
        //std::cout << vec << '\n';
        std::shuffle(begin(vec), end(vec), rng);
        //std::cout << vec << std::endl;

        std::vector<std::vector<double>> teaching = {};
        std::vector<std::vector<double>> work = {};

        for (int i = 0; i < data.size(); ++i){
            if (i < percentOfTeaching*data.size()){
                //std::cout << i << '\n';
                teaching.push_back(data[vec[i]]);
            }
             else {
                work.push_back(data[vec[i]]);
            }
            
        }
       

        return std::make_pair(teaching, work);
        
    }