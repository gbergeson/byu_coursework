#include <set>
#include "tuple.h"
#include "scheme.h"

using namespace std;

class Relation {
	public:
	Relation(){};
	Relation(string rel_name, Scheme first_scheme);
	~Relation();
	
	Relation select(int &index, string &match);
	Relation select(int index1, int index2);
	Relation project(vector<int> &indices);
	void rename(Scheme new_scheme);
	void add(Tuple &new_tuple);
	int numTuples();
	Relation unify(Relation &first, Relation &second);
	Relation join(Relation &r1, Relation &r2);
	void findElements(Scheme &s1, Scheme &s2);
	bool v2HasIt(int i);
	Scheme make_scheme(Scheme s1, Scheme s2);
	bool joinable(Tuple &t1, Tuple &t2);
	Tuple join_tuples(Tuple &t1, Tuple &t2);
	void remove_element(int i, Scheme &t);
	
	string print();
	
	string name;
	Scheme my_scheme;
	set<Tuple> tuples;
	vector<int> v1, v2, v3;
};