#include "push.hh"
#include "simulator.hh"
#include "assert.h"

Push::Push(int FANOUT, int NUM_CONTENTS) {
	assert(FANOUT > 0);
	assert(NUM_CONTENTS > 0);
        this->FANOUT = FANOUT;
	this->NUM_CONTENTS = NUM_CONTENTS;
	// TODO should disappear
        nodesId = new int[FANOUT];
        
	// Remplaced by that
        nodesPerContentId = new int*[NUM_CONTENTS];
        for (int contentId = 0; contentId < NUM_CONTENTS; contentId++) {
                nodesPerContentId[contentId] = new int[FANOUT];
        }
        updatesPerContentId = new vector<Update>[NUM_CONTENTS];
}

Push::~Push() {
        delete[] nodesId;
	
        for (int contentId = 0; contentId < NUM_CONTENTS; contentId++) {
		delete[] nodesPerContentId[contentId];
	}
	delete[] nodesPerContentId;
        delete[] updatesPerContentId;
}

void Push::clear() {
	for (int i = 0; i < FANOUT; i++) {
		nodesId[i] = -1;
	}
	updates.clear();
	
        for (int contentId = 0; contentId < NUM_CONTENTS; contentId++) {
		for (int i = 0; i < FANOUT; i++) {
			nodesPerContentId[contentId][i] = -1;
		}	
		updatesPerContentId[contentId].clear();
	}
}

int Push::getNodesId(int i) { 
	if (i < FANOUT) return nodesId[i];
}

int Push::getNodesId(int contentId, int nodePos) { 
	assert(0 <= contentId && contentId < NUM_CONTENTS);
	assert(0 <= nodePos && nodePos < FANOUT);
	return nodesPerContentId[contentId][nodePos];
}

void Push::insertNodeId(int i, int destNode) {
	nodesId[i] = destNode;
}

void Push::insertNodeId(int contentId, int nodePos, int destNode) {
	nodesPerContentId[contentId][nodePos] = destNode;
}

Update Push::getUpdates(int i) { 
	return updates[i];
}

Update Push::getUpdates(int contentId, int i) {
	return updatesPerContentId[contentId][i];
}

int Push::getUpdatesSize() {
	return updates.size();
}

int Push::getUpdatesSize(int contentId) {
	return updatesPerContentId[contentId].size();
}

void Push::insertUpdate(Update &update) {
	updates.push_back(update);
}

void Push::insertUpdate(int contentId, Update &update) {
	updatesPerContentId[contentId].push_back(update);
}
