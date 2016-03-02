#ifndef node_hh
#define node_hh

#include "buffermap.hh"
#include "push.hh"
#include "update.hh"
#include <set>

class Node {
	int id;
	int selfContentId; // Id of the content of interest of the node

	int FANOUT;
	int NUM_CONTENTS;
	int NUM_NODES;
	int RTE;
	int DURATION_PROPOSE;

	Buffermap *bm;
	int roundId;
	int *rcvdUpdatesPerContentId;

	set<int> selectNodesFromSameContent(int numNodes);
	set<int> selectNodesFromDifferentContent(int numNodes);

public:
	Node();
	void init(int id, int contentId, int FANOUT, int NUM_CONTENTS, int NUM_NODES, int RTE, int DURATION_PROPOSE);
	~Node();        
	void incRoundId();
	void pushUpdates(class Push *push, int contentId);
	void rcvInUpdates(set<Update> &inUpdates);
	void endOfRound();
	int getRcvdUpdatesPerContentId(int contentId);
};

#endif
