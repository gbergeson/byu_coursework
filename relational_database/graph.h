#include <set>
#include <map>
#include <string>
#include <stack>

using namespace std;

class Graph {
public:
	Graph(){};
	~Graph(){};
	
	void addNode(int a, int b){
		my_map[a].addNode(b);
	};
	
	void addNode(int a){
		my_map[a] = Node();
	};
	
	bool isSelfDependent(int a){
		return my_map[a].isSelfDependent(a);
	};
	
	void DFSForest(stack<int> &my_stack){
		for(auto &map_el : my_map)
			if(!map_el.second.visited)
				DFS(map_el.first, my_stack);
		return;
	};
	
	void DFS(int n, stack<int> &my_stack){
		my_map[n].visited = true;
		for(int i : my_map[n].adjacent_nodes)
			if(!my_map[i].visited)
				DFS(i, my_stack);
		my_stack.push(n);
		return;
	};
	
	vector<set<int>> SCCs(stack<int> &my_stack){
		vector<set<int>> strong_conns;
		
		while(!my_stack.empty()){
			int a = my_stack.top();
			if(!my_map[a].visited){
				set<int> sccomp;
				DFSSC(a, sccomp);
				strong_conns.push_back(sccomp);
			}
			my_stack.pop();
		}
		
		return strong_conns;
	};
	
	void DFSSC(int a, set<int> &sccomp){
		my_map[a].visited = true;
		sccomp.insert(a);
		for(int i : my_map[a].adjacent_nodes)
			if(!my_map[i].visited)
				DFSSC(i, sccomp);
		return;
	};
	
	string toString(){
		string output = "";
		
		for(auto &kv : my_map)
			output = output + "R" + to_string(kv.first) + ":" + kv.second.toString() + '\n';
		
		return output;
	};
	
private:
	struct Node {
		Node(){
			visited = false;
		};
		~Node(){};
		
		bool isSelfDependent(int i){
			for(int j : adjacent_nodes)
				if(i == j)
					return true;
				
			return false;
		};
		
		void addNode(int a){
			adjacent_nodes.insert(a);
		};
		
		string toString(){
			string output = "";
			
			if(adjacent_nodes.size() == 0)
				return output;
			
			set<int>::iterator it = adjacent_nodes.end();
			it--;
			for(int i : adjacent_nodes){
				output = output + "R" + to_string(i);
				if(i != *it)
					output = output + ",";
			}
			
			return output;
		};
		
		set<int> adjacent_nodes;
		bool visited;
	};
	
	map<int, Node> my_map;
};