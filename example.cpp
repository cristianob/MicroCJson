#include "micro_cjson.hpp"

using namespace std;
int main()
{
	string test = "{\"test\":123}";

	JSONData *data = parseJSON(test);
	JSONData_dump(data);

	return 0;
}
