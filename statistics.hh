#ifndef statistics_hh
#define statistics_hh

#include "simulator.hh"
#include <stdio.h>
#include <string>

using namespace std;

class Statistics {
        int NUM_NODES;
        int NUM_CONTENTS;
        int NUM_UPDS_PER_ROUND;
        
        FILE *outRcvdUpdates;
        int numWrites;
        int **numRcvdUpdsPerRound;

public:
        Statistics(int NUM_NODES, int NUM_CONTENTS, int NUM_UPDS_PER_ROUND);
        ~Statistics();
        int getNumWrites();
        void openOutputFile(string fileName);
        void closeOutputFile();
        void clearRoundProportions();
        void writeProportionsToFile(int roundId);
        void insertNumRcvdUpdates(int nodeId, int contentId, int numReceivedUpdates);
};

#endif