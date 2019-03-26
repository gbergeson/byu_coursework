#include "database.h"

Database::Database(parser dtlg_prgrm){
	
	for(Predicate* p : dtlg_prgrm.schemes_list){
		string name;
		Scheme new_scheme;
		name = p->this_predicate[0]->toString();
		for(unsigned int i = 1; i < p->this_predicate.size(); i++)
			new_scheme.push_back(p->this_predicate[i]->toString());
		my_map[name] = Relation(name, new_scheme);
	}
	
	for(Predicate* p : dtlg_prgrm.facts_list){
		string name = p->this_predicate[0]->toString();
		Tuple new_tuple;
		for(unsigned int i = 1; i < p->this_predicate.size(); i++)
			new_tuple.push_back(p->this_predicate[i]->toString());
		my_map[name].add(new_tuple);
	}
	
}

Database::~Database(){
	
}

int Database::size(){
	int count = 0;
	
	for(auto &kv : my_map)
		count = count + kv.second.numTuples();

	return count;
}

















