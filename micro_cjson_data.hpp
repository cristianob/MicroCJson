#include <map>
#include <vector>
#include <string>
#include <cstdio>
#include <iostream>

using namespace std;

typedef enum {
	VDT_NULL, VDT_BOOL, VDT_INT, VDT_ULONG, VDT_STRING, VDT_LIST, VDT_MAP
} JSONDataType;

typedef struct JSONData_t {
	void *pointer;
	JSONDataType type;
	
	struct JSONData_t *operator[](const char *s)
	{
		if(this->type != VDT_MAP)
			return false;
	
		return ((map<string, struct JSONData_t*> *) this->pointer)->find(string(s))->second;
	}
	
	struct JSONData_t *operator[](unsigned int s)
	{
		if(this->type != VDT_LIST)
			return false;
	
		return ((vector<struct JSONData_t*> *) this->pointer)->at(s);
	}
} JSONData;

void JSONData_listDump(JSONData* d_list, int i);

JSONData *vdgi(JSONData *p, int k){
	return ((* ((JSONData *) p))[k]);
}
JSONData *vdgc(JSONData *p, const char *k){
	return ((* ((JSONData *) p))[k]);
}

string JSONData_toString(JSONData* data)
{
	if(data->type == VDT_NULL)
		return "(null value)";
	else if(data->type == VDT_BOOL){
		if(*(bool *) data->pointer == true)
			return "true";
		else
			return "false";
	}
	else if(data->type == VDT_INT){
		char rtr[32];
		sprintf(rtr,"%d", *(int *) data->pointer);
		return string(rtr);
	}
	else if(data->type == VDT_ULONG){
		char rtr[64];
		sprintf(rtr,"%u", *(unsigned long *) data->pointer);
		return string(rtr);
	}
	else if(data->type == VDT_STRING)
		return *((string *) data->pointer);
	else if(data->type == VDT_LIST)
		return "LIST";
	else if(data->type == VDT_MAP)
		return "MAP";
	else
		return "";
}

JSONData *JSONData_mapCreate()
{
	JSONData *vd = new JSONData;

	vd->pointer	= (void *) new map<string, JSONData*>();
	vd->type	= VDT_MAP;

	return vd;
}
bool JSONData_mapAdd(JSONData* d_map, string key, JSONData *value)
{
	if(d_map->type != VDT_MAP)
		return false;
	
	((map<string, JSONData*> *) d_map->pointer)->insert( pair<string,JSONData*>(key, value) );
	return true;
}
JSONData *JSONData_mapGet(JSONData* d_map, string key)
{
	if(d_map->type != VDT_MAP)
		return false;
	
	return ((map<string, JSONData*> *) d_map->pointer)->find(key)->second;
}
void JSONData_mapDump(JSONData* d_map, int i)
{	
	map<string, JSONData*> *p_map = (map<string, JSONData*> *) d_map->pointer;
	map<string, JSONData*>::iterator it;
	
	for(int j=0; j<i; j++)
		cout << '\t';
	cout << "{" << endl;
	for ( it=p_map->begin() ; it != p_map->end(); it++ ){
		if(it->second->type == VDT_MAP){
			for(int j=0; j<i; j++)
				cout << '\t';
			cout << it->first << " => ";
			
			JSONData_mapDump(it->second, i+1);
		}

		else if(it->second->type == VDT_LIST){
			for(int j=0; j<i; j++)
				cout << '\t';
			cout << it->first << " => ";
			
			JSONData_listDump(it->second, i+1);
		}
		
		else if(it->second->type == VDT_STRING){
			for(int j=0; j<i; j++)
				cout << '\t';
			cout << it->first << " => \"" << JSONData_toString(it->second) << '\"' << endl;
		}
		
		else {
			for(int j=0; j<i; j++)
				cout << '\t';
			cout << it->first << " => " << JSONData_toString(it->second) << endl;
		}
	}
	for(int j=0; j<i; j++)
		cout << '\t';
	cout << "}" << endl;
}


JSONData *JSONData_listCreate()
{
	JSONData *vd = new JSONData;

	vd->pointer	= (void *) new vector<JSONData*>();
	vd->type	= VDT_LIST;

	return vd;
}
bool JSONData_listAdd(JSONData* d_map, JSONData *value)
{
	if(d_map->type != VDT_LIST)
		return false;
	
	((vector<JSONData*> *) d_map->pointer)->push_back( value );
	return true;
}
void JSONData_listDump(JSONData* d_list, int i)
{
	vector<JSONData*> *p_list = (vector<JSONData*> *) d_list->pointer;
	
	cout << "[" << endl;
	for ( unsigned int z=0; z<p_list->size(); z++ ){
		if((*p_list)[z]->type == VDT_MAP){
			for(int j=0; j<i; j++)
				cout << '\t';
			
			JSONData_mapDump((JSONData *) (*p_list)[z], i+1);
		}

		else if((*p_list)[z]->type == VDT_LIST){
			for(int j=0; j<i; j++)
				cout << '\t';
			
			JSONData_listDump((JSONData *) (*p_list)[z]->pointer, i+1);
			
			for(int j=0; j<i; j++)
				cout << '\t';
			cout << "]" << endl;
		}		
		
		else if((*p_list)[z]->type == VDT_STRING){
			for(int j=0; j<i+1; j++)
				cout << '\t';
			cout << JSONData_toString((*p_list)[z]) << endl;
		}
		
		else {
			for(int j=0; j<i; j++)
				cout << '\t';
			cout << JSONData_toString((*p_list)[z]) << ", ";
		}
	}
	for(int j=0; j<i; j++)
		cout << '\t';
	cout << "]" << endl;
}

JSONData *JSONData_createNull()
{
	JSONData *vd = new JSONData;

	vd->pointer	= NULL;
	vd->type	= VDT_NULL;

	return vd;
}
JSONData *JSONData_createByBool(bool d)
{
	JSONData *vd = new JSONData;

	vd->pointer	= (void *) new bool(d);
	vd->type	= VDT_BOOL;

	return vd;
}
JSONData *JSONData_createByInt(int d)
{
	JSONData *vd = new JSONData;

	vd->pointer	= (void *) new int(d);
	vd->type	= VDT_INT;

	return vd;
}
JSONData *JSONData_createByULong(unsigned long d)
{
	JSONData *vd = new JSONData;

	vd->pointer	= (void *) new unsigned long(d);
	vd->type	= VDT_ULONG;

	return vd;
}
JSONData *JSONData_createByString(string d)
{
	JSONData *vd = new JSONData;

	vd->pointer	= (void *) new string(d);
	vd->type	= VDT_STRING;

	return vd;
}

bool *JSONData_readBool(JSONData *data)
{
	if(data->type == VDT_BOOL)
		return (bool *) data->pointer;
	else
		return NULL;
}
int *JSONData_readInt(JSONData *data)
{
	if(data->type == VDT_INT)
		return (int *) data->pointer;
	else
		return NULL;
}
unsigned long *JSONData_readULong(JSONData *data)
{
	if(data->type == VDT_ULONG)
		return (unsigned long *) data->pointer;
	else
		return NULL;
}
string *JSONData_readString(JSONData *data)
{
	if(data->type == VDT_STRING)
		return (string *) data->pointer;
	else
		return NULL;
}

void JSONData_delete(JSONData *data)
{
	if(data->type == VDT_INT)
		delete (int *) data->pointer;
	else if(data->type == VDT_STRING)
		delete (string *) data->pointer;

	delete data;
}


void JSONData_dump(JSONData *data)
{
	if(data->type == VDT_MAP){
		JSONData_mapDump(data, 0);
	}else if(data->type == VDT_LIST)
		JSONData_listDump(data, 0);

}
