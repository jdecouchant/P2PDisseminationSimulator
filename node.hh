#ifndef node_hh
#define node_hh

#include "buffermap.hh"
#include "push.hh"
#include "update.hh"
#include "simulator.hh"
#include <set>

class Node {
	int id;
	int selfContentId; // Id of the content of interest of the node

	int FANOUT;
	int NUM_CONTENTS;
	int NUM_NODES;
	int RTE;
	int DURATION_PROPOSE;
	int PROBAITOI;
	int PROBAITOJ;

	Buffermap *bm;
	int roundId;
	int *rcvdUpdatesPerContentId;
        
        set<int> selectNodesFromContent(int numNodes, int contentId);
        set<int> selectNodesFromOtherContent(int numNodes, int contentId);

public:
	Node();
	void init(int id, int FANOUT, int NUM_CONTENTS, int NUM_NODES, int RTE, int DURATION_PROPOSE,
	  int PROBAITOI, int PROBAITOJ);
	~Node();        
	void incRoundId();
	void pushUpdates(class Push *push, int contentId);
	void pushUpdatesAsymmetrically(class Push *push, int contentId);
	void rcvInUpdates(set<Update> &inUpdates);
	void endOfRound();
	int getRcvdUpdatesPerContentId(int contentId);
};

#endif
