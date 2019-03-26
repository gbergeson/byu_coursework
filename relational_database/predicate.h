#pragma once
#include "parameter.h"
#include "token.h"

using namespace std;

class Predicate{
public:
	Predicate(string kind_of_predicate);
	~Predicate();
	
	string toString();
	
	vector<Parameter*> this_predicate;
	string type;
};