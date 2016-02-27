#ifndef node_hh
#define node_hh

#include "buffermap.hh"
#include "push.hh"
#include "update.hh"
#include <set>

class Node {
        int FANOUT;
        int NUM_CONTENTS;
        int NUM_NODES;
        int RTE;
        int DURATION_PROPOSE;
	int id;
        Buffermap *bm;
	int roundId;
        int *rcvdUpdatesPerContentId;
public:
        Node();
        void init(int id, int FANOUT, int NUM_CONTENTS, int NUM_NODES, int RTE, int DURATION_PROPOSE);
        ~Node();        
	void incRoundId();
	void pushUpdates(class Push *push, int contentId);
	void rcvInUpdates(set<Update> &inUpdates);
	void endOfRound();
        int getRcvdUpdatesPerContentId(int contentId);
};

#endif
