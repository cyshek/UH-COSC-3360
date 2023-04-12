#include <unordered_map>
#include <queue>
#include <iostream>
#include "huffmanTree.h"
using namespace std;

// Function to traverse the Huffman Tree and store the Huffman Codes for each symbol
void huffmanTree::printCodes(Node* node, string code) {
    if (node) {
        // Check if the node is a leaf node (contains symbol)
        if (node->data != '\0') {
            cout << "Symbol: " << node->data << ", " << "Frequency: " << node->freq << ", " << "Code: " << code << endl;
            huffman_Codes[node->data] = code;
        }
        // Recursively traverse the left and right subtree
        printCodes(node->left, code + "0");
        printCodes(node->right, code + "1");    
    }
}

// Function to find the symbol corresponding to a Huffman code
char huffmanTree::findCharacter(string code) {
    for (auto x : huffman_Codes) {
        if (x.second == code) {
            // Return the symbol corresponding to the code
            return x.first;
        }
    }
    
    return '0';
}

// Function to return the root of the Huffman Tree
Node* huffmanTree::getRoot () {
    return root;
}

// Constructor to build the Huffman Tree
huffmanTree::huffmanTree(unordered_map<char, int> freq) {
    // Push each symbol and its frequency into the priority queue
    for (auto pair: freq) {
        pq.push(new Node(pair.first, pair.second));
    }
    
    // Keep building the Huffman Tree until only one node is left in the priority queue
    while (pq.size() != 1) {
        // Pop two nodes with the smallest frequency
        Node* left = pq.top(); 
        pq.pop();
        Node* right = pq.top(); 
        pq.pop();
        // Create a new internal node with the frequency equal to the sum of the two nodes
        pq.push(new Node('\0', left->freq + right->freq, left, right));
    }
    
    // The root of the Huffman Tree is the only node left in the priority queue
    root = pq.top();
}
