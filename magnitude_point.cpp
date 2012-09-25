class magnitude_point {

public:

	int index;
	double magnitude;
	
	magnitude_point(int index, double magnitude) {
		this->index = index;
		this->magnitude = magnitude;
	}
	
	bool operator <(const magnitude_point &other) const {
		return magnitude < other.magnitude;
	}
};

bool index_sort(const magnitude_point &i, const magnitude_point &j) {
	return i.index < j.index;
}
