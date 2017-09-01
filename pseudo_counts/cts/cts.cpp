//
// Created by Christopher Grimm on 8/7/17.
//

#include <cmath>
#include "cts.h"

void kill_tree_recursive(cts_node *ptr) {
    if (ptr->m_left != 0)
        kill_tree_recursive(ptr->m_left);
    if (ptr->m_right != 0)
        kill_tree_recursive(ptr->m_right);
    delete ptr;
}

cts::cts(int context_depth) {
    m_context_depth = context_depth;
    m_root_node = new cts_node(&m_alpha, 0, context_depth);
    m_n = 1;
    m_alpha = 1.0 / (m_n + 1.0);
}

cts::~cts() {
    kill_tree_recursive(m_root_node);
};

double cts::logprob(uint8 *context, uint8 bit) {
    double before = logprob_block();
    cts_node *updated_nodes[m_context_depth];
    double after = m_root_node->update(context, bit, updated_nodes);

    for (int i = 0; i < m_context_depth; ++i) {
        updated_nodes[i]->reset();
    }

    return after - before;
}

double cts::update_and_logprob(uint8 *context, uint8 bit) {
    double before = logprob_block();
    cts_node *updated_nodes[m_context_depth];
    double after = m_root_node->update(context, bit, updated_nodes);

    m_n += 1;
    m_alpha = 1.0 / (m_n + 1.0);
    return after - before;
}

double cts::logprob_block() {
    return m_root_node->m_weighted_P;
}