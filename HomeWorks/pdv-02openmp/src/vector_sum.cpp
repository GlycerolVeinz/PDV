#include "vector_sum.h"
#include "omp.h"
#include <numeric>
#include <algorithm>
#include <random>

// typove aliasy pouzite v argumentech jsou definovane ve `vector_sum.h`
void vector_sum_omp_per_vector(const InputVectors& data, OutputVector& solution, size_t min_vector_size) {
    // V teto metode si vyzkousite paralelizaci na urovni vektoru. Naimplementujte paralelni pristup
    // k vypocteni sum jednotlivych vektoru (sumu vektoru data[i] ulozte do solution[i]). V teto
    // funkci zpracovavejte jednotlive vektory sekvencne a paralelizujte nalezeni sumy v jednom
    // konkretnim vektoru. Tento pristup by mel byt zejmena vhodny, pokud mate maly pocet vektoru
    // velke delky.

    for (size_t i = 0; i < data.size(); i++) {
        int64_t sum = 0;

        #pragma omp parallel for reduction(+:sum) default(none) shared(data, solution, i)
        for (auto n : data[i]) {
            sum += n;
        }

        solution[i] = sum;
    }
}

void vector_sum_omp_static(const InputVectors& data, OutputVector& solution, size_t min_vector_size) {
    // Pokud vektory, ktere zpracovavame nejsou prilis dlouhe, ale naopak jich musime zpracovat
    // velky pocet, muzeme zparalelizovat vnejsi for smycku, ktera prochazi pres jednotlive
    // vektory. Vyuzijte paralelizaci pres #pragma omp parallel for se statickym schedulingem.

    #pragma omp parallel for schedule(static) default(none) shared (data, solution)
    for (size_t i = 0; i < data.size(); i++) {
        int64_t sum = 0;
        for (auto n : data[i]) {
            sum += n;
        }
        solution[i] = sum;
    }
}

void vector_sum_omp_dynamic(const InputVectors& data, OutputVector& solution, size_t min_vector_size) {
    // Na cviceni jsme si ukazali, ze staticky scheduling je nevhodny, pokud praci mezi
    // jednotliva vlakna nedokaze rozdelit rovnomerne. V teto situaci muze byt vhodnym
    // resenim pouziti dynamickeho schedulingu. Ten je rovnez vhodny v situacich, kdy
    // nevime predem, jak dlouho budou jednotlive vypocty trvat. Dani za to je vyssi
    // rezie pri zjistovani indexu 'i', ktery ma vlakno v dane chvili zpracovavat.

    #pragma omp parallel for schedule(dynamic) default(none) shared(data, solution)
    for (size_t i = 0; i < data.size(); i++) {
        int64_t sum = 0;
        for (auto n : data[i]) {
            sum += n;
        }
        solution[i] = sum;
    }
}

std::vector<size_t> generateRandomIndexes(size_t size, size_t lowestIndex){
    std::vector<size_t> indexes = std::vector<size_t>(size);
    std::iota(indexes.begin(), indexes.end(), lowestIndex);

    std::random_device rd;
    std::mt19937 g(rd());

    std::shuffle(indexes.begin(), indexes.end(), g);
    return indexes;
}

void vector_sum_omp_shuffle(const InputVectors& data, OutputVector& solution, size_t min_vector_size) {
    // Dalsi moznosti, jak se vyhnout problemum pri pouziti statickeho schedulingu (tj.,
    // zejmena nevyvazenemu rozdeleni prace) je predzpracovani dat. V teto funkci zkuste
    // data pred zparalelizovanim vnejsi for smycky (se statickym schedulingem) nejprve
    // prohazet. To samozrejme predpoklada, ze cas potrebny na predzpracovani je radove
    // mensi, nez zisk, ktery ziskame nahrazenim dynamickeho schedulingu za staticky.
    //
    // Tip: Abyste se vyhnuli kopirovani vektoru pri "prohazovani" (ktere muze byt velmi
    // drahe!), muzete prohazovat pouze pointery na vektory. Alternativou je vytvorit si
    // nejprve pole nahodne serazenych indexu a ty pak pouzit pro pristup k poli.
    // Uzitecnymi metodami mohou byt metody `std::iota(...)` (ktera Vam vygeneruje posloupnost
    // indexu od 0 do N) a `std::shuffle(...)`, ktera zajisti nahodne prohazeni prvku.

    std::vector<size_t> indexes = generateRandomIndexes(data.size(), 0);

    #pragma omp parallel for schedule(static) default(none) shared(data, solution, indexes)
    for (size_t index : indexes){
        int64_t sum = 0;
        for (auto n : data.at(index)){
            sum += n;
        }
        solution.at(index) = sum;
    }
}
