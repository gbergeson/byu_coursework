#pragma once
#include "token.h"

using namespace std;

class Parameter{
public:
	Parameter(string contents);
	Parameter(string contents, string token_type);
	~Parameter();
	
	string toString();

	string value;
	string type;
};