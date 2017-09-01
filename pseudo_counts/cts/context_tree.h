//
// Created by Mel Roderick on 8/9/17.
//

#ifndef CONTEXT_WEIGHTING_TREE_CONTEXT_TREE_H
#define CONTEXT_WEIGHTING_TREE_CONTEXT_TREE_H


#include <tiff.h>
#include "cts_node.h"

class context_tree {
public:
    virtual ~context_tree() {};
    virtual double update_and_logprob(uint8 *context, uint8 bit) = 0;
    virtual double logprob(uint8 *context, uint8 bit) = 0;
    virtual double logprob_block() = 0;
};


#endif //CONTEXT_WEIGHTING_TREE_CONTEXT_TREE_H
