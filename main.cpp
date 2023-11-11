#include <fstream>
#include <iostream>
#include <list>
#include <unordered_map>
#include <string>

using namespace std;

// Creates the encoded and mapping structures
void setup(fstream &file, unordered_map<string, int>& wordToIDMap, list<string> &wordList, list<int> &encodedData) {
    string word;
    int id = 1;

    while (file >> word) {
        if (wordToIDMap.find(word) == wordToIDMap.end()) {
            wordToIDMap[word] = id;
            id++;
        }
        wordList.push_back(word);
    }
    file.close();

    // Display the word-to-ID mapping
    for (const auto &pair : wordToIDMap) {
        cout << pair.first << " " << pair.second << endl;
    }

    // Set the encodedData properly
    for (const string &word : wordList) {
        encodedData.push_back(wordToIDMap[word]);
    }
}

int main() {
    string file_name;
    unordered_map<string, int> wordToIDMap;
    list<string> wordList;
    list<int> encodedData;

    cout << "file name: ";
    cin >> file_name;

    fstream file(file_name);
    if (file.good()) {
        setup(file, wordToIDMap, wordList, encodedData);
    } else {
        cout << "Unable to open file. Exiting program." << endl;
    }

    return 0;
}
