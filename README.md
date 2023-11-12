# Project_4

## Usage
In order to run this program, the command line argument to be used is ```g++ -pthread -mavx -Wall -Wextra -g main.cpp```. When this format is used, the compiled code is automatically given the name ```a.out```. You start the program with this command, ```./a.out```. Once this is run, the program will first ask the user for the file name including the extension. If the file is opened correctly, the program will the user to type ```y``` for dictionary encoding and ```n``` for the baseline benchmarking. Afterwards, you are asked for the number of threads you would like to use to create the encoding. 

## Encoding
### Baseline
To be able to compare the encoding techniques, we first begin with the normal implementation of searching for data. In this implementation, each value was read into a ```list<std::string>``` data structure. To implement the query, the user is prompted for the word they would like to find, and the program serially goes through the entire ```list<std::string>```. As expected, this implementation is very slow. This specific dataset that was provided has "139999654" values that are not filtered. Since this implementation was a lazy solution, no attempt was made to optimize it. This means that in order to query a value, the program has to go through all of the data and find those values. As can be shown in the graph below, for each query, it took a relatively constant time to find the data. 

![https://github.com/danielle-den/Project_4/blob/main/Figures/Time(s)%20vs.%20Trial.png](https://github.com/danielle-den/Project_4/blob/main/Figures/Time(s)%20vs.%20Trial.png)

We notice an interesting trend on the graph for the first 3 points. As the number of occurences were increasing, the time it took to find them were also increasing. For the last data point however, the duration is lower than the previous values. To see what could have caused this, we looked at the length of the 4 values and we realized that for the first three, the length of the string increased up to the 3rd point. At the 4th point however, it was made up of only 4 letters. This seems to conclude that the size of the data being queried, as well as the number of occurences it has, influences the time it takes to find it. The values that were used for this experiment were, `bojt`, `oedtz`, `byasa`, and `ffrn`. 
