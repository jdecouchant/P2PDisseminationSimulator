#include <stdlib.h>
#include <vector>

#include "node.hh"
#include "push.hh"

using namespace std;

void Node::init(int id) {
	this->id = id;
	this->roundId = 0;
}
	
void Node::incRoundId() {
	++roundId;
}


void Node::pushUpdates(Push *push) {
	set<int> destNodes;
	for (int destId = 0; destId < FANOUT; destId++) {
		int destNode = rand() % NB_NODES;
		while (destNodes.find(destNode) != destNodes.end() || destNode == id)
			destNode = rand() % NB_NODES;
		push->insertNodeId(destId, destNode);
	}
	vector<Update> v;
	bm.getNewUpdates(v);	
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
	bm.insertSet(roundId, inUpdates);
}
	
void Node::endOfRound() { 
	bm.endOfRound(roundId, id < 5);
}
