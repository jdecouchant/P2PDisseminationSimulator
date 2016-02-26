#include <stdlib.h>
#include <vector>

#include "node.hh"
#include "push.hh"

using namespace std;

void Node::init(int id) {
	this->id = id;
	this->roundId = 0;
        for (int contentId = 0; contentId < NUM_CONTENTS; contentId++) {
                bm[contentId].init(id, contentId);
        }
}
	
void Node::incRoundId() {
	++roundId;
}


void Node::pushUpdates(Push *push, int contentId) {
	set<int> destNodes;
	for (int destId = 0; destId < FANOUT; destId++) {
		int destNode = rand() % NUM_NODES;
		while (destNodes.find(destNode) != destNodes.end() || destNode == id)
			destNode = rand() % NUM_NODES;
		push->insertNodeId(destId, destNode);
	}
	vector<Update> v;
	bm[contentId].getNewUpdates(v);
	vector<Update>::iterator iter;
	for (iter=v.begin(); iter!=v.end(); ++iter) {
		if (iter->getRoundId() >= roundId - RTE) {
			push->insertUpdate(*iter);
		}
	}

	//if (id==0)
	//	cout << "\tSend " << v.size() << endl;

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