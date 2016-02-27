#include "push.hh"
#include "simulator.hh"

Push::Push(int FANOUT) {
        this->FANOUT = FANOUT;
        nodesId = new int[FANOUT];
}

Push::~Push() {
        delete[] nodesId;
}

void Push::clear() {
	for (int i = 0; i < FANOUT; i++)
		nodesId[i] = -1;
	updates.clear();
}

int Push::getNodesId(int i) { 
	if (i < FANOUT) return nodesId[i];
}

void Push::insertNodeId(int i, int destNode) {
	nodesId[i] = destNode;
}

Update Push::getUpdates(int i) { 
	return updates[i];
}

int Push::getUpdatesSize() {
	return updates.size();
}

void getUpdates(vector<Update> &v) {

}

void Push::insertUpdate(Update &update) {
	updates.push_back(update);
}
