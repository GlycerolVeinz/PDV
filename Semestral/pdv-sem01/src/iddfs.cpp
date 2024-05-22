#include "iddfs.h"
#include <limits>
#include <atomic>
#include <memory>

std::shared_ptr<const state> solution = nullptr;
std::atomic<bool> found(false);

static bool operator < (const std::shared_ptr<const state>& a, const std::shared_ptr<const state>& b) {
    if (!a) return false;
    if (!b) return true;
    return a->get_identifier() < b->get_identifier();
}

void dfs(size_t depth, const std::shared_ptr<const state> &node) {
    if (!node || (solution && solution->current_cost() < node->current_cost())) {
        return;
    }

    if (node->is_goal()) {
        #pragma omp critical
        {
            if (!found || node < solution) {
                solution = node;
                found = true;
            }
        }
        return;
    }

    if (depth > 0) {
        for (const auto& next : node->next_states()) {
            if (!node->get_predecessor() || node->get_predecessor()->get_identifier() != next->get_identifier()) {
                #pragma omp task
                dfs(depth - 1, next);
            }
        }
    }
}

void dfsParallel(size_t depth, const std::shared_ptr<const state>& node) {
    if (!node || (solution && solution->current_cost() < node->current_cost())) {
        return;
    }

    if (node->is_goal()) {
        #pragma omp critical
        {
            if (!found || node < solution) {
                solution = node;
                found = true;
            }
        }
        return;
    }

    if (depth > 0) {
        for (const auto& next : node->next_states()) {
            if (!node->get_predecessor() || node->get_predecessor()->get_identifier() != next->get_identifier()) {
                #pragma omp task
                dfsParallel(depth - 1, next);
            }
        }
    }
}

std::shared_ptr<const state> iddfsRecursiveTasks(std::shared_ptr<const state> root) {
    if (root->is_goal()) {
        return root;
    }

    size_t depth = 0;
    while (!found) {
    #pragma omp parallel
        {
        #pragma omp single
            {
            #pragma omp task
                dfsParallel(depth++, root);
            }
        }
    }
    return solution;
}

std::shared_ptr<const state> iddfs(std::shared_ptr<const state> root) {
    return iddfsRecursiveTasks(root);
}
