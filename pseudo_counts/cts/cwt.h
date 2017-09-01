//
// Created by cgrimm on 7/21/17.
//

#ifndef CWT_CWT_H
#define CWT_CWT_H


#include <tiff.h>
#include "cwt_node.h"
#include "context_tree.h"

class cwt : public context_tree {
public:
    cwt(int context_depth);
    ~cwt();
    double update_and_logprob(uint8 *context, uint8 bit) override ;
    double logprob(uint8 *context, uint8 bit) override ;
    double logprob_block() override ;

    cwt_node *m_root_node;
    int m_context_depth;
};


#endif //CWT_CWT_H
