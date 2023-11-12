# Project_4

## Usage
In order to run this program, the command line argument to be used is ```g++ -pthread -mavx -Wall -Wextra -g main.cpp```. When this format is used, the compiled code is automatically given the name ```a.out```. You start the program with this command, ```./a.out```. Once this is run, the program will first ask the user for the file name including the extension. If the file is opened correctly, the program will the user to type ```y``` for dictionary encoding and ```n``` for the baseline benchmarking. Afterwards, you are asked for the number of threads you would like to use to create the encoding. Due to the fact that the test data is 1GB in size, once you choose between running a baseline version or doing the dictionary encoding version, you are required to operate in that mode throughout the program.

## Encoding
### Baseline
To be able to compare the encoding techniques, we first begin with the normal implementation of searching for data. In this implementation, each value was read into a ```list<std::string>``` data structure. To implement the query, the user is prompted for the word they would like to find, and the program serially goes through the entire ```list<std::string>```. As expected, this implementation is very slow. This specific dataset that was provided has "139999654" values that are not filtered. Since this implementation was a lazy solution, no attempt was made to optimize it. This means that in order to query a value, the program has to go through all of the data and find those values. As can be shown in the graph below, for each query, it took a relatively constant time to find the data. 

![https://github.com/danielle-den/Project_4/blob/main/Figures/Time(s)%20vs.%20Trial.png](https://github.com/danielle-den/Project_4/blob/main/Figures/Time(s)%20vs.%20Trial.png)

We notice an interesting trend on the graph for the first 3 points. As the number of occurences were increasing, the time it took to find them were also increasing. For the last data point however, the duration is lower than the previous values. To see what could have caused this, we looked at the length of the 4 values and we realized that for the first three, the length of the string increased up to the 3rd point. At the 4th point however, it was made up of only 4 letters. This seems to conclude that the size of the data being queried, as well as the number of occurences it has, influences the time it takes to find it. The values that were used for this experiment were, `bojt`, `oedtz`, `byasa`, and `ffrn`. 

### Thread
The obvious way of using `multi-threading` in this experiment was in conjunction with the integer compressions. The user specifies how many threads they want to use, and the program runs a separate instance of the `delta` compression algorithm for each thread. The `delta` encoding algorithm was tested using `2, 4, 6, 8, 10, 12` number of threads since 12 was the maximum for the device we were using. The expectation were the same as the one that was seen in Project 2, where having lots of threads were good, leading to shorter overally computational time. On the other hand, having too many threads can hurt performance. As can be seen from the graph below, the time it took for the encoding to be done started off high. With the increase of the number of threads however, the overall time decreased. 
                            ![stuff](https://github.com/danielle-den/Project_4/blob/main/Figures/Encoding%20Time(micro)%20vs.%20Thread%20%23.png)
                            
### Integer Compression - Delta Encoding
For the encoding section, we had a choice between two integer compression techniques: `delta compression`, and `variable-byte compression`. While the `variable-byte` algorithm is much faster, it can be a bit more difficult to implement. The `delta` one though works more for sorted data. Our encoded is luckily sorted, since we assign initial values based on the data that is read first. When testing out the delta encoding, we had the option between using it with SIMD and using it without SIMD. For this part of the test, we redid the test performed for the thread but only for the SIMD encode. We expected to get even better values than when we did the thread experiment without the SIMD. The first graph below is the SIMD experiment by itself while the second is in comparison. 

|   SIMD   | SIMD vs Normal |
|------|------|
| ![](https://github.com/danielle-den/Project_4/blob/main/Figures/SIMD.png)| ![](https://github.com/danielle-den/Project_4/blob/main/Figures/No_SIMD.png)|

As can be seen, the difference in performance was immense. Although the performance of the algorithms depend on the programmer, we saw that with our implementation there was a huge difference in performance. As the number of threads increased, the time it took for the encoding to finish running also decreased. Unlike the experiment done on the thread along, the duration of this experiment kept decreasing and never went up.


# Query
Performing the query was a bit difficult given the constraints. 


