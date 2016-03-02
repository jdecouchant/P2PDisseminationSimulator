#include <stdlib.h>
#include <vector>

#include "node.hh"
#include "push.hh"
#include <assert.h> 

using namespace std;

Node::Node() {} // To allow the creation of vector of nodes

void Node::init(int id, int FANOUT, int NUM_CONTENTS, int NUM_NODES, 
		int RTE, int DURATION_PROPOSE) {
	this->id = id;
	this->roundId = 0;
	this->FANOUT = FANOUT;
	this->NUM_CONTENTS = NUM_CONTENTS;
	this->NUM_NODES = NUM_NODES;
	this->RTE = RTE;
	this->DURATION_PROPOSE = DURATION_PROPOSE;
	selfContentId = getContentIdFromNodeId(id);
	bm = new Buffermap[NUM_CONTENTS];
	for (int contentId = 0; contentId < NUM_CONTENTS; contentId++) {
		bm[contentId].init(DURATION_PROPOSE, RTE, id, contentId);
	}
	rcvdUpdatesPerContentId = new int[NUM_CONTENTS];
}

Node::~Node() {
	delete[] bm;
	delete[] rcvdUpdatesPerContentId;
}

void Node::incRoundId() {
	++roundId;
}

int Node::getContentIdFromNodeId(int nodeId) {
    	int contentId = 0;
	int curContentMax = (NUM_NODES / NUM_CONTENTS);
	while (nodeId >= curContentMax) {
	    curContentMax += (NUM_NODES / NUM_CONTENTS);
	    contentId++;
	}
	assert(contentId >= 0 && contentId < NUM_CONTENTS);
	return contentId;
}

set<int> Node::selectNodesFromSameContent(int numNodes) {
	int numNodesPerContent = NUM_NODES / NUM_CONTENTS;
	set<int> destNodes;
	for (int destId = 0; destId < numNodes; destId++) {
		int destNode = selfContentId * numNodesPerContent + (rand() % numNodesPerContent);
		while (destNodes.find(destNode) != destNodes.end() || destNode == id) {
			destNode = selfContentId * numNodesPerContent + rand() % numNodesPerContent;
		}
		assert(destNode < NUM_NODES && destNode >= 0);
		assert(getContentIdFromNodeId(id) == getContentIdFromNodeId(destNode));
		destNodes.insert(destNode);
	}
	return destNodes;
}

set<int> Node::selectNodesFromDifferentContent(int numNodes) {
	int numNodesPerContent = NUM_NODES / NUM_CONTENTS;
	set<int> destNodes;
	for (int destId = 0; destId < numNodes; destId++) {
		int destNode = rand() % NUM_NODES;
		while (destNodes.find(destNode) != destNodes.end() || destNode == id 
		  || (selfContentId * numNodesPerContent <= destNode &&  destNode < (selfContentId+1) * numNodesPerContent) ) {
			destNode = rand() % NUM_NODES;
		}
		assert(destNode < NUM_NODES && destNode >= 0);
		assert(getContentIdFromNodeId(id) != getContentIdFromNodeId(destNode));
		destNodes.insert(destNode);
	}
	return destNodes;
}


void Node::pushUpdates(Push *push, int contentId) {
	set<int> destNodes;
	for (int destId = 0; destId < FANOUT; destId++) {
		int destNode = rand() % NUM_NODES;
		while (destNodes.find(destNode) != destNodes.end() || destNode == id)
			destNode = rand() % NUM_NODES;
		push->insertNodeId(destId, destNode);
		destNodes.insert(destNode);
	}

	vector<Update> v;
	bm[contentId].getNewUpdates(v);
	vector<Update>::iterator iter;
	for (iter=v.begin(); iter!=v.end(); ++iter) {
		if (iter->getRoundId() >= roundId - RTE) {
			push->insertUpdate(*iter);
		}
	}
}

void Node::pushUpdatesAsymmetrically(class Push *push, int contentId) {
  
	set<int> destNodesSameContent, destNodesOtherContent;
	if (contentId == selfContentId) {
 		destNodesOtherContent = selectNodesFromDifferentContent(FANOUT / 3);
		destNodesSameContent = selectNodesFromSameContent(FANOUT - (FANOUT/3));
	} else {
		destNodesSameContent = selectNodesFromSameContent(FANOUT / 3);
		destNodesOtherContent = selectNodesFromDifferentContent(FANOUT - (FANOUT / 3));
	}
	
	int destId = 0;
	set<int>::iterator it;
	for (it = destNodesSameContent.begin(); it != destNodesSameContent.end(); it++, destId++) {
		push->insertNodeId(destId, *it);
	}
	for (it = destNodesOtherContent.begin(); it != destNodesOtherContent.end(); it++, destId++) {
		push->insertNodeId(destId, *it);
	}	
	
	vector<Update> v;
	bm[contentId].getNewUpdates(v);
	vector<Update>::iterator iter;
	for (iter=v.begin(); iter!=v.end(); ++iter) {
		if (iter->getRoundId() >= roundId - RTE) {
			push->insertUpdate(*iter);
		}
	}
}

void Node::rcvInUpdates(set<Update> &inUpdates) {
	//if (id == 0)
	//	cout << "\tReceive " << inUpdates.size() << endl;
	set<Update> updatesPerContent[NUM_CONTENTS];
	set<Update>::iterator it;
	for (it = inUpdates.begin(); it != inUpdates.end(); it++) {
		Update update = *it;
		int contentId = update.getContentId();
		updatesPerContent[contentId].insert(update);
	}
	for (int contentId = 0; contentId < NUM_CONTENTS; contentId++) {  
		bm[contentId].insertSet(roundId, updatesPerContent[contentId]);
	}
}

void Node::endOfRound() { 
	for (int contentId = 0; contentId < NUM_CONTENTS; contentId++) {
		int numRcvdUpdates = bm[contentId].endOfRound(roundId, id < 10);
		rcvdUpdatesPerContentId[contentId] = numRcvdUpdates;
	}
}

int Node::getRcvdUpdatesPerContentId(int contentId) {
	return rcvdUpdatesPerContentId[contentId];
}
