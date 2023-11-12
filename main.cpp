#include <fstream>
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <list>
#include <thread>
#include <pthread.h>
#include <bits/stdc++.h>

using namespace std;

//Querey
/*
 *  type => Decides if this is a prefix scan or a single item scan. Can assume value of 1 or 0
 * 
/*/
void Querey(int type, string prefix,map<string, int>& mapping, vector<int> &Vals, map<int,vector<int>> &OrderData){
  map<string, int>::iterator itr_mapping;
  vector<int> lookupVals;
  Vals.clear();

//Process data if valid request
  if(type == 1 || type == 0){
    if(type == 1){
      //get the value associated with the input word
      itr_mapping = mapping.find(prefix);
      if(itr_mapping != mapping.end()){
        lookupVals.push_back(mapping[prefix]);
      }

    }else if (type == 0){
      //based on given prefix find indexes of entries that have that prefix
      for(itr_mapping = mapping.begin(); itr_mapping != mapping.end(); itr_mapping++){
        if((*itr_mapping).first.find(prefix) == 0){
          lookupVals.push_back((*itr_mapping).second);
        }
      }
    }

    // Find the indicies for all of the requests that were made
    for(int i = 0; i < lookupVals.size(); i++){
      for(int j = 0; j < OrderData[lookupVals[i]].size(); j++){

        Vals.push_back(OrderData[lookupVals[i]][j]);
      }
    }


  }else{
    cerr << "ERR 401: Bad Data passed to Querey function \"type\" argument"<<endl;
  }
}


//check if 1 item exists in column
  // if so return all inidcies of all matching entries in the column

//Given a prefix  find 7 return all unique matching data and their indicies
  // must supposrt use of SIMD instructionsto speed up seach/ scan


//
void encode(map<string, int>& mapping, int en, int end){
  // Get the mapping and store it in the library
  int count = 0;
  for(auto i = mapping.begin(); count < int(mapping.size()); i++, count++){ // i is a pointer to the beginning of the mapped structure
    if(count < end and count >= en){                                        // For each thread, only operate on the section of data dedicated for you
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
  map<int,list<int>>::iterator itr_map;
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

  threads.push_back(thread());



  // Set the encoded_data properly
  for(auto i = data.begin(); i != data.end(); i++){
    encoded_data.push_back(mapping[*i]);
    // OrderData.insert();
  }
}


void setOrder(map<int,vector<int>> &OrderData, list<int> &encoded_data,map<string, int>& mapping){
  map<int,vector<int>>::iterator itr_map;
  list<int>::iterator itr;
  int idx = 0;

  for(itr = encoded_data.begin(); itr != encoded_data.end(); itr++){
    itr_map = OrderData.find(*itr); //check if number in Ordered map

    if(itr_map != OrderData.end()){          // same occurance of number
      (itr_map -> second).push_back(idx);
    }else{
      OrderData.insert(make_pair(*itr, idx));      // new ocurance of number
    }
    idx++;
  }

}



int main(){
    string quit_CMD = "";
    string option1 = "";
    string file_name, read;
    map<string, int> mapping;
    map<int,vector<int>> OrderData;
    vector<int> Values;
    list<string> data;                       
    list<int> encoded_data;

    cout << "file name: ";
    cin >> file_name;

    fstream file(file_name);
    if(file.good()){

      setup(file, mapping, data, encoded_data);
      setOrder(OrderData,encoded_data,mapping);

    }
    
    else 
      cout << "Unable to open file. Exiting program." << endl;

    // list<int>::iterator itr;
    // for(itr = encoded_data.begin(); itr != encoded_data.end(); itr++){
    //   cout << *itr << endl;
    // }
    

    while(quit_CMD != "quit"){
      cout << "Would you like to quit? If so type \"quit\" below type \"no\" otherwise" << endl;
      cin >> quit_CMD;

      transform(quit_CMD.begin(),quit_CMD.end(),quit_CMD.begin(), ::tolower);

      if(quit_CMD != "quit"){
        cout << "What type of search would you like to do? 1 = single 0 = prefix" << endl;
        cin >> option1;

        if(option1.length() != 1){
          cerr << "A digit was not input to the program pls try again" << endl;

        }else{
          if( isdigit(option1[0]) && stoi(option1) == 1 ){
          // Do single search
          string word = "";
          cout << "Please enter a Word to search for" << endl;
          cin >> word;

          Querey(stoi(option1),word ,mapping,Values,OrderData);

          }else if (isdigit(option1[0])){
            // Hand to query to do search specified or send ERR messsage

            string prefix1 = "";
            cout << "Please enter a prefix" << endl;
            cin >> prefix1;

            Querey(stoi(option1), prefix1,mapping,Values,OrderData);
          }else{
            break;
          }

          for(int i = 0; i < Values.size(); i ++){
            cout << Values[i] << ", ";
          }
          cout<<endl;
        }
      }
    }

  return 0;
}