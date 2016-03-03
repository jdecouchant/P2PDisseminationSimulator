#include "simulator.hh"
#include <string>
#include <sstream>

// #define TRACE

int main(int argc, char* argv[]) {
        
        if (argc < 11) {
                cout << "Usage: NUM_NODES NUM_ROUNDS RTE NUM_UPDS_PER_ROUND"
                << " FANOUT DURATION_PROPOSE NUM_CONTENTS NUM_THREADS PROBAITOI PROBAITOJ" << endl; 
                return 1;                
        }
        
        // TODO atoi is deprecated, use strtol, of isstream
        int NUM_NODES = atoi(argv[1]);
        int NUM_ROUNDS = atoi(argv[2]);
        int RTE = atoi(argv[3]);
        int NUM_UPDS_PER_ROUND = atoi(argv[4]);
        int FANOUT = atoi(argv[5]);
        int DURATION_PROPOSE = atoi(argv[6]);
        int NUM_CONTENTS = atoi(argv[7]);
        int NUM_THREADS = atoi(argv[8]);  
	int PROBAITOI = atoi(argv[9]);
	int PROBAITOJ = atoi(argv[10]);
        
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