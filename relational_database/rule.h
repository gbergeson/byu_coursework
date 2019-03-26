#pragma once
#include "parameter.h"
#include "predicate.h"
#include "token.h"

using namespace std;

class Rule {
public:
	Rule();
	~Rule();
	string toString();

	vector<Predicate*> this_rule;
};