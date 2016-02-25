#ifndef simulator_hh
#define simulator_hh

#define 	NB_NODES			1000
#define 	NB_ROUNDS			100	
#define		RTE				10
#define 	NB_UPD_PER_ROUND		40
#define 	FANOUT				4
#define 	DURATION_PROPOSE		1

#define 	NUM_THREADS			4

#include "node.hh"
#include "update.hh"
#include <mutex>

struct thread_data{
	int  tid;
	class Node *nodes;
	mutex *inUpdatesLocks;
	set<Update> *inUpdates;
};

class Simulator {	
	class Node *nodes;	
	set<Update> *inUpdates; // Messages that are being transmitted during one round
	
	mutex *inUpdatesLocks;
	struct thread_data td[NUM_THREADS];

public: 
	bool *donePush;
	bool *doneReceive;
	bool shouldExit;
	pthread_t threads[NUM_THREADS];

	Simulator();
	~Simulator();

	void createThreads();

	void printInUpdates();
	void sourceSendNewUpdates(int roundId);
	void peersPushUpdates();
	void peersEndOfRound();
};


#endif
