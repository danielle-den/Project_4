#include <fstream>
#include <map>
#include <chrono>
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
      // cout << i->first << " " << i->second << endl;
    }
  }

  return;
}


void vanilla(fstream &file, list<string> &data){
  string value;

  while(file >> read) data.push_back(read);             // Store column data

  cout << "Enter column data to search: ";
  cin >> value;

  list<string>::iterator itr = data.begin();
  for(int i = 0; i < int(data.size()); i++, itr++){
    if(*itr == value) cout << i << ", ";
  }
}


// Creates the encoded and mapping structures
void setup(fstream &file, map<string, int>& mapping, list<string> &data, list<int> &encoded_data){
  string read;
  vector<thread> threads;
  int en = 0; int num_threads = 2;

  while(file >> read){
    if(mapping.find(read) == mapping.end()){    // Add unique words from column
      mapping[read] = 0;
    }

    data.push_back(read);
  }
  cout << "here" << endl;
  file.close();

  threads.push_back(thread(encode, ref(mapping), en, int(mapping.size()/2)));                       // First thread goes from beginning to middle
  threads.push_back(thread(encode, ref(mapping), mapping.size()/2, int(mapping.size())));          // Second thread goes from middle to end
  threads[0].join(); threads[1].join();


  // Set the encoded_data properly
  for(auto i = data.begin(); i != data.end(); i++){
    encoded_data.push_back(mapping[*i]);
  }
}


int main(){
    string file_name, read;
    map<string, int> mapping;
    list<string> data;    list<int> encoded_data;

    cout << "file name: ";
    cin >> file_name;

    fstream file(file_name);
    if(file.good()) {
      cout << "Enter y for dictionary encoding and n for regular: ";
      cin >> file_name;

      if(file_name == "y") setup(file, mapping, data, encoded_data);
      else vanilla();
    }
    else cout << "Unable to open file. Exiting program." << endl;


    return 0;
}
