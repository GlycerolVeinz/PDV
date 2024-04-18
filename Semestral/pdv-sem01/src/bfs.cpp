#include "bfs.h"
#include <unordered_set>
#include <queue>

static bool operator < (const state_ptr& a, const state_ptr& b) {
    if (!a) return false;
    if (!b) return true;
    return a->get_identifier() < b->get_identifier();
}

// Naimplementujte efektivni algoritmus pro nalezeni nejkratsi cesty v grafu.
// V teto metode nemusite prilis optimalizovat pametove naroky, a vhodnym algo-
// ritmem tak muze byt napriklad pouziti prohledavani do sirky (breadth-first
// search.
//
// Metoda ma za ukol vratit ukazatel na cilovy stav, ktery je dosazitelny pomoci
// nejkratsi cesty.
state_ptr bfs(state_ptr root) {
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

        for (const state_ptr& child : current->next_states()) {
            if (child->is_goal() && child < goal)
                goal = child;

            if (!visited.contains(child->get_identifier()))
                q.push(child);
        }
    }

    return goal;
}