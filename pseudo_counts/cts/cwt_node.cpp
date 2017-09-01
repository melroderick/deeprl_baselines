//
// Created by cgrimm on 7/21/17.
//

#include <tiff.h>
#include <cmath>
#include <iostream>
#include "cwt_node.h"
#include "fastmath.hpp"
#include "common.h"

#define LOG_PT_5 (-0.6931471805599453094172321) // log(0.5)

cwt_node::cwt_node(int depth, int max_depth) {
    m_old_a = 0;
    m_a = 0;

    m_old_b = 0;
    m_b = 0;

    m_old_P = 0;
    m_P = 0;

    m_old_weighted_P = 0;
    m_weighted_P = 0;

    m_left = 0;
    m_right = 0;

    m_depth = depth;
    m_max_depth = max_depth;
}

cwt_node::~cwt_node() {}

double cwt_node::update(uint8 *context, uint8 bit, cwt_node **updated_nodes) {

    m_old_a = m_a;
    m_old_b = m_b;
    m_old_P = m_P;
    m_old_weighted_P = m_weighted_P;

    updated_nodes[m_depth] = this;
    double result;

    if (bit == 1) m_b = m_b + 1;
    else m_a = m_a + 1;

    if (m_depth == m_max_depth) {
        result = update_P((bit == 0));
        return result;
    }
    if (*context == 1) {
        if (m_left == 0) {
            m_left = new cwt_node(m_depth+1, m_max_depth);
        }
        m_left->update(context+1, bit, updated_nodes);
        result = update_P((bit == 0));
    } else {
        if (m_right == 0) {
            m_right = new cwt_node(m_depth+1, m_max_depth);
        }
        m_right->update(context+1, bit, updated_nodes);
        result = update_P((bit == 0));
    }

    return result;
}

void cwt_node::reset() {
    m_a = m_old_a;
    m_b = m_old_b;
    m_P = m_old_P;
    m_weighted_P = m_old_weighted_P;
}

double cwt_node::update_P(bool a_updated) {
    if (a_updated) {
        int a = m_a - 1, b = m_b;
        m_P = fast_log((a + 0.5) / (a + b + 1)) + m_P;
    } else {
        int a = m_a, b = m_b - 1;
        m_P = fast_log((b + 0.5) / (a + b + 1)) + m_P;
    }
    if ((m_left == 0 && m_right == 0)) {
        m_weighted_P = m_P;
    } else {
        double term1 = LOG_PT_5 + m_P;
        double weighted_p_left = (m_left == 0) ? 0 : m_left->m_weighted_P;
        double weighted_p_right = (m_right == 0) ? 0 : m_right->m_weighted_P;
        double term2 = LOG_PT_5 + weighted_p_left + weighted_p_right;
        m_weighted_P = log_add(term1, term2);
    }
    return m_weighted_P;
}