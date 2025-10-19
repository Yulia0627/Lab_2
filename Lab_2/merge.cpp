#include "merge.h"

vector<int> Experiment::seq_generation(int n, function<bool(int, int)>& comp) {
    mt19937 gen(random_device{}());
    uniform_int_distribution<int> dist(0, 1000);
    vector<int> result(n);
    for (int i = 0; i < n; i++)
        result[i] = dist(gen);

    sort(result.begin(), result.end(), comp);
    return result;
}

Experiment::Experiment(int n, function<bool(int, int)>& comp)
{
    seq_1 = seq_generation(n, comp);
    seq_2 = seq_generation(n, comp);
    merge_result.resize(seq_1.size() + seq_2.size());
}

template <typename T>
double Experiment::timeit(T&& func) {
    auto t0 = chrono::steady_clock::now();
    func();
    auto t1 = chrono::steady_clock::now();
    auto diff = t1 - t0;
    auto obj_res = chrono::duration <double, milli>(diff);
    return obj_res.count();
}


double Experiment::run_sequential_merge(const function<bool(int, int)>& comp) {
    return timeit([&]() {
        merge(seq_1.begin(), seq_1.end(),
            seq_2.begin(), seq_2.end(),
            merge_result.begin(), comp);
        });
}

double Experiment::merge_seq_policy(const function<bool(int, int)>& comp) {
    return timeit([&]() {
        merge(execution::seq, seq_1.begin(), seq_1.end(),
            seq_2.begin(), seq_2.end(),
            merge_result.begin(), comp);
        });
}

double Experiment::merge_par_policy(const function<bool(int, int)>& comp) {
    return timeit([&]() {
        merge(execution::par, seq_1.begin(), seq_1.end(),
            seq_2.begin(), seq_2.end(),
            merge_result.begin(), comp);
        });
}

double Experiment::merge_par_unseq_policy(const function<bool(int, int)>& comp) {
    return timeit([&]() {
        merge(execution::par_unseq, seq_1.begin(), seq_1.end(),
            seq_2.begin(), seq_2.end(),
            merge_result.begin(), comp);
        });
}


template <typename Iterator>
void Experiment::parallel_merge_K(
    Iterator first1, Iterator last1,
    Iterator first2, Iterator last2,
    Iterator output, int K,
    const function<bool(int, int)>& comp)
{
    unsigned long len1 = distance(first1, last1);
    unsigned long len2 = distance(first2, last2);

    if (len1 + len2 < 10000 || K <= 1)
    {
        std::merge(first1, last1, first2, last2, output, comp);
        return;
    }

    unsigned long step1 = len1 / K;
    unsigned long step2 = len2 / K;

    vector<vector<typename iterator_traits<Iterator>::value_type>> partial_results(K);
    vector<thread> threads;

    Iterator it1 = first1;
    Iterator it2 = first2;

    for (unsigned int i = 0; i < K; ++i)
    {
        Iterator end1 = (i == K - 1) ? last1 : next(it1, step1);
        Iterator end2 = (i == K - 1) ? last2 : next(it2, step2);

        threads.emplace_back([=, &partial_results, &comp]()
            {
                vector<typename iterator_traits<Iterator>::value_type> local;
                std::merge(it1, end1, it2, end2, back_inserter(local), comp);
                partial_results[i] = move(local);
            });

        it1 = end1;
        it2 = end2;
    }

    for (auto& t : threads)
        t.join();

    vector<typename iterator_traits<Iterator>::value_type> merged = partial_results[0];
    for (unsigned int i = 1; i < K; ++i)
    {
        vector<typename iterator_traits<Iterator>::value_type> temp;
        std::merge(merged.begin(), merged.end(),
            partial_results[i].begin(), partial_results[i].end(),
            back_inserter(temp), comp);
        merged.swap(temp);
    }

    move(merged.begin(), merged.end(), output);
}



void Experiment::test_parallel_merge(const function<bool(int, int)>& comp) {
    cout << "Testing parallel merge with different K...\n";

    double best_time = 1e9;
    int best_K = 1;
    merge_result.assign(seq_1.size() + seq_2.size(), 0);
    for (int K : {1, 2, 4, 8, 16, 32}) {
        merge_result.assign(seq_1.size() + seq_2.size(), 0);

        double t = timeit([&]() {
            parallel_merge_K(seq_1.begin(), seq_1.end(),
                seq_2.begin(), seq_2.end(),
                merge_result.begin(), K, comp);
            });

        cout << "K = " << K << " => time: " << t << " ms\n";

        if (t < best_time) {
            best_time = t;
            best_K = K;
        }
    }

    cout << "\nBest performance at K = " << best_K
        << " with time = " << best_time << " ms\n";
    cout << "Hardware threads available: "
        << thread::hardware_concurrency() << "\n\n";
}
