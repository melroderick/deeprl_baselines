import tensorflow as tf
#
# helper functions + correct var scopes for relational network functions

# initialize g
def init_g(input_dim, num_neurons=256, init_w=tf.contrib.layers.xavier_initializer(), init_b=tf.constant_initializer()):
    with tf.variable_scope('g'):
        tf.get_variable('w1', [input_dim, num_neurons], initializer=init_w)
        tf.get_variable('b1', [num_neurons], initializer=init_b)
        tf.get_variable('w2', [num_neurons, num_neurons], initializer=init_w)
        tf.get_variable('b2', [num_neurons], initializer=init_b)
        tf.get_variable('w3', [num_neurons, num_neurons], initializer=init_w)
        tf.get_variable('b3', [num_neurons], initializer=init_b)
        tf.get_variable('w4', [num_neurons, num_neurons], initializer=init_w)
        tf.get_variable('b4', [num_neurons], initializer=init_b)

# mimicking the exact layout of g(x, \theta) in the relational network paper.
# four-layer MLP with 256 neurons per layer, relu_activation
# TODO: more generalizable MLP function
def mlp_g(inp, rectifier=tf.nn.relu, num_neurons=256 ):
    with tf.variable_scope('g', reuse=True):
        #retrieving reused variables
        w1 = tf.get_variable('w1', [inp.get_shape()[1].value, num_neurons])
        b1 = tf.get_variable('b1', [num_neurons])
        fc1 = rectifier(tf.matmul(inp, w1) + b1)
        w2 = tf.get_variable('w2', [num_neurons, num_neurons])
        b2 = tf.get_variable('b2', [num_neurons])
        fc2 = rectifier(tf.matmul(fc1, w2) + b2)
        w3 = tf.get_variable("w3", [num_neurons, num_neurons])
        b3 = tf.get_variable('b3', [num_neurons])
        fc3 = rectifier(tf.matmul(fc2, w3) + b3)
        w4 = tf.get_variable('w4', [num_neurons, num_neurons])
        b4 = tf.get_variable('b4', [num_neurons])
        fc4 = rectifier(tf.matmul(fc3, w4) + b4)
        return fc4

#f function, three-layer MLP  with two 256-neuron layers and one 29-neuron layer with relu activation
def mlp_f(inp, dropout_prob, rectifier=tf.nn.relu, intermed_neurons=256, out_neurons=29,init_w=tf.contrib.layers.xavier_initializer(), init_b=tf.constant_initializer()):
    with tf.variable_scope('f'):
        w1 = tf.get_variable('w1', [inp.get_shape()[1].value, intermed_neurons], initializer=init_w)
        b1 = tf.get_variable('b1', [intermed_neurons], initializer=init_b)
        fc1 = rectifier(tf.matmul(inp, w1) + b1)
        w2 = tf.get_variable('w2', [intermed_neurons, intermed_neurons], initializer=init_w)
        b2 = tf.get_variable('b2', [intermed_neurons], initializer=init_b)
        #layer with dropout
        fc2 = tf.nn.dropout(rectifier(tf.matmul(fc1, w2) + b2), keep_prob=dropout_prob)
        w3 = tf.get_variable('w3', [intermed_neurons, out_neurons], initializer=init_w)
        b3 = tf.get_variable('b3', [out_neurons], initializer=init_b)
        fc3 = rectifier(tf.matmul(fc2, w3) + b3)
        return fc3