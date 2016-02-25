#ifndef push_hh
#define push_hh

#include <set>
#include <iostream>
#include "simulator.hh"

using namespace std;

class Push {
	int nodesId[FANOUT];
	vector<Update> updates;

public:
	void clear();
	int getNodesId(int i);
	void insertNodeId(int i, int destNode);
	int getUpdatesSize();
	Update getUpdates(int i);
	void insertUpdate(Update &update);
};


#endif
