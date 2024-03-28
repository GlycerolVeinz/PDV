#pragma once

#include <cstddef>
#include <vector>
#include <functional>
#include <atomic>

template<typename row_t>
using predicate_t = std::function<bool(const row_t&)>;


template<typename row_t>
bool is_satisfied_for_all(std::vector<predicate_t<row_t>> predicates, std::vector<row_t> data_table);

template<typename row_t>
bool is_satisfied_for_any(std::vector<predicate_t<row_t>> predicates, std::vector<row_t> data_table);


template<typename row_t>
bool is_satisfied_for_all(std::vector<predicate_t<row_t>> predicates, std::vector<row_t> data_table) {
//   TODO
    // Doimplementujte telo funkce, ktera rozhodne, zda pro VSECHNY dilci dotazy (obsazene ve
    // vektoru 'predicates') existuje alespon jeden zaznam v tabulce (reprezentovane vektorem
    // 'data_table'), pro ktery je dany predikat splneny.

    // Pro inspiraci si prostudujte kod, kterym muzete zjistit, zda prvni dilci dotaz plati,
    // tj., zda existuje alespon jeden radek v tabulce, pro ktery predikat reprezentovany
    // funkci predicates[i] plati:

    throw std::runtime_error{"Not implemented yet"};

    auto& first_predicate = predicates[0];        // Funkce reprezentujici predikat prvniho poddotazu

    for (size_t i = 0; i < data_table.size(); i++) {
        auto row = data_table[i];                // i-ty radek tabulky ...
        bool is_satisfied = first_predicate(row);  // ... splnuje prvni predikat, pokud funkce first_predicate
        //     vrati true

        if (is_satisfied) {
            // Nalezli jsme radek, pro ktery je predikat splneny.
            // Dilci poddotaz tedy plati

            return true;
        } else {
            // V opacnem pripade hledame dal - stale muzeme najit radek, pro ktery predikat plati
        }
    }

    // Radek, pro ktery by prvni predikat platil, jsme nenasli. Prvni dilci dotaz je tedy
    // nepravdivy
    return false;
}

template<typename row_t>
bool is_satisfied_for_any(std::vector<predicate_t<row_t>> predicates, std::vector<row_t> data_table) {
//    TODO
    // Doimplementujte telo funkce, ktera rozhodne, zda je ALESPON JEDEN dilci dotaz pravdivy.
    // To znamena, ze mate zjistit, zda existuje alespon jeden predikat 'p' a jeden zaznam
    // v tabulce 'r' takovy, ze p(r) vraci true.

    // Zamyslete se nad tim, pro ktery druh dotazu je vhodny jaky druh paralelizace. Vas
    // kod optimalizujte na situaci, kdy si myslite, ze navratova hodnota funkce bude true.
    // Je pro Vas dulezitejsi rychle najit splnujici radek pro jeden vybrany predikat, nebo
    // je dulezitejsi zkouset najit takovy radek pro vice predikatu paralelne?

    throw std::runtime_error{"Not implemented yet"};
}
