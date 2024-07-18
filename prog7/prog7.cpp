#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <string>
#include <sstream>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>

using namespace std;

#define PORT 9033   // Port we're listening on

int listener;     // Listening socket descriptor

// Define global variables for graph representation
int vertexCount = 0, edgeCount = 0; // Number of vertices and edges
vector<list<int>> adjList; // Adjacency list for the graph
vector<list<int>> adjListTranspose; // Transpose adjacency list for Kosaraju's algorithm

pthread_mutex_t graph_mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex for protecting the graph

// Function prototypes
void* handleClient(void* arg);
void initializeGraph(int numVertices, int numEdges);
void executeKosarajuAlgorithm(int client_fd);
void addGraphEdge(int u, int v);
void removeGraphEdge(int u, int v);
void signalHandler(int signum);

int main() {
    struct sockaddr_in serveraddr;    // Server address
    struct sockaddr_in clientaddr;    // Client address
    socklen_t addrlen;
    char buf[1024];    // Buffer for client data

    // Set up the signal handler for SIGINT and SIGTSTP
    signal(SIGINT, signalHandler);
    signal(SIGTSTP, signalHandler);

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

    while (1) {
        addrlen = sizeof(clientaddr);
        int newfd = accept(listener, (struct sockaddr *)&clientaddr, &addrlen);

        if (newfd == -1) {
            perror("accept");
            continue;
        }

        // Log the new connection
        printf("New connection from %s on socket %d\n", inet_ntoa(clientaddr.sin_addr), newfd);

        pthread_t client_thread;
        if (pthread_create(&client_thread, NULL, handleClient, (void *)(intptr_t)newfd) != 0) {
            perror("pthread_create");
            close(newfd);
        }
    }

    return 0;
}

// Function to initialize a new graph
void initializeGraph(int numVertices, int numEdges) {
    pthread_mutex_lock(&graph_mutex);
    vertexCount = numVertices;
    edgeCount = numEdges;

    adjList.clear();
    adjList.resize(vertexCount);
    adjListTranspose.clear();
    adjListTranspose.resize(vertexCount);

    pthread_mutex_unlock(&graph_mutex);

    cout << "New graph initialized with " << vertexCount << " vertices and " << edgeCount << " edges." << endl;
}

// Kosaraju's algorithm function
void executeKosarajuAlgorithm(int client_fd) {
    pthread_mutex_lock(&graph_mutex);
    if (vertexCount <= 0 || edgeCount <= 0 || edgeCount > 2 * vertexCount) {
        string msg = "Invalid input\n";
        send(client_fd, msg.c_str(), msg.size(), 0);
        pthread_mutex_unlock(&graph_mutex);
        return;
    }

    vector<bool> visited(vertexCount, false);
    list<int> order;

    // First DFS to fill order of vertices
    function<void(int)> dfs1 = [&](int u) {
        visited[u] = true;
        for (int v : adjList[u]) {
            if (!visited[v]) {
                dfs1(v);
            }
        }
        order.push_back(u);
    };

    // Perform DFS on each vertex to fill the order
    for (int i = 0; i < vertexCount; ++i) {
        if (!visited[i]) {
            dfs1(i);
        }
    }

    reverse(order.begin(), order.end());
    vector<int> component(vertexCount, -1);
    vector<list<int>> components; // To store the nodes of each component

    // Second DFS on the transpose graph
    function<void(int, int)> dfs2 = [&](int u, int comp) {
        component[u] = comp;
        components[comp].push_back(u);
        for (int v : adjListTranspose[u]) {
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

    // Prepare the result to send to the client
    string result = "Number of strongly connected components: " + to_string(comp) + "\n";
    for (int i = 0; i < comp; ++i) {
        result += "Component " + to_string(i + 1) + ": ";
        for (int node : components[i]) {
            result += to_string(node) + " ";
        }
        result += "\n";
    }

    send(client_fd, result.c_str(), result.size(), 0);
    pthread_mutex_unlock(&graph_mutex);

    cout << "Kosaraju's algorithm executed. Results sent to client." << endl;
}

// Function to add a new edge
void addGraphEdge(int u, int v) {
    pthread_mutex_lock(&graph_mutex);
    adjList[u].push_back(v);
    adjListTranspose[v].push_back(u);
    pthread_mutex_unlock(&graph_mutex);

    cout << "Edge added from " << u << " to " << v << "." << endl;
}

// Function to remove an edge
void removeGraphEdge(int u, int v) {
    pthread_mutex_lock(&graph_mutex);

    // Remove from the original adjacency list
    auto it = find(adjList[u].begin(), adjList[u].end(), v);
    if (it != adjList[u].end()) {
        adjList[u].erase(it);
    }

    // Remove from the transpose adjacency list
    auto itT = find(adjListTranspose[v].begin(), adjListTranspose[v].end(), u);
    if (itT != adjListTranspose[v].end()) {
        adjListTranspose[v].erase(itT);
    }

    pthread_mutex_unlock(&graph_mutex);

    cout << "Edge removed from " << u << " to " << v << "." << endl;
}

// Function to handle each client in a separate thread
void* handleClient(void* arg) {
    int client_fd = (intptr_t)arg;
    char buf[1024];
    int numbytes;

    // Log the new connection
    printf("New connection from socket %d (Thread ID: %lu)\n", client_fd, pthread_self());

    while ((numbytes = recv(client_fd, buf, sizeof(buf) - 1, 0)) > 0) {
        buf[numbytes] = '\0';
        string command(buf);
        stringstream ss(command);
        string cmd;
        ss >> cmd;

        if (cmd == "Newgraph") {
            int numVertices, numEdges;
            ss >> numVertices >> numEdges;
            initializeGraph(numVertices, numEdges);

            // Inform the client to send the edges
            string msg = "Enter " + to_string(numEdges) + " edges:\n";
            send(client_fd, msg.c_str(), msg.size(), 0);

            for (int i = 0; i < numEdges; ++i) {
                if ((numbytes = recv(client_fd, buf, sizeof(buf) - 1, 0)) > 0) {
                    buf[numbytes] = '\0';
                    stringstream edge_ss(buf);
                    int u, v;
                    edge_ss >> u >> v;
                    addGraphEdge(u, v);
                }
            }

            string msg1 = "Graph created with " + to_string(numVertices) + " vertices and " + to_string(numEdges) + " edges.\n";
            send(client_fd, msg1.c_str(), msg1.size(), 0);
        } else if (cmd == "Kosaraju") {
            executeKosarajuAlgorithm(client_fd);
        } else if (cmd == "Newedge") {
            int u, v;
            ss >> u >> v;
            addGraphEdge(u, v);
            string msg = "Edge " + to_string(u) + " " + to_string(v) + " added.\n";
            send(client_fd, msg.c_str(), msg.size(), 0);
        } else if (cmd == "Removeedge") {
            int u, v;
            ss >> u >> v;
            removeGraphEdge(u, v);
            string msg = "Edge " + to_string(u) + " " + to_string(v) + " removed.\n";
            send(client_fd, msg.c_str(), msg.size(), 0);
        } else if (cmd == "Exit") {
            string msg = "Goodbye!\n";
            send(client_fd, msg.c_str(), msg.size(), 0);
            break;
        } else {
            string msg = "Invalid command\n";
            send(client_fd, msg.c_str(), msg.size(), 0);
        }
    }

    close(client_fd);
    return NULL;
}

// Signal handler to gracefully shut down the server
void signalHandler(int signum) {
    // Close the listening socket
    close(listener);
    printf("Server shut down gracefully\n");
    exit(signum);
}