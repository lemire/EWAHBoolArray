
#include <vector>
#include <iostream>
#include <unordered_map>
#include <string>
#include <ewah.h>
#include <string>
#include <algorithm>
#include <fstream>
#include <sstream>

using namespace std;

/**
* Simple DEMO to test the intersects function
*/

unordered_map<string, EWAHBoolArray<uint32_t> > indexColumn(vector<string> col) {
	unordered_map<string, EWAHBoolArray<uint32_t> > indexes;
	for (size_t i = 0; i < col.size(); i++) {
		indexes[col[i]].set(i);
	}
	return indexes;
}


int main() {

	// read the data from the CSV file
	vector <string> col1, col2;
	ifstream infile("data/data.csv");

	while (infile)
	{
		string s;
		if (!getline(infile, s)) break;

		istringstream ss(s);

		int numCommas = 0;
		while (ss)
		{
			if (!getline(ss, s, ',')) break;
			if (numCommas == 0)
			{
				col1.push_back(s);
				numCommas++;
			}
			else
			{
				col2.push_back(s);
				numCommas = 0;
			}
		}
	}


	unordered_map<string, EWAHBoolArray<uint32_t> > index1 = indexColumn(col1);
	unordered_map<string, EWAHBoolArray<uint32_t> > index2 = indexColumn(col2);
	size_t testcount = 0;

	for (unordered_map<string, EWAHBoolArray<uint32_t> >::iterator i = index1.begin(); i!= index1.end(); ++i) {
		EWAHBoolArray<uint32_t> & bitmap1 = i->second;
		for (unordered_map<string, EWAHBoolArray<uint32_t> >::iterator j = index2.begin(); j != index2.end(); ++j) {
			EWAHBoolArray<uint32_t> & bitmap2 = j->second;
			bool intersect = bitmap1.intersects(bitmap2);
			EWAHBoolArray<uint32_t> result;
			testcount ++;
			bitmap1.logicaland(bitmap2, result);
			if (intersect)
			{
				assert(result.numberOfOnes() > 0);
			}
			else
			{
				assert(result.numberOfOnes() == 0);
			}
		}

	}
	cout<< "Ran " << testcount << " tests. Your code is probably ok. "<<endl;


	return 0;
}
