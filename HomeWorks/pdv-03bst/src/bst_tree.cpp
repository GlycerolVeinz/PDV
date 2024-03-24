#include "bst_tree.h"

void bst_tree::insert(int64_t data) {
    node* new_node = new node(data);
    node* current = root;

    if ( !current ) {
        if ( root.compare_exchange_strong(current, new_node, std::memory_order_acq_rel) ) {
            return;
        }
    }

    while( true ){
        node* nextLeft = current->left.load(std::memory_order_acquire);
        node* nextRight = current->right.load(std::memory_order_acquire);

        if ( data < current->data ) {
            if ( !nextLeft ) {
                if ( current->left.compare_exchange_strong(nextLeft, new_node, std::memory_order_acq_rel) ) {
                    return;
                }
            }
            else {
                current = nextLeft;
            }
        }
        else {
            if ( !nextRight ) {
                if ( current->right.compare_exchange_strong(nextRight, new_node, std::memory_order_acq_rel) ) {
                    return;
                }
            }
            else {
                current = nextRight;
            }
        }
    }
}

// Rekurzivni funkce pro pruchod stromu a dealokaci pameti prirazene jednotlivym uzlum
void delete_node(bst_tree::node* node) {
    if ( !node ) {
        return;
    }

    delete_node(node->left);
    delete_node(node->right);
    delete node;
}

bst_tree::~bst_tree() {
    delete_node(root);
}
