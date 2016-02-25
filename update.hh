#ifndef update_hh
#define update_hh

class Update {
	int roundId; // Creation of update
	int id;

public:
	Update(int roundId, int id);
	int getRoundId() const;
	int getId() const;

	bool operator<(Update other) const {
        	return id < other.id;
    	}
};

#endif
