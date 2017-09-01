//
// Created by cgrimm on 7/21/17.
//

#include "cwt.h"
#include "cwt_node.h"


void kill_tree_recursive(cwt_node *ptr) {
    if (ptr->m_left != 0)
        kill_tree_recursive(ptr->m_left);
    if (ptr->m_right != 0)
        kill_tree_recursive(ptr->m_right);
    delete ptr;

}

cwt::cwt(int context_depth) {
    m_context_depth = context_depth;
    m_root_node = new cwt_node(0, context_depth);
}

cwt::~cwt() {
    kill_tree_recursive(m_root_node);
};

double cwt::logprob(uint8 *context, uint8 bit) {
    double before = logprob_block();
    cwt_node *updated_nodes[m_context_depth];
    double after = m_root_node->update(context, bit, updated_nodes);

    for (int i = 0; i < m_context_depth; ++i) {
        updated_nodes[i]->reset();
    }

    return after - before;
}

double cwt::update_and_logprob(uint8 *context, uint8 bit) {
    double before = logprob_block();
    cwt_node *updated_nodes[m_context_depth];
    double after = m_root_node->update(context, bit, updated_nodes);

    return after - before;
}

double cwt::logprob_block() {
    return m_root_node->m_weighted_P;
}