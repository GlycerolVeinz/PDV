#ifndef SORTING_MERGESORT_SEQUENTIAL_H
#define SORTING_MERGESORT_SEQUENTIAL_H

#include <vector>
#include "mergesort.h"

template <typename elem_t>
void mergesort_sequential_worker(elem_t* data, unsigned long size, elem_t* tmp);


template <typename elem_t>
void mergesort_sequential(std::vector<elem_t> & data) {
    const size_t size = data.size();

	// Vytvorime si pomocne pole, do ktereho budeme provadet operaci 'merge'
	std::vector<elem_t> tmp(size);

    // Zavolame rekurzivni funkci mergesortu
    mergesort_sequential_worker(&data[0], static_cast<unsigned long>(size), &tmp[0]);
}


template <typename elem_t>
void mergesort_sequential_worker(elem_t* data, unsigned long size, elem_t* tmp) {
    // Pole velikosti mensi nez 1 je serazene
    if (size <= 1) {
        return;
    }

    size_t h_size = size / 2;

    // Nejprve si pole rozdelime na dve casti [a .. b-1] a [b .. end-1]
    elem_t* a = data;             // Pointer na zacatek segmentu
    elem_t* b = data + h_size;    // Pointer doprostred segmentu (konec leve poloviny)
    elem_t* end = data + size;    // Konec segmentu (konec prave poloviny)

    // Stejnym zpusobem si rozdelime i pomocne pole
    elem_t* tmp_a = tmp;
    elem_t* tmp_b = tmp + h_size;

    // Zavolame rekurzivni volani na levou a pravou polovinu
    mergesort_sequential_worker(a, static_cast<unsigned long>(h_size), tmp_a);

    mergesort_sequential_worker(b, static_cast<unsigned long>(size - h_size), tmp_b);

    // A vysledky nakonec slijeme pomoci operace merge
    merge(a, b, end, tmp);
}

#endif //SORTING_MERGESORT_SEQUENTIAL_H
