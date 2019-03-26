#include "parser.h"

parser::parser(vector<Token*> to_parse){
	token_list = to_parse;
	current_parse = 0;
	left_count = 0;
	right_count = 0;
	push_type = "SCHEMES";
	expression_track = "";
	adding_expression = false;
}

parser::~parser(){
}

void parser::datalogProgram() {
	
	try {
		match("SCHEMES");
		match("COLON");
		scheme();
		schemeList();
		
		match("FACTS");
		match("COLON");
		push_type = "FACTS";
		factList();
		
		match("RULES");
		match("COLON");
		push_type = "RULES";
		ruleList();
		
		match("QUERIES");
		match("COLON");
		push_type = "QUERIES";
		query();
		queryList();
	}
	catch(Token* offending_token) {
		cout << "Failure!" << '\n' << "  ";
		cout << offending_token->make_string() << endl;
	}
	
}

void parser::scheme() {
	schemes_list.push_back(new Predicate("SCHEMES"));
	match("ID");
	push();
	match("LEFT_PAREN");
	match("ID");
	push();
	idList();
	match("RIGHT_PAREN");
}

void parser::schemeList() {
	
	if(lambda("FACTS")) return;
	
	scheme();
	schemeList();
}

void parser::idList() {
	
	if(lambda("RIGHT_PAREN")) return;
	
	match("COMMA");
	match("ID");
	push();
	idList();
}

void parser::fact() {
	match("ID");
	push();
	match("LEFT_PAREN");
	match("STRING");
	push();
	stringList();
	match("RIGHT_PAREN");
	match("PERIOD");
}

void parser::factList() {
	if(lambda("RULES")) return;
	
	facts_list.push_back(new Predicate("FACTS"));
	fact();	
	factList();
}

void parser::rule() {
	rules_list.push_back(new Rule());
	int current = rules_list.size() - 1;
	rules_list[current]->this_rule.push_back(new Predicate("RULES"));
	
	headPredicate();
	match("COLON_DASH");
	predicate();
	predicateList();
	match("PERIOD");
}

void parser::ruleList() {
	if(lambda("QUERIES")) return;
	rule();
	ruleList();
}

void parser::headPredicate() {	
	match("ID");
	push();
	match("LEFT_PAREN");
	match("ID");
	push();
	idList();
	match("RIGHT_PAREN");
}

void parser::predicate() {
	if(push_type == "RULES"){
		int current = rules_list.size() - 1;
		rules_list[current]->this_rule.push_back(new Predicate("RULES"));
	}
	
	match("ID");
	push();
	match("LEFT_PAREN");
	parameter();
	parameterList();
	match("RIGHT_PAREN");
}

void parser::predicateList() {
	
	if(lambda("PERIOD")) return;
	
	match("COMMA");
	predicate();
	predicateList();
}

void parser::parameter() {
	Parameter* added_parameter;
	string check = token_list[current_parse]->Token_Type;
	
	if(check == "STRING" || check == "ID"){
		current_parse++;
		added_parameter = push();
		if(added_parameter != NULL)
			added_parameter->type = check;
		return;
	}
	else {
		adding_expression = true;
		expression();
	}
}

void parser::parameterList() {
	if(lambda("RIGHT_PAREN")) return;
	match("COMMA");
	parameter();
	parameterList();
}

void parser::expression() {
	match("LEFT_PAREN");
	left_count++;
	push();
	parameter();
	valid_operator();
	push();
	parameter();
	match("RIGHT_PAREN");
	right_count++;
	push();
	if(left_count == right_count){
		adding_expression = false;
		left_count = 0;
		right_count = 0;
		pushExpression(expression_track);
	}
}

void parser::valid_operator() {
	string check = token_list[current_parse]->Token_Type;
	
	if(check == "ADD" || check == "MULTIPLY"){
		current_parse++;
		return;
	}
	else
		throw token_list[current_parse];
}

void parser::query() {
	
	queries_list.push_back(new Predicate("QUERIES"));
	
	predicate();
	match("Q_MARK");
}

void parser::queryList() {
	
	if(lambda("EOF")) return;
	
	query();
	queryList();
}

void parser::stringList() {
	
	if(lambda("RIGHT_PAREN")) return;
	
	match("COMMA");
	match("STRING");
	push();
	stringList();
}

void parser::match(string name){
	string check = token_list[current_parse]->Token_Type;
	
	if(check == name)
		current_parse++;
	else
		throw token_list[current_parse];
}

bool parser::lambda(string lambda_condition) {
	string check = token_list[current_parse]->Token_Type;
	
	return check == lambda_condition;
}

Parameter* parser::push() {
	Parameter* added_parameter = NULL;
	string value = token_list[current_parse - 1]->contents;
	string token_type = token_list[current_parse - 1]->Token_Type;
	
	if(adding_expression){
		expression_track = expression_track + value;
		return NULL;
	}
	else if(push_type == "SCHEMES"){
		int current_scheme = schemes_list.size() - 1;
		
		schemes_list[current_scheme]->this_predicate.push_back(new Parameter(value, token_type));
		int count = schemes_list[current_scheme]->this_predicate.size() - 1;
		added_parameter = schemes_list[current_scheme]->this_predicate[count];
	}
	else if(push_type == "FACTS"){
		int current_fact = facts_list.size() - 1;
		
		if(token_list[current_parse - 1]->Token_Type == "STRING")
			domain.insert(value);
		
		facts_list[current_fact]->this_predicate.push_back(new Parameter(value, token_type));
		int count = facts_list[current_fact]->this_predicate.size() - 1;
		added_parameter = facts_list[current_fact]->this_predicate[count];
	}
	else if(push_type == "RULES"){
		int current_rule = rules_list.size() - 1;
		int current_predicate = rules_list[current_rule]->this_rule.size() - 1;
		if(current_predicate < 0)
			current_predicate = 0;
		
		rules_list[current_rule]->this_rule[current_predicate]->this_predicate.push_back(new Parameter(value, token_type));
		int count = rules_list[current_rule]->this_rule[current_predicate]->this_predicate.size() - 1;
		added_parameter = rules_list[current_rule]->this_rule[current_predicate]->this_predicate[count];
	}
	else if(push_type == "QUERIES"){
		int current_query = queries_list.size() - 1;
		
		queries_list[current_query]->this_predicate.push_back(new Parameter(value, token_type));
		int count = queries_list[current_query]->this_predicate.size() - 1;
		added_parameter = queries_list[current_query]->this_predicate[count];
	}
	return added_parameter;
}

void parser::print() {
	cout << "Schemes(" << schemes_list.size() << "):" << endl;
	for(unsigned int i = 0; i < schemes_list.size(); i++)
		cout << "  " << schemes_list[i]->toString() << endl;
	
	cout << "Facts(" << facts_list.size() << "):" << endl;
	for(unsigned int i = 0; i < facts_list.size(); i++)
		cout << "  " << facts_list[i]->toString() << endl;
	
	cout << "Rules(" << rules_list.size() << "):" << endl;
	for(unsigned int i = 0; i < rules_list.size(); i++)
		cout << "  " << rules_list[i]->toString() << endl;
	
	cout << "Queries(" << queries_list.size() << "):" << endl;
	for(unsigned int i = 0; i < queries_list.size(); i++)
		cout << "  " << queries_list[i]->toString() << endl;
	
	cout << "Domain(" << domain.size() << "):" << endl;
	if(domain.size() > 0){
		for(string s : domain)
			cout << "  " << s << endl;
	}
}

void parser::pushExpression(string value){
	if(push_type == "RULES"){
		int current_rule = rules_list.size() - 1;
		int current_predicate = rules_list[current_rule]->this_rule.size() - 1;
		if(current_predicate < 0)
			current_predicate = 0;
		
		rules_list[current_rule]->this_rule[current_predicate]->this_predicate.push_back(new Parameter(value));
		int count = rules_list[current_rule]->this_rule[current_predicate]->this_predicate.size() - 1;
		rules_list[current_rule]->this_rule[current_predicate]->this_predicate[count]->type = "EXPRESSION";
	}
	else if(push_type == "QUERIES"){
		int current_query = queries_list.size() - 1;
		
		queries_list[current_query]->this_predicate.push_back(new Parameter(value));
		int count = queries_list[current_query]->this_predicate.size() - 1;
		queries_list[current_query]->this_predicate[count]->type = "EXPRESSION";
	}
	expression_track = "";
}