//
// Created by Mel Roderick on 8/11/17.
//

#include "cwt_node_symbol.h"
#include "fastmath.hpp"
#include "common.h"
#include "cwt_symbol.h"

#define LOG_PT_5 (-0.6931471805599453094172321) // log(0.5)

cwt_node_symbol::cwt_node_symbol(cwt_symbol* model, int depth, int max_depth, int alphabet_size) {
    m_log_stay_prob = LOG_PT_5;
    m_log_split_prob = LOG_PT_5;

    m_model = model;

    m_alphabet_size = alphabet_size;
    m_children = new cwt_node_symbol*[alphabet_size]();
    m_count_total = 1;
    m_counts = new double[alphabet_size]();
    for (int i = 0; i < alphabet_size; ++i) {
        m_children[i] = 0;

        m_counts[i] = 1./alphabet_size;
    }

    m_depth = depth;
    m_max_depth = max_depth;
}

cwt_node_symbol::~cwt_node_symbol() {}

bool cwt_node_symbol::is_leaf() {
    return m_depth == m_max_depth;
}

double cwt_node_symbol::update(uint8 *context, uint8 symbol, bool update) {
    double log_p = fast_log(m_counts[symbol] / m_count_total);
    double result;

    if (is_leaf()) {
        m_log_stay_prob = 0;
        result = log_p;
    } else {
        uint8 c = *context;
        cwt_node_symbol *child = m_children[c];
        if (child == 0) {
            child = new cwt_node_symbol(m_model, m_depth+1, m_max_depth, m_alphabet_size);
            m_children[c] = child;
        }
        double log_child_p = child->update(context+1, symbol, update);

        result = mix_prediction(log_p, log_child_p);

        if (update) update_switching_weights(log_p, log_child_p);
    }

    if (update) {
        m_counts[symbol] += 1;
        m_count_total += 1;
    }

    return result;
}

/**
 * Returns the mixture x = w * p + (1 - w) * q.
 * Here, w is the posterior probability of using the estimator at this
 * node, versus using recursively calling our child node.
 *
 * The mixture is computed in log space, which makes things slightly
 * trickier.
 *
 * Let log_stay_prob_ = p' = log p, log_split_prob_ = q' = log q.
 * The mixing coefficient w is
 *
 * w = e^p' / (e^p' + e^q'),
 * v = e^q' / (e^p' + e^q').
 *
 * Then
 * x = (e^{p' w'} + e^{q' v'}) / (e^w' + e^v').
*/
double cwt_node_symbol::mix_prediction(double log_p, double log_child_p) {

    double numerator = log_add(log_p + m_log_stay_prob,
                               log_child_p + m_log_split_prob);
    double denominator = log_add(m_log_stay_prob,
                                 m_log_split_prob);
    return numerator - denominator;
}

void cwt_node_symbol::update_switching_weights(double log_p, double log_child_p) {
    double log_alpha = m_model->m_log_alpha;
    double log_1_minus_alpha = m_model->m_log_1_minus_alpha;

    // Avoid numerical issues with alpha = 1. This reverts to straight up weighting.
    if (log_1_minus_alpha == 0) {
        m_log_stay_prob += log_p;
        m_log_split_prob += log_child_p;
    } else {
        m_log_stay_prob = log_add(log_1_minus_alpha
                                  + log_p
                                  + m_log_stay_prob,
                                  log_alpha
                                  + log_child_p
                                  + m_log_split_prob);

        m_log_split_prob = log_add(log_1_minus_alpha
                                   + log_child_p
                                   + m_log_split_prob,
                                   log_alpha
                                   + log_p
                                   + m_log_stay_prob);
    }
}