#include "iddfs_weighted.h"

#include "iddfs.h"

// Pokud chcete implementovat ID-DFS pro ulohy s promenlivymi cenami kroku
// pomoci jineho algoritmu nez pro uniformni ceny, implementujte ho v teto funkci.
//
// Je mozne vyuzit pro obe ulohy stejny algoritmus, v takovem pripade ponechte obsah
// teto funkce nezmeneny.
state_ptr iddfs_weighted(state_ptr root) {
    return iddfs(root); // NOLINT(*-unnecessary-value-param)
}
