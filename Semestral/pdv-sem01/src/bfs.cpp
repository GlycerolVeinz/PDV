#include "bfs.h"
#include <queue>
#include <unordered_set>
#include <omp.h>

static bool operator<(const state_ptr &a, const state_ptr &b) {
    if (!a) return false;
    if (!b) return true;
    return a->get_identifier() < b->get_identifier();
}

/*
 *  These function declarations show my progress on this task.
 *  some of them don't work at all, and some of them are slow,
 *  but final goal will be encapsulated in bfs() function
 * */
state_ptr bfsSingle(state_ptr root);

state_ptr bfsFirstTry(state_ptr root);

state_ptr bfsLevel(state_ptr root);

state_ptr bfsMultiPop(state_ptr root);

state_ptr bfsLevelSpinGoal(state_ptr root);

state_ptr bfsTwoFor(state_ptr root);


// Naimplementujte efektivni algoritmus pro nalezeni nejkratsi cesty v grafu.
// V teto metode nemusite prilis optimalizovat pametove naroky, a vhodnym algo-
// ritmem tak muze byt napriklad pouziti prohledavani do sirky (breadth-first
// search.
//
// Metoda ma za ukol vratit ukazatel na cilovy stav, ktery je dosazitelny pomoci
// nejkratsi cesty.
state_ptr bfs(state_ptr root) {
    return bfsTwoFor(root);
}


state_ptr bfsSingle(state_ptr root) {
    std::unordered_set<unsigned long long> visited;
    std::queue<state_ptr> q;
    state_ptr goal = nullptr;

    q.push(root);

    while (!q.empty()) {
        state_ptr current = q.front();
        q.pop();

        if (visited.contains(current->get_identifier()))
            continue;
        visited.insert(current->get_identifier());

        for (const state_ptr &child: current->next_states()) {

            {
                if (child->is_goal() && child < goal)
                    goal = child;
            }

            {
                if (!visited.contains(child->get_identifier()))
                    q.push(child);
            }
        }
    }

    return goal;
}

state_ptr bfsTwoFor(state_ptr root) {
    std::unordered_set<unsigned long long> visited;
    std::queue<state_ptr> q;
    state_ptr goal = nullptr;

    q.push(root);
    visited.insert(root->get_identifier());

    while (!q.empty() && !goal) {
        size_t qSize = q.size();

        #pragma omp parallel for
        for (size_t i = 0; i < qSize; ++i) {
            state_ptr current;

            #pragma omp critical
            {
                current = q.front();
                q.pop();
            }

            #pragma omp parallel for
            for (const state_ptr &child: current->next_states()) {
                #pragma omp critical
                {
                    if (!visited.contains(child->get_identifier())){
                        visited.insert(child->get_identifier());
                        if (child->is_goal() && child < goal){
                            goal = child;
                        }
                        q.push(child);
                    }
                }
            }

        }
    }

    return goal;
}

state_ptr bfsFirstTry(state_ptr root) {
    if (root->is_goal() || !root)
        return root;

    std::vector<std::vector<state_ptr>> graphLevels;
    std::unordered_set<unsigned long long> visited;
    state_ptr goal = nullptr;

#pragma omp parallel
    {
#pragma omp single
        {
            std::queue<state_ptr> q;
            q.push(root);

            while (!q.empty()) {
                size_t levelSize = q.size();
                std::vector<state_ptr> currentLevel;

                for (size_t i = 0; i < levelSize; ++i) {
                    state_ptr current = q.front();
                    q.pop();

                    if (visited.contains(current->get_identifier()))
                        continue;

#pragma omp critical
                    {
                        visited.insert(current->get_identifier());
                    }
                    currentLevel.push_back(current);

                    for (const state_ptr &child: current->next_states()) {
                        if (!visited.contains(child->get_identifier()))
                            q.push(child);
                    }
                }

#pragma omp critical
                {
                    graphLevels.push_back(currentLevel);
                }
            }
        }

        size_t level = 0;
        while (level < graphLevels.size()) {
            std::vector<state_ptr> &currentLevel = graphLevels[level];

            for (state_ptr &current: currentLevel) {
                if (current->is_goal() && current < goal)
                    goal = current;
            }
            ++level;
        }
    }

    return goal;
}

state_ptr bfsLevel(state_ptr root) {
    std::unordered_set<unsigned long long> visited;
    std::queue<state_ptr> q;
    state_ptr goal = nullptr;

    q.push(root);

    while (!q.empty()) {
        size_t level_size = q.size();
        std::vector<state_ptr> current_level_nodes;
        current_level_nodes.reserve(level_size);

        for (size_t i = 0; i < level_size; ++i) {
            state_ptr current = q.front();
            q.pop();

            if (visited.insert(current->get_identifier()).second) {
                current_level_nodes.push_back(current);
            }
        }

        // Use OpenMP to parallelize processing of children of all nodes at the current level
#pragma omp parallel
        for (const auto &node: current_level_nodes) {
            std::vector<state_ptr> children = node->next_states();

            for (auto &child: children) {
                unsigned long long child_id = child->get_identifier();
                bool is_goal_node = child->is_goal();

#pragma omp critical
                {
                    if (is_goal_node && (!goal || child < goal)) {
                        goal = child;
                    }
                    if (visited.find(child_id) == visited.end()) {
                        visited.insert(child_id);
                        q.push(child);
                    }
                }
            }
        }
    }

    return goal;
}



std::vector<state_ptr> queueMultiPop(std::queue<state_ptr> &q, std::mutex &vectorMutex, std::mutex &queueMutex) {
    size_t amount = omp_get_max_threads();
    std::vector<state_ptr> result;

    #pragma omp parallel for
    for (size_t i = 0; i < amount; ++i){
        state_ptr current;

        queueMutex.lock();
            if (!q.empty()){
                current = q.front();
                q.pop();
            }
        queueMutex.unlock();


        if (current){
            vectorMutex.lock();
                result.push_back(current);
            vectorMutex.unlock();
        }
    }

    return result;
}

void
multiStateEval(std::vector<state_ptr> &current,
               std::unordered_set<unsigned long long int> &visited,
               state_ptr &goal,
               std::queue<state_ptr> &q,
               std::mutex &vectorMutex,
               std::mutex &queueMutex,
               std::mutex &goalMutex,
               std::mutex &visitedMutex) {

    #pragma omp parallel
    {
        vectorMutex.lock();
        state_ptr currentState = current[omp_get_thread_num()];
        vectorMutex.unlock();

        visitedMutex.lock();
        if (visited.contains(currentState->get_identifier()))
            currentState = nullptr;
        else
            visited.insert(currentState->get_identifier());
        visitedMutex.unlock();

        if (currentState) {
            for (const state_ptr &child: currentState->next_states()) {

                if (visited.contains(child->get_identifier()))
                    continue;

                goalMutex.lock();
                if (child->is_goal() && child < goal)
                    goal = child;

                goalMutex.unlock();

                queueMutex.lock();
                q.push(child);
                queueMutex.unlock();
            }
        }
    }
}

state_ptr bfsMultiPop(state_ptr root){
    std::unordered_set<unsigned long long> visited;
    std::queue<state_ptr> q;
    state_ptr goal = nullptr;

    std::mutex vectorMutex;
    std::mutex queueMutex;
    std::mutex goalMutex;
    std::mutex visitedMutex;

    q.push(root);

    while (!q.empty()) {
        std::vector<state_ptr> current = queueMultiPop(q, vectorMutex, queueMutex);

        multiStateEval(current, visited, goal, q, vectorMutex, queueMutex, goalMutex, visitedMutex);
    }

    return goal;
}



state_ptr bfsLevelSpinGoal(state_ptr root) {
    std::unordered_set<unsigned long long> visited;
    std::queue<state_ptr> q;

    std::atomic_flag goal_lock = ATOMIC_FLAG_INIT;
    state_ptr goal = nullptr;

    q.push(root);

    while (!q.empty()) {
        size_t level_size = q.size();
        std::vector<state_ptr> current_level_nodes;
        current_level_nodes.reserve(level_size);

        // Populating the current level nodes
        for (size_t i = 0; i < level_size; ++i) {
            state_ptr current = q.front();
            q.pop();

            if (!visited.contains(current->get_identifier())) {
                current_level_nodes.push_back(current);
            }
        }

        // Use OpenMP to parallelize processing of children of all nodes at the current level
        #pragma omp parallel for
        for (size_t idx = 0; idx < current_level_nodes.size(); ++idx) {
            state_ptr node = current_level_nodes[idx];
            std::vector<state_ptr> children = node->next_states();

            for (auto &child : children) {
                unsigned long long child_id = child->get_identifier();
                bool is_goal_node = child->is_goal();

                if (!visited.contains(child_id)) {
                    // Attempt to lock the goal
                    while (goal_lock.test_and_set(std::memory_order_acquire)); // Spin-wait lock

                    if (is_goal_node && child < goal) {
                        goal = child;
                    }

                    // Release the lock
                    goal_lock.clear(std::memory_order_release);

                    // Safely push children to the queue
                    #pragma omp critical (queue_update)
                    q.push(child);
                }
            }
        }
    }

    return goal;
}

