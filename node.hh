#ifndef node_hh
#define node_hh

#include "buffermap.hh"
#include "push.hh"
#include "update.hh"
#include <set>

class Node {
	int id;
	Buffermap bm[NUM_CONTENTS];
	int roundId;
        int rcvdUpdatesPerContentId[NUM_CONTENTS];

public:
	void init(int id);
	void incRoundId();
	void pushUpdates(class Push *push, int contentId);
	void rcvInUpdates(set<Update> &inUpdates);
	void endOfRound();
        int getRcvdUpdatesPerContentId(int contentId);
};

#endif
