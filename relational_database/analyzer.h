#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include "token.h"

using namespace std;

class analyzer {
public:
	analyzer(string to_analyze);
	~analyzer();
	bool run();
	void start();
	void state1();
	void state2();
	void state3();
	void printTokens();
	void addToken(string tType, string value, int number_of_line);
	bool oneChar();
	bool isColDash();
	bool isEndBlockComment();
	bool isEndComment();
	void addNewLine();
	void getNewToken();
	bool isStringComment();
	void callState();
	vector<Token*> getListOfTokens();
	
private:
	Token* new_token;
	char token;
	ifstream input;
	int lineNum;
	vector<Token*> list_of_tokens;
	map<char, string> token_map = { {',',"COMMA"}, {'.',"PERIOD"}, {'?',"Q_MARK"}, {'(',"LEFT_PAREN"}, {')',"RIGHT_PAREN"}, {':',"COLON"}, {'*',"MULTIPLY"}, {'+',"ADD"}};
};