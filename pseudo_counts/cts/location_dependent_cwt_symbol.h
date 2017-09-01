//
// Created by Mel Roderick on 8/11/17.
//

#ifndef CONTEXT_WEIGHTING_TREE_LOCATION_DEPENDENT_CWT_SYMBOL_H
#define CONTEXT_WEIGHTING_TREE_LOCATION_DEPENDENT_CWT_SYMBOL_H

#include "cwt_symbol.h"

using namespace std;

//void process_image_cols(cwt_symbol ***cwt_array, uint8 *image, tuple<double, double>* result, int from_x, int to_x, int w, int h, int num_bits);

class location_dependent_cwt_symbol {
public:
    location_dependent_cwt_symbol(int w, int h, int alphabet_size);
    ~location_dependent_cwt_symbol();
    double process_image(uint8 *image);

    uint8 get_pixel(uint8 *image, int x, int y, int w, int h);
    tuple<double, double> feed_bits_to_tree(cwt_symbol *tree, uint8 inp_pixel, uint8 *context);
    cwt_symbol ***build_cwt_array(int w, int h, int alphabet_size);
    void destroy_cwt_array(cwt_symbol ***cwt_array, int w, int h);
    void process_image_cols(cwt_symbol ***cwt_array, uint8 *image, tuple<double, double>* result, int from_x, int to_x, int w, int h);

private:
    cwt_symbol ***m_cwt_array;
    int m_width, m_height, m_alphabet_size;
};

#endif //CONTEXT_WEIGHTING_TREE_LOCATION_DEPENDENT_CWT_SYMBOL_H
