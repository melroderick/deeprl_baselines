//
// Created by maroderi on 7/23/17.
//

#include <tiff.h>
#include "location_dependent_cwt.h"
#include "cts.h"

#include <cmath>
#include <tuple>
#include <thread>
#include <future>

#define UseThreads false
#define NumThreads 2


uint8 get_pixel(uint8 *image, int x, int y, int w, int h) {
    if (x < w && x >= 0 && y < h && y >= 0) {
        return image[w*y + x];
    } else {
        return 0;
    }
}

tuple<double, double> feed_bits_to_tree(context_tree *tree, uint8 inp_pixel, int inp_bit_num, int num_bits, uint8 *context, int len_context) {
    // build flat context array.
    uint8 context_flat[len_context*num_bits];
    int context_flat_pos = 0;
    for (int i=0; i<len_context; i++) {
        uint8 pixel = context[i];
        for (int c=0; c<num_bits; c++) {
            uint8 bit = (pixel >> c) & (uint8) 1;
            context_flat[context_flat_pos++] = bit;
        }
    }
    uint8 inp_bit = (inp_pixel >> inp_bit_num) & (uint8) 1;
    double bit_prob = tree->update_and_logprob(context_flat, inp_bit);
    double recoding_prob = tree->logprob(context_flat, inp_bit);
    return make_tuple(bit_prob, recoding_prob);
}


context_tree ****build_cwt_array(int w, int h, int num_bits, bool switching) {
    context_tree ****cwt_array = new context_tree***[w];
    int context_depth = 4*num_bits;
    for (int x=0; x<w; x++) {
        cwt_array[x] = new context_tree**[h];
        for (int y=0; y<h; y++) {
            cwt_array[x][y] = new context_tree*[num_bits];
            for (int c=0; c<num_bits; c++) {
                if (switching) {
                    cwt_array[x][y][c] = new cts(context_depth);
                } else {
                    cwt_array[x][y][c] = new cwt(context_depth);
                }
            }
        }
    }
    return cwt_array;
}


void destroy_cwt_array(context_tree ****cwt_array, int w, int h, int num_bits) {
    for (int x=0; x<w; x++) {
        for (int y=0; y<h; y++) {
            for (int c=0; c<num_bits; c++) {
                delete cwt_array[x][y][c];
            }
            delete cwt_array[x][y];
        }
        delete cwt_array[x];
    }
    delete cwt_array;
}


void process_image_cols(context_tree ****cwt_array, uint8 *image, tuple<double, double>* result, int from_x, int to_x, int w, int h, int num_bits) {
    double cum_prob = 0.0;
    double cum_recoding_prob = 0.0;

    for (int x=from_x; x < to_x; x++) {
        for (int y = 0; y < h; y++) {
            for (int c = 0; c < num_bits; c++) {
                context_tree *tree = cwt_array[x][y][c];
                uint8 pixel_value = get_pixel(image, x, y, w, h);
                uint8 context[4];
                context[0] = get_pixel(image, x - 1, y, w, h);
                context[1] = get_pixel(image, x - 1, y - 1, w, h);
                context[2] = get_pixel(image, x, y - 1, w, h);
                context[3] = get_pixel(image, x + 1, y - 1, w, h);
                auto prob_pair = feed_bits_to_tree(tree, pixel_value, c, 3, context, 4);
                double bit_prob = std::get<0>(prob_pair);
                double recoding_prob = std::get<1>(prob_pair);
                cum_prob += bit_prob;
                cum_recoding_prob += recoding_prob;
            }
        }
    }
    *result = make_tuple(cum_prob, cum_recoding_prob);
}


location_dependent_cwt::location_dependent_cwt(int w, int h, int num_bits, bool switching) {
    m_cwt_array = build_cwt_array(w, h, num_bits, switching);
    m_width = w;
    m_height = h;
    m_num_bits = num_bits;
}
location_dependent_cwt::~location_dependent_cwt() {
    destroy_cwt_array(m_cwt_array, m_width, m_height, m_num_bits);
}

double location_dependent_cwt::process_image(uint8 *image) {
    int w = m_width, h = m_height, num_bits = m_num_bits;

    double cum_prob = 0.0;
    double cum_recoding_prob = 0.0;

#if UseThreads
    std::tuple<double, double> ret[NumThreads];
    std::thread threads[NumThreads];
    int cols_per_thread = w/NumThreads;

    for (int i=0; i < NumThreads; ++i) {
        int from_x = i * cols_per_thread;
        int to_x = std::max(from_x + cols_per_thread, w);
        threads[i] = std::thread(&process_image_cols,
                                 m_cwt_array, image, ret + i, from_x, to_x, w, h, num_bits);
    }
    for (int i=0; i < NumThreads; ++i) {
        threads[i].join();
        tuple<double, double> result = ret[i];
        cum_prob += std::get<0>(result);
        cum_recoding_prob += std::get<1>(result);
    }
#else
    tuple<double, double> result;
    process_image_cols(m_cwt_array, image, &result, 0, w, w, m_height, m_num_bits);
    cum_prob += std::get<0>(result);
    cum_recoding_prob += std::get<1>(result);
#endif

    cum_prob = exp(cum_prob);
    cum_recoding_prob = exp(cum_recoding_prob);
    if (cum_prob >= cum_recoding_prob) {
        return 0;
    } else {
        double pseudocount = cum_prob*(1 - cum_recoding_prob)/(cum_recoding_prob - cum_prob);
        return pseudocount;
    }
}