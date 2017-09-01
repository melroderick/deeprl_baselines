//
// Created by Christopher Grimm on 8/7/17.
//

#ifndef CONTEXT_WEIGHTING_TREE_CTS_NODE_H
#define CONTEXT_WEIGHTING_TREE_CTS_NODE_H


#include <tiff.h>
#include <tuple>

class cts_node {
public:
    cts_node(double *alpha, int depth, int max_depth);
    ~cts_node();
    double update_P(bool a_updated);
    double update(uint8 *context, uint8 bit, cts_node **updated_nodes);
    void reset();
    cts_node* m_left = 0;
    cts_node* m_right = 0;
    int m_a, m_old_a, m_b, m_old_b;
    double *m_alpha;
    double m_P, m_old_P, m_weighted_P, m_old_weighted_P;
    double m_kc, m_sc;
    int m_depth;
    int m_max_depth;
    double m_old_sc;
    double m_old_kc;
};


#endif //CONTEXT_WEIGHTING_TREE_CTS_NODE_H
