#include "analyzer.h"
#include "parser.h"
#include "interpreter.h"

using namespace std;

int main(int argC, char* argV[]){
	string input = argV[1];
	vector<Token*> tokenized;
	
	analyzer lexer(input);
	
	lexer.run();
	
	tokenized = lexer.getListOfTokens();
	
	parser part_1(tokenized);
	
	part_1.datalogProgram();
	
	Interpreter interpret(part_1);
	
	interpret.processor();

	return 0;
}