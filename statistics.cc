#include "statistics.hh"  
#include <stdio.h>
 
using namespace std;
 
int Statistics::getNumWrites() {
        return numWrites;
}

void Statistics::openOutputFile(string fileName) {
        outRcvdUpdates = fopen(fileName.c_str(), "w");
        fprintf(outRcvdUpdates, "RoundId (ContentId, ProportionReceived)\n");
}

void Statistics::closeOutputFile() {
        fclose(outRcvdUpdates);
}

void Statistics::clearRoundProportions() {
        numWrites = 0;
}

void Statistics::insertNumRcvdUpdates(int nodeId, int contentId, int numReceivedUpdates) {
        numRcvdUpdsPerRound[nodeId][contentId] = numReceivedUpdates;
        numWrites++;
}

void Statistics::writeProportionsToFile(int roundId) {
        if (numWrites != NUM_NODES * NUM_CONTENTS) {
                printf("Incorrect number of writes!! \n");
                return;
        }
        
        fprintf(outRcvdUpdates, "%d", roundId);
        for (int contentId = 0; contentId < NUM_CONTENTS; contentId++) {
                double avgReceived = 0;
                for (int nodeId = 0; nodeId < NUM_NODES; nodeId++) {
                        avgReceived += ((double) numRcvdUpdsPerRound[nodeId][contentId]) / ((double) NUM_NODES);
                }
                avgReceived = (avgReceived / ((double) NUM_UPDS_PER_ROUND)) * ((double) 100);
                fprintf(outRcvdUpdates, " (%d, %.2lf) ", contentId, avgReceived);        
        }
        fprintf(outRcvdUpdates, "\n");
}