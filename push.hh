#ifndef push_hh
#define push_hh

#include <set>
#include <iostream>
#include "simulator.hh"

using namespace std;

class Push {
        int NUM_CONTENTS;
        int FANOUT;
        // TODO: Should disappear
	int *nodesId;
	vector<Update> updates;
        
        // Should be used instead
        int **nodesPerContentId;
        vector<Update> *updatesPerContentId;

public:
        Push(int FANOUT, int NUM_CONTENTS);
        ~Push();
	void clear();
	int getNodesId(int i); // TODO Should disappear
	int getNodesId(int i, int nodePos);
	void insertNodeId(int i, int destNode); // TODO Should disappear
	void insertNodeId(int contentId, int nodePos, int destNode);
	
	int getUpdatesSize(); // TODO disappear
	int getUpdatesSize(int contentId);
	
	Update getUpdates(int i); // TODO: should disappear
	Update getUpdates(int contentId, int i);
	void insertUpdate(Update &update); // TODO: should disappear
	void insertUpdate(int contentId, Update &update);
};


#endif
