#include <pthread.h>    
#include <iostream>    
#include <vector>    
#include <string>      
#include <map>          
#include <cstdlib>    
#include <unistd.h>    
#include <stdio.h>      
#include <string.h>     
#include <stdlib.h>    
#include <sys/types.h>  
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h>      

using namespace std;

// Struct to store thread data.
struct threadData {
    string host_name;           
    int port_num;               
    vector<int> pos;            // vector of positions to store decoded character
    vector<char> decoded;       // vector to store decoded characters
    string code;                // binary code
};

// Function that each thread will execute.
void* function(void* args) {
    // character to store decoded value
    char decoded_char;                  
    threadData* x = (threadData*) args; 
    int sockfd, portno, n;              
    struct sockaddr_in serv_addr;      
    struct hostent *server;             

    // Initialize the socket and create a socket file descriptor
    portno = x->port_num;                       
    sockfd = socket(AF_INET, SOCK_STREAM, 0);   
    if (sockfd < 0)                            
        perror("ERROR opening socket");
    
    server = gethostbyname(x->host_name.c_str()); 
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    
    //store the address of the server that the client is connecting to
    bzero((char *) &serv_addr, sizeof(serv_addr));   
    serv_addr.sin_family = AF_INET;                   
    bcopy((char *)server->h_addr,                      
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);                
    
    // Connect to the server
    if (connect(sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)  
        perror("ERROR connecting");
    
    // Send the binary code to the server
    n = write(sockfd, x->code.c_str(), x->code.length() + 1);  
    
    // Read the decoded character from the server
    n = read(sockfd, &decoded_char, sizeof(char));    
    
    // Decode each character using the decoded_char and the correct position.
    for (int i = 0; i < x->pos.size(); i++) {
        x->decoded[x->pos[i]] = decoded_char;        
    }

    close(sockfd);     
    return NULL;
}

int main (int argc, char* argv[]) {
    string hostname = argv[1];
    int port_number = atoi(argv[2]);
    
    //Store binary codes and the position of where each character belongs in the string
    map<string, vector<int>> symbols;
    
    string line;
    int n = 0;
    
    int message_size = 0;
    
    //Process the binary codes and character positions
    while(getline(cin, line)) {
        // Keep track of the number of lines in the file.
        n++;
        // Initialize the variables to store the binary code and character positions.
        string binaryCode;
        vector<int> positions;
        int spaceIndex = line.find(" ");
        
        // Extract the binary code from the current line.
        binaryCode = line.substr(0, spaceIndex);
        line = line.substr(spaceIndex + 1);
        
        // Extract positions of the binary code
        while(line.length() > 0) {
            spaceIndex = line.find(" ");
            // Check if the current line has more character positions.
            if(spaceIndex == string::npos) {
                spaceIndex = line.length();
            }
            int position = stoi(line.substr(0, spaceIndex));
            positions.push_back(position);
            message_size++;
            // If there are more character positions, update the line string.
            if(spaceIndex != line.length())
                line = line.substr(spaceIndex + 1);
            else 
                line = "";
        }
        // Store the binary code and character positions in the symbols map.
        symbols[binaryCode] = positions;
    }
    
    // Store the final message 
    vector<char> message (message_size);
    
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
        thread_arg->code = x.first;
        thread_arg->pos = x.second;
        thread_arg->decoded = message;
        thread_arg->host_name = hostname;
        thread_arg->port_num = port_number;
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
    for (auto c: message) {
        cout << c;
    }
    
    cout << endl;
    
    return 0;
}