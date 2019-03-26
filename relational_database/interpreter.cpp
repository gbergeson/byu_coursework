#include "interpreter.h"

Interpreter::Interpreter(parser dtlg_prgrm){
	my_data = new Database(dtlg_prgrm);
	dtlg_objct = dtlg_prgrm;
	rule_count = 0;
}

Interpreter::~Interpreter(){
	
}

void Interpreter::runQueries(){
	for(Predicate* q : dtlg_objct.queries_list){
		Relation r = makeRelation(q);
		
		selectStrings(r);
		selectIDs(r);
		clearDuplicateVariables();
		r = r.project(variable_indices);
		Scheme new_scheme = makeScheme(variable_list);
		r.rename(new_scheme);
		
		print(q, r);
		clearVectors();
	}
}

Scheme Interpreter::makeScheme(vector<string> scheme_base){
	Scheme to_return;
	
	for(string s : scheme_base)
		to_return.push_back(s);
	
	return to_return;
}
	
void Interpreter::print(Predicate* p, Relation r){
	cout << p->toString() << " ";
	int number = r.numTuples();
	
	if(number == 0)
		cout << "No" << endl;
	else{
		cout << "Yes(" << number << ")";
		
		for(Tuple t : r.tuples){
			for(unsigned int i = 0; i < r.my_scheme.size(); i++){
				if(i == 0)
					cout << endl << "  ";
				cout << r.my_scheme[i] << "=" << t[i];
				if(i != r.my_scheme.size() - 1)
					cout << ", ";
			}
		}
		cout << endl;
	}
}

void Interpreter::clearDuplicateVariables(){
	for(unsigned int i = 0; i < variable_list.size(); i++){
		for(unsigned int i2 = i + 1; i2 < variable_list.size(); i2++){
			if(variable_list[i2] == variable_list[i]){
				for(unsigned int replace = i2 + 1; replace < variable_list.size(); replace++){
					variable_list[replace - 1] = variable_list[replace];
					variable_indices[replace - 1] = variable_indices[replace];
				}
				variable_list.pop_back();
				variable_indices.pop_back();
				i2--;
			}
		}
	}
}

Relation Interpreter::makeRelation(Predicate * p){
	string name = p->this_predicate[0]->toString();
	Scheme new_scheme;
	
	for(unsigned int i = 1; i < p->this_predicate.size(); i++){
		new_scheme.push_back(p->this_predicate[i]->toString());
		if(p->this_predicate[i]->type == "STRING")
			string_indices.push_back(i-1);
		if(p->this_predicate[i]->type == "ID"){
			variable_indices.push_back(i-1);
			variable_list.push_back(p->this_predicate[i]->value);
		}
	}
	
	Relation r(name, new_scheme);
	
	for(Tuple t : my_data->my_map[name].tuples)
		r.add(t);
	
	return r;
}


void Interpreter::processor(){
	int counti, countf;
	
	buildGraphs();
	findSCCs();
	
	string graph_print = fgraph.toString();
	
	cout << "Dependency Graph\n" << graph_print << endl << "Rule Evaluation\n";
	
	for(set<int> s : rule_order){
		if(s.size() == 1 && !fgraph.isSelfDependent(*s.begin())){
			processRule(dtlg_objct.rules_list[*s.begin()]);
			cout << "1 passes: R" << *s.begin() << endl;
		}
		else{
			do{
				counti = my_data->size();
				for(int i : s)
					processRule(dtlg_objct.rules_list[i]);
				rule_count++;
				countf = my_data->size();
			}while(counti != countf);
			printPasses(s, rule_count);
		}
	}
	
	cout << "\nQuery Evaluation\n";
	
	clearVectors();
	
	runQueries();
}

void Interpreter::processRule(Rule * r){
	clearVectors();
	intermediates = makeRelations(r);
	joinIntermediates();
	findVariables();
	intermediate = intermediate.project(variable_indices);
	intermediate.rename(my_data->my_map[head_predicate.name].my_scheme);
	my_data->my_map[head_predicate.name] = intermediate.unify(my_data->my_map[head_predicate.name], intermediate);	
}

vector<Relation> Interpreter::makeRelations(Rule * r){
	vector<Relation> new_rels;
	
	for(Predicate* p : r->this_rule) {
		clearVectors();
		Relation rel = makeRelation(p);
		selectStrings(rel);
		selectIDs(rel);
		clearDuplicateVariables();
		rel = rel.project(variable_indices);
		Scheme new_scheme = makeScheme(variable_list);
		rel.rename(new_scheme);
		new_rels.push_back(rel);
	}
	
	return new_rels;
}

void Interpreter::selectStrings(Relation &r){
	for(unsigned int i = 0; i < string_indices.size(); i++){
		int index = string_indices[i];
		string match = r.my_scheme[index];
		r = r.select(index, match);
	}
}

void Interpreter::selectIDs(Relation &r){
	for(unsigned int i = 0; i < variable_indices.size(); i++)
		for(unsigned int j = i + 1; j < r.my_scheme.size(); j++)
			if(r.my_scheme[i] == r.my_scheme[j])
				r = r.select(i, j);
}

void Interpreter::joinIntermediates(){
	head_predicate = intermediates[0];
	intermediate = intermediates[1];	
	
	if(intermediates.size() == 2)
		return;
	
	for(unsigned int i = 2; i < intermediates.size(); i++)
		intermediate = intermediate.join(intermediate, intermediates[i]);
	
	return;
}

void Interpreter::findVariables(){ 
	clearVectors();
	for(string s : head_predicate.my_scheme)
		for(unsigned int i = 0; i < intermediate.my_scheme.size(); i++)
			if(s == intermediate.my_scheme[i])
				variable_indices.push_back(i);
}

void Interpreter::clearVectors(){
	intermediates.clear();
	variable_indices.clear();
	variable_list.clear();
	string_indices.clear();
}

void Interpreter::buildGraphs(){
	for(unsigned int i = 0; i < dtlg_objct.rules_list.size(); i++){
		fgraph.addNode(i);
		rgraph.addNode(i);
		for(unsigned int j = 1; j < dtlg_objct.rules_list[i]->this_rule.size(); j++){
			string jname = dtlg_objct.rules_list[i]->this_rule[j]->this_predicate[0]->toString();
			for(unsigned int k = 0; k < dtlg_objct.rules_list.size(); k++){
				string kname = dtlg_objct.rules_list[k]->this_rule[0]->this_predicate[0]->toString();
				if(jname == kname){
					fgraph.addNode(i, k);
					rgraph.addNode(k, i);
				}
			}
		}
	}
}

void Interpreter::findSCCs(){
	rgraph.DFSForest(my_stack);
	rule_order = fgraph.SCCs(my_stack);
}

void Interpreter::printPasses(set<int> &s, int &passes){
	cout << rule_count << " passes: ";
	set<int>::iterator it = s.end();
	it--;
	for(int i : s){
		cout << "R" << i;
		if(i != *it)
			cout << ",";
	}
	cout << endl;
	rule_count = 0;
}
















