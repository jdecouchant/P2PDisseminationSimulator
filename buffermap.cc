#include <iostream>

#include "set"
#include "buffermap.hh"
#include "simulator.hh"

void Buffermap::init(int nodeId, int contentId) {
        this->nodeId = nodeId;
        this->contentId = contentId;        
}

void Buffermap::insertSet(int roundId, set<Update> &setUpdate) {
	set<Update>::iterator iter;

	for (iter = setUpdate.begin(); iter != setUpdate.end(); ++iter) {
		Update update = *iter;

		newUpdates[update] = DURATION_PROPOSE;

		int updRoundId = update.getRoundId();
		map<int, set<Update> >::iterator it = updatesPerRound.find(updRoundId);
		if (it != updatesPerRound.end()) {
			it->second.insert(update);
		} else {
			set<Update> s;
			s.insert(update);
			updatesPerRound.insert(pair<int, set<Update> >(updRoundId, s));
		}

	}
}

void Buffermap::getNewUpdates(vector<Update> &v) {
	map<Update, int>::iterator iter;
	for (iter = newUpdates.begin(); iter != newUpdates.end(); ++iter) {
		v.push_back(iter->first);
	}
}

int Buffermap::endOfRound(int roundId, bool print) {
	// Update the set of updates that has to be forwarded
	map<Update, int>::iterator iter;
	for (iter = newUpdates.begin(); iter != newUpdates.end(); ) {
		iter->second--;		
		if (iter->second == 0) {
			newUpdates.erase(iter++);
		} else {
			++iter;
		}
	}

	// Update the buffermap of received updates
	int nbDeletedUpdates = 0;
	map<int, set<Update> >::iterator it;
	for (it = updatesPerRound.begin(); it != updatesPerRound.end(); ++it) {
		if (it->first < roundId - RTE) {
			nbDeletedUpdates = it->second.size();			
			updatesPerRound.erase(it);
			break;
		}
	}

	//if (print)
	//	cout << "\t(" << nodeId << "," << contentId << ") : deleted " << nbDeletedUpdates  << " updates" << endl;

        return nbDeletedUpdates;
}


