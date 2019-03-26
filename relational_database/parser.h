#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <set>
#include "token.h"
#include "rule.h"
#include "predicate.h"
#include "parameter.h"

using namespace std;

class parser {
public:
	
	parser(){};
	parser(vector<Token*> to_parse);
	~parser();
	
	//grammar methods
	void datalogProgram();
	void scheme();
	void schemeList();
	void idList();
	void fact();
	void factList();
	void rule();
	void ruleList();
	void headPredicate();
	void predicate();
	void predicateList();
	void parameter();
	void parameterList();
	void expression();
	void valid_operator();
	void query();
	void queryList();
	void stringList();
	Parameter* push();
	void print();
	void pushExpression(string value);
	
	//token check methods
	void match(string name);
	bool lambda(string lambda_condition);
	
	vector<Rule*> rules_list;
	vector<Predicate*> schemes_list;
	vector<Predicate*> facts_list;
	vector<Predicate*> queries_list;
	
private:
	vector<Token*> token_list;
	vector<Parameter*> expression_tracker;
	set<string> domain;
	
	int current_parse, left_count, right_count;
	string push_type, expression_track;
	bool adding_expression;
};


















