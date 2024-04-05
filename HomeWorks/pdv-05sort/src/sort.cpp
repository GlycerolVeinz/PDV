#include "sort.h"

#include <stdexcept>

void radixHelperRecursive(std::vector<std::string*>& vector_to_sort, MappingFunction mapping_function,
                          size_t alphabet_size, size_t str_size, size_t start, size_t end, size_t current_char);

// implementace vaseho radiciho algoritmu. Detalnejsi popis zadani najdete v "sort.h"
void radix_par(std::vector<std::string*>& vector_to_sort, MappingFunction mapping_function,
               size_t alphabet_size, size_t str_size) {

    #pragma omp parallel default(none) shared(vector_to_sort, mapping_function, alphabet_size, str_size)
    {
        #pragma omp single
        {
            radixHelperRecursive(vector_to_sort, mapping_function, alphabet_size, str_size, 0, vector_to_sort.size(), 0);
        }
    }
    // sem prijde vase implementace. zakomentujte tuto radku

    // abeceda se nemeni. jednotlive buckety by mely reprezentovat znaky teto abecedy. poradi znaku v abecede
    // dostanete volanim funkce mappingFunction nasledovne: mappingFunction((*p_retezec).at(poradi_znaku))

    // vytvorte si spravnou reprezentaci bucketu, kam budete retezce umistovat

    // pro vetsi jednoduchost uvazujte, ze vsechny retezce maji stejnou delku - string_lengths. nemusite tedy resit
    // zadne krajni pripady

    // na konci metody by melo byt zaruceno, ze vector pointeru - vector_to_sort bude spravne serazeny.
    // pointery budou serazeny podle retezcu, na ktere odkazuji, kdy retezcu jsou serazeny abecedne
}


void radixHelperRecursive(std::vector<std::string*>& vector_to_sort, MappingFunction mapping_function,
                          size_t alphabet_size, size_t str_size, size_t start, size_t end, size_t current_char) {
    // base case
    if (current_char == str_size) {
        return;
    }

    // create buckets
    std::vector<std::vector<std::string*>> buckets(alphabet_size);

    // fill buckets
    for (size_t i = start; i < end; i++) {
        size_t bucket_index = mapping_function((*vector_to_sort[i]).at(current_char));
        buckets[bucket_index].push_back(vector_to_sort[i]);
    }

    // merge buckets
    size_t index = start;
    for (const auto& bucket : buckets) {
        for (auto & j : bucket) {
            vector_to_sort[index++] = j;
        }
    }

    // recursive call
    size_t current_start = start;
    for (size_t i = 0; i < alphabet_size; i++) {
        size_t current_end = current_start + buckets[i].size();

        #pragma omp task default(none) shared(vector_to_sort, mapping_function, alphabet_size, str_size, current_start, current_end, current_char)
        {
            radixHelperRecursive(vector_to_sort, mapping_function, alphabet_size, str_size, current_start, current_end,
                                 current_char + 1);
        }
        current_start = current_end;
    }
}
