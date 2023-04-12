#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include "huffmanTree.h"

using namespace std;

int main(int argc, char* argv[]) {
    // get the port number from command line argument
    int port_number = stoi(argv[1]);

    // initialize variables
    int number_of_connections = 0;
    string temp;
    string input_number;
    unordered_map<char, int> chars_and_freq;
    int freq = 0;
    
    // read input from standard input and build a map of character frequencies
    while(getline(cin, temp)) {
        // Extract the frequency of each character and add the character and frequency to map
        input_number = temp.substr(2, temp.length() - 1);
        freq += stoi(input_number);
        chars_and_freq[temp[0]] = stoi(input_number);
        number_of_connections++;
    }
    
    // Create a Huffman Tree from the characters and frequency data.
    huffmanTree h_tree(chars_and_freq);
    h_tree.printCodes(h_tree.getRoot(), "");
    
    // create a socket
    int sockfd;
    struct sockaddr_in serv_addr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    // set server address and port number
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port_number);

    // bind the socket to the server address and port number
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }
    
    // listen for incoming connections
    listen(sockfd, number_of_connections);
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int data;
    
    // Accept incoming connections
    for (int i = 0; i < number_of_connections; i++) {
        int client_socket = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len);

        // fork a child process to handle the client connection
        int pid = fork();
        if (pid == 0) { // child process
            // handle client connection
            char buffer [256];
            data = read(client_socket, &buffer, sizeof(buffer));
            
            // Decode the message using the Huffman tree
            string str(buffer);
            char decoded_char = h_tree.findCharacter(str);

            // Write the decoded character back to the client
            data = write(client_socket, &decoded_char, sizeof(char));
            
            // close the socket for this client and exit the child process
            close(client_socket);
            exit(0);
        }
    }
    
    // Wait for all child processes to exit
    for (int i = 0; i < number_of_connections; i++) {
        wait(nullptr);
    }

    close(sockfd);
    return 0;
}