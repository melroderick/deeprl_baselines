//
// Created by Mel Roderick on 8/11/17.
//

#ifndef CONTEXT_WEIGHTING_TREE_CWT_NODE_SYMBOL_H
#define CONTEXT_WEIGHTING_TREE_CWT_NODE_SYMBOL_H


#include <tiff.h>
class cwt_symbol;

class cwt_node_symbol {
public:
    cwt_node_symbol(cwt_symbol* model, int depth, int max_depth, int alphabet_size);
    ~cwt_node_symbol();
    double update(uint8 *context, uint8 symbol, bool update);

    double mix_prediction(double log_p, double log_child_p);
    void update_switching_weights(double log_p, double log_child_p);
    bool is_leaf();

    double* m_counts;
    double m_count_total;
    cwt_node_symbol** m_children;
    double m_log_stay_prob, m_log_split_prob;
    cwt_symbol* m_model;
    int m_depth;
    int m_max_depth;
    int m_alphabet_size;
};


#endif //CONTEXT_WEIGHTING_TREE_CWT_NODE_SYMBOL_H
