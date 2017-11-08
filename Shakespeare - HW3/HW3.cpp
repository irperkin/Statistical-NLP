#include <iostream>
#include <fstream>
#include <unordered_map>
#include <tuple>
#include <list>
#include <math.h>

using namespace std;
#define BigramList list<tuple<string, string, int, long double> >
#define BigramMap unordered_map<string, BigramList>
#define UnigramMap unordered_map<string, int>

// Using c++11
// TO COMPILE: g++ -std=c++11 HW3.cpp

void mapInit(string inputFile, BigramMap& bigramMap, UnigramMap& unigramMap) {
	ifstream iStream(inputFile.c_str());
	string firstWord, secondWord;
	BigramMap::iterator bigramIter;
	BigramList::iterator listIter;
	tuple<string, string, int, long double> tup; // tuple<firstWord, secondWord, frequency/count, probability>

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

void normalizeByUnigram(BigramMap& bm, UnigramMap& um) {
	for(BigramMap::iterator bIter = bm.begin(); bIter != bm.end(); bIter++) {
		for(BigramList::iterator lIter = bIter->second.begin(); lIter != bIter->second.end(); lIter++) {
			get<3>(*lIter) = (long double)get<2>(*lIter) / um[bIter->first];
		}
	}
}

void logProbability(BigramMap bm, list<string> words) {
	BigramMap::iterator bIter;
	long double answer = 0.0;
	for(size_t i = 0; i < words.size() - 1; i++) {
		bIter = bm.find(words.front());
		words.pop_front();
		if(bIter != bm.end()) {
			for(auto lIter = bIter->second.begin(); lIter != bIter->second.end(); lIter++) {
				if(get<1>(*lIter) == words.front()) {
					answer += (long double)log10(get<3>(*lIter));
					break;
				}
			}
		}
	}
	cout << "log probability = " << (long double)answer << endl;
}

void shannonVisualization(BigramMap bm, string fw, string sw) {

}

void printBigramMap(BigramMap bm) {
	for(BigramMap::const_iterator bigramIter = bm.begin(); bigramIter != bm.end(); bigramIter++) {
		for(BigramList::const_iterator listIter = bigramIter->second.begin(); listIter != bigramIter->second.end(); listIter++) {
			cout << get<0>(*listIter) << " " << get<1>(*listIter) << " frequency = " << get<2>(*listIter) << " probability = " << get<3>(*listIter) << endl;
		}
	}
}

int main(int argc, char * argv[]) {
	if(argc < 4) {
		cout << "USAGE: ./a.out dir/to/file.txt sentence" << endl;
		cout << "OR" << endl << "USAGE: ./a.out dir/to/file.txt firstWord secondWord" << endl;
		return 1;
	}

	BigramMap bigramMap;
	UnigramMap unigramMap;
	string inputFile = string(argv[1]), firstWord, secondWord;
	list<string> words;

	mapInit(inputFile, bigramMap, unigramMap);
	normalizeByUnigram(bigramMap, unigramMap);
	// printBigramMap(bigramMap);

	if(argc == 4) {
		firstWord = argv[2];
		secondWord = argv[3];
		shannonVisualization(bigramMap, firstWord, secondWord);
	} else {
		for(int i = 2; i < argc; i++) { words.push_back(argv[i]); }
		logProbability(bigramMap, words);
	}

	return 0;
}
