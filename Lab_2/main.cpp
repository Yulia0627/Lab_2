// Compiler: Microsoft (R) C/C++ Optimizing Compiler Version 19.44.35216 for x64
// Standart: C++20

#include "merge.h"


void print(Experiment& obj, const string& name, function<bool(int, int)> comp) {
    cout << "\n=== Comparator: " << name << " ===\n";
    cout << "Sequential merge time (no policy): "
        << obj.run_sequential_merge(comp) << " ms\n";
    cout << "Sequential merge time (seq policy): "
        << obj.merge_seq_policy(comp) << " ms\n";
    cout << "Parallel merge time (par policy): "
        << obj.merge_par_policy(comp) << " ms\n";
    cout << "Parallel unsequenced merge time: "
        << obj.merge_par_unseq_policy(comp) << " ms\n";
    obj.test_parallel_merge(comp);
}

int main() {

    function<bool(int, int)> comp_normal = [](int a, int b) { return a < b; };

    function<bool(int, int)> comp_slow = [](int a, int b) {
        double x = std::sin(a) * std::cos(b) + std::sqrt(std::abs(a - b) + 1.0);
        return x < 100.0;
        };

    Experiment experiment_1(100, comp_normal);
    Experiment experiment_2(100, comp_slow);
    Experiment experiment_3(10000, comp_normal);
    Experiment experiment_4(10000, comp_slow);
    Experiment experiment_5(3000000, comp_normal);
    Experiment experiment_6(3000000, comp_slow);

    print(experiment_1, "Normal comparator for 100 elements", comp_normal);
    print(experiment_2, "Slow math comparator for 100 elements", comp_slow);

    print(experiment_3, "Normal comparator for 10000 elements", comp_normal);
    print(experiment_4, "Slow math comparator for 10000 elements", comp_slow);

    print(experiment_5, "Normal comparator for 3000000 elements", comp_normal);
    print(experiment_6, "Slow math comparator for 3000000 elements", comp_slow);

    return 0;
}


