#pragma once

#include <cstddef>
#include <cstdint>

//
// Parametry ovlivnujici generovana data
//

// Kvuli replikovatelnosti vysledku pouzivame zafixovany seed pouzivany pri generaovani nahodne
// databaze. Budeme samozrejme radi, pokud si Vase reseni otestujete i na jinych seedech.
constexpr uint64_t SEED = 15;


// nastaveni parametru pro generovani konjunkci/disjunkci
// ocekavana pravdepodobnost, ze predikat v disjunkci/konjunkci bude pro vygenerovana data pravdivy
constexpr double t_probability_disjunction_predicate = 0.8;
constexpr double t_probability_conjunction_predicate = 0.99;


//
// Parametry pro generovani datasetu, pro ktere ma vyhodnocovany dotaz platit (tj. <true>)
//

// parametry data setu pro dotazy typu GENERATE ALL (konjunkce predikatu)
// pocet predikatu jako delka dotazu
constexpr size_t length_of_query_all_true = 100'000;
// pocet radku tabulky
constexpr size_t count_of_rows_all_true = 100'000;

// parametry data setu pro dotazy typu GENERATE ANY (disjunkce predikatu)
// pocet predikatu jako delka dotazu
constexpr size_t length_of_query_any_true = 500;
// pocet radku tabulky
constexpr size_t count_of_rows_any_true = 500'000;


//
// Parametry pro generovani datasetu, pro ktere nema vyhodnocovany dotaz platit (tj. <false>)
//

// pocet predikatu jako delka dotazu
constexpr size_t length_of_query_all_false = 30;
// pocet radku tabulky
constexpr size_t count_of_rows_all_false = 30'000;


// pocet predikatu jako delka dotazu
constexpr size_t length_of_query_any_false = 300;
// pocet radku tabulky
constexpr size_t count_of_rows_any_false = 2000;
