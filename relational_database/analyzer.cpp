
#include "analyzer.h"

analyzer::analyzer(string to_analyze){
	input.open(to_analyze);
	lineNum = 1;
}

analyzer::~analyzer(){
	for(size_t i = 0; i < list_of_tokens.size(); i++){
		delete list_of_tokens[i];
	}
}

bool analyzer::run(){
	
	while (!input.eof()){
		start();
	}
	
	addToken("EOF", "", lineNum);
	
	//printTokens();
	if(input.eof())
		return true;
	else
		return false;
}

void analyzer::start(){
	token = input.get();
	
	while (!input.eof()){
		
		while(isspace(token)){
				addNewLine();
				token = input.get();
		}
		
		if (isColDash()){
			token = input.get();
			addToken("COLON_DASH", ":-", lineNum);
			token = input.get();
		}
		else if (oneChar()){
			addToken(token_map[token], string(1, token), lineNum);
			token = input.get();
		}
		else if (isalpha(token)){
			state1();
		}
		else if (isStringComment()){
			callState();
		}
		else if (!input.eof()){
			addToken("UNDEFINED", string(1,token),lineNum);
			token = input.get();
		}
	}
	
	return;
}

void analyzer::state1(){
	string output = "";
	
	while(isalnum(token)){
		output.push_back(token);
		token = input.get();
	}
	
	if(output == "Facts")
		addToken("FACTS", output, lineNum);
	else if(output == "Rules")
		addToken("RULES", output, lineNum);
	else if(output == "Schemes")
		addToken("SCHEMES", output, lineNum);
	else if(output == "Queries")
		addToken("QUERIES", output, lineNum);
	else 
		addToken("ID", output, lineNum);

}

void analyzer::state2(){
	string output;
	char check;
	bool end_str = false;
	int token_line = lineNum;
	
	output.push_back(token);
	while(!end_str){
			
		token = input.get();
		
		if(input.eof()){
			addToken("UNDEFINED", output, token_line);
			return;
		}		

		output.push_back(token);
		
		if(token == '\''){
			check = input.peek();
			if(check != '\'')
				end_str = true;
			else {
				token = input.get();
				output.push_back(token);
			}
		}
		addNewLine();
	}
	
	addToken("STRING", output, token_line);
	token = input.get();
}

void analyzer::state3(){
	string output;
	int token_line = lineNum;
	
	output.push_back(token);
	token = input.get();
	if(token == '|'){
		do{
			if(input.eof()){
			addToken("UNDEFINED", output, token_line);
			return;
			}

			output.push_back(token);
			token = input.get();
			
			addNewLine();
			
		}while(!isEndBlockComment());
			
		output.push_back(token);
		token = input.get();
		output.push_back(token);
		token = input.get();
	
		//addToken("COMMENT", output, token_line); --> this code is no longer necessary for the project.
		return;
	}
	else {
		while(!isEndComment()){
			output.push_back(token);
			token = input.get();
		}

		//addToken("COMMENT", output, token_line);
		addNewLine();
		token = input.get();
	}
}

void analyzer::printTokens(){
	string tType, value;
	int number;
	
	for(size_t i = 0; i < list_of_tokens.size(); i++){
		tType = list_of_tokens[i]->Token_Type;
		value = list_of_tokens[i]->contents;
		number = list_of_tokens[i]->line_number;
		cout << "(" << tType << ",\"" << value << "\"," << number << ")" << endl;
	}
	
	cout << "Total Tokens = " << list_of_tokens.size() << endl;
}

void analyzer::addToken(string tType, string value, int number_of_line){
	new_token = new Token(tType, value, number_of_line);
	list_of_tokens.push_back(new_token);
}

bool analyzer::oneChar(){
	return token == '.' || token == ',' || token == '?' || token == '(' || token == ')' || token == ':' || token == '*' || token == '+';
}

bool analyzer::isColDash(){
	return token == ':' && input.peek() == '-';
}

bool analyzer::isEndBlockComment(){
	return  token == '|' && input.peek() == '#';
}

bool analyzer::isEndComment(){
	return token == '\n' || input.eof();
}

void analyzer::addNewLine(){
	if (token == '\n')
		lineNum++;
}

void analyzer::getNewToken(){
	if (!input.eof())
		token = input.get();
}

bool analyzer::isStringComment() {
	return token == '\'' || token == '#';
}

void analyzer::callState() {
	if(token == '\'')
		state2();
	else if (token == '#')
		state3();
	else
		return;
}

vector<Token*> analyzer::getListOfTokens(){
	return list_of_tokens;
}