#include "simulator.hh"
#include <string>
#include <sstream>

// #define TRACE

int main(int argc, char* argv[]) {
        
	int NUM_NODES, NUM_CONTENTS, RTE, NUM_UPDS_PER_ROUND;
	int FANOUT, DURATION_PROPOSE, NUM_ROUNDS, NUM_THREADS;
	int PROBAITOI, PROBAITOJ;
	
        if (argc != 11) {
		cout << argc << " arguments provided instead of 10!" << endl;
                cout << "Usage: NUM_NODES NUM_ROUNDS RTE NUM_UPDS_PER_ROUND"
                << " FANOUT DURATION_PROPOSE NUM_CONTENTS NUM_THREADS PROBAITOI PROBAITOJ" << endl; 
                cout << "Example:  ./run 10000 100 8 10 4 1 4 2 80 40" << endl;
		cout << "Now running with default values" << endl;             
		NUM_NODES = 1000;
		NUM_ROUNDS = 20;
		RTE = 8;
		NUM_UPDS_PER_ROUND = 10;
		FANOUT = 3;
		DURATION_PROPOSE = 1;
		NUM_CONTENTS = 2;
		NUM_THREADS = 2;  
		PROBAITOI = 80;
		PROBAITOJ = 40;
        } else {
		// TODO atoi is deprecated, use strtol, of isstream
		NUM_NODES = atoi(argv[1]);
		NUM_ROUNDS = atoi(argv[2]);
		RTE = atoi(argv[3]);
		NUM_UPDS_PER_ROUND = atoi(argv[4]);
		FANOUT = atoi(argv[5]);
		DURATION_PROPOSE = atoi(argv[6]);
		NUM_CONTENTS = atoi(argv[7]);
		NUM_THREADS = atoi(argv[8]);  
		PROBAITOI = atoi(argv[9]);
		PROBAITOJ = atoi(argv[10]);
	}
	
	
        cout << "Running with: NUM_NODES=" << NUM_NODES
        << ", NUM_ROUNDS=" << NUM_ROUNDS
        << ", RTE=" << RTE
        << ", NUM_UPDS_PER_ROUND=" << NUM_UPDS_PER_ROUND
        << ", FANOUT=" << FANOUT 
        << ", DURATION_PROPOSE=" << DURATION_PROPOSE
        << ", NUM_CONTENTS=" << NUM_CONTENTS
        << ", NUM_THREADS=" << NUM_THREADS 
        << ", PROBAITOI=" << PROBAITOI
        << ", PROBAITOJ=" << PROBAITOJ
        << endl;
        
        
        Simulator sim(NUM_NODES, NUM_ROUNDS, RTE, NUM_UPDS_PER_ROUND, 
                      FANOUT, DURATION_PROPOSE, NUM_CONTENTS, NUM_THREADS, 
		      PROBAITOI, PROBAITOJ
 		    );

        for (int roundId = 0; roundId < NUM_ROUNDS; roundId++) {
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
                sim.peersEndOfRound(roundId);

        }

        return 0;
}