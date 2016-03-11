#!/usr/bin/python

import subprocess

NUM_NODES='10000'
NUM_ROUNDS='20'
NUM_UPDS_PER_ROUND='10'
NUM_CONTENTS='2'
NUM_THREADS='4'
DURATION_PROPOSE=1

PROBAITOJ='40' # not used currently

for DURATION_PROPOSE in range(1, 3):
  for num_nodes in range(10000, 105000, 10000):
    for rte in range(5,11):
      for fanout in range(2,8):
        for num_contents in range(2, 7):
          for probaitoi in range(int(100.0/num_contents),75,10):
	    subprocess.call(['./run', str(num_nodes), NUM_ROUNDS, str(rte), NUM_UPDS_PER_ROUND, str(fanout), str(DURATION_PROPOSE), str(num_contents), NUM_THREADS, str(probaitoi), PROBAITOJ])
	    subprocess.call(['mv', 
		       'outputRcvd.txt', 
		       'simulations/out_'+ str(num_nodes) + "_" + "%03d" % probaitoi +'_'+str(rte)+'_'+str(fanout)+'_'+str(num_contents)+'.txt'])
