#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "update.hh"
#include "buffermap.hh"
#include "node.hh"
#include "simulator.hh"

#include <pthread.h>

using namespace std;

//#define TRACE

Simulator::Simulator() {
	nodes = new Node[NUM_NODES];
	inUpdates = new set<Update>[NUM_NODES];
	inUpdatesLocks = new mutex[NUM_NODES];
	for (int i=0; i<NUM_NODES; i++) {
		nodes[i].init(i);
	}

	for (int i = 0; i < NUM_THREADS; i++) {
		td[i].tid = i;
		td[i].nodes = nodes;
		td[i].inUpdatesLocks = inUpdatesLocks;
		td[i].inUpdates = inUpdates;
	}
	
	stats = new Statistics();
	stats->openOutputFile("outputRcvd.txt");
}

Simulator::~Simulator() {
	delete[] nodes;	
	delete[] inUpdates;
	delete[] inUpdatesLocks;
        
        stats->closeOutputFile();
        delete stats;
}

void Simulator::printInUpdates() {
	for (int i = 0; i < NUM_NODES; i++) {
		cout << "\t" << i << " " << inUpdates[i].size();
		set<Update>::iterator iter;
		for (iter = inUpdates[i].begin(); iter!=inUpdates[i].end(); ++iter) {
			cout << iter->getId() << " ";
		}		
		cout << endl;		
	}
}

void Simulator::sourceSendNewUpdates(int roundId) {
	srand(roundId);
	int firstUpdateId = (NUM_UPDS_PER_ROUND * roundId) + 1; 
	set<int> destNodes;
	int destNode;
        for (int contentId = 0; contentId < NUM_CONTENTS; contentId++) {
                for (int updateId = firstUpdateId; updateId < firstUpdateId + NUM_UPDS_PER_ROUND; updateId++) {
                        for (int destId = 0; destId < FANOUT; destId++) {
                                destNode = rand() % NUM_NODES;
                                while (destNodes.find(destNode) != destNodes.end())
                                        destNode = rand() % NUM_NODES;
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

	Push push;
	for (int nodeId = tid; nodeId < NUM_NODES; nodeId += NUM_THREADS) {
                for (int contentId = 0; contentId < NUM_CONTENTS; contentId++) {
                        nodes[nodeId].pushUpdates(&push, contentId);

                        for (int destId = 0; destId < FANOUT; destId++) {
                                int nodeId = push.getNodesId(destId);
                                inUpdatesLocks[nodeId].lock();
                                for (int updPos = 0; updPos < push.getUpdatesSize(); updPos++) {
                                        inUpdates[nodeId].insert(push.getUpdates(updPos));
                                }
                                inUpdatesLocks[nodeId].unlock();
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
	struct thread_data *my_data;
	my_data = (struct thread_data *) threadarg;
	int tid = my_data->tid;
	Node *nodes = my_data->nodes;
	set<Update> *inUpdates = my_data->inUpdates;

	for (int i = tid; i < NUM_NODES; i += NUM_THREADS) {
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
