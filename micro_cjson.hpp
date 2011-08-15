#include <cstdlib>
#include <vector>
#include "micro_cjson_data.hpp"

typedef enum {
	UNKNOW, IN_KEY, IN_VALUE
} JsonParserState;

typedef enum {
	MAP, LIST
} JsonParserContainer;


bool checknum(char s)
{
	return (s >= 48 && s <= 57);
}


extern "C" JSONData *parseJSON(string data)
{
	const char *dc = data.c_str();
	
	int p = -1;
	JsonParserState state = UNKNOW;
	string key = "";
	vector<string> key_level;
	vector<JSONData*> stack;
	vector<JsonParserContainer> stack_type;
	
	for(int i=0; dc[i] != '\0'; i++){
		if(dc[i] == '{'){
			state = UNKNOW;
			key_level.push_back(key);
			stack.push_back(JSONData_mapCreate());
			stack_type.push_back(MAP);
			p++;
		}
		else if(dc[i] == '['){
			state == UNKNOW;
			key_level.push_back(key);
			stack.push_back(JSONData_listCreate());
			stack_type.push_back(LIST);
			p++;
		}

		else if((dc[i] == '"' && (state == IN_VALUE || state == UNKNOW)) && stack_type[stack_type.size() - 1] == MAP){
			key = "";

			state = IN_KEY;

			i++;
			while(dc[i] != '"'){
				key += dc[i];
				i++;
			}
		}

		else if((dc[i] == ']' || dc[i] == '}') && p>0){
			if(stack_type[stack_type.size() - 2] == LIST)
				JSONData_listAdd(stack[stack.size() - 2], stack[stack.size() - 1]);
			else if(stack_type[stack_type.size() - 2] == MAP)
				JSONData_mapAdd(stack[stack.size() - 2], key_level[key_level.size() - 1], stack[stack.size() - 1]);
			
			stack.pop_back();
			stack_type.pop_back();
			key_level.pop_back();
			p--;
		}

		else if(state == IN_KEY || (stack_type[stack_type.size() - 1] == LIST)){
			state = IN_VALUE;

			if(dc[i] == ':'){
				state = IN_KEY;
				continue;
			}

			if(dc[i] == '"'){
				string value = "";
				
				i++;
				while(dc[i] != '"'){
					if(dc[i] == '\\' && dc[i+1] != 'u'){
						i++;
					}
		
					value += dc[i];
					i++;
				}
				
				if(stack_type[stack_type.size() - 1] == MAP)
					JSONData_mapAdd(stack[stack.size() - 1], key, JSONData_createByString(value));
				else
					JSONData_listAdd(stack[stack.size() - 1], JSONData_createByString(value));
			}
			else if(dc[i] == 'f'){
				i += 4;
				
				if(stack_type[stack_type.size() - 1] == MAP)
					JSONData_mapAdd(stack[stack.size() - 1], key, JSONData_createByBool(false));
				else
					JSONData_listAdd(stack[stack.size() - 1], JSONData_createByBool(false));
			}
			else if(dc[i] == 't'){
				i += 3;
				
				if(stack_type[stack_type.size() - 1] == MAP)
					JSONData_mapAdd(stack[stack.size() - 1], key, JSONData_createByBool(true));
				else
					JSONData_listAdd(stack[stack.size() - 1], JSONData_createByBool(true));
			}
			else if(dc[i] == 'n'){
				i += 3;
				
				if(stack_type[stack_type.size() - 1] == MAP)
					JSONData_mapAdd(stack[stack.size() - 1], key, JSONData_createNull());
				else
					JSONData_listAdd(stack[stack.size() - 1], JSONData_createNull());
			}
			else if( checknum(dc[i]) ){
				string value = "";

				while( checknum(dc[i]) ){
					value += dc[i];
					i++;
				}
				
				if(stack_type[stack_type.size() - 1] == MAP)
					JSONData_mapAdd(stack[stack.size() - 1], key, JSONData_createByULong( atoi(value.c_str()) ));
				else
					JSONData_listAdd(stack[stack.size() - 1], JSONData_createByULong( atoi(value.c_str()) ));
			}
		}
	}

	return stack[stack.size() - 1];
}
