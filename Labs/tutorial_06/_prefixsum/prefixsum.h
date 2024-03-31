#ifndef SORTING_PREFIXSUM_H
#define SORTING_PREFIXSUM_H

#include <cstdlib>
#include <chrono>
#include <iostream>
#include <omp.h>
#include <vector>
#include <cmath>

template <typename T>
void prefix_sum_sequential(T * data, const size_t size);

template <typename T>
void prefix_sum_parallel(T * data, const size_t size);



template <typename T>
void prefix_sum_sequential(T * data, const size_t size) {
    for(size_t i = 1 ; i < size ; i++) {
        data[i] += data[i-1];
    }
}



template <typename T>
void prefix_sum_parallel(T * data, const size_t size) {
    std::vector<T>histogram(omp_get_max_threads());

    #pragma omp parallel default(none) shared(data, size, histogram)
    {
        // Rozdelte vypocet prefixni sumy pole mezi jednotliva vlakna
        // Zamyslete se, jak byste se mohli "vyhnout" zavislosti sumy
        // na prefixni sume predchazejicich prvku. (Teto zavislosti se
        // nelze vyhnout - ale mozna by nam stacilo "vysbirat" vysledky
        // ostatnich vlaken a nemuseli bychom prochazet cele pole?)

        int thread_id = omp_get_thread_num();
        int thread_count = omp_get_num_threads();
        size_t chunk_size = size / thread_count;
        size_t start = thread_id * chunk_size;
        size_t end = (thread_id == thread_count - 1) ? size : start + chunk_size;

        for(size_t i = start + 1 ; i < end ; i++) {
            data[i] += data[i-1];
        }

        histogram[thread_id] = data[end - 1];

        #pragma omp barrier

        T sum = 0;

        for(int i = 0 ; i < thread_id ; i++) {
            sum += histogram[i];
        }

        for(size_t i = start ; i < end ; i++) {
            data[i] += sum;
        }
    }

//	throw "Not implemented yet";
}


#endif //SORTING_PREFIXSUM_H
