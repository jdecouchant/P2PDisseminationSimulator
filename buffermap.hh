#ifndef buffermap_hh
#define buffermap_hh

#include <map>
#include <set>
#include <vector>
#include "update.hh"

using namespace std;

class Buffermap {
        int nodeId;
        int contentId;
	map<int, set<Update> > updatesPerRound; // creationRoundId -> updatesId
	map<Update, int> newUpdates;
	
public:
        void init(int nodeId, int contentId);
	void insertSet(int roundId, set<Update> &setUpdate);
	void getNewUpdates(vector<Update> &v);
	int endOfRound(int roundId, bool print);  
};

#endif
