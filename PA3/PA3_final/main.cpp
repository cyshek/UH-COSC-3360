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
    pthread_mutex_t* bsem;
    pthread_cond_t* condition_var;
    int* turn;
    int j_var;
    vector<int> pos;   // vector of positions to store decoded character
    vector<char> *decoded;   // vector to store decoded characters
    huffmanTree* hTree;
    string b_code;
};

// Function that each thread will execute.
void* function (void* args) {
    threadData* x = (threadData*) args;
    vector <int> local_pos;
    local_pos = x->pos;
    int i = x->j_var;
    string binaryCode = x->b_code;
    
    while (i != *x->turn) {
        pthread_cond_wait(x->condition_var, x->bsem);
    }
    
    pthread_mutex_unlock(x->bsem);
    // Traverse and print the huffman codes for each symbol in the tree
    x->hTree->obtainCodes(x->hTree->getRoot(), binaryCode);
    // Decode the binary code to get the character
    char decoded_char = x->hTree->findCharacter(x->hTree->getRoot(), binaryCode);
    
    pthread_mutex_lock(x->bsem);
    *(x->turn) += 1;
    pthread_cond_broadcast(x->condition_var);
    pthread_mutex_unlock(x->bsem);
    
    // Update the message vector with the decoded character in the correct positions
    for (int i = 0; i < local_pos.size(); i++) {
        int pos = local_pos[i];
        (*x->decoded)[pos] = decoded_char;
    }
    
    return NULL;
}

int main () {
    // Get the number of symbols in the alphabet
    string temp;
    int freq = 0;
    int total_freq = 0;
    getline(cin, temp);
    freq = stoi(temp);
    string input_number;
    unordered_map<char, int> chars_and_freq;
    int iterations = freq;
    
    // Process the characters and their respective frequencies that will be added to the tree
    while(iterations > 0) {
        getline(cin, temp); 
        // Extract the frequency of each character and add the character and frequency to map
        input_number = temp.substr(2, temp.length() - 1);
        chars_and_freq[temp[0]] = stoi(input_number);
        total_freq += stoi(input_number);
        iterations--;
    }

    // Create a message vector (contains the final message) of size equal to the total frequency of characters.
    vector<char> message (total_freq);
    
    // Create a Huffman Tree from the characters and frequency data.
    huffmanTree h_tree(chars_and_freq);
    
    //Stores the processed binary codes and character positions
    vector<pair<string, vector<int> > > symbols;
    string line;
    int i = 0;
    
    //Process the binary codes and character positions
    while(getline(cin, line)) {
        // Initialize the variables to store the binary code and character positions.
        string binaryCode;
        vector<int> positions;
        int spaceIndex = line.find(" ");
        
        // Extract the binary code from the current line.
        binaryCode = line.substr(0, spaceIndex);
        line = line.substr(spaceIndex + 1);
        // cout << binaryCode << endl;
        
        // Extract positions of the binary code
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
        symbols.push_back(make_pair(binaryCode, positions));
        i++;
    }
    
    // Declare pthread array with freq elements
    pthread_t thread [freq];   
    static threadData thread_argument;
    
    // Declare and initialize mutex semaphore and conditional variable
    static pthread_cond_t condition_var = PTHREAD_COND_INITIALIZER;
    static pthread_mutex_t bsem;
    int turn = 0;
    pthread_mutex_init(&bsem, NULL);
    
    //Iterate through the symbols map and create a threadData object for each binary code.
    int j = 0;
    for (auto x : symbols) {
        pthread_mutex_lock(&bsem);
        // Fill member variables in the threadData object.
        thread_argument.pos = x.second;
        thread_argument.decoded = &message;
        thread_argument.hTree = &h_tree;
        thread_argument.b_code = x.first;
        thread_argument.condition_var = &condition_var;
        thread_argument.bsem = &bsem;
        thread_argument.turn = &turn;
        thread_argument.j_var = j;
        //Create each thread with a threadData object
        pthread_create(&thread[j], NULL, function, &thread_argument);
        j++;
    }
    
    //Wait for all of the threads to complete
    for (int i = 0; i < freq; i++) {
        pthread_join(thread[i], NULL);
    }
    
    cout << "Original message: ";
    
    //Print each character in the original message
    for (auto i: message) {
        cout << i;
    }
    
    cout << endl;
    return 0;
}