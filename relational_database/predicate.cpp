#include "predicate.h"

Predicate::Predicate(string kind_of_predicate){
	type = kind_of_predicate;
}

Predicate::~Predicate(){
	
}
	
string Predicate::toString(){
	string output = this_predicate[0]->toString() + "(";
	
	for(unsigned int i = 1; i < this_predicate.size(); i++){
		output = output + this_predicate[i]->toString();
		if( i + 1 != this_predicate.size())
			output = output + ",";
	}
	
	output = output + ")";
	
	if(type == "FACTS")
		output = output + ".";
	if(type == "QUERIES")
		output = output + "?";
	
	return output;
}