//
// Created by cgrimm on 7/21/17.
//

#ifndef CWT_CWT_NODE_H
#define CWT_CWT_NODE_H

class cwt_node {
public:
    cwt_node(int depth, int max_depth);
    ~cwt_node();
    double update(uint8 *context, uint8 bit, cwt_node **updated_nodes);
    double update_P(bool a_updated);
    void reset();
    cwt_node* m_left = 0;
    cwt_node* m_right = 0;
    int m_a, m_old_a, m_b, m_old_b;
    double m_P, m_old_P, m_weighted_P, m_old_weighted_P;
    int m_depth;
    int m_max_depth;
};


#endif //CWT_CWT_NODE_H
