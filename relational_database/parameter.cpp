#include "parameter.h"

Parameter::Parameter(string contents){
	value = contents;
	type = "";
}

Parameter::Parameter(string contents, string token_type){
	value = contents;
	type = token_type;
}

Parameter::~Parameter(){
	
}

string Parameter::toString(){
	return value;
}