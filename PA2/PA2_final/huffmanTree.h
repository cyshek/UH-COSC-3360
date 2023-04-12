#include <unordered_map>
#include <queue>
#include <iostream>

using namespace std;

// A struct that represents a node in a Huffman tree.
struct Node {
    char data;   // The character stored in this node.
    int freq;    // The frequency of this character in the original text.
    Node* left;  
    Node* right; 
    int nodePos;
    
    // Constructor for an internal node.
    Node(char data, int freq, Node* l, Node* r, int count) : data(data), freq(freq), left(l), right(r), nodePos(count){}
    
    // Constructor for a leaf node.
    Node(char data, int freq, int count) : data(data), freq(freq), left(nullptr), right(nullptr), nodePos(count){}
};

// A struct for comparing the priority of two nodes in the priority queue.
struct Compare {
    bool operator()(Node* l, Node* r) {
        if (l->freq == r->freq) {
            // If the frequencies are equal, compare the characters.
            if (l->data == r->data) {
                // If the characters are equal, compare by ASCII.
                return l->nodePos < r->nodePos;
            }
            return l->data > r->data;
        }
        return l->freq > r->freq;
    }
};

// Class that represents a Huffman tree.
class huffmanTree {
    private: 
        Node* root; // Pointer to the root node of the Huffman tree.
        unordered_map<char, string> huffman_Codes; // Maps characters to their Huffman codes.
        priority_queue<Node*, vector<Node*>, Compare> pq; // A priority queue for constructing the tree.
    public:
        // Constructor that builds the Huffman tree from the input frequency map.
        huffmanTree(unordered_map<char, int>);
        // Returns the root node of the Huffman tree.
        Node* getRoot();
        // Recursive function that generates the Huffman codes.
        void printCodes(Node*, string);
        // Finds the character corresponding to a given Huffman code.
        char findCharacter (string);
};