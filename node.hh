#ifndef node_hh
#define node_hh

#include "buffermap.hh"
#include "push.hh"
#include "update.hh"
#include <set>

class Node {
	int id;
	Buffermap bm;
	int roundId;

public:
	void init(int id);	
	void incRoundId();
	void pushUpdates(class Push *push);
	void rcvInUpdates(set<Update> &inUpdates);
	void endOfRound();
};

#endif
