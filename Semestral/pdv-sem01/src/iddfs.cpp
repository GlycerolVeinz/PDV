#include "iddfs.h"
#include <stack>
#include <unordered_set>
#include <limits>


static bool operator < (const state_ptr& a, const state_ptr& b) {
    if (!a) return false;
    if (!b) return true;
    return a->get_identifier() < b->get_identifier();
}


// Naimplementujte efektivni algoritmus pro nalezeni nejkratsi (respektive nej-
// levnejsi) cesty v grafu. V teto metode mate ze ukol naimplementovat pametove
// efektivni algoritmus pro prohledavani velkeho stavoveho prostoru. Pocitejte
// s tim, ze Vami navrzeny algoritmus muze bezet na stroji s omezenym mnozstvim
// pameti (radove nizke stovky megabytu). Vhodnym pristupem tak muze byt napr.
// iterative-deepening depth-first search.
//
// Metoda ma za ukol vratit ukazatel na cilovy stav, ktery je dosazitelny pomoci
// nejkratsi/nejlevnejsi cesty.

std::pair<state_ptr, size_t> iddfsHelper(const state_ptr& root, size_t maxDepth);

state_ptr iddfs(state_ptr root) {
    size_t maxDepth = 1;
    state_ptr goal = nullptr;
    size_t depthReachedSoFar = 0;

    while(true){
        std::pair<state_ptr, size_t> res = iddfsHelper(root, maxDepth++);
        if (res.first < goal){
            goal = res.first;
        }

        if (res.second == depthReachedSoFar){
            break;
        }

        depthReachedSoFar = res.second;
    }

    return goal;
}

std::pair<state_ptr, size_t> iddfsHelper(const state_ptr& root, size_t maxDepth){
    std::stack<state_ptr> stack;
    std::unordered_set<unsigned long long> visited;
    stack.push(root);
    size_t curDepth = 0;
    state_ptr goal = nullptr;

    while(!stack.empty()){
        state_ptr current = stack.top();
        stack.pop();

        if(visited.contains(current->get_identifier())){
            continue;
        }
        visited.insert(current->get_identifier());

        if(current->is_goal() && current < goal){
            goal = current;
        }

        if(curDepth < maxDepth){
            for(const state_ptr& child : current->next_states()){
                stack.push(child);
            }
        }
    }

    return std::make_pair(goal, curDepth);
}