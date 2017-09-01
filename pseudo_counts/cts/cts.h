//
// Created by Christopher Grimm on 8/7/17.
//

#ifndef CONTEXT_WEIGHTING_TREE_CTS_H
#define CONTEXT_WEIGHTING_TREE_CTS_H


#include <tiff.h>
#include "cts_node.h"
#include "context_tree.h"

class cts : public context_tree {
public:
    cts(int context_depth);
    ~cts() override ;
    double update_and_logprob(uint8 *context, uint8 bit) override ;
    double logprob(uint8 *context, uint8 bit) override ;
    double logprob_block() override ;

private:
    long m_n;
    double m_alpha;
    cts_node *m_root_node;
    int m_context_depth;
};


#endif //CONTEXT_WEIGHTING_TREE_CTS_H
