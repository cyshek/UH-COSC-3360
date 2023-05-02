#include <queue>
#include <iostream>
#include "huffmanTree.h"
#include <unordered_map>
using namespace std;

// Function to traverse the Huffman Tree and print the Huffman Codes for each symbol
void huffmanTree::obtainCodes(Node* node, string code) {
    if (node) {
        for (int i = 0; i < code.length(); i++) {
            code[i] == '1' ?  node = node->right : node = node->left;
        }
        cout << "Symbol: " << node->data << ", " << "Frequency: " << node->freq << ", " << "Code: " << code << endl;
    }
}

// Function to find the symbol corresponding to a Huffman code
char huffmanTree::findCharacter(Node* node, string code) {
    if (node) {
        for (int i = 0; i < code.length(); i++) {
            code[i] == '1' ?  node = node->right : node = node->left;
        }        
    }
    return node->data;
}

// Function to return the root of the Huffman Tree
Node* huffmanTree::getRoot () {
    return root;
}

// Constructor to build the Huffman Tree
huffmanTree::huffmanTree(unordered_map<char, int> freq) {
    int positionCounter = 0;
    // Push each symbol and its frequency into the priority queue
    for (auto pair: freq) {
        pq.push(new Node(pair.first, pair.second, positionCounter++));
    }
    
    // Keep building the Huffman Tree until only one node is left in the priority queue
    while (pq.size() != 1) {
        // Pop two nodes with the smallest frequency
        Node* left = pq.top(); 
        pq.pop();
        Node* right = pq.top(); 
        pq.pop();
        // Create a new internal node with the frequency equal to the sum of the two nodes
        pq.push(new Node('\0', left->freq + right->freq, left, right, positionCounter++));
    }
    
    // The root of the Huffman Tree is the only node left in the priority queue
    root = pq.top();
}