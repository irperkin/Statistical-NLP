#include <iostream>
#include <fstream>
#include <unordered_map>
#include <tuple>
#include <list>

using namespace std;
#define BigramList list<tuple<string, string, int, float> >
#define BigramMap unordered_map<string, BigramList>
#define UnigramMap unordered_map<string, int>

// Using c++11
// TO COMPILE: g++ -std=c++11 main.cpp

void mapInit(string inputFile, BigramMap& bigramMap, UnigramMap& unigramMap) {
	ifstream iStream(inputFile.c_str());
	string firstWord, secondWord;
	BigramMap::iterator bigramIter, bigramIter2;
	BigramList::iterator listIter;
	tuple<string, string, int, float> tup;

	iStream >> firstWord;

	while(iStream.is_open()) {
		iStream >> secondWord;
		cout << firstWord + " " + secondWord << endl;
		bigramIter = bigramMap.find(firstWord);
		if(bigramIter != bigramMap.end()) {
			// checks if bigram already exists by iterating through list of tuples
			for(listIter = bigramIter->second.begin(); get<1>(*listIter) != secondWord && listIter != bigramIter->second.end(); listIter++) { }
			if(listIter != bigramIter->second.end()) {
				// second word of bigram found so incrementing frequency in appropriate tuple
				get<2>(*listIter)++;
			} else {
				// TODO: FIX THIS
				// second word of bigram not found so creating new tuple and adding to list of bigrams that start with first word in bigram
				tup = make_tuple(firstWord, secondWord, 1, 0.0);
				BigramList temp = new BigramList(bigramIter->second);
				temp.push_back(tup);
				bigramMap[firstWord] = temp;
			}
		} else {
			// first word not found in hash table; create new list
			tup = make_tuple(firstWord, secondWord, 1, 0.0);
			BigramList newList;
			newList.push_back(tup);
			bigramMap[firstWord] = newList;
		}
		firstWord = secondWord;
	}
}

void printBigramMap(BigramMap bm) {
	int count = 0;
	cout << "wtf is going on" << endl;
	for(BigramMap::const_iterator iter = bm.begin(); iter != bm.end(); iter++) {
		cout << "count = " << count++;
		for(BigramList::const_iterator iter2 = iter->second.begin(); iter2 != iter->second.end(); iter2++) {
			cout << "test" << endl;
			cout << get<0>(*iter2) + " " + get<1>(*iter2) << endl;
		}
	}
}

int main(int argc, char * argv[]) {
	if(argc != 3) {
		cout << "USAGE: ./a.out dir/to/file.txt sentence" << endl;
		return 1;
	}

	BigramMap bigramMap;
	UnigramMap unigramMap;
	string inputFile = string(argv[1]);
	string sentence = string(argv[2]);

	mapInit(inputFile, bigramMap, unigramMap);
	printBigramMap(bigramMap);

	return 0;
}