#ifndef statistics_hh
#define statistics_hh

#include "simulator.hh"
#include <stdio.h>
#include <string>

using namespace std;

class Statistics {
        FILE *outRcvdUpdates;
        int numWrites;
        int numRcvdUpdsPerRound[NUM_NODES][NUM_CONTENTS];

public:
        int getNumWrites();
        void openOutputFile(string fileName);
        void closeOutputFile();
        void clearRoundProportions();
        void writeProportionsToFile(int roundId);
        void insertNumRcvdUpdates(int nodeId, int contentId, int numReceivedUpdates);
};

#endif