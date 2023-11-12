#include <fstream>
#include <map>
#include <chrono>
#include <vector>
#include <immintrin.h>
#include <iostream>
#include <list>
#include <thread>
#include <pthread.h>

using namespace std;

// print vectors
void print(vector<int> &vec){
  for(uint i = 0; i < vec.size(); i++){
    cout << vec[i] << " ";
  }
  cout << endl;
}

// Regular encoding without integer compression
//Query
/*
 *  type => Decides if this is a prefix scan or a single item scan. Can assume value of 1 or 0
 *
/*/
void Query(int type, string prefix,map<string, int>& mapping, vector<int> &Vals, map<int,vector<int>> &OrderData){
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
    for(int i = 0; i < int(lookupVals.size()); i++){
      for(int j = 0; j < int(OrderData[lookupVals[i]].size()); j++){

        Vals.push_back(OrderData[lookupVals[i]][j]);
      }
    }


  }else{
    cerr << "ERR 401: Bad Data passed to Querey function \"type\" argument"<<endl;
  }
}

// Baseline
void vanilla(fstream &file, list<string> &data){
  string value, read;

  while(file >> read) data.push_back(read);             // Store column data

  while(true){
    cout << "Enter column data to search: ";
    cin >> value;

    list<string>::iterator itr = data.begin();
    auto time_start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < int(data.size()); i++, itr++){
      if(*itr == value) cout << i << ", ";
    }
    auto time_end = std::chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::seconds>(time_start - time_end);
    cout << endl;

    cout << "time: " << (duration).count() << endl;
  }
}

// Encode using the delta algorithm
void delta_encode(vector<int>& data, int start, int end) {
  if(start == 0) start = 1;
    for(; start < end; start++) {
        data[start] = data[start] - data[start - 1];
    }
}

// Decode using the delta algorithm
vector<int> delta_decode(const vector<int>& compressed) {
    vector<int> data;
    if(compressed.empty()) return data;

    data.push_back(compressed[0]); // First value remains the same
    for(int i = 1; i < int(compressed.size()); i++) {
        int original_value = data[i - 1] + compressed[i];
        data.push_back(original_value);
    }
    return data;
}

// Compress using the variable byte algorithm
vector<uint8_t> variable_byte_encode(int value) {
    std::vector<uint8_t> bytes;
    while (value >= 128) {
        bytes.push_back(static_cast<uint8_t>(value & 0x7F) | 0x80);
        value >>= 7;
    }
    bytes.push_back(static_cast<uint8_t>(value));
    return bytes;
}

// Uncompress using the variable_byte algorithm
inline int variable_byte_decode_simd(const std::vector<uint8_t>& bytes){
  __m128i result = _mm_setzero_si128();
  __m128i mask = _mm_set1_epi8(0x7F);

  int shift = 0;
  size_t i = 0;

  while(i < bytes.size()){
    __m128i byte_data = _mm_set1_epi8(bytes[i]);
    __m128i shift_data = _mm_slli_si128(byte_data, shift);
    result = _mm_or_si128(result, _mm_and_si128(shift_data, mask));
    shift += 7;
    i++;

    if((bytes[i - 1] & 0x80) == 0){
      break;
    }
  }

  int decoded_value;
  _mm_storeu_si128(reinterpret_cast<__m128i*>(&decoded_value), result);
  return decoded_value;
}

// Creates the encoded and mapping structures
void setup(fstream &file, map<string, int>& mapping, vector<int> &encoded_data){
  string read;
  list<thread> threads;
  vector<int> compressed, uncompressed;
  int numThreads = 2; int count = 0;
  char technique;

  cout << "Enter the number of threads to use for encoding, between 1 and " <<
  std::thread::hardware_concurrency() << ": ";
  cin >> numThreads;
  cout << "Enter the letter 'd' to use delta encoding, or the letter 'v' to use " <<
  "variable byte encoding: ";
  cin >> technique;

  while(file >> read){
    // Add the data without duplicates
    auto i = mapping.find(read);              // Check to see if we've already added this value
    if(i == mapping.end()) {                  // If we haven't, asign a temporary value as the codec
      mapping[read] = count;
      encoded_data.push_back(count++);
    }else{                                    // Otherwise, find the codec that was set for it when it was first found
      encoded_data.push_back(i->second);
    }
  }
  file.close();

  cout << "Starting threads" << endl;
  int chunkSize = int(encoded_data.size()) / numThreads;
  auto time_start = std::chrono::high_resolution_clock::now();
  for(int i = 0; i < numThreads; i++) {
    int start = i * chunkSize;
    int end = (i == numThreads - 1) ? int(mapping.size()) : (i + 1) * chunkSize;

    threads.push_back(thread(delta_encode, ref(encoded_data), start, end));
    // else threads.push_back(thread(variable_byte_encode, ref(encoded_data), start, end));
  }

  for(auto i = threads.begin(); i != threads.end(); i++){
    i->join();
  }
  auto time_end = std::chrono::high_resolution_clock::now();
  auto duration = (time_end - time_start);
  cout << endl;
  cout << "time: " << (duration).count() << endl;

  /*cout << "Threads done" << endl;

  // Further reduce
  cout << "starting encoding" << endl;
  auto time_start = std::chrono::high_resolution_clock::now();

  if(technique == 'd'){
    compressed = delta_encode(encoded_data);
    // uncompressed = delta_decode(compressed);
  }else if(technique == 'v'){
    vector<vector<uint8_t> >variable_encode;
    for(int i = 0; i < int(encoded_data.size()); i++){             // Encode the data
      variable_byte_encode(encoded_data[i]);
    }
  }


    // print(encoded_data); cout << endl;
    // print(compressed); cout << endl;
    // print(uncompressed);*/
}


int main(){
    string file_name, read;
    map<string, int> mapping;
    list<string> data;    vector<int> encoded_data;

    cout << "file name: ";
    cin >> file_name;

    fstream file(file_name);
    if(file.good()) {
      cout << "Enter y for dictionary encoding and n for regular: ";
      cin >> file_name;

      if(file_name == "y") setup(file, mapping, encoded_data);
      else vanilla(file, data);
    }
    else cout << "Unable to open file. Exiting program." << endl;


    return 0;
}
