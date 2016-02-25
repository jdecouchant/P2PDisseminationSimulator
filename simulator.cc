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
	nodes = new Node[NB_NODES];
	inUpdates = new set<Update>[NB_NODES];
	inUpdatesLocks = new mutex[NB_NODES];
	for (int i=0; i<NB_NODES; i++) {
		nodes[i].init(i);
	}

	for (int i = 0; i < NUM_THREADS; i++) {
		td[i].tid = i;
		td[i].nodes = nodes;
		td[i].inUpdatesLocks = inUpdatesLocks;
		td[i].inUpdates = inUpdates;
	}
}

Simulator::~Simulator() {
	delete[] nodes;	
	delete[] inUpdates;
	delete[] inUpdatesLocks;
}

void Simulator::printInUpdates() {
	for (int i = 0; i < NB_NODES; i++) {
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
	int firstUpdateId = (NB_UPD_PER_ROUND * roundId) + 1; 
	set<int> destNodes;
	int destNode;
	for (int updateId = firstUpdateId; updateId < firstUpdateId + NB_UPD_PER_ROUND; updateId++) {
		for (int destId = 0; destId < FANOUT; destId++) {
			destNode = rand() % NB_NODES;
			while (destNodes.find(destNode) != destNodes.end())
				destNode = rand() % NB_NODES;
			destNodes.insert(destNode);
			Update update(roundId, updateId);
			inUpdates[destNode].insert(update);
		}
		destNodes.clear();
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
	for (int i = tid; i < NB_NODES; i += NUM_THREADS) {
		nodes[i].pushUpdates(&push);

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

	for (int i = tid; i < NB_NODES; i += NUM_THREADS) {
		nodes[i].endOfRound();
		nodes[i].rcvInUpdates(inUpdates[i]);	
		inUpdates[i].clear();	
		nodes[i].incRoundId();
	}

	pthread_exit(NULL);
}


void Simulator::peersEndOfRound() {
	for (int i = 0; i < NUM_THREADS; i++) {
		pthread_create(&threads[i], NULL, threadEndOfRound, (void *)&td[i]);
	}

	for (int i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}
}


int main() {
	Simulator sim;

	for (int roundId = 0; roundId < NB_ROUNDS; roundId++) {
		cout << "Round " << roundId << endl;

#ifdef TRACE 
		cout << "\tSending new updates to nodes" << endl;
#endif
		sim.sourceSendNewUpdates(roundId);

#ifdef TRACE 
		cout << "\tPeers forward updates" << endl;
#endif
		sim.peersPushUpdates();

#ifdef TRACE 
		cout << "\tEnd of round" << endl;
#endif	
		sim.peersEndOfRound();

	}

	return 0;
}
