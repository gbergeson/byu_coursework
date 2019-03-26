#include "parser.h"
#include "relation.h"
#include <map>

using namespace std;

class Database {
public:
	Database(parser dtlg_prgrm);
	~Database();
	int size();
	map<string, Relation> my_map;
};