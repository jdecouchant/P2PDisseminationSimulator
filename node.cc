#include <stdlib.h>
#include <vector>

#include "node.hh"
#include "push.hh"
#include <assert.h> 

using namespace std;

Node::Node() {} // To allow the creation of vector of nodes

void Node::init(int id, int FANOUT, int NUM_CONTENTS, int NUM_NODES, 
		int RTE, int DURATION_PROPOSE, int PROBAITOI, int PROBAITOJ) {
	this->id = id;
	this->roundId = 0;
	this->FANOUT = FANOUT;
	this->NUM_CONTENTS = NUM_CONTENTS;
	this->NUM_NODES = NUM_NODES;
	this->RTE = RTE;
	this->DURATION_PROPOSE = DURATION_PROPOSE;
	this->PROBAITOI = PROBAITOI;
	assert(0 <= PROBAITOI && PROBAITOI <= 100);
	this->PROBAITOJ = PROBAITOJ;
	assert(0 <= PROBAITOJ && PROBAITOJ <= 100);
	selfContentId = Simulator::getContentIdFromNodeId(id, NUM_CONTENTS);
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

set<int> Node::selectNodesFromContent(int numNodes, int contentId) {
        set<int> destNodes;
        for (int destId = 0; destId < numNodes; destId++) {
                int destNode = Simulator::getRandNodeIdFromContent(contentId, NUM_NODES, NUM_CONTENTS);
                while (destNodes.find(destNode) != destNodes.end() || destNode == id) {
                        destNode = Simulator::getRandNodeIdFromContent(contentId, NUM_NODES, NUM_CONTENTS);
                }
                assert(destNode < NUM_NODES && destNode >= 0);
                destNodes.insert(destNode);
        }
        return destNodes;
}

set<int> Node::selectNodesFromOtherContent(int numNodes, int contentId) {
        set<int> destNodes;
        for (int destId = 0; destId < numNodes; destId++) {
                int destNode = Simulator::getRandNodeIdFromOtherContent(contentId, NUM_NODES, NUM_CONTENTS);
                while (destNodes.find(destNode) != destNodes.end() || destNode == id) {
                        destNode = Simulator::getRandNodeIdFromOtherContent(contentId, NUM_NODES, NUM_CONTENTS);
                }
                assert(destNode < NUM_NODES && destNode >= 0);
                destNodes.insert(destNode);
        }
        return destNodes;
}

// if rand() < pitoi then send content i to a majority of i nodes
// else send content i to a majority of j nodes
// Use sets of nodes

void Node::pushUpdatesAsymmetrically(class Push *push, int contentId) {

	set<int> nodesFromContent, nodesFromOtherContent;
	set<int>::iterator it;
	
	if (contentId == 0) {
		for (int cId = 0; cId < NUM_CONTENTS; cId++) {
			// TODO: values are important
			nodesFromContent = selectNodesFromContent(FANOUT - FANOUT/4, cId);
			if (cId == selfContentId) {
				nodesFromOtherContent = selectNodesFromOtherContent(FANOUT/4, cId);
			} else {
				nodesFromOtherContent = selectNodesFromContent(FANOUT/4, selfContentId);
			}
			int nodePos = 0;
			for (it = nodesFromContent.begin(); it != nodesFromContent.end(); it++) {
				push->insertNodeId(cId, nodePos, *it);
				nodePos++;
			}
			for (it = nodesFromOtherContent.begin(); it != nodesFromOtherContent.end(); it++) {
				push->insertNodeId(cId, nodePos, *it);
				nodePos++;
			}
		}
	}
	
	vector<Update> v;
	bm[contentId].getNewUpdates(v); // Updates from contentId
	vector<Update>::iterator iter;
	int randOtherContentId;

	for (iter=v.begin(); iter!=v.end(); ++iter) {
		if (iter->getRoundId() >= roundId - RTE) {
			if (NUM_CONTENTS == 1 || rand() % 100 < PROBAITOI) {
				push->insertUpdate(contentId, *iter);
			} else {
				if (rand() % 100 < PROBAITOJ) {
					do {
						randOtherContentId = rand() % NUM_CONTENTS;
					} while (randOtherContentId == contentId);
					push->insertUpdate(randOtherContentId, *iter);
				}	
			}
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
		int updateContentId = update.getContentId();
		updatesPerContent[updateContentId].insert(update);
	}
	for (int contentId = 0; contentId < NUM_CONTENTS; contentId++) {  
		bm[contentId].insertSet(roundId, updatesPerContent[contentId]);
	}
}

void Node::endOfRound() { 
	for (int contentId = 0; contentId < NUM_CONTENTS; contentId++) {
		int numRcvdUpdates = 
		bm[contentId].endOfRound(roundId, false);
		rcvdUpdatesPerContentId[contentId] = numRcvdUpdates;
	}
}

int Node::getRcvdUpdatesPerContentId(int contentId) {
	return rcvdUpdatesPerContentId[contentId];
}
