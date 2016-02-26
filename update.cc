#include "update.hh"

Update::Update(int roundId, int id, int contentId) {
	this->roundId = roundId;
	this->id = id;
        this->contentId = contentId;
}

int Update::getRoundId() const {
	return roundId;
}

int Update::getId() const {
	return id;
}

int Update::getContentId() const {
        return contentId;
}


