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

// Encode using the delta algorithm. No SIMD
void delta_encode(vector<int>& data, int start, int end) {
  if(start == 0) start = 1;
    for(; start < end; start++) {
        data[start] = data[start] - data[start - 1];
    }
}

// Delta encoding using SIMD
void delta_encode_simd(vector<int>& data, int start, int end){
  if(data.empty()) return; // Nothing to encode in an empty vector.

  // Use SSE2 intrinsics for SIMD processing
  __m128i prev;
  if(start == 0){
    prev = _mm_set1_epi32(data[0]);                     // The first index stays the same
    start = 1;
  }

  for(size_t i = start; i < size_t(end); i += 4){
    __m128i current = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&data[i]));
    __m128i delta = _mm_sub_epi32(current, prev);

    // Store the deltas back in the original data
    _mm_storeu_si128(reinterpret_cast<__m128i*>(&data[i]), delta);

    prev = current;
  }
}

// Delta decoding with SIMD
void delta_decode_simd(vector<int>& data) {
  if(data.empty()) return; // Nothing to decode in an empty vector.

  // Use SSE2 intrinsics for SIMD processing
  __m128i prev = _mm_set1_epi32(data[0]);

  for(size_t i = 1; i < data.size(); i += 4){
    __m128i delta = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&data[i]));
    __m128i current = _mm_add_epi32(prev, delta);

    // Store the current values back in the original data
    _mm_storeu_si128(reinterpret_cast<__m128i*>(&data[i]), current);

    prev = current;
  }
}

// Delta decoding without SIMD
void delta_decode(vector<int>& data) {
  if(data.empty()) return; // Nothing to decode in an empty vector.

  int prev = data[0]; // Initialize with the first element.

  for(size_t i = 1; i < data.size(); ++i){
    int delta = data[i];
    data[i] = prev + delta; // Decode the value.
    prev = data[i];
  }
}

// Single word querying
void query(const string& value, const map<string,int> &mapping, vector<int>& encoded_data, bool SIMD, bool prefix = false){
  if(not prefix){                     // If we're not doing prefix search
    auto find = mapping.find(value);
    if(find == mapping.end()){
      cout << "value does not exist" << endl;
      return;
    }
  }

  // Get the decode
  auto time_start = std::chrono::high_resolution_clock::now();
  if(SIMD && not prefix) delta_decode_simd(encoded_data);           // Only decode from this function if we are doing normal query
  if(not SIMD && not prefix) delta_decode(encoded_data);            // Use the regular decoding

  for(int i = 0; i < int(encoded_data.size()); i++){
    // if(encoded_data[i] == find->second) cout << i+1 << ", ";
  }

  cout << endl;
  auto time_end = std::chrono::high_resolution_clock::now();
  auto duration = chrono::duration_cast<chrono::milliseconds>(time_end - time_start);
  cout << endl;
  cout << "time: " << (duration).count() << endl;
}

// Prefix query
void query_prefix(const string& value, const map<string,int> &mapping, vector<int>& encoded_data, bool SIMD){
  // Find matches
  cout << "Beginning prefix query timer" << endl;
  auto time_start = std::chrono::high_resolution_clock::now();
  if(SIMD){
    cout << "SIMD" << endl;
    delta_decode_simd(encoded_data);
  }else{
    delta_decode(encoded_data);
  }

  // Go through mapping map to find possible matches
  for(auto i = mapping.begin(); i != mapping.end(); i++){
    if(i->first.substr(0, value.size()) == value) {         // Once we find a match
      cout << i->first << endl;
      query(i->first, mapping, encoded_data, true);         // Use the original query function to find the value
      cout << endl;
    }
  }

  auto time_end = std::chrono::high_resolution_clock::now();
  auto duration = chrono::duration_cast<chrono::milliseconds>(time_end - time_start);
  cout << endl;
  cout << "Prefix query ending. Duration: " << (duration).count() << endl;
}

// Export data
void export_file(const map<string,int>& mapping, vector<int>& encoded_data){
  ofstream encoded_file, mapping_file;

  encoded_file.open("encoded_data.txt");
  mapping_file.open("mapping.txt");

  for(auto i = mapping.begin(); i != mapping.end(); i++){
    mapping_file << i->first << " " << i->second << "\n";
  }

  for(auto i = encoded_data.begin(); i != encoded_data.end(); i++){
    encoded_file << *i << "\n" ;
  }

  encoded_file.close(); mapping_file.close();

  return;
}

// Creates the encoded and mapping structures
void setup(fstream &file, map<string, int>& mapping, vector<int> &encoded_data, bool SIMD){
  string read;
  list<thread> threads;
  vector<int> compressed, uncompressed;
  int numThreads = 2; int count = 0;

  cout << "Enter the number of threads to use for encoding, between 1 and " <<
  std::thread::hardware_concurrency() << ": ";
  cin >> numThreads;
  cout << "Reading data. Operation may take up to 5 minutes." << endl;

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

    if(not SIMD) threads.push_back(thread(delta_encode, ref(encoded_data), start, end));
    else threads.push_back(thread(delta_encode_simd, ref(encoded_data), start, end));
  }

  for(auto i = threads.begin(); i != threads.end(); i++){
    i->join();
  }
  auto time_end = std::chrono::high_resolution_clock::now();
  auto duration = chrono::duration_cast<chrono::seconds>(time_end - time_start);
  cout << endl;
  cout << "Threads stopped. Encoding duration: " << (duration).count() << endl;
}


int main(){
    string file_name, read;
    bool query_type = 0; bool SIMD = 0;
    map<string, int> mapping;
    list<string> data;    vector<int> encoded_data;

    cout << "\nfile name: ";
    cin >> file_name;

    fstream file(file_name);
    if(file.good()) {
      cout << "Enter y for dictionary encoding and n for regular: ";
      cin >> file_name;

      if(file_name == "y") {
        cout << "Enter 1 to use SIMD, other for no SIMD: ";
        cin >> SIMD;
        setup(file, mapping, encoded_data, SIMD);
        // export_file(mapping, encoded_data);
        while(true){
          cout << "Enter 1 for prefix query and 0 for regular query: ";
          cin >> query_type;
          cout << "enter value to search: ";
          cin >> file_name;

          if(not query_type) query(file_name,mapping, encoded_data, SIMD, query_type);    // Variable "file_name", is being reused as the variable to find out the query string
          else query_prefix(file_name, mapping, encoded_data, SIMD);
        }
      }
      else vanilla(file, data);
    }
    else cout << "Unable to open file. Exiting program." << endl;

    return 0;
}
