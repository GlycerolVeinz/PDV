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
bool is_satisfied_for_any_parallel_predicates(std::vector<predicate_t<row_t>> predicates, std::vector<row_t> data_table);

template<typename row_t>
bool is_satisfied_for_any_parallel_rows(std::vector<predicate_t<row_t>> predicates, std::vector<row_t> data_table);

template<typename row_t>
bool is_satisfied_for_all(std::vector<predicate_t<row_t>> predicates, std::vector<row_t> data_table) {
//   TODO parallelize this function
    std::atomic<bool> still_satisfies = true;

    #pragma omp parallel for default(none) shared(still_satisfies, predicates, data_table)
    for (auto predicate : predicates) {
        #pragma omp cancellation point for
        for (size_t i = 0; i < data_table.size(); i++) {
            auto row = data_table[i];
            bool is_satisfied = predicate(row);

            if (is_satisfied) {
                still_satisfies = true;
                break;
            } else if (!is_satisfied && i == data_table.size() - 1) {
                still_satisfies = false;
            }
        }

        #pragma omp cancel for if(!still_satisfies)
    }

    return still_satisfies;
}

template<typename row_t>
bool is_satisfied_for_any(std::vector<predicate_t<row_t>> predicates, std::vector<row_t> data_table) {
//    TODO parallelize this function
    return is_satisfied_for_any_parallel_rows(predicates, data_table);
}

template<typename row_t>
bool is_satisfied_for_any_parallel_rows(std::vector<predicate_t<row_t>> predicates, std::vector<row_t> data_table){
    std::atomic<bool> one_satisfies = false;

    #pragma omp parallel for default(none)  shared(one_satisfies, predicates, data_table)
    for (size_t i = 0; i < data_table.size(); ++i){
        #pragma omp cancellation point for
        for (auto predicate : predicates){
            auto row = data_table[i];
            bool is_satisfied = predicate(row);

            if (is_satisfied) {
                one_satisfies = true;
                break;
            }
        }

        #pragma omp cancel for if (one_satisfies)
    }

    return one_satisfies;
}

template <typename row_t>
bool is_satisfied_for_any_parallel_predicates(std::vector<predicate_t<row_t>> predicates, std::vector<row_t> data_table){
    std::atomic<bool> one_satisfies = false;
    std::atomic<bool> ret = false;

    #pragma omp parallel for default(none)  shared(one_satisfies, predicates, data_table, ret)
    for (auto predicate : predicates) {
        #pragma omp cancellation point for
        for (size_t i = 0; i < data_table.size(); i++) {
            auto row = data_table[i];
            bool is_satisfied = predicate(row);

            if (is_satisfied) {
                one_satisfies = true;
                ret = true;
                break;
            } else if (!is_satisfied && i == data_table.size() - 1) {
                one_satisfies = false;
            }
        }
        #pragma omp cancel for if (ret)
    }

    return ret;
}
