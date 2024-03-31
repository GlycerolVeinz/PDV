#ifndef SORTING_MERGESORT_PARALLEL_H
#define SORTING_MERGESORT_PARALLEL_H

#include "mergesort.h"
#include "mergesort_sequential.h"

template <typename elem_t>
void mergesort_parallel_worker(elem_t* data, unsigned long size, elem_t* tmp);


template <typename elem_t>
void mergesort_parallel(std::vector<elem_t> & data) {
    const size_t size = data.size();
    std::vector<elem_t> tmp(size);

    // Doimplementujte paralelni verzi algoritmu mergesort za pouziti 'task'
    // v OpenMP. Muzete se inspirovat sekvencni verzi algoritmu v souboru
    // 'mergesort_sequential.h' a muzete take pouzit sekvencni metody 'merge'
    // pro sliti dvou serazenych poli do jednoho (implementovanou v souboru
    // 'mergesort.h'). 

//	throw "Not implemented yet";

    #pragma omp parallel default(none) shared(data, size, tmp)
    #pragma omp single
    mergesort_parallel_worker(&data[0], static_cast<unsigned long>(size), &tmp[0]);
}

#define CUTOFF 1000

template <typename elem_t>
void mergesort_parallel_worker(elem_t* data, unsigned long size, elem_t* tmp){
    if (size <= CUTOFF) {
        mergesort_sequential_worker(data, size, tmp);
        return;
    }

    size_t h_size = size / 2;

    elem_t* a = data;
    elem_t* b = data + h_size;

    elem_t* end = data + size;
    elem_t* tmp_a = tmp;
    elem_t* tmp_b = tmp + h_size;

    #pragma omp task default(none) shared(a, size, h_size, tmp_a)
    mergesort_parallel_worker(a, static_cast<unsigned long>(h_size), tmp_a);

    #pragma omp task default(none) shared(b, size, h_size, tmp_b)
    mergesort_parallel_worker(b, static_cast<unsigned long>(size - h_size), tmp_b);

    #pragma omp taskwait
    merge(a, b, end, tmp);
}

#endif //SORTING_MERGESORT_PARALLEL_H

