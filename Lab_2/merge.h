#pragma once
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <thread>
#include <iostream>
#include <execution>
#include <vector>
#include <exception>

using namespace std;

class Experiment {
private:
    vector<int> seq_1;
    vector<int> seq_2;
    vector<int> merge_result;

    vector<int> seq_generation(int n, int mode);

    template <typename T>
    double timeit(T&& func);

public:
    Experiment(int n, int mode = 1);

    double run_sequential_merge();
    double merge_seq_policy();
    double merge_par_policy();
    double merge_par_unseq_policy();

    int get_value_at_index(size_t index) const;
    double run_parallel_merge(int K);
    
    void test_parallel_merge(const vector<int>& K_values);
};
