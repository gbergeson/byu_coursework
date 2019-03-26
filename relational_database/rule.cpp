#include "rule.h"

Rule::Rule(){
	
}

Rule::~Rule(){
	
}

string Rule::toString(){
	string output = this_rule[0]->toString() + " :- ";
	
	for(unsigned int i = 1; i < this_rule.size(); i++){
		output = output + this_rule[i]->toString();
		if( i + 1 != this_rule.size())
			output = output + ",";
	}
	
	output = output + ".";
	
	return output;
}