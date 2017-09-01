//
// Created by maroderi on 7/23/17.
//

#ifndef CONTEXT_WEIGHTING_TREE_LOCATION_DEPENDENT_CWT_H
#define CONTEXT_WEIGHTING_TREE_LOCATION_DEPENDENT_CWT_H

#include <iostream>
#include <vector>
#include "cwt.h"

using namespace std;

void process_image_cols(cwt ****cwt_array, uint8 *image, tuple<double, double>* result, int from_x, int to_x, int w, int h, int num_bits);

class location_dependent_cwt {
public:
    location_dependent_cwt(int w, int h, int num_bits, bool switching);
    ~location_dependent_cwt();
    double process_image(uint8 *image);
private:
    context_tree ****m_cwt_array;
    int m_width, m_height, m_num_bits;
};

#endif //CONTEXT_WEIGHTING_TREE_LOCATION_DEPENDENT_CWT_H
