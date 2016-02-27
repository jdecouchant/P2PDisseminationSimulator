#ifndef push_hh
#define push_hh

#include <set>
#include <iostream>
#include "simulator.hh"

using namespace std;

class Push {
        int FANOUT;
	int *nodesId;
	vector<Update> updates;

public:
        Push(int FANOUT);
        ~Push();
	void clear();
	int getNodesId(int i);
	void insertNodeId(int i, int destNode);
	int getUpdatesSize();
	Update getUpdates(int i);
	void insertUpdate(Update &update);
};


#endif
