#ifndef simulator_hh
#define simulator_hh

#define 	NUM_NODES			1000
#define 	NUM_ROUNDS			100	
#define		RTE				10
#define 	NUM_UPDS_PER_ROUND		10
#define 	FANOUT				3
#define 	DURATION_PROPOSE		1
#define         NUM_CONTENTS                    4

#define 	NUM_THREADS			4

#include "statistics.hh"
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
        pthread_t threads[NUM_THREADS];
        
        class Statistics *stats;
        
public: 
	Simulator();
	~Simulator();

	void createThreads();

	void printInUpdates();
	void sourceSendNewUpdates(int roundId);
	void peersPushUpdates();
	void peersEndOfRound(int roundId);
};


#endif
