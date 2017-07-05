import interfaces
import numpy as np
import tensorflow as tf
import tf_helpers as th
import itertools as it
import relational_network as rn

class AtariDQRN(interfaces.DQNInterface):
    def __init__(self, frame_history, num_actions, shared_bias=True, append_conv_output= False):
        self.frame_history = frame_history
        self.num_actions = num_actions
        self.shared_bias = shared_bias
        #whether or not to include the original conv layer output in the final output
        #TODO: use this value
        self.append_conv_output = append_conv_output

    def get_input_shape(self):
        return [84, 84]

    def get_input_dtype(self):
        return 'uint8'

    def construct_q_network(self, input):
        #standard CNN layers - used 4 in the RN paper but building off of standard q-network
        input = tf.image.convert_image_dtype(input, tf.float32)
        with tf.variable_scope('c1'):
            c1 = th.down_convolution(input, 8, 4, self.frame_history, 32, tf.nn.relu)
        with tf.variable_scope('c2'):
            c2 = th.down_convolution(c1, 4, 2, 32, 64, tf.nn.relu)
        with tf.variable_scope('c3'):
            c3 = th.down_convolution(c2, 3, 1, 64, 64, tf.nn.relu)
        with tf.variable_scope('rn'):
            #all pairs of "objects" sent through mlp_g
            #get all combinations of indices
            x_dim, y_dim = c3.get_shape()[1].value, c3.get_shape()[2].value
            #get all depth columns, modify with location
            objects = []
            for i in range(x_dim):
                for j in range(y_dim):
                    depth_col = tf.slice(c3, [0, i, j, 0], [-1, 1, 1, -1])
                    # reshape to 2-D array of [num_batches, num_kernels]
                    depth_col = tf.reshape(depth_col, [-1, depth_col.get_shape()[3].value])
                    # tag with location info
                    #normalized dimension
                    loc = tf.constant([[float(i)/x_dim, float(j)/y_dim]])
                    location_info = tf.tile(loc, [tf.shape(depth_col)[0], 1])
                    # I think this is correct axis? should be shape [num_batches, num_kernels + 2]
                    depth_appended = tf.concat([depth_col, location_info], 1)
                    objects.append(depth_appended)

            # initialize shared variables for g
            rn.init_g(objects[0].get_shape()[1]*2)  # initializing with doubled tensor dimension

            relations = []
            for obj1, obj2 in it.combinations(objects, 2):
                # concatenate together
                # should be shape [num_batches, 2*(num_kernels + 2)
                object_pair = tf.concat([obj1, obj2], 1)
                g_rel = rn.mlp_g(object_pair)
                relations.append(g_rel)  # shape [num_batches, 256]

            # sum results elementwise
            # NOTE: is this the best way to combine results, overall?
            stacked = tf.stack(relations, axis=2) # shape [num_batches, 256, x_dim*y_dim]
            sum_relations = tf.reduce_sum(stacked, axis=2) # should be shape [num_batches, 256]

            # put output through mlp_f
            # TODO: does dropout make sense here? should we have different dropout val for the target network?
            f = rn.mlp_f(sum_relations, 0.5)

            # final linear output layer with second dimension self.num_actions
        with tf.variable_scope('fc2'):
            if self.shared_bias:
                q_values = th.fully_connected_shared_bias(f, self.num_actions, lambda x: x)
            else:
                q_values = th.fully_connected(f, self.num_actions, lambda x: x)
        return q_values
