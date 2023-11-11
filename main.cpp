#include <fstream>
#include <map>
#include <vector>
#include <iostream>
#include <list>
#include <thread>
#include <pthread.h>

using namespace std;

//
void encode(map<string, int>& mapping, int en, int end){
  // Get the mapping and store it in the library
  int count = 0;
  for(auto i = mapping.begin(); count < int(mapping.size()); i++, count++){
    if(count < end and count >= en){         // For each thread, only operate on the section of data dedicated for you
      i->second += ++en;
      cout << i->first << " " << i->second << endl;
    }
  }

  return;
}

// Creates the encoded and mapping structures
void setup(fstream &file, map<string, int>& mapping, list<string> &data, list<int> &encoded_data){
  string read;
  vector<thread> threads;
  int en = 0;

  while(file >> read){
    if(mapping.find(read) == mapping.end()){    // Check to see if we have already read this word
      mapping[read] = 0;
    }

    data.push_back(read);
  }
  file.close();

  threads.push_back(thread(encode, ref(mapping), en, int(mapping.size()/2)));                       // First thread goes from beginning to middle
  threads.push_back(thread(encode, ref(mapping), mapping.size()/2, int(mapping.size())));          // Second thread goes from middle to end
  threads[0].join(); threads[1].join()  ;


  // Set the encoded_data properly
  for(auto i = data.begin(); i != data.end(); i++){
    encoded_data.push_back(mapping[*i]);
  }
}


int main(){
    string file_name, read;
    map<string, int> mapping;
    list<string> data;                       list<int> encoded_data;

    cout << "file name: ";
    cin >> file_name;

    fstream file(file_name);
    if(file.good()) setup(file, mapping, data, encoded_data);
    else cout << "Unable to open file. Exiting program." << endl;


    return 0;
}
