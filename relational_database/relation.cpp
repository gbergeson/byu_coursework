#include "relation.h"

Relation::Relation(string rel_name, Scheme first_scheme){
	name = rel_name;
	my_scheme = first_scheme;
}

Relation::~Relation(){
	
}

Relation Relation::select(int &index, string &match){
	Relation output(name, my_scheme);
	
	for(Tuple t : tuples){
		if(t[index] == match)
			output.add(t);
	}
	
	return output;
}

Relation Relation::select(int index1, int index2){
	Relation output(name, my_scheme);
	
	for(Tuple t : tuples){
		if(t[index1] == t[index2])
			output.add(t);
	}
	
	return output;
}

Relation Relation::project(vector<int> &indices){
	Relation output(name, my_scheme);
	
	for(Tuple t : tuples){
		Tuple add_this;
		for(unsigned int i = 0; i < indices.size(); i++)
			add_this.push_back(t[indices[i]]);
		output.add(add_this);
	}
	
	return output;
}

void Relation::rename(Scheme new_scheme){
	my_scheme = new_scheme;
}

void Relation::add(Tuple &new_tuple){
	tuples.insert(new_tuple);
}

int Relation::numTuples(){
	int i = 0;
	
	for(Tuple t : tuples)
		i++;
	
	return i;
}

Relation Relation::unify(Relation &first, Relation &second){
	Relation r;
	
	for(Tuple t : first.tuples){
		r.add(t);
	}
	for(Tuple t : second.tuples){
		r.add(t);
	}
	r.rename(first.my_scheme);
	
	return r;
}

Relation Relation::join(Relation &r1, Relation &r2){
	Relation new_r;
	
	new_r.rename(make_scheme(r1.my_scheme, r2.my_scheme));
	
	findElements(r1.my_scheme, r2.my_scheme);
	
	for(Tuple t1 : r1.tuples){
		for(Tuple t2 : r2.tuples){
			if(joinable(t1, t2)){
				Tuple t = join_tuples(t1, t2);
				new_r.add(t);
			}
		}
	}
	
	return new_r;
}

void Relation::findElements(Scheme &s1, Scheme &s2){ 
	v1.clear();
	v2.clear();
	v3.clear();
	
	for(unsigned int i1 = 0; i1 < s1.size(); i1++)
		for(unsigned int i2 = 0; i2 < s2.size(); i2++)
			if(s1[i1] == s2[i2]){
				v1.push_back(i1);
				v2.push_back(i2);
			}
			
	for(unsigned int i3 = 0; i3 < s2.size(); i3++)
		if(!v2HasIt(i3))
			v3.push_back(i3);
}

bool Relation::v2HasIt(int i){
	for(int x : v2)
		if(i == x)
			return true;
	
	return false;
}

Scheme Relation::make_scheme(Scheme s1, Scheme s2){
	Scheme output;
	
	for(string s : s1){
		output.push_back(s);
		for(unsigned int t = 0; t < s2.size(); t++)
			if(s == s2[t])
				remove_element(t, s2);
	}

	for(string s : s2)
		output.push_back(s);
	
	return output;
}

bool Relation::joinable(Tuple &t1, Tuple &t2){
	for(unsigned int i = 0; i < v1.size(); i++){
		if(t1[v1[i]] != t2[v2[i]])
			return false;
	}

	return true;
}

Tuple Relation::join_tuples(Tuple &t1, Tuple &t2){
	Tuple new_t;
	
	for(string s : t1)
		new_t.push_back(s);
	for(unsigned int i = 0; i < v3.size(); i++)
		new_t.push_back(t2[v3[i]]);
		
	return new_t;
}

void Relation::remove_element(int i, Scheme &t){
	for(unsigned int current = i; current < t.size() - 1; current++){
		t[current] = t[current + 1];
	}
	
	t.pop_back();
}

string Relation::print(){
	string scheme_out = "";
	
	for(string s : my_scheme)
		scheme_out = scheme_out + s;
	
	string tuples_out = "(";
	
	for(Tuple t : tuples){
		for(string s : t){
			tuples_out = tuples_out + s + ",";
		}
		tuples_out = tuples_out + ")(";
	}
	
	string output = scheme_out + tuples_out; 
	
	return output;
}