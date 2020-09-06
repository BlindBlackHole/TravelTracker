#include "structures.h"

bool operator > (const Weight& lhs, const Weight& rhs) {
	return lhs.weight > rhs.weight;
}

bool operator >= (const Weight& lhs, int64_t value) {
	return lhs.weight >= value;
}

Weight::Weight(EdgeType edge, double weight, std::string bus_or_stop, size_t stop_count) {
	this->edge = edge;
	this->weight = weight;
	this->bus_or_stop = bus_or_stop;
	this->stop_count = stop_count;
}

Weight::Weight(double value) {
	weight = value;
}

Weight& Weight::operator=(int64_t value) {
	weight = value;
	return *this;
}