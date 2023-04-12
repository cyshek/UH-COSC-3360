#include <fstream>
#include <pthread.h>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <map>
#include "huffmanTree.h"

using namespace std;

// Struct to store thread data.
struct threadData {
    vector<int> pos;   // vector of positions to store decoded character
    vector<char> decoded;   // vector to store decoded characters
    char c;     // the character to be decoded
};

// Function that each thread will execute.
void* function (void* args) {
    threadData* x = (threadData*) args;
    
    // Decode each character using the position and char stored in the threadData struct.
    for (int i = 0; i < x->pos.size(); i++) {
       x->decoded[x->pos[i]] = x->c;
    }

    return NULL;
}

int main () {
    string first_file, compressed_file;
    string temp;
    cin >> first_file >> compressed_file;
    
    // Open first file to get the frequency of each character.
    ifstream input(first_file);
    string input_number;
    unordered_map<char, int> chars_and_freq;
    int freq = 0;
    
    while(getline(input, temp)) {
        // Extract the frequency of each character and add the character and frequency to map
        input_number = temp.substr(2, temp.length() - 1);
        freq += stoi(input_number);
        chars_and_freq[temp[0]] = stoi(input_number);
    }
    
    // Create a message vector (contains the final message) of size equal to the total frequency of characters.
    vector<char> message (freq);
    
    // Create a Huffman Tree from the characters and frequency data.
    huffmanTree h_tree(chars_and_freq);
    h_tree.printCodes(h_tree.getRoot(), "");
    
    // Open the compressed file to get the encoded data.
    ifstream inputFile(compressed_file); 
    map<string, vector<int>> symbols;

    string line;
    int n = 0;
    
    //Process the binary codes and character positions in the compressed file
    while(getline(inputFile, line)) {
        // Keep track of the number of lines in the file.
        n++;
        
        // Initialize the variables to store the binary code and character positions.
        string binaryCode;
        vector<int> positions;
        int spaceIndex = line.find(" ");
        
        // Extract the binary code from the current line.
        binaryCode = line.substr(0, spaceIndex);
        line = line.substr(spaceIndex + 1);
        
        // Extract positions of the binary code in the compressed file
        while(line.length() > 0) {
            spaceIndex = line.find(" ");
            // Check if the current line has more character positions.
            if(spaceIndex == string::npos) {
                spaceIndex = line.length();
            }
            int position = stoi(line.substr(0, spaceIndex));
            positions.push_back(position);
            // If there are more character positions, update the line string.
            if(spaceIndex != line.length())
                line = line.substr(spaceIndex + 1);
            else 
                line = "";
        }
        // Store the binary code and character positions in the symbols map.
        symbols[binaryCode] = positions;
    }
    
    inputFile.close();
    
    // Declare pthread array with n elements
    pthread_t* thread = new pthread_t [n];
    int j = 0;     
    
    // Create a vector to store the threadData objects
    vector<threadData*> thread_arguments;
    
    //Iterate through the symbols map and create a threadData object for each binary code.
    for (auto x : symbols) {
        // Create a new threadData object
        threadData* thread_arg = new threadData();
        
        // Fill member variables in the threadData object.
        thread_arg->c = h_tree.findCharacter(x.first);
        thread_arg->pos = x.second;
        thread_arg->decoded = message;
        
        //Create each thread with a threadData object
        pthread_create(&thread[j], NULL, function, thread_arg);
        thread_arguments.push_back(thread_arg);
        j++;
    }
    
    //Wait for all of the threads to complete
    for (int i = 0; i < n; i++) {
        pthread_join(thread[i], NULL);
    }
    
    // Update the message (contains the final message) vector with the decoded characters in the correct positions.
    for (auto arg : thread_arguments) {
        for (int i = 0; i < arg->pos.size(); i++) {
            message[arg->pos[i]] = arg->decoded[arg->pos[i]];
        }
    }
    
    cout << "Original message: ";
    
    //Print each character in the original message
    for (auto i: message) {
        cout << i;
    }
    
    cout << endl;
    
    return 0;
}

