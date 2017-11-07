#include <iostream>
#include <fstream>

using namespace std;

void preprocess(string inputFile) {
	ifstream iStream(inputFile.c_str());
	string line;

	while(iStream.is_open() && iStream >> line) {

	}
}

int main(int argc, char * argv[]) {
	if(argc != 3) {
		cout << "USAGE: ./a.out Shakespeare.txt sentence" << endl;
		return 1;
	}

	string sentence = string(argv[1]);
	string inputFile = string(argv[2]);

	preprocess(inputFile);

	return 0;
}