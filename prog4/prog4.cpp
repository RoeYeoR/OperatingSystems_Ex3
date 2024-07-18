// // #include <iostream>
// // #include <vector>
// // #include <stack>
// // #include <limits>
// // #include <string>
// // #include <cstring>
// // #include <unistd.h>
// // #include <sys/socket.h>
// // #include <netinet/in.h>
// // #include <arpa/inet.h>

// // #define PORT 8081
// // #define MAX_CLIENTS 5

// // // Function to add an edge to the adjacency list
// // void addEdge(std::vector<std::vector<long>>& adj, long u, long v) {
// //     adj[u].push_back(v);
// // }

// // // Function to remove an edge from the adjacency list
// // void removeEdge(std::vector<std::vector<long>>& adj, long u, long v) {
// //     for (auto it = adj[u].begin(); it != adj[u].end(); ++it) {
// //         if (*it == v) {
// //             adj[u].erase(it);
// //             break;
// //         }
// //     }
// // }

// // // Function to perform a DFS and fill the stack with vertices in the order of their completion times
// // void fillOrder(long v, std::vector<std::vector<long>>& adj, std::vector<bool>& visited, std::stack<long>& Stack) {
// //     visited[v] = true;
// //     for (long i : adj[v]) {
// //         if (!visited[i]) {
// //             fillOrder(i, adj, visited, Stack);
// //         }
// //     }
// //     Stack.push(v);
// // }

// // // Function to perform a DFS and collect the vertices of a strongly connected component
// // void DFSUtil(long v, std::vector<std::vector<long>>& adjTranspose, std::vector<bool>& visited, std::vector<long>& component) {
// //     visited[v] = true;
// //     component.push_back(v);
// //     for (long i : adjTranspose[v]) {
// //         if (!visited[i]) {
// //             DFSUtil(i, adjTranspose, visited, component);
// //         }
// //     }
// // }

// // // Function to transpose the graph
// // std::vector<std::vector<long>> transposeGraph(long vertices, std::vector<std::vector<long>>& adj) {
// //     std::vector<std::vector<long>> adjTranspose(vertices);
// //     for (long v = 0; v < vertices; ++v) {
// //         for (long i : adj[v]) {
// //             adjTranspose[i].push_back(v);
// //         }
// //     }
// //     return adjTranspose;
// // }

// // // Function to find and print all strongly connected components using Kosaraju's algorithm
// // void findSCCs(long vertices, std::vector<std::vector<long>>& adj) {
// //     std::stack<long> Stack;
// //     std::vector<bool> visited(vertices, false);

// //     // Fill vertices in the stack according to their finishing times
// //     for (long i = 0; i < vertices; ++i) {
// //         if (!visited[i]) {
// //             fillOrder(i, adj, visited, Stack);
// //         }
// //     }

// //     // Transpose the graph
// //     std::vector<std::vector<long>> adjTranspose = transposeGraph(vertices, adj);

// //     // Mark all vertices as not visited for the second DFS
// //     std::fill(visited.begin(), visited.end(), false);

// //     // Process all vertices in the order defined by the stack
// //     while (!Stack.empty()) {
// //         long v = Stack.top();
// //         Stack.pop();

// //         if (!visited[v]) {
// //             std::vector<long> component;
// //             DFSUtil(v, adjTranspose, visited, component);

// //             // Print the strongly connected component (for server, you might send it to client or log it)
// //             std::cout << "Strongly Connected Component: ";
// //             for (long i : component) {
// //                 std::cout << i << " ";
// //             }
// //             std::cout << std::endl;
// //         }
// //     }
// // }

// // int main(int argc, char const *argv[]) {
// //     int server_fd = 0, new_socket = 0, valread = 0;
// //     struct sockaddr_in address;
// //     int addrlen = sizeof(address);
// //     int sock = 0;
// //     struct sockaddr_in serv_addr;
// //     char buffer[1024] = {0};
// //     std::vector<std::vector<long>> adj; // Graph adjacency list

// //     // Create server socket
// //     if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
// //         perror("socket failed");
// //         exit(EXIT_FAILURE);
// //     }

// //     address.sin_family = AF_INET;
// //     address.sin_addr.s_addr = INADDR_ANY;
// //     address.sin_port = htons( PORT );

// //     // Bind socket to port
// //     if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
// //         perror("bind failed");
// //         exit(EXIT_FAILURE);
// //     }

// //     // Listen for incoming connections
// //     if (listen(server_fd, MAX_CLIENTS) < 0) {
// //         perror("listen");
// //         exit(EXIT_FAILURE);
// //     }

// //     std::cout << "Server listening on port " << PORT << std::endl;

// //     // Main server loop
// //     while (true) {
// //         // Accept incoming connection
// //         if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
// //             perror("accept");
// //             exit(EXIT_FAILURE);
// //         }

// //         std::cout << "New client connected" << std::endl;

// //         // Continue interacting with the client until it disconnects
// //         while (true) {
// //             valread = read( new_socket , buffer, 1024);
// //             if (valread <= 0) {
// //                 // Handle client disconnect
// //                 close(new_socket);
// //                 std::cout << "Client disconnected" << std::endl;
// //                 break;
// //             }

// //             std::string command(buffer);
// //             std::cout << "Received command: " << command << std::endl;

// //             // Process the command received from client
// //             if (command.substr(0, 8) == "Newgraph") {
// //                 long n = std::stol(command.substr(9, command.find(' ', 9)));
// //                 long m = std::stol(command.substr(command.find(' ', 9) + 1));

// //                 if (n <= 0 || m < 0 ) {
// //                     std::cout << "Invalid number of vertices or edges. Please enter positive number of vertices and non-negative number of edges." << std::endl;
// //                     continue;
// //                 }

// //                 adj.assign(n, std::vector<long>());
// //                 std::cout << "Enter " << m << " pairs of vertices (each pair separated by a space): " << std::endl;

// //                 for (long i = 0; i < m; ++i) {
// //                     long u, v;
// //                     valread = read( new_socket , buffer, 1024); // Read edge pair from client
// //                     if (valread <= 0) {
// //                         // Handle client disconnect
// //                         close(new_socket);
// //                         std::cout << "Client disconnected" << std::endl;
// //                         break;
// //                     }

// //                     std::string edgePair(buffer);
// //                     u = std::stol(edgePair.substr(0, edgePair.find(' ')));
// //                     v = std::stol(edgePair.substr(edgePair.find(' ') + 1));

// //                     if (u < 0 || u >= n || v < 0 || v >= n) {
// //                         std::cout << "Invalid vertices for edge addition. Vertices should be within the range [0, " << n-1 << "]." << std::endl;
// //                         continue;
// //                     }

// //                     addEdge(adj, u, v);
// //                 }

// //                 std::cout << "Graph created successfully with " << n << " vertices and " << m << " edges." << std::endl;

// //             } else if (command.substr(0, 7) == "Newedge") {
// //                 long u = std::stol(command.substr(8, command.find(' ', 8)));
// //                 long v = std::stol(command.substr(command.find(' ', 8) + 1));

// //                 if (u < 0 || u >= adj.size() || v < 0 || v >= adj.size()) {
// //                     std::cout << "Invalid vertices for edge addition. Vertices should be within the range [0, " << adj.size()-1 << "]." << std::endl;
// //                     continue;
// //                 }

// //                 addEdge(adj, u, v);
// //                 std::cout << "Edge (" << u << "," << v << ") added successfully." << std::endl;

// //             } else if (command.substr(0, 10) == "Removeedge") {
// //                 long u = std::stol(command.substr(11, command.find(' ', 11)));
// //                 long v = std::stol(command.substr(command.find(' ', 11) + 1));

// //                 if (u < 0 || u >= adj.size() || v < 0 || v >= adj.size()) {
// //                     std::cout << "Invalid vertices for edge removal. Vertices should be within the range [0, " << adj.size()-1 << "]." << std::endl;
// //                     continue;
// //                 }

// //                 removeEdge(adj, u, v);
// //                 std::cout << "Edge (" << u << "," << v << ") removed successfully." << std::endl;

// //             } else if (command == "Kosaraju") {
// //                 findSCCs(adj.size(), adj);

// //             } else if (command == "exit") {
// //                 close(new_socket);
// //                 std::cout << "Client disconnected" << std::endl;
// //                 break;

// //             } else {
// //                 std::cout << "Unknown command: " << command << std::endl;
// //             }
// //         }
// //     }

// //     return 0;
// // }


// #include <iostream>
// #include <vector>
// #include <list>
// #include <functional>
// #include <algorithm>
// #include <netinet/in.h>
// #include <arpa/inet.h>
// #include <unistd.h>
// #include <cstring>
// #include <sstream>
// #include <fcntl.h>
// #include <sys/select.h>

// using namespace std;

// // Function declarations
// void Newgraph(int numVertices, int numEdges);
// void Newedge(int u, int v);
// void Removeedge(int u, int v);
// void Kosaraju(int client_fd);
// void handle_client(int client_fd, fd_set &master_set, fd_set &read_fds);

// // Global variables for graph representation
// int n = 0, m = 0;
// vector<list<int>> adj;
// vector<list<int>> adjT;

// // Function to initialize a new graph
// void Newgraph(int numVertices, int numEdges) {
//     n = numVertices;
//     m = numEdges;

//     adj.clear();
//     adj.resize(n);
//     adjT.clear();
//     adjT.resize(n);

//     cout << "New graph created with " << numVertices << " vertices and " << numEdges << " edges." << endl;
// }

// // Function to add a new edge
// void Newedge(int u, int v) {
//     adj[u].push_back(v);
//     adjT[v].push_back(u);
//     cout << "Edge added: " << u << " -> " << v << endl;
// }

// // Function to remove an edge
// void Removeedge(int u, int v) {
//     auto it = find(adj[u].begin(), adj[u].end(), v);
//     if (it != adj[u].end()) {
//         adj[u].erase(it);
//         cout << "Edge removed: " << u << " -> " << v << endl;
//     }

//     it = find(adjT[v].begin(), adjT[v].end(), u);
//     if (it != adjT[v].end()) {
//         adjT[v].erase(it);
//     }
// }

// // Kosaraju's algorithm function
// void Kosaraju(int client_fd) {
//     if (n <= 0 || m <= 0 || m > 2 * n) {
//         string msg = "Invalid input\n";
//         send(client_fd, msg.c_str(), msg.size(), 0);
//         return;
//     }

//     vector<bool> visited(n, false);
//     list<int> order;

//     function<void(int)> dfs1 = [&](int u) {
//         visited[u] = true;
//         for (int v : adj[u]) {
//             if (!visited[v]) {
//                 dfs1(v);
//             }
//         }
//         order.push_back(u);
//     };

//     for (int i = 0; i < n; ++i) {
//         if (!visited[i]) {
//             dfs1(i);
//         }
//     }

//     order.reverse();
//     vector<int> component(n, -1);
//     vector<list<int>> components; // To store the nodes of each component

//     function<void(int, int)> dfs2 = [&](int u, int comp) {
//         component[u] = comp;
//         components[comp].push_back(u);
//         for (int v : adjT[u]) {
//             if (component[v] == -1) {
//                 dfs2(v, comp);
//             }
//         }
//     };

//     int comp = 0;
//     for (int u : order) {
//         if (component[u] == -1) {
//             components.push_back(list<int>()); // Add a new component
//             dfs2(u, comp++);
//         }
//     }

//     string result = "Number of strongly connected components: " + to_string(comp) + "\n";
//     for (int i = 0; i < comp; ++i) {
//         result += "Component " + to_string(i + 1) + ": ";
//         for (int node : components[i]) {
//             result += to_string(node) + " ";
//         }
//         result += "\n";
//     }

//     send(client_fd, result.c_str(), result.size(), 0);
//     cout << "Kosaraju's algorithm executed. Result sent to client." << endl;
// }

// void handle_client(int client_fd, fd_set &master_set, fd_set &read_fds) {
//     char buf[1024];
//     int numbytes;
//     string command;
//     stringstream ss;

//     if ((numbytes = recv(client_fd, buf, sizeof(buf) - 1, 0)) <= 0) {
//         if (numbytes == 0) {
//             cout << "Socket " << client_fd << " hung up" << endl;
//         } else {
//             perror("recv");
//         }
//         close(client_fd);
//         FD_CLR(client_fd, &master_set);
//     } else {
//         buf[numbytes] = '\0';
//         command.append(buf);
//         ss.clear();
//         ss.str(command);
//         string cmd;
//         ss >> cmd;

//         if (cmd == "Newgraph") {
//             int numVertices, numEdges;
//             ss >> numVertices >> numEdges;
//             Newgraph(numVertices, numEdges);

//             // Inform the client to send the edges
//             string msg = "Enter " + to_string(numEdges) + " edges:\n";
//             send(client_fd, msg.c_str(), msg.size(), 0);

//             for (int i = 0; i < numEdges; ++i) {
//                 numbytes = recv(client_fd, buf, sizeof(buf) - 1, 0);
//                 if (numbytes <= 0) {
//                     if (numbytes == 0) {
//                         cout << "Socket " << client_fd << " hung up" << endl;
//                     } else {
//                         perror("recv");
//                     }
//                     close(client_fd);
//                     FD_CLR(client_fd, &master_set);
//                     return;
//                 }
//                 buf[numbytes] = '\0';
//                 stringstream edge_ss(buf);
//                 int u, v;
//                 edge_ss >> u >> v;
//                 Newedge(u, v);
//             }

//             msg = "Graph created with " + to_string(numVertices) + " vertices and " + to_string(numEdges) + " edges.\n";
//             send(client_fd, msg.c_str(), msg.size(), 0);
//         } else if (cmd == "Kosaraju") {
//             Kosaraju(client_fd);
//         } else if (cmd == "Newedge") {
//             int u, v;
//             ss >> u >> v;
//             Newedge(u, v);
//             string msg = "Edge " + to_string(u) + " " + to_string(v) + " added.\n";
//             send(client_fd, msg.c_str(), msg.size(), 0);
//         } else if (cmd == "Removeedge") {
//             int u, v;
//             ss >> u >> v;
//             Removeedge(u, v);
//             string msg = "Edge " + to_string(u) + " " + to_string(v) + " removed.\n";
//             send(client_fd, msg.c_str(), msg.size(), 0);
//         } else {
//             string msg = "Invalid command\n";
//             send(client_fd, msg.c_str(), msg.size(), 0);
//         }

//         command.clear(); // Clear the command buffer
//     }
// }

// int main() {
//     int listener;     // Listening socket descriptor
//     struct sockaddr_in serveraddr;    // server address
//     struct sockaddr_in clientaddr;    // client address
//     socklen_t addrlen;
//     const int PORT = 9034;


//     fd_set master_set, read_fds;
//     int fdmax;

//     // Create a new listener socket
//     if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
//         perror("socket");
//         exit(1);
//     }

//     // Set the socket option to reuse the address
//     int yes = 1;
//     if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
//         perror("setsockopt");
//         exit(1);
//     }

//     // Set the listener socket to non-blocking
//     fcntl(listener, F_SETFL, O_NONBLOCK);

//     // Set up the server address struct
//     serveraddr.sin_family = AF_INET;
//     serveraddr.sin_addr.s_addr = INADDR_ANY;
//     serveraddr.sin_port = htons(PORT);
//     memset(&(serveraddr.sin_zero), '\0', 8);

//     // Bind the listener socket to our port
//     if (bind(listener, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1) {
//         perror("bind");
//         exit(1);
//     }

//     // Start listening for incoming connections
//     if (listen(listener, 10) == -1) {
//         perror("listen");
//         exit(1);
//     }

//     printf("Server is running on port %d\n", PORT);

//     FD_ZERO(&master_set);
//     FD_ZERO(&read_fds);
//     FD_SET(listener, &master_set);
//     fdmax = listener;

//     while (true) {
//         read_fds = master_set; // Copy the master set to the read set

//         if (select(fdmax + 1, &read_fds, nullptr, nullptr, nullptr) == -1) {
//             perror("select");
//             exit(1);
//         }

//         for (int i = 0; i <= fdmax; ++i) {
//             if (FD_ISSET(i, &read_fds)) { // We got one!!
//                 if (i == listener) {
//                     // Handle new connections
//                     addrlen = sizeof(clientaddr);
//                     int newfd = accept(listener, (struct sockaddr *)&clientaddr, &addrlen);

//                     if (newfd == -1) {
//                         perror("accept");
//                     } else {
//                         printf("New connection from %s on socket %d\n", inet_ntoa(clientaddr.sin_addr), newfd);
//                         FD_SET(newfd, &master_set); // Add to master set
//                         if (newfd > fdmax) {
//                             fdmax = newfd;
//                         }
//                     }
//                 } else {
//                     // Handle data from a client
//                     handle_client(i, master_set, read_fds);
//                 }
//             }
//         }
//     }

//     return 0;
// }
#include <iostream>
#include <vector>
#include <list>
#include <functional>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <algorithm>
#include <fcntl.h>
#include <sys/select.h>
#include <arpa/inet.h>

#define PORT 9034

using namespace std;


// Global variables for graph representation
int n = 0, m = 0;
vector<list<int>> adj;
vector<list<int>> adjT;

// Function declarations
void Newgraph(int numVertices, int numEdges);
void Newedge(int u, int v);
void Removeedge(int u, int v);
void Kosaraju(int client_fd);
void handle_client(int client_fd, fd_set &master_set);





int main() {
    int listener;     // Listening socket descriptor
    struct sockaddr_in serveraddr;    // server address
    struct sockaddr_in clientaddr;    // client address
    socklen_t addrlen;
    fd_set master_set, read_fds;
    int fdmax;

    // Create a new listener socket
    if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    // Set the socket option to reuse the address
    int yes = 1;
    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    }

    // Set the listener socket to non-blocking
    fcntl(listener, F_SETFL, O_NONBLOCK);

    // Set up the server address struct
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = INADDR_ANY;
    serveraddr.sin_port = htons(PORT);
    memset(&(serveraddr.sin_zero), '\0', 8);

    // Bind the listener socket to our port
    if (bind(listener, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1) {
        perror("bind");
        exit(1);
    }

    // Start listening for incoming connections
    if (listen(listener, 10) == -1) {
        perror("listen");
        exit(1);
    }

    printf("Server is running on port %d\n", PORT);

    FD_ZERO(&master_set);
    FD_ZERO(&read_fds);
    FD_SET(listener, &master_set);
    fdmax = listener;

    while (true) {
        read_fds = master_set; // Copy the master set to the read set

        if (select(fdmax + 1, &read_fds, nullptr, nullptr, nullptr) == -1) {
            perror("select");
            exit(1);
        }

        for (int i = 0; i <= fdmax; ++i) {
            if (FD_ISSET(i, &read_fds)) { // We got one!!
                if (i == listener) {
                    // Handle new connections
                    addrlen = sizeof(clientaddr);
                    int newfd = accept(listener, (struct sockaddr *)&clientaddr, &addrlen);

                    if (newfd == -1) {
                        perror("accept");
                    } else {
                        printf("New connection from %s on socket %d\n", inet_ntoa(clientaddr.sin_addr), newfd);
                        FD_SET(newfd, &master_set); // Add to master set
                        if (newfd > fdmax) {
                            fdmax = newfd;
                        }
                    }
                } else {
                    // Handle data from a client
                    handle_client(i, master_set);
                }
            }
        }
    }

    return 0;
}


// Function to initialize a new graph
void Newgraph(int numVertices, int numEdges) {
    n = numVertices;
    m = numEdges;

    adj.clear();
    adj.resize(n);
    adjT.clear();
    adjT.resize(n);

    cout << "New graph created with " << numVertices << " vertices and " << numEdges << " edges." << endl;
}

// Function to add a new edge
void Newedge(int u, int v) {
    adj[u].push_back(v);
    adjT[v].push_back(u);
    cout << "Edge added: " << u << " -> " << v << endl;
}

// Function to remove an edge
void Removeedge(int u, int v) {
    auto it = find(adj[u].begin(), adj[u].end(), v);
    if (it != adj[u].end()) {
        adj[u].erase(it);
        cout << "Edge removed: " << u << " -> " << v << endl;
    }

    it = find(adjT[v].begin(), adjT[v].end(), u);
    if (it != adjT[v].end()) {
        adjT[v].erase(it);
    }
}

// Kosaraju's algorithm function
void Kosaraju(int client_fd) {
    if (n <= 0 || m <= 0 || m > 2 * n) {
        string msg = "Invalid input\n";
        send(client_fd, msg.c_str(), msg.size(), 0);
        return;
    }

    vector<bool> visited(n, false);
    list<int> order;

    function<void(int)> dfs1 = [&](int u) {
        visited[u] = true;
        for (int v : adj[u]) {
            if (!visited[v]) {
                dfs1(v);
            }
        }
        order.push_back(u);
    };

    for (int i = 0; i < n; ++i) {
        if (!visited[i]) {
            dfs1(i);
        }
    }

    order.reverse();
    vector<int> component(n, -1);
    vector<list<int>> components; // To store the nodes of each component

    function<void(int, int)> dfs2 = [&](int u, int comp) {
        component[u] = comp;
        components[comp].push_back(u);
        for (int v : adjT[u]) {
            if (component[v] == -1) {
                dfs2(v, comp);
            }
        }
    };

    int comp = 0;
    for (int u : order) {
        if (component[u] == -1) {
            components.push_back(list<int>()); // Add a new component
            dfs2(u, comp++);
        }
    }

    string result = "Number of strongly connected components: " + to_string(comp) + "\n";
    for (int i = 0; i < comp; ++i) {
        result += "Component " + to_string(i + 1) + ": ";
        for (int node : components[i]) {
            result += to_string(node) + " ";
        }
        result += "\n";
    }

    send(client_fd, result.c_str(), result.size(), 0);
    cout << "Kosaraju's algorithm executed. Result sent to client." << endl;
}

void handle_client(int client_fd, fd_set &master_set) {
    char buf[1024];
    int numbytes;
    string command;
    stringstream ss;

    if ((numbytes = recv(client_fd, buf, sizeof(buf) - 1, 0)) <= 0) {
        if (numbytes == 0) {
            cout << "Socket " << client_fd << " hung up" << endl;
        } else {
            perror("recv");
        }
        close(client_fd);
        FD_CLR(client_fd, &master_set);
    } else {
        buf[numbytes] = '\0';
        command.append(buf);
        ss.clear();
        ss.str(command);
        string cmd;
        ss >> cmd;

        if (cmd == "Newgraph") {
            int numVertices, numEdges;
            ss >> numVertices >> numEdges;
            Newgraph(numVertices, numEdges);

            // Inform the client to send the edges
            string msg = "Enter " + to_string(numEdges) + " edges:\n";
            send(client_fd, msg.c_str(), msg.size(), 0);

            for (int i = 0; i < numEdges; ++i) {
                numbytes = recv(client_fd, buf, sizeof(buf) - 1, 0);
                if (numbytes <= 0) {
                    if (numbytes == 0) {
                        cout << "Socket " << client_fd << " hung up" << endl;
                    } else {
                        perror("recv");
                    }
                    close(client_fd);
                    FD_CLR(client_fd, &master_set);
                    return;
                }
                buf[numbytes] = '\0';
                stringstream edge_ss(buf);
                int u, v;
                edge_ss >> u >> v;
                Newedge(u, v);
            }

            msg = "Graph created with " + to_string(numVertices) + " vertices and " + to_string(numEdges) + " edges.\n";
            send(client_fd, msg.c_str(), msg.size(), 0);
        } else if (cmd == "Kosaraju") {
            Kosaraju(client_fd);
        } else if (cmd == "Newedge") {
            int u, v;
            ss >> u >> v;
            Newedge(u, v);
            string msg = "Edge " + to_string(u) + " " + to_string(v) + " added.\n";
            send(client_fd, msg.c_str(), msg.size(), 0);
        } else if (cmd == "Removeedge") {
            int u, v;
            ss >> u >> v;
            Removeedge(u, v);
            string msg = "Edge " + to_string(u) + " " + to_string(v) + " removed.\n";
            send(client_fd, msg.c_str(), msg.size(), 0);
        } else {
            string msg = "Invalid command\n";
            send(client_fd, msg.c_str(), msg.size(), 0);
        }

        command.clear(); // Clear the command buffer
    }
}

