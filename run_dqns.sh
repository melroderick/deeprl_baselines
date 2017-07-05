#!/usr/bin/env bash

runner=DQN/train_dqn.py

game=pong

#TODO: parallelize?

python ${runner} --game roms/${game}.bin --dqn dqrn --results results/no_append/dqrn/${game}

#python ${runner} --game roms/${game}.bin --dqn dqn --results results/dqn/${game}

#python ${runner} --game roms/${game}.bin --dqn double_dqrn --results results/no_append/double_dqrn/${game}

#python ${runner} --game roms/${game}.bin --dqn double_dqn --results results/double_dqn/${game}