#include "merge.h"
#include <vector>
using namespace std;

void print(Experiment obj) {
    cout << "Sequential merge time without policy: " << obj.run_sequential_merge() << " ms\n";
    cout << "Sequential merge time with policy: " << obj.merge_seq_policy() << " ms\n";
    cout << "Parallel merge time: " << obj.merge_par_policy() << " ms\n";
    cout << "Parallel unsequenced merge time: " << obj.merge_par_unseq_policy() << " ms\n";
    vector<int> K_values = { 1, 2, 4, 8, 16 };
    obj.test_parallel_merge(K_values);
}

int main() {
    Experiment experiment_1(500);
    print(experiment_1);

    Experiment experiment_2(500, 2);
    print(experiment_2);

    Experiment experiment_3(500, 3);
    print(experiment_3);
    
    Experiment experiment_4(50000);
    print(experiment_4);

    Experiment experiment_5(50000, 2);
    print(experiment_5);

    Experiment experiment_6(50000, 3);
    print(experiment_6);

    Experiment experiment_7(200000);
    print(experiment_7);

    Experiment experiment_8(200000, 2);
    print(experiment_8);
    Experiment experiment_9(200000, 3);
    print(experiment_9);

    /*Experiment experiment_4(1000000);
    print(experiment_4);*/

    Experiment experiment_10(300000000);
    print(experiment_10);

    Experiment experiment_11(300000000, 2);
    print(experiment_11);
    Experiment experiment_12(300000000, 3);
    print(experiment_12);
    

    /*Experiment exp(n);
    exp.run_parallel_merge(2);

    cout << "seq_1: ";
    for (int v : exp.seq_1) cout << v << " ";
    cout << "\nseq_2: ";
    for (int v : exp.seq_2) cout << v << " ";
    cout << "\nmerged: ";
    for (int v : exp.merge_result) cout << v << " ";
    cout << "\n";*/

    return 0;
}
