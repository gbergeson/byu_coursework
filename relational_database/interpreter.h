#include "database.h"
#include "graph.h"

using namespace std;

class Interpreter {
public:
	Interpreter(parser dtlg_prgrm);
	~Interpreter();
	
	//Query functions
	void runQueries();
	Scheme makeScheme(vector<string> scheme_base);
	void print(Predicate* p, Relation r);
	void clearDuplicateVariables();
	
	Relation makeRelation(Predicate * q);
	void selectStrings(Relation &r);
	void selectIDs(Relation &r);
	
	//Rule functions
	void processor();
	void processRule(Rule * r);
	vector<Relation> makeRelations(Rule * r);
	void joinIntermediates();
	void findVariables();
	void clearVectors();
	
	void buildGraphs();
	void findSCCs();
	void printPasses(set<int> &s, int &passes);
	
	Database* my_data;
	
private:
	int rule_count;
	parser dtlg_objct;
	vector<string> variable_list;
	vector<int> variable_indices, string_indices;
	vector<Relation> intermediates;
	Relation intermediate;
	Relation head_predicate;
	Graph fgraph;
	Graph rgraph;
	stack<int> my_stack;
	vector<set<int>> rule_order;
};