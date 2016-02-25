#include "update.hh"

Update::Update(int roundId, int id) {
	this->roundId = roundId;
	this->id = id;
}

int Update::getRoundId() const {
	return roundId;
}

int Update::getId() const {
	return id;
}



