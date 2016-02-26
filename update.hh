#ifndef update_hh
#define update_hh

class Update {
	int roundId; // Creation of update
	int id;
        int contentId;

public:
	Update(int roundId, int id, int contentId);
	int getRoundId() const;
	int getId() const;
        int getContentId() const;

	bool operator<(Update other) const {
        	return id < other.id || (id == other.id && contentId < other.contentId);
    	}
};

#endif
