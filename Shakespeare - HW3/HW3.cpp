#include <iostream>
#include <fstream>
#include <unordered_map>
#include <tuple>
#include <list>
#include <math.h>

using namespace std;
#define BigramList list<tuple<string, string, int, float> >
#define BigramMap unordered_map<string, BigramList>
#define UnigramMap unordered_map<string, int>

// Using c++11
// TO COMPILE: g++ -std=c++11 HW3.cpp

void mapInit(string inputFile, BigramMap& bigramMap, UnigramMap& unigramMap) {
	ifstream iStream(inputFile.c_str());
	string firstWord, secondWord;
	BigramMap::iterator bigramIter;
	BigramList::iterator listIter;
	tuple<string, string, int, float> tup; // tuple<firstWord, secondWord, frequency/count, probability>

	iStream >> firstWord;
	while(iStream) {
		iStream >> secondWord;
		bigramIter = bigramMap.find(firstWord);
		if(bigramIter != bigramMap.end()) {
			// checks if bigram already exists by iterating through list of tuples
			for(listIter = (bigramIter->second).begin(); listIter != (bigramIter->second).end(); listIter++) {
				if(get<1>(*listIter) == secondWord) {
					break;
				}
			}

			if(listIter != bigramIter->second.end()) {
				// second word of bigram found so incrementing frequency in appropriate tuple value
				get<2>(*listIter)++;
			} else {
				// second word of bigram not found so creating new tuple and adding to list of bigrams that start with first word in bigram
				tup = make_tuple(firstWord, secondWord, 1, 0.0);
				bigramIter->second.push_back(tup);
			}
		} else {
			// first word not found in hash table; create new list
			tup = make_tuple(firstWord, secondWord, 1, 0.0);
			BigramList newList;
			newList.push_back(tup);
			bigramMap[firstWord] = newList;
		}
		unigramMap[firstWord]++;
		firstWord = secondWord;
	}
}

void printBigramMap(BigramMap bm) {
	for(BigramMap::const_iterator bigramIter = bm.begin(); bigramIter != bm.end(); bigramIter++) {
		for(BigramList::const_iterator listIter = bigramIter->second.begin(); listIter != bigramIter->second.end(); listIter++) {
			cout << get<0>(*listIter) << " " << get<1>(*listIter) << " frequency = " << get<2>(*listIter) << " probability = " << get<3>(*listIter) << endl;
		}
	}
}

int main(int argc, char * argv[]) {
	if(argc <= 3) {
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
