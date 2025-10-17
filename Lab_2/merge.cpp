#include "merge.h"

vector<int> Experiment::seq_generation(int n, int mode) {
    mt19937 gen(random_device{}());
    uniform_int_distribution<int> dist(0, 1000);
    vector<int> result(n);
    for (int i = 0; i < n; i++)
        result[i] = dist(gen);

    switch (mode) {

    case 1: // сортуємо за зростанням
        sort(result.begin(), result.end());
        break;
    case 2: // множимо на 3
        for (auto& x : result) x *= 3;
        break;
    case 3: // квадрати всіх елементів
        for (auto& x : result) x = x * x;
        break;
    }

    return result;
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

Experiment ::Experiment(int n, int mode) {
    seq_1 = seq_generation(n, mode);
    seq_2 = seq_generation(n, mode);
    sort(seq_1.begin(), seq_1.end());
    sort(seq_2.begin(), seq_2.end());
    merge_result.resize(seq_1.size() + seq_2.size());
}

double Experiment::run_sequential_merge() {
    return timeit([&]() {
        merge(seq_1.begin(), seq_1.end(), seq_2.begin(), seq_2.end(), merge_result.begin());
        });
}

double Experiment::merge_seq_policy() {
    return timeit([&]() {
        merge(execution::seq, seq_1.begin(), seq_1.end(), seq_2.begin(), seq_2.end(), merge_result.begin());
        });
}
double Experiment::merge_par_policy() {
    return timeit([&]() {
        merge(execution::par, seq_1.begin(), seq_1.end(), seq_2.begin(), seq_2.end(), merge_result.begin());
        });
}
double Experiment::merge_par_unseq_policy() {
    return timeit([&]() {
        merge(execution::par_unseq, seq_1.begin(), seq_1.end(), seq_2.begin(), seq_2.end(), merge_result.begin());
        });
}

int Experiment::get_value_at_index(size_t index) const {
    size_t size_1 = seq_1.size();
    size_t size_2 = seq_2.size();

    // Якщо індекс більший за розмір обох масивів — помилка
    if (index >= size_1 + size_2)
        throw out_of_range("Index out of merged range");

    size_t low_limit;
    if (index > size_2) {
        low_limit = index - size_2;
    }
    else {
        low_limit = 0;
    }

    size_t high_limit;
    if (index < size_1) {
        high_limit = index;
    }
    else {
        high_limit = size_1;
    }


    // Бінарний пошук — шукаємо скільки брати з seq_1
    while (low_limit < high_limit) {
        size_t mid = (low_limit + high_limit) / 2;
        size_t b = index - mid;

        if (mid < size_1 && b > 0 && b <= size_2 && seq_2[b - 1] > seq_1[mid])
            low_limit = mid + 1;
        else
            high_limit = mid;
    }

    size_t a = low_limit;
    size_t b = index - a;

    if (a < size_1 && b < size_2)
        return std::min(seq_1[a], seq_2[b]);
    else if (a < size_1)
        return seq_1[a];
    else if (b < size_2)
        return seq_2[b];
    else
        throw out_of_range("Index out of merged range");
}


double Experiment::run_parallel_merge(int K) {
    return timeit([&]() {
        size_t n1 = seq_1.size();
        size_t n2 = seq_2.size();
        size_t total = n1 + n2;

        merge_result.resize(total);
        vector<thread> threads;
        threads.reserve(K);

        for (int i = 0; i < K; ++i) {
            size_t out_begin = i * total / K;
            size_t out_end = (i + 1) * total / K;

            threads.emplace_back([&, out_begin, out_end]() mutable {
                // Гарантуємо, що індекси не вийдуть за межі
                if (out_begin >= total) return;
                if (out_end > total) out_end = total;

                // Безпечні значення меж
                int val_begin = get_value_at_index(out_begin);
                int val_end = get_value_at_index(out_end - 1);

                // Знаходимо межі у першій послідовності
                auto a_begin = upper_bound(seq_1.begin(), seq_1.end(), val_begin);
                auto a_end = upper_bound(seq_1.begin(), seq_1.end(), val_end);

                // Розрахунок індексів для другої послідовності
                size_t taken_a_begin = a_begin - seq_1.begin();
                size_t taken_a_end = a_end - seq_1.begin();

                size_t b_begin_index;
                if (out_begin > taken_a_begin) {
                    b_begin_index = out_begin - taken_a_begin;
                }
                else {
                    b_begin_index = 0;
                }

                size_t b_end_index;
                if (out_end > taken_a_end) {
                    b_end_index = out_end - taken_a_end;
                }
                else {
                    b_end_index = 0;
                }

                if (b_begin_index > n2) b_begin_index = n2;
                if (b_end_index > n2) b_end_index = n2;

                auto b_begin = seq_2.begin() + b_begin_index;
                auto b_end = seq_2.begin() + b_end_index;

                // Обчислюємо вихідний ітератор без виходу за межі
                auto out_iter = merge_result.begin() + out_begin;

                merge(a_begin, a_end, b_begin, b_end, out_iter);
                });
        }

        for (auto& t : threads)
            t.join();
        });
}




void Experiment::test_parallel_merge(const vector<int>& K_values) {
    cout << "K\tTime (ms)\n";
    double best_time = 1e9;
    int best_K = 1;

    for (int K : K_values) {
        double t = run_parallel_merge(K);
        cout << K << "\t" << t << "\n";
        if (t < best_time) {
            best_time = t;
            best_K = K;
        }
    }

    cout << "\nBest K = " << best_K << " with time " << best_time << " ms\n";
    cout << "Hardware threads = " << thread::hardware_concurrency() << "\n\n\n";
}