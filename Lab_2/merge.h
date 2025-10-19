#pragma once
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <thread>
#include <iostream>
#include <execution>
#include <functional>
#include <cmath>

using namespace std;

class Experiment {
public:
    vector<int> seq_1;
    vector<int> seq_2;
    vector<int> merge_result;

    vector<int> seq_generation(int n, function<bool(int, int)>& comp);

    template <typename T>
    double timeit(T&& func);

public:
    Experiment(int n, function<bool(int, int)>& comp);


    double run_sequential_merge(const function<bool(int, int)>& comp);
    double merge_seq_policy(const function<bool(int, int)>& comp);
    double merge_par_policy(const function<bool(int, int)>& comp);
    double merge_par_unseq_policy(const function<bool(int, int)>& comp);

    template <typename Iterator>
    void parallel_merge_K(Iterator first1, Iterator last1,
        Iterator first2, Iterator last2,
        Iterator output, int K,
        const std::function<bool(int, int)>& comp);

    void test_parallel_merge(const function<bool(int, int)>& comp);

};

