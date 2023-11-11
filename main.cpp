#include <fstream>
#include <map>
#include <iostream>
#include <list>
#include <pthread.h>
#include <istream>

using namespace std;

// Creates the encoded and mapping structures
void setup(fstream &file, map<string, int>& mapping, list<string> &data, list<int> &encoded_data){
  string read;
  int en = 1;

  while(file >> read){
    if(mapping.find(read) == mapping.end()){    // Check to see if we have already read this word
      mapping[read] = 0;
    }

    data.push_back(read);
  }
  file.close();
  // Get the mapping and store it in the library
  for(auto i = mapping.begin(); i != mapping.end(); i++, en++){
    i->second+= en;
    cout << i->first << " " << i->second << endl;
  }

  // Set the encoded_data properly
  for(auto i = data.begin(); i != data.end(); i++){
    encoded_data.push_back(mapping[*i]);
  }
}


int main(){
    string file_name, read;
    map<string, int> mapping;
    list<string> data;
    list<int> encoded_data;

    cout << "file name: ";
    cin >> file_name;

    fstream file(file_name);
    if(file.good()) {
        cout << "file is good" << endl;
        setup(file, mapping, data, encoded_data);
    }
    else cout << "Unable to open file. Exiting program." << endl;


    return 0;
}
