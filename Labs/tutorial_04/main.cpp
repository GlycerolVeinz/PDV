#include <vector>
#include <iostream>
#include <atomic>
#include <chrono>
#include <numeric>
#include <algorithm>
#include <random>

#include "sequential.h"
#include "lockBased.h"
#include "lockFree.h"

using namespace std::chrono;

// Pocet prvku, ktere budeme do listu vkladat
constexpr int N = 30000;

// Pocet iteraci, po ktere budeme zkouset spravnost implementace mazani
constexpr int M = 1000000;

// Metoda, ktera zkontroluje, ze list obsahuje prvky 0..(elems-1) v serazenem poradi.
// Tato metoda funguje na libovolnem listu, ktery ma vnitrni uzly reprezentovane tridou
// 'Node', ktera obsahuje hodnotu v clenske promenne 'value' a ukazatel (neatomicky i
// atomicky) v clenske promenne 'next'. Dale potrebujeme, aby ukazatel (opet neatomicky
// i atomicky) byl v clenske promenne 'head' listu.
template<typename T>
bool check(const T & list, unsigned elems) {
    // Vsimnete si explicitniho typu 'typename T::Node *' misto 'auto'. V opacnem pri-
    // pade by se nam totiz stalo, ze current bude typu 'std::atomic<Node*>' a kod
    // nepujde zkompilovat.
    typename T::Node * current = list.head;

    // V nasi implementaci spojoveho seznamu je 'head' dummy prvek, takze ho preskocime.
    current = current->next;

    unsigned i = 0;

    // Nyni prochazime spojovy seznam a kontrolujeme, zda na 'i'-te pozici je hodnota 'i'
    while(current != nullptr) {
        if(current->value != i) return false;
        else {
            current = current->next;
            ++i;
        }
    }

    // Na zaver zkontrolujeme, ze jsme skutecne prosli 'elems' prvku
    return i == elems;
}


// Typovy system C++ nam umoznuje definovat specializovane tridy (podle typoveho parametru
// LLType). Zde rikame, ze kazdy typ spojoveho seznamu je konkurentni...
template <typename LLType>
class is_concurrent {
public:
    static constexpr bool value = true;
};
// ... az na jednu instanci, spojovy seznam typu Sequential.
template<>
class is_concurrent<Sequential> {
public:
    static constexpr bool value = false;
};

// Metoda, ktera provede kontrolu spravnosti implementace spojoveho seznamu typu LLType
// (LLType je Sequential, Concurrent a nebo Lockfree).
template <typename LLType>
double evalInsert(std::string name, double ref_time) {
    // Nejprve si vytvorime instanci spojoveho seznamu (bez ohledu na konkretni typ).
    LLType ll;

    long elapsed = -1;

    try {
        // Spojovy seznam testujeme tak, ze do nej vkladame prvky 0 .. (N-1) v nahodnem
        // poradi. Proto si je nyni predpripravime.
        std::vector<long long> data(N);
        std::iota(data.begin(), data.end(), 0);
		std::random_device rd;
		std::mt19937 g(rd());
        std::shuffle(data.begin(), data.end(), g);

        // Cas zacatku behu metody
        auto begin = steady_clock::now();

        // OpenMP ma problemy, pokud dojde k vyhozeni vyjimky (tj. v situaci, kdy metoda
        // neni jeste naimplementovana). Abychom se vyhnuli problemum, vlozime prvni prvek
        // mimo paralelni region OpenMP.
        ll.insert(data[0]);

        // Nyni uz vime, ze k vyhozeni vyjimky nedoslo a muzeme do spojoveho seznamu zacit
        // paralelne vkladat data. Vsimnete si podminky if(is_concurrent<LLType>::value).
        // Ta nam zajisti, ze pokud LLType=Sequential, vkladani do spojoveho seznamu bude
        // provadet pouze jedno vlakno (viz specializace tridy is_concurrent na radku 50).
#pragma omp parallel for if(is_concurrent<LLType>::value)
        for(int i = 1 ; i < N ; i++) ll.insert(data[i]);

        // Cas konce behu metody
        auto end = steady_clock::now();

        elapsed = static_cast<long>(duration_cast<microseconds>(end - begin).count());
        if(ref_time < 0) ref_time = elapsed;
        double speedup = ref_time / (double)elapsed;

        // Nyni uz nam staci vypsat vysledky a zkontrolovat spravnost reseni
        printf("%s %9ldus (speedup %6.3fx)           result: %s\n", name.c_str(), elapsed,
               speedup, check(ll, N) ? "correct" : "incorrect");
    } catch(...) {
        printf("%s               ----- not implemented yet -----\n", name.c_str());
    }

    return elapsed;
}

template <typename LLType>
void to_vector(std::vector<long long> &out, LLType & ll) {
    typename LLType::Node * current = ll.head->next;
    while(current != nullptr) {
        out.push_back(current->value);
        current = current->next;
    }
}

template <typename LLType>
double evalRemove(std::string name, double ref_time) {

    long elapsed = -1;
    bool isCorrect = true;

    try {
        for (int i = 0; i < M; i++) {
            // Nejprve si vytvorime instanci spojoveho seznamu (bez ohledu na konkretni typ).
            LLType ll;

            // Odtestujeme zda jsou metody naimplementovane
            ll.insert(0); ll.remove(0);

            // Cas zacatku behu metody
            auto begin = steady_clock::now();

            // Zkontrolujeme typicky problem nastavajici pri vlozeni a mazani v jeden okamzik
            #pragma omp parallel num_threads(2) if(is_concurrent<LLType>::value)
            {
                #pragma omp single
                {
                    ll.insert(1);
                    ll.insert(2);
                    ll.insert(4);
                }

                #pragma omp barrier

                #pragma omp sections
                {
                    #pragma omp section
                    {
                        ll.remove(2);
                    }

                    #pragma omp section
                    {
                        ll.insert(3);
                    }
                }

                #pragma omp barrier

                #pragma omp single
                {
                    // Cas konce behu metody
                    auto end = steady_clock::now();
                    elapsed += static_cast<long>(duration_cast<microseconds>(end - begin).count());

                    std::vector<long long> content;
                    to_vector(content, ll);

                    std::vector<long long> expected = {1, 3, 4};

                    if (content != expected) {
                        isCorrect = false;
                    }

                }
            }
        }

        if(ref_time < 0) ref_time = elapsed;
        double speedup = ref_time / (double)elapsed;

        // Nyni uz nam staci vypsat vysledky
        printf("%s %9ldus (speedup %6.3fx)           result: %s\n", name.c_str(), elapsed,
               speedup, isCorrect ? "correct" : "incorrect");

    } catch(...) {
        printf("%s               ----- not implemented yet -----\n", name.c_str());
    }

    return elapsed;
}

int main() {
    // Spustime testy jednotlivych implementaci spojoveho seznamu:
    printf("INSERT:\n");
    double sequential_time = evalInsert<Sequential>("Sequential linked-list   ", -1);
    evalInsert<Concurrent>("Lock-based linked-list   ", sequential_time);
    evalInsert<Lockfree>("Lock-free linked-list    ", sequential_time);

    printf("\n\nINSERT & REMOVE:\n");
    sequential_time = evalRemove<Sequential>("Sequential linked-list   ", -1);
    evalRemove<Concurrent>("Lock-based linked-list   ", sequential_time);
    evalRemove<Lockfree>("Lock-free linked-list    ", sequential_time);


    printf("\n\n");
    return 0;
}