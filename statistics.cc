#include "statistics.hh"  
#include <stdio.h>
#include "assert.h"
#include "simulator.hh"
 
using namespace std;
 
Statistics::Statistics(int NUM_NODES, int NUM_CONTENTS, int NUM_UPDS_PER_ROUND) {
        this->NUM_NODES = NUM_NODES;
        this->NUM_CONTENTS = NUM_CONTENTS;
        this->NUM_UPDS_PER_ROUND = NUM_UPDS_PER_ROUND;
        numRcvdUpdsPerRound = new int*[NUM_NODES];
        for (int i = 0; i < NUM_NODES; i++) {
                numRcvdUpdsPerRound[i] = new int[NUM_CONTENTS];
        }
}

Statistics::~Statistics() {
        for (int i = 0; i < NUM_NODES; i++) {
                delete[] numRcvdUpdsPerRound[i];
        }
        delete[] numRcvdUpdsPerRound;        
}
        
int Statistics::getNumWrites() {
        return numWrites;
}

void Statistics::openOutputFile(string fileName) {
        outRcvdUpdates = fopen(fileName.c_str(), "w");
        fprintf(outRcvdUpdates, "RoundId -> (ContentId, \tProportionOfContentReceivedPerContentId)\n\n");
}

void Statistics::closeOutputFile() {
        fclose(outRcvdUpdates);
}

void Statistics::clearRoundProportions() {
        for (int nodeId = 0; nodeId < NUM_NODES; nodeId++) {
                for (int contentId = 0; contentId < NUM_CONTENTS; contentId++) {
                        numRcvdUpdsPerRound[nodeId][contentId] = 0;
                }
        }
        numWrites = 0;
}

void Statistics::insertNumRcvdUpdates(int nodeId, int contentId, int numReceivedUpdates) {
        numRcvdUpdsPerRound[nodeId][contentId] = numReceivedUpdates;
        numWrites++;
}

void Statistics::writeProportionsToFile(int roundId) {
        assert(numWrites == NUM_NODES * NUM_CONTENTS);
        
        int numNodesPerContent = NUM_NODES / NUM_CONTENTS;
        
        fprintf(outRcvdUpdates, "%03d", roundId);
        double avgReceivedPerContentId[NUM_CONTENTS][NUM_CONTENTS];
        for (int i = 0; i < NUM_CONTENTS; i++) {
                for (int j = 0; j < NUM_CONTENTS; j++) {
                        avgReceivedPerContentId[i][j] = 0.0;
                }
        }

        for (int nodeId = 0; nodeId < NUM_NODES; nodeId++) {
                int nodeContentId = Simulator::getContentIdFromNodeId(nodeId, NUM_NODES, NUM_CONTENTS);
                for (int contentId = 0; contentId < NUM_CONTENTS; contentId++) {
                        avgReceivedPerContentId[nodeContentId][contentId] += 
                        ((double) numRcvdUpdsPerRound[nodeId][contentId]) / ((double) NUM_UPDS_PER_ROUND);
                }
        }
        
        for (int i = 0; i < NUM_CONTENTS; i++) {
                for (int j = 0; j < NUM_CONTENTS; j++) {
                        avgReceivedPerContentId[i][j] = 
                        (100.0 * avgReceivedPerContentId[i][j]) /  ((double) numNodesPerContent);
                }
        }
        
        for (int i = 0; i < NUM_CONTENTS; i++) {
                fprintf(outRcvdUpdates, "\t%d -> (", i);
                for (int j = 0; j < NUM_CONTENTS; j++) {
                        fprintf(outRcvdUpdates, "%5.1lf", avgReceivedPerContentId[i][j]);
                        if (j+1 < NUM_CONTENTS) {
                                fprintf(outRcvdUpdates, ", ");
                        } else {
                                fprintf(outRcvdUpdates, ")");
                        }
                }
        }
        fprintf(outRcvdUpdates, "\n");
}