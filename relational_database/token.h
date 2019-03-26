//Everything in this class is defined in the header file
//because I'd like to do it as a struct, but this is the 
//best solution.

#pragma once
#include <string>
#include <vector>

using namespace std;

class Token {
public:
	Token(string tType, string value, int lineNum){
		Token_Type = tType;
		contents = value;
		line_number = lineNum;
	}
	
	string make_string(){
		return "(" + Token_Type + ",\"" + contents + "\"," + to_string(line_number) + ")";
	}
	
	string Token_Type;
	string contents;
	int line_number;
};