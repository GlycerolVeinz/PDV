#include "integrate.h"

#include <omp.h>
#include <cmath>

double
integrate_sequential(std::function<double(double)> integrand, double a, double step_size, int step_count) {
    // Promenna kumulujici obsahy jednotlivych obdelniku
    double acc = 0.0;

    for(int i = 0 ; i < step_count ; ++i) {
        double x = a + ((2 * i + 1) * step_size) / 2.0;
        acc += integrand(x) * step_size;
    }

    // Celkovy obsah aproximuje hodnotu integralu funkce
    return acc;
}

double
integrate_omp_critical(std::function<double(double)> integrand, double a, double step_size, int step_count) {
    double acc = 0.0;

    //TODO
    // Rozdelte celkovy interval na podintervaly prislusici jednotlivym vlaknum
    // Identifikujte kritickou sekci, kde musi dojit k zajisteni konzistence mezi vlakny

    #pragma omp parallel default(none) shared(acc, a, step_size, step_count, integrand)
    {
        const int threadId = omp_get_thread_num();
        const int threadsCount = omp_get_num_threads();

        const int chunk_size = 1 + step_count / threadsCount;

        const int start = threadId * chunk_size;
        const int end = std::min(chunk_size * (threadId + 1), step_count);

        for (int i = start; i < end; ++i) {
            double x = a + ((2 * i + 1) * step_size) / 2.0;

             #pragma omp critical
            {
                acc += integrand(x) * step_size;
            }
        }
    }

    return acc;
}

double
integrate_omp_atomic(std::function<double(double)> integrand, double a, double step_size, int step_count) {
    double acc = 0.0;

    //TODO
    // Rozdelte celkovy interval na podintervaly prislusici jednotlivym vlaknum
    // Identifikujte kritickou sekci, kde musi dojit k zajisteni konzistence mezi vlakny

    #pragma omp parallel
    {
        const int threadId = omp_get_thread_num();
        const int threadsCount = omp_get_num_threads();

        const int chunk_size = 1 + step_count / threadsCount;

        const int start = threadId * chunk_size;
        const int end = std::min(chunk_size * (threadId + 1), step_count);

        for (int i = start; i < end; ++i) {
            double x = a + ((2 * i + 1) * step_size) / 2.0;

            #pragma omp atomic
            acc += integrand(x) * step_size;

        }
    }
    return acc;
}

double integrate_omp_reduction(std::function<double(double)> integrand, double a, double step_size, int step_count) {
    double acc = 0.0;

    //TODO
    // Rozdelte celkovy interval na podintervaly prislusici jednotlivym vlaknum
    // Identifikujte kritickou sekci, kde musi dojit k zajisteni konzistence mezi vlakny

    #pragma omp parallel for reduction(+:acc)
    {
        for(int i = 0 ; i < step_count ; ++i) {
            double x = a + ((2 * i + 1) * step_size) / 2.0;
            acc += integrand(x) * step_size;
        }
    }

    return acc;
}

double integrate_omp_for_static(std::function<double(double)> integrand, double a, double step_size, int step_count) {
    // Promenna kumulujici obsahy jednotlivych obdelniku
    double acc = 0.0;

    //TODO
    //rozsirte integrate_omp_reduction o staticke rozvrhovani
    #pragma omp parallel for reduction(+:acc) schedule(static)
    {
        for(int i = 0 ; i < step_count ; ++i) {
            double x = a + ((2 * i + 1) * step_size) / 2.0;
            acc += integrand(x) * step_size;
        }
    }
    // Celkovy obsah aproximuje hodnotu integralu funkce
    return acc;
}

double integrate_omp_for_dynamic(std::function<double(double)> integrand, double a, double step_size, int step_count) {
    // Promenna kumulujici obsahy jednotlivych obdelniku
    double acc = 0.0;

    //TODO
    //rozsirte integrate_omp_reduction o dynamicke rozvrhovani
    #pragma omp parallel for reduction(+:acc) schedule(dynamic)
    {
        for(int i = 0 ; i < step_count ; ++i) {
            double x = a + ((2 * i + 1) * step_size) / 2.0;
            acc += integrand(x) * step_size;
        }
    }
    
    // Celkovy obsah aproximuje hodnotu integralu funkce
    return acc;
}
