# Project_4

## Usage
In order to run this program, the command line argument to be used is ```g++ -pthread -mavx -Wall -Wextra -g main.cpp```. When this format is used, the compiled code is automatically given the name ```a.out```. You start the program with this command, ```./a.out```. Once this is run, the program will first ask the user for the file name including the extension. If the file is opened correctly, the program will the user to type ```y``` for dictionary encoding and ```n``` for the baseline benchmarking. Afterwards, you are asked for the number of threads you would like to use to create the encoding. Due to the fact that the test data is 1GB in size, once you choose between running a baseline version or doing the dictionary encoding version, you are required to operate in that mode throughout the program.

## Encoding
### Baseline
To be able to compare the encoding techniques, we first begin with the normal implementation of searching for data. In this implementation, each value was read into a ```list<std::string>``` data structure. To implement the query, the user is prompted for the word they would like to find, and the program serially goes through the entire ```list<std::string>```. As expected, this implementation is very slow. This specific dataset that was provided has "139999654" values that are not filtered. Since this implementation was a lazy solution, no attempt was made to optimize it. This means that in order to query a value, the program has to go through all of the data and find those values. As can be shown in the graph below, for each query, it took a relatively constant time to find the data. 

![https://github.com/danielle-den/Project_4/blob/main/Figures/Time(s)%20vs.%20Trial.png](https://github.com/danielle-den/Project_4/blob/main/Figures/Time(s)%20vs.%20Trial.png)

We notice an interesting trend on the graph for the first 3 points. As the number of occurences were increasing, the time it took to find them were also increasing. For the last data point however, the duration is lower than the previous values. To see what could have caused this, we looked at the length of the 4 values and we realized that for the first three, the length of the string increased up to the 3rd point. At the 4th point however, it was made up of only 4 letters. This seems to conclude that the size of the data being queried, as well as the number of occurences it has, influences the time it takes to find it. The values that were used for this experiment were, `bojt`, `oedtz`, `byasa`, and `ffrn`. 

### Thread
The obvious way of using `multi-threading` in this experiment was in conjunction with the integer compressions. The user specifies how many threads they want to use, and the program runs a separate instance of the `delta` compression algorithm for each thread. The `delta` encoding algorithm was tested using `2, 4, 6, 8, 10, 12` number of threads since 12 was the maximum for the device we were using. The expectation were the same as the one that was seen in Project 2, where having lots of threads were good, leading to shorter overally computational time. On the other hand, having too many threads can hurt performance. As can be seen from the graph below, the time it took for the encoding to be done started off high. With the increase of the number of threads however, the overall time decreased. The values on the y axis are time in the microseconds. THey ranged from 15 seconds to 25 seconds, which is to say that it took a while for the operation to be performed.
                            ![stuff](https://github.com/danielle-den/Project_4/blob/main/Figures/Encoding%20Time(micro)%20vs.%20Thread%20%23.png)
                            
### Integer Compression - Delta Encoding
For the encoding section, we had a choice between two integer compression techniques: `delta compression`, and `variable-byte compression`. While the `variable-byte` algorithm is much faster, it can be a bit more difficult to implement. The `delta` one though works more for sorted data. Our encoded is luckily sorted, since we assign initial values based on the data that is read first. When testing out the delta encoding, we had the option between using it with SIMD and using it without SIMD. For this part of the test, we redid the test performed for the thread but only for the SIMD encode. We expected to get even better values than when we did the thread experiment without the SIMD. The first graph below is the SIMD experiment by itself while the second is in comparison. 

|   SIMD   | No SIMD vs SIMD |
|------|------|
| ![](https://github.com/danielle-den/Project_4/blob/main/Figures/SIMD.png)| ![](https://github.com/danielle-den/Project_4/blob/main/Figures/No_SIMD.png)|

As can be seen, the difference in performance was immense. Although the performance of the algorithms depend on the programmer, we saw that with our implementation there was a huge difference in performance. As the number of threads increased, the time it took for the encoding to finish running also decreased. Unlike the experiment done on the thread along, the duration of this experiment kept decreasing and never went up.


## Query
#### Single Data Query with SIMD
Quering the data was a bit inefficient. The reason for this is because the encoded table and the mapping table are not storing the same values. The encoded table stores the integer compressed version, while the mapping table stores the original. Therefore in order to look for any values, the program must first decode the encoded table first. The good thing is that we only need to encode and decode once. When the program starts for the first time, it uses the integer compression algorithm (`delta encoding`) in an attempt to reduce the footprint. When we need to find a value, it decodes the entire table in order to use it. For this experiment, we will only test the query function, including the decode function. 

|Word  | Encoding Time (s)| Count|
|------|------------------|------|
|ajzt  |2|473|
|jahkb |2|165|
|prwjiy|2|71|
|synkfiaoyj|2|113|
|mikvozuhba|2|110|

Looking at the data above, the interesting phenomenom that we saw was that the search time was 2 seconds when looking for the data. 2 threads were used for this one. When we compare this with the baseline implementation, we can see that this algorithm does not seem to depend on the size of the letter, nor does it depend on the number of times the word appears in the file. To be certain, we reran the test and measured the time in milliseconds to see how different the results were between the words. As can be seen below, the duration in milliseconds may be different, but to the average user there is no difference.

|Word  | Encoding Time (ms)| Count|
|------|------------------|------|
|ajzt  |2282|473|
|jahkb |2248|165|
|prwjiy|2429|71|
|synkfiaoyj|2263|113|
|mikvozuhba|2256|110|

The current implementation of Query only uses SIMD and does not depend on thread. If we were to modify it to depend on threads however, there are 2 behaviors that we would expect. The first behavior is that the performance would increase just like what we saw from the previous experiment as well as from Project 2. The other behavior might actually be no noticeable difference. The functions used in our program uses pointers to the encoded data vector and other data structures. When threads are used, it's possible that the operating system might need to lock the data to one thread at a time, thereby reducing the potential performance advantage. On the other hand, what we can see is that the encode and decode functions work well when using threads. This seems to point to the conclusion that the a Query using threads would also increase performance.

#### Single Data without SIMD


|Word  | Encoding Time (ms)| Count|
|------|------------------|------|
|aj  |2282|473|
|jah |2248|165|
|prwj|2429|71|
|s|2263|113|
|mikvoz|2256|110|

|Benchmarking Query

### Prefix Query
Due to the fact that this dataset has hundreds of thousands of values and therefore performing a query search on all of them as well as finding their indices will not only take a long time but will also be taxing on our device, we decided to limit our test to simply looking for the matching words using each of the words and printing out the word itself. The program however, does work when it is queried for this function. Moreover, it truly prints out the matched words as well as the indices in the original file. The pictures below are example outputs showing the time it took for the program to run when we limit the output to time only. We took the value `pikgyaqet` and took the substring, `pikgyaqe` and put it through the algorithm.

|Benchmarking Prefix Query|Prefix Query with SIMD|Prefix Query without SIMD|
|-------------------------|----------------------|-------------------------|
|          10 mins          |                >20 mins  |           >20 mins         |


## Conclusion
This project explores the results of compression on RAM and SSD usage. In this experiment, we use `multithreading` and `SIMD` to implement dictionary encoding to reduce the size of a file. For our implementation, we were unable to use any prebuilt library to help us with using the `delta` encoding technique, so we had to do the heavy lifiting from scratch. The way we utilized multithreading was that when the input file had already been read into the `mapping` map and the `encoded_data` vector, we used the `delta` encoding technique in an effort to further reduce the amount of space being used.

Our program asks the user for the number of threads they want to use, and we would perform the `delta` encoding on the `encoded_data` vector by spliting it up between the different threads specified by the user. We saw that by increasing the number of threads, the performance of the encoding increased. Moreover, when we utilized SIMD in conjunction with multithreading, we saw that the basic queries were insanely fast, with the program able to find queried data within 2 seconds. 

The trivial part of the experiment was the `prefix` query. C++ maps do not have a function where you are able to look for values using prefixes, so we had to step through the entire map. After finding a match, we then input the value into the original `query` function, thereby restarting the query function all over again. Unfortunately, it has to redo the `query` for every match that it finds which is pretty slow. Moreover, our current implementation does not actually benefit from using the SIMD operations since we only use it for encoding. In our `main.cpp`, we have 2 query functions. The very first one is much better, but it requires additional arrays to be made, which takes up more space. 

Additionally, our initial implementation had an additional vector that stored the entire file. We used it later on to be able to have a `mapping` map that mirrored the one from lecture (Part 5 Sotware Data Storage). Unfortunately our computer would crash seeing as there was too much data being used by our program, for which reason we resorted to this approach. The overral data usage, while better, was not what we expected. The original file is 1.04 GB in size, while our `encoded_data` file is 939MB and the `mapping` file is 13MB. Regardless of the low turnout, the reduction in size is still much better. At least for our program the prefix implementation was not the best, there were certainly much better ways it could have been implemented. We can see if being used in the device we're using right now. When you look for a file, you only have to type the first couple of letters and it can find it for you as well as similar files. Had we implemeneted it better, we would have realized a very good use of the dictionary encoding.

### File Sizes
|Encoded.txt |Mapping.txt | Column.txt|
|-|-|-|
|![](/Figures/mapping.png) |![](/Figures/encoded.png) | ![](/Figures/Column.png)|
