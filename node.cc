#include <stdlib.h>
#include <vector>

#include "node.hh"
#include "push.hh"
#include <assert.h> 

using namespace std;

Node::Node() {} // To allow the creation of vector of nodes

void Node::init(int id, int FANOUT, int NUM_CONTENTS, int NUM_NODES, 
		int RTE, int DURATION_PROPOSE, int PROBAITOI) {
	this->id = id;
	this->roundId = 0;
	this->FANOUT = FANOUT;
	this->NUM_CONTENTS = NUM_CONTENTS;
	this->NUM_NODES = NUM_NODES;
	this->RTE = RTE;
	this->DURATION_PROPOSE = DURATION_PROPOSE;
	this->PROBAITOI = PROBAITOI;
	assert(0 <= PROBAITOI && PROBAITOI <= 100);
	selfContentId = Simulator::getContentIdFromNodeId(id, NUM_NODES, NUM_CONTENTS);
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
        int numNodesPerContent = NUM_NODES / NUM_CONTENTS;
        int firstNodeId = contentId * numNodesPerContent;
        set<int> destNodes;
        for (int destId = 0; destId < numNodes; destId++) {
                int destNode = firstNodeId + (rand() % numNodesPerContent);
                while (destNodes.find(destNode) != destNodes.end() || destNode == id) {
                        destNode = firstNodeId + rand() % numNodesPerContent;
                }
                assert(destNode < NUM_NODES && destNode >= 0);
                destNodes.insert(destNode);
        }
        return destNodes;
}

set<int> Node::selectNodesFromOtherContent(int numNodes, int contentId) {
        int numNodesPerContent = NUM_NODES / NUM_CONTENTS;
        set<int> destNodes;
        for (int destId = 0; destId < numNodes; destId++) {
                int destNode = rand() % NUM_NODES;
                while (destNodes.find(destNode) != destNodes.end() || destNode == id 
                  || (contentId * numNodesPerContent <= destNode &&  destNode < (contentId+1) * numNodesPerContent) ) {
                        destNode = rand() % NUM_NODES;
                }
                assert(destNode < NUM_NODES && destNode >= 0);
                assert(numNodesPerContent*contentId > destNode || destNode >= numNodesPerContent*(contentId+1));
                destNodes.insert(destNode);
        }
        return destNodes;
}

// TODO define a function that takes numNodes from a given contentId

set<int> Node::selectNodesFromSameContent(int numNodes) {
	int numNodesPerContent = NUM_NODES / NUM_CONTENTS;
	set<int> destNodes;
	for (int destId = 0; destId < numNodes; destId++) {
		int destNode = selfContentId * numNodesPerContent + (rand() % numNodesPerContent);
		while (destNodes.find(destNode) != destNodes.end() || destNode == id) {
			destNode = selfContentId * numNodesPerContent + rand() % numNodesPerContent;
		}
		assert(destNode < NUM_NODES && destNode >= 0);
		assert(Simulator::getContentIdFromNodeId(id, NUM_NODES, NUM_CONTENTS) == Simulator::getContentIdFromNodeId(destNode, NUM_NODES, NUM_CONTENTS));
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
		assert(Simulator::getContentIdFromNodeId(id, NUM_NODES, NUM_CONTENTS) 
                != Simulator::getContentIdFromNodeId(destNode, NUM_NODES, NUM_CONTENTS));
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

// TODO: use PROBAITOI, PROBAITOJ
// if rand() < pitoi then send content i to a majority of i nodes
// else send content i to a majority of j nodes
// Use sets of nodes

void Node::pushUpdatesAsymmetrically(class Push *push, int contentId) {
  
	// TODO: should disappear
	set<int> destNodesSameContent, destNodesOtherContent;
	if (contentId == selfContentId) {
 		destNodesOtherContent = selectNodesFromDifferentContent(FANOUT / 3);
		destNodesSameContent = selectNodesFromSameContent(FANOUT - FANOUT/3);
	} else {
		destNodesSameContent = selectNodesFromSameContent(FANOUT / 3);
		destNodesOtherContent = selectNodesFromDifferentContent(FANOUT - FANOUT/3);
	}

	int destId = 0;
	set<int>::iterator it;
	for (it = destNodesSameContent.begin(); it != destNodesSameContent.end(); it++, destId++) {
		push->insertNodeId(destId, *it);
	}
	for (it = destNodesOtherContent.begin(); it != destNodesOtherContent.end(); it++, destId++) {
		push->insertNodeId(destId, *it);
	}
	//
	set<int> nodesFromContent, nodesFromOtherContent;
	for (int contentId = 0; contentId < NUM_CONTENTS; contentId++) {
		nodesFromContent = selectNodesFromContent(FANOUT - FANOUT/3, contentId);
		nodesFromOtherContent = selectNodesFromOtherContent(FANOUT/3, contentId);
		
		int nodePos = 0;
		set<int>::iterator it;
		for (it = nodesFromContent.begin(); it != nodesFromContent.end(); it++) {
			push->insertNodeId(contentId, nodePos, *it);
			nodePos++;
		}
		for (it = nodesFromOtherContent.begin(); it != nodesFromOtherContent.end(); it++) {
			push->insertNodeId(contentId, nodePos, *it);
			nodePos++;
		}
	}
	
	//
	
	vector<Update> v;
	bm[contentId].getNewUpdates(v); // Updates from contentId
	vector<Update>::iterator iter;
	// TODO Should disappear
	for (iter=v.begin(); iter!=v.end(); ++iter) {
		if (iter->getRoundId() >= roundId - RTE) {
                        // Other updates are less often transmitted
                        if (contentId != selfContentId) {
                                if (rand() % 100 < 40)
                                        push->insertUpdate(*iter);
                        } else 
                                push->insertUpdate(*iter);
		}
	}
	// 
	for (iter=v.begin(); iter!=v.end(); ++iter) {
		if (iter->getRoundId() >= roundId - RTE) {
			if (rand() % 100 < PROBAITOI) {
				push->insertUpdate(contentId, *iter);
			} else {
				int randOtherContentId = contentId;
				while (randOtherContentId == contentId) {
					randOtherContentId = rand() % NUM_CONTENTS;
				}
				push->insertUpdate(randOtherContentId, *iter);
			}
		}
	}
	//
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
