#!/usr/bin/python

import subprocess

NUM_NODES='10000'
NUM_ROUNDS='20'
NUM_UPDS_PER_ROUND='10'
NUM_CONTENTS='2'
NUM_THREADS='4'

RTE='8'
FANOUT='3'
DURATION_PROPOSE='1'
PROBAITOI='80'

PROBAITOJ='40' # not used currently

for rte in range(7,11):
  for fanout in range(2,6):
    for probaitoi in range(50,110,10):
      for num_contents in range(2, 6):
	subprocess.call(['./run', NUM_NODES, NUM_ROUNDS, str(rte), NUM_UPDS_PER_ROUND, 
  str(fanout), DURATION_PROPOSE, str(num_contents), NUM_THREADS, str(probaitoi), PROBAITOJ])
	subprocess.call(['mv', 
		       'outputRcvd.txt', 
		       'simulations/out_'+str(rte)+'_'+str(fanout)+'_'+str(probaitoi)+'_'+str(num_contents)+'.txt'])