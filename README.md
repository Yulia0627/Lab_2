The goal of this project is to study the performance of the merge algorithm when processing datasets of different sizes under various execution conditions and data conversion. The research includes both standard library implementations and a custom parallel algorithm.

The following experiments were conducted:

Baseline experiment: measuring the execution time of the library merge algorithm without using any execution policy.

Execution policies: analyzing the impact of different policies (seq, par, par_unseq) on the performance of the library merge algorithm.

Custom parallel implementation: developing an algorithm that splits the dataset into K approximately equal parts. Each part is processed in a separate thread using the sequential merge algorithm, and then the results are combined using a sequential merge.

Execution time was measured for various values of parameter K, and the dependence of runtime on the number of threads was plotted. The value of K that provides the best performance was determined and compared with the number of physical CPU threads.

As a result, the relationship between runtime and increasing K was analyzed, and the efficiency of parallel processing was evaluated.
