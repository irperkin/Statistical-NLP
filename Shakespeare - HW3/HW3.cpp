#include <iostream>

using namespace std;

int main(int argc, char * argv[]) {
	if(argc != 2) {
		cout << "USAGE: ./a.out sentence" << endl;
		return 1;
	}

	string sentence = argv[1];
	

	return 0;
}