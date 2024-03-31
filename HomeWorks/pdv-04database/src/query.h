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
//   TODO parallelize this function
    bool still_satisfies = true;

    for (auto predicate : predicates) {
        for (size_t i = 0; i < data_table.size(); i++) {
            auto row = data_table[i];
            bool is_satisfied = predicate(row);

            if (is_satisfied) {
                still_satisfies = true;
                break;
            } else if (!is_satisfied && i == data_table.size() - 1) {
                still_satisfies = false;
                break;
            }
        }

        if (!still_satisfies) {
            break;
        }
    }

    return still_satisfies;
}

template<typename row_t>
bool is_satisfied_for_any(std::vector<predicate_t<row_t>> predicates, std::vector<row_t> data_table) {
//    TODO parallelize this function
    bool one_satisfies = false;

    #pragma omp parallel for default(none)  shared(one_satisfies, predicates, data_table)
    for (auto predicate : predicates) {
        #pragma omp cancellation point for
        for (size_t i = 0; i < data_table.size(); i++) {
            auto row = data_table[i];
            bool is_satisfied = predicate(row);

            if (is_satisfied) {
                one_satisfies = true;
                break;
            } else if (!is_satisfied && i == data_table.size() - 1) {
                one_satisfies = false;
                break;
            }
        }

        if (one_satisfies) {
            #pragma omp cancel for
        }
    }

    return one_satisfies;
}
