#ifndef SORTING_COUNTINGSORT_H
#define SORTING_COUNTINGSORT_H

#include <vector>
#include <limits>
#include <cstdlib>
#include <chrono>
#include <omp.h>
#include <atomic>
#include "../_prefixsum/prefixsum.h"

template<typename elem_t>
void counting_sequential(std::vector<elem_t> &data) {
    elem_t max = std::numeric_limits<elem_t>::min();
    elem_t min = std::numeric_limits<elem_t>::max();

    for (auto &&el: data) {
        if (el < min) min = el;
        if (el > max) max = el;
    }

    const size_t range = max - min + 1;
    std::vector<unsigned int> histogams(range);

    for (auto &&el: data) {
        ++histogams[el - min];
    }

    unsigned int idx = 0;
    for (unsigned int i = 0; i < range; i++) {
        unsigned int count = histogams[i];
        unsigned int number = min + i;
        for (unsigned int k = 0; k < count; k++) {
            data[idx++] = number;
        }
    }
}

template<typename elem_t>
void counting_parallel(std::vector<elem_t> &data) {
    // Naimplementujte paralelni verzi counting-sortu. Zamyslete se nad tim,
    // jak muzete paralelizovat:
    //   1) Nalezeni rozsahu hodnot v poli
    //   2) Napocitani "histogramu"
    //   3) "Vypsani" histogramu do pole 'data'
    // Inspirovat se muzete sekvencni implementaci

//	throw "Not implemented yet";

    elem_t maxElem = std::numeric_limits<elem_t>::min();
    elem_t minElem = std::numeric_limits<elem_t>::max();

#pragma omp parallel for reduction(min:minElem) reduction(max:maxElem) default(none) shared(data)
    for (auto &&el: data) {
        if (el < minElem) minElem = el;
        if (el > maxElem) maxElem = el;
    }

    const size_t range = maxElem - minElem + 1;
    std::vector<unsigned int> histogams(range);

#pragma omp parallel for default(none) shared(data, histogams, minElem)
    for (auto &&el: data) {
#pragma omp atomic
        ++histogams[el - minElem];
    }

    prefix_sum_sequential(&histogams[0], range);

//    unsigned int idx = 0;
//    #pragma omp parallel for default(none) shared(data, histogams, minElem, range)
    for (unsigned int i = 0; i < range; i++) {
        size_t start = i == 0 ? 0 : histogams[i - 1];
        size_t end = histogams[i];
        unsigned int count = histogams[i];
        unsigned int number = minElem + i;

#pragma omp parallel for default(none) shared(data, start, end, number)
        for (unsigned int k = start; k < end; k++) {

            data[k] = number;

//            data[idx] = number;
        }
    }
}

#endif //SORTING_COUNTINGSORT_H

