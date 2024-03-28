#include "tests.h"

#include <chrono>
#include <cstdio>
#include <omp.h>

template<class Test>
void eval(const char* test_name) {
    // Nejprve si vytvorime instanci testu
    Test test;

    try {
        // Cas zacatku behu testu
        auto begin = std::chrono::steady_clock::now();
        // Beh testu
        test.run_test();
        // Konec behu testu
        auto end = std::chrono::steady_clock::now();

        // Kontrola spravnosti vysledku
        if (!test.verify()) {
            printf("%s       --- wrong result ---\n", test_name);
        } else {
            auto ms = duration_cast<std::chrono::milliseconds>(end - begin);
            printf("%s          %7ldms\n", test_name, ms.count());
        }
    } catch (...) {
        printf("%s      --- not implemented ---\n", test_name);
    }
}

int main() {
    if (!omp_get_cancellation()) {
        printf("-----------------------------------------------------------------------------\n");
        printf("| WARNING: OpenMP cancellations are not enabled                             |\n");
        printf("| You can enable them by setting environment variable OMP_CANCELLATION=true |\n");
        printf("-----------------------------------------------------------------------------\n");
    }

    // V tomto testovacim pripade testujeme rychlost vyhodnocovani dotazu typu:
    //  "Jsou vsechny dilci dotazy splnene?"
    // tj., existuje pro kazdy dilci dotaz alespon jeden radek v tabulce, pro
    // ktery predikat plati?
    eval<TestAll<true>>("true = is_satisfied_for_all(...)");

    // Analogie predchoziho dotazu, ale v tomto pripade ma dotaz formu:
    //  "Existuje alespon jeden dilci dotaz, ktery je splneny?"
    // tj., existuje alespon jeden predikat a jeden radek v tabulce takovy, ze
    // je pro tento radek predikat pravdivy?
    eval<TestAny<true>>("true = is_satisfied_for_any(...)");

    // Hodnota <true> udava, zda ma byt dotaz pravdivy nebo nikoliv. Vykon Vasi
    // implementace ale budeme testovat pouze na <true> dotazech.
    //
    // POZOR! Vase implementace ale samozrejme musi byt funkcni i pro <false>
    // dotazy! (tj., return true neni reseni ;-) To si muzete otestovat na techto
    // testech:
    printf("\n");
    eval<TestAll<false>>("false = is_satisfied_for_all(...)");
    eval<TestAny<false>>("false = is_satisfied_for_any(...)");

    // Parametry generovani dat si muzete upravit v souboru params.h

    return 0;
}
