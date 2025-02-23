#include "decompose.h"

#include <cmath>
#include <atomic>
#include <iostream>
#include <limits>

// Collatzuv problem (znamy take jako uloha 3n+1) je definovany jako
// posloupnost cisel generovana podle nasledujicich pravidel
//   1) Pokud je n liche, dalsi prvek posloupnosti je 3n+1
//   2) Pokud je n sude, dalsi prvek posloupnosti je n/2
// Plati, ze pro libovolne prirozene cislo n, posloupnost cisel
// "dosahne" cisla 1. Na dnesnim cviceni nas bude zajimat, po kolika
// krocich (tj. kolik operaci (1) a (2)) je pro to potreba. To muzeme
// zjistit pomoci nasledujici jednoduche funkce:

unsigned long collatz(unsigned long k) {
    long steps = 0;
    while (k > 1) {
        ++steps;
        if (k % 2)
            k = 3 * k + 1;
        else
            k /= 2;
    }
    return steps;
}



// V prvni sade uloh 'findmin_*' hledame cislo 'n' z vektoru 'data', pro
// ktere Collatzova sekvence dosahne cisla 1 nejrychleji. Takova uloha
// se da dobre paralelizovat, protoze muzeme hodnotu funkce 'collatz(n)'
// spocitat pro kazdy prvek pole nezavisle. V nasem pripade hledame dane
// cislo v intervalu [MIN_ELEMENT .. MAX_ELEMENT]. Tyto hodnoty si muzete
// upravit v souboru 'main.cpp'.
//
// Mnoho optimalizacnich problemu jde resit efektivneji. Pokud zjistime,
// ze dany prvek garantovane neni optimalni, nemusime se snazit spocitat
// jeho presnou hodnotu (v tomto pripade pocet potrebnych kroku). Mohli
// bychom takoveto odrezavani pouzit i v tomto pripade?
unsigned long findmin_sequential(const std::vector<unsigned long> &data) {
    unsigned long min = std::numeric_limits<unsigned long>::max();

    for (auto startingVal : data){
        int idx = static_cast<int>(startingVal);
        unsigned long steps = 0;
        while (idx > 1) {
            ++steps;
            if (steps > min) break;
            if (idx % 2) idx = 3 * idx + 1;
            else idx /= 2;
        }
        if (steps < min) {
            min = steps;
        }
    }

    return min;

}

unsigned long findmin_parallel(const std::vector<unsigned long> &data) {
    std::atomic<unsigned long> min{std::numeric_limits<unsigned long>::max()};
//    unsigned long min = std::numeric_limits<unsigned long>::max();

//    #pragma omp parallel for reduction(min:min)  default(none) shared(data)
    #pragma omp parallel for default(none) shared(data, min)
    for (auto startingVal : data){
        int idx = static_cast<int>(startingVal);
        unsigned long steps = 0;
        while (idx > 1) {
            ++steps;
            if (steps > min) break;
            if (idx % 2) idx = 3 * idx + 1;
            else idx /= 2;
        }
        if (steps < min) {
            min = steps;
        }
    }

    return min;
}

// V druhe sade uloh 'findn_*' se zamerime na opacny problem. Dostaneme
// delku sekvence 'criteria' a nasim ukolem bude nalezt cislo 'n', pro
// ktere je hodnota collatz(n) >= criteria. Hodnotu parametru 'criteria'
// si muzete upravit v souboru main.cpp.
//
// Vsimnete si, ze v tomto pripade nevime predem kolik prvku budeme muset
// zpracovat, nez narazime na potrebne 'n'. Nevime proto predem, jak mame
// data rozdelit mezi jednotliva vlakna.

unsigned long findn_sequential(unsigned long criteria) {
    for (unsigned long i = 1 ; ; i++){
        unsigned long value = collatz(i);
        if (value > criteria) return i;
    }
}

unsigned long findn_parallel(unsigned long criteria) {
    std::atomic<unsigned long> i{1};
    unsigned long answer = 0;

    #pragma omp parallel default(none) shared(i, criteria, answer)
    while(true) {
        #pragma omp cancellation point parallel
        unsigned long current_i = i;

        ++i;

        auto collatz_value = collatz(current_i);
        if (collatz_value >= criteria) [[unlikely]] {
            #pragma omp critical(name)
            {
                answer = current_i;
            }
            #pragma omp cancel parallel
        }
    }

    return answer;
}

// V posledni sade uloh se zamerime na vypocet Fibonacciho cisla pomoci
// rekurze a budete si moci vyzkouset #pragma omp task pro rekurzivni
// paralelizaci. V testech pocitame cislo FIB_QUERY, ktere si muzete
// upravit v souboru main.cpp.

#define ull unsigned long long

ull fibonacci_sequential(unsigned int n) {
    if(n <= 2) return 1;
    else return fibonacci_sequential(n-1) + fibonacci_sequential(n-2);
}

ull fibonacci_parallel_worker(unsigned int n);

ull fibonacci_parallel(unsigned int n) {
    unsigned long long result = 0;

    #pragma omp parallel default(none) shared(result, n)
    {
        #pragma omp single
        {
            result = fibonacci_parallel_worker(n);
        };
    }
    return result;
}


#define CUTOFF 20

ull fibonacci_parallel_worker(unsigned int n) {
    if (n <= CUTOFF) return fibonacci_sequential(n);
    else {
        ull a, b;
        #pragma omp task shared(a, n) default(none)
        a = fibonacci_parallel_worker(n-1);

        #pragma omp task shared(b, n) default(none)
        b = fibonacci_parallel_worker(n-2);

        #pragma omp taskwait
        return a + b;
    }
}