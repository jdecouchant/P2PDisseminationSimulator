#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "update.hh"
#include "buffermap.hh"
#include "node.hh"
#include "simulator.hh"

#include <pthread.h>
#include "assert.h"

using namespace std;

//#define TRACE

Simulator::Simulator(int NUM_NODES, int NUM_ROUNDS, int RTE, int NUM_UPDS_PER_ROUND, 
                  int FANOUT, int DURATION_PROPOSE, int NUM_CONTENTS, int NUM_THREADS, 
		  int PROBAITOI, int PROBAITOJ) {
        
        this->NUM_NODES = NUM_NODES; 
        this->NUM_ROUNDS = NUM_ROUNDS;
        this->RTE = RTE;
        this->NUM_UPDS_PER_ROUND = NUM_UPDS_PER_ROUND;
        this->FANOUT = FANOUT;
        this->DURATION_PROPOSE = DURATION_PROPOSE;
        this->NUM_CONTENTS = NUM_CONTENTS;
        this->NUM_THREADS = NUM_THREADS;       
	
	
        nodes = new Node[NUM_NODES];
        for (int nodeId = 0; nodeId < NUM_NODES; nodeId++) {
                nodes[nodeId].init(nodeId, FANOUT, NUM_CONTENTS, NUM_NODES, 
           RTE, DURATION_PROPOSE, PROBAITOI, PROBAITOJ);
        }

        inUpdates = new set<Update>[NUM_NODES];
        inUpdatesLocks = new mutex[NUM_NODES];

        td = new struct thread_data[NUM_THREADS];
        threads = new pthread_t[NUM_THREADS];
        for (int i = 0; i < NUM_THREADS; i++) {
                td[i].tid = i;
                td[i].nodes = nodes;
                td[i].inUpdatesLocks = inUpdatesLocks;
                td[i].inUpdates = inUpdates;
                td[i].FANOUT = FANOUT;
                td[i].NUM_NODES = NUM_NODES;
                td[i].NUM_CONTENTS = NUM_CONTENTS;
                td[i].NUM_THREADS = NUM_THREADS;
        }
        
        stats = new Statistics(NUM_NODES, NUM_CONTENTS, NUM_UPDS_PER_ROUND);
        stats->openOutputFile("outputRcvd.txt");
}

Simulator::~Simulator() {
	delete[] nodes;
	delete[] inUpdates;
	delete[] inUpdatesLocks;
        
        stats->closeOutputFile();
        delete stats;
        
        delete[] td;
        delete[] threads;
}

int Simulator::getRandNodeIdFromContent(int contentId, int NUM_NODES, int NUM_CONTENTS) {
	int nodeId = rand() % NUM_NODES;
	while (nodeId % NUM_CONTENTS != contentId) {
		nodeId = rand() % NUM_NODES;
	}
	return nodeId; 
}

int Simulator::getRandNodeIdFromOtherContent(int contentId, int NUM_NODES, int NUM_CONTENTS) {
	int nodeId = rand() % NUM_NODES;
	while (nodeId % NUM_CONTENTS == contentId) {
		nodeId = rand() % NUM_NODES;
	}
	return nodeId; 	
}

// TODO not working correctly if NUM_CONTENTS does not divide NUM_NODES
// Replace by a modulo
int Simulator::getContentIdFromNodeId(int nodeId, int NUM_CONTENTS) {
        return nodeId % NUM_CONTENTS;
}

void Simulator::sourceSendNewUpdates(int roundId) {
	srand(roundId);
	int firstUpdateId = (NUM_UPDS_PER_ROUND * roundId) + 1; 
	set<int> destNodes;
	int destNode;
        for (int contentId = 0; contentId < NUM_CONTENTS; contentId++) {
                for (int updateId = firstUpdateId; updateId < firstUpdateId + NUM_UPDS_PER_ROUND; updateId++) {
                        for (int destId = 0; destId < FANOUT; destId++) {
                                destNode = getRandNodeIdFromContent(contentId, NUM_NODES, NUM_CONTENTS);
                                while (destNodes.find(destNode) != destNodes.end()) {
                                        destNode = getRandNodeIdFromContent(contentId, NUM_NODES, NUM_CONTENTS);
                                }
                                destNodes.insert(destNode);
                                Update update(roundId, updateId, contentId);
                                inUpdates[destNode].insert(update);
                        }
                        destNodes.clear();
                }
        }
}

void *threadPushUpdates(void *threadarg) {
	struct thread_data *my_data;
	my_data = (struct thread_data *) threadarg;

	int tid = my_data->tid;
	Node *nodes = my_data->nodes;
	mutex *inUpdatesLocks = my_data->inUpdatesLocks;
	set<Update> *inUpdates = my_data->inUpdates;
        int FANOUT = my_data->FANOUT;
        int NUM_NODES = my_data->NUM_NODES;
        int NUM_CONTENTS = my_data->NUM_CONTENTS;
        int NUM_THREADS = my_data->NUM_THREADS;
        
	Push push(FANOUT, NUM_CONTENTS);
        for (int nodeId = tid; nodeId < NUM_NODES; nodeId += NUM_THREADS) {
                for (int contentId = 0; contentId < NUM_CONTENTS; contentId++) {
			nodes[nodeId].pushUpdatesAsymmetrically(&push, contentId);
			for (int j = 0; j < NUM_CONTENTS; j++) {
				for (int nodePos = 0; nodePos < FANOUT; nodePos++) {
					int nodeId = push.getNodesId(j, nodePos);
					inUpdatesLocks[nodeId].lock();
					for (int updPos = 0; updPos < push.getUpdatesSize(j); updPos++) {
						inUpdates[nodeId].insert(push.getUpdates(j, updPos));
					}
					inUpdatesLocks[nodeId].unlock();
				}
			}

                        push.clear();
                }
	}

	pthread_exit(NULL);
}

void Simulator::peersPushUpdates() {
	for (int i = 0; i < NUM_THREADS; i++) {
		pthread_create(&threads[i], NULL, threadPushUpdates, (void *)&td[i]);
	}
	for (int i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}
}

void *threadEndOfRound(void *threadarg) {
	struct thread_data *my_data = (struct thread_data *) threadarg;
	Node *nodes = my_data->nodes;
	set<Update> *inUpdates = my_data->inUpdates;

	for (int i = my_data->tid; i < my_data->NUM_NODES; i += my_data->NUM_THREADS) {
		nodes[i].endOfRound();
		nodes[i].rcvInUpdates(inUpdates[i]);	
		inUpdates[i].clear();
		nodes[i].incRoundId();
	}

	pthread_exit(NULL);
}


void Simulator::peersEndOfRound(int roundId) {
	for (int tid = 0; tid < NUM_THREADS; tid++) {
		pthread_create(&threads[tid], NULL, threadEndOfRound, (void *)&td[tid]);
	}

	for (int tid = 0; tid < NUM_THREADS; tid++) {
		pthread_join(threads[tid], NULL);
	}
	
	for (int nodeId = 0; nodeId < NUM_NODES; nodeId++) {
                for (int contentId = 0; contentId < NUM_CONTENTS; contentId++) {
                        int numRcvdUpdates = nodes[nodeId].getRcvdUpdatesPerContentId(contentId);
                        stats->insertNumRcvdUpdates(nodeId, contentId, numRcvdUpdates);
                }
        }
        
        stats->writeProportionsToFile(roundId);
        stats->clearRoundProportions();
}
