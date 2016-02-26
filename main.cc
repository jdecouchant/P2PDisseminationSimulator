#include "simulator.hh"
#include <string>

int main() {
        Simulator sim;

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