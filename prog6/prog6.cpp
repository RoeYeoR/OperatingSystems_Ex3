#include "prog5.hpp"
#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <string>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <signal.h>

#define PORT 9034   // Port we're listening on

using namespace std;

// Global variables for graph representation
int vertexCount = 0, edgeCount = 0; // Number of vertices and edges
vector<list<int>> adjList; // Adjacency list for the graph
vector<list<int>> adjListTranspose; // Transpose adjacency list for Kosaraju's algorithm

// Global Reactor pointer
void* globalReactor = nullptr;

// Function prototypes
void* acceptConnection(int listener_fd);
void* handleClient(int client_fd);
void createNewGraph(int numVertices, int numEdges);
void runKosarajuAlgorithm(int client_fd);
void addEdge(int u, int v);
void deleteEdge(int u, int v);
void signalHandler(int signum);

int main() {
    int listener;     // Listening socket descriptor
    struct sockaddr_in server_addr;

    // Set up signal handlers
    signal(SIGINT, signalHandler);
    signal(SIGTSTP, signalHandler);

    // Create a listening socket
    if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    // Set socket options
    int yes = 1;
    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    }

    // Bind the socket
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    memset(&(server_addr.sin_zero), '\0', 8);

    if (bind(listener, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        exit(1);
    }

    // Listen for incoming connections
    if (listen(listener, 10) == -1) {
        perror("listen");
        exit(1);
    }

    cout << "Server is running on port " << PORT << endl;

    // Start the reactor
    globalReactor = Reactor::startReactor();

    // Add the listener socket to the reactor
    Reactor::addFdToReactor(globalReactor, listener, acceptConnection);

    // Run the reactor
    static_cast<Reactor*>(globalReactor)->run();

    // This point is reached when the reactor stops
    close(listener);
    cout << "Server shut down" << endl;

    return 0;
}

// Function to initialize a new graph
void createNewGraph(int numVertices, int numEdges) {
    vertexCount = numVertices;
    edgeCount = numEdges;

    adjList.clear();
    adjList.resize(vertexCount);
    adjListTranspose.clear();
    adjListTranspose.resize(vertexCount);

    cout << "New graph created with " << vertexCount << " vertices and " << edgeCount << " edges." << endl;
}

// Kosaraju's algorithm function
void runKosarajuAlgorithm(int client_fd) {
    if (vertexCount <= 0 || edgeCount <= 0 || edgeCount > 2 * vertexCount) {
        string msg = "Invalid input\n";
        send(client_fd, msg.c_str(), msg.size(), 0);
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
    cout << "Kosaraju's algorithm completed. Results sent to client." << endl;
}

// Function to add a new edge
void addEdge(int u, int v) {
    adjList[u].push_back(v);
    adjListTranspose[v].push_back(u);
    cout << "Edge added from " << u << " to " << v << "." << endl;
}

// Function to remove an edge
void deleteEdge(int u, int v) {
    auto it = find(adjList[u].begin(), adjList[u].end(), v);
    if (it != adjList[u].end()) {
        adjList[u].erase(it);
    }

    it = find(adjListTranspose[v].begin(), adjListTranspose[v].end(), u);
    if (it != adjListTranspose[v].end()) {
        adjListTranspose[v].erase(it);
    }
    cout << "Edge removed from " << u << " to " << v << "." << endl;
}

// Signal handler to gracefully shut down the server
void signalHandler(int signum) {
    cout << "Shutting down server..." << endl;
    if (globalReactor) {
        Reactor::stopReactor(globalReactor);
    }
    exit(signum);
}

// Function to handle new connections
void* acceptConnection(int listener_fd) {
    struct sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);
    int new_fd = accept(listener_fd, (struct sockaddr *)&client_addr, &addrlen);

    if (new_fd == -1) {
        perror("accept");
        return nullptr;
    }

    cout << "New connection from " << inet_ntoa(client_addr.sin_addr) << " on socket " << new_fd << endl;

    // Add the new client socket to the reactor
    Reactor::addFdToReactor(globalReactor, new_fd, handleClient);

    return nullptr;
}

// Function to handle client communication
void* handleClient(int client_fd) {
    char buf[1024];
    int numbytes = recv(client_fd, buf, sizeof(buf) - 1, 0);

    if (numbytes <= 0) {
        if (numbytes == 0) {
            cout << "Client disconnected" << endl;
        } else {
            perror("recv");
        }
        close(client_fd);
        Reactor::removeFdFromReactor(globalReactor, client_fd);
        return nullptr;
    }

    buf[numbytes] = '\0';
    string command(buf);
    stringstream ss(command);
    string cmd;
    ss >> cmd;

    if (cmd == "Newgraph") {
        int numVertices, numEdges;
        ss >> numVertices >> numEdges;
        createNewGraph(numVertices, numEdges);

        string msg1 = "Enter " + to_string(numEdges) + " edges:\n";
        send(client_fd, msg1.c_str(), msg1.size(), 0);

        for (int i = 0; i < numEdges; ++i) {
            numbytes = recv(client_fd, buf, sizeof(buf) - 1, 0);
            if (numbytes > 0) {
                buf[numbytes] = '\0';
                stringstream edge_ss(buf);
                int u, v;
                edge_ss >> u >> v;
                addEdge(u, v);
            }
        }

        string msg = "Graph created with " + to_string(numVertices) + " vertices and " + to_string(numEdges) + " edges.\n";
        send(client_fd, msg.c_str(), msg.size(), 0);
    } else if (cmd == "Kosaraju") {
        runKosarajuAlgorithm(client_fd);
    } else if (cmd == "Newedge") {
        int u, v;
        ss >> u >> v;
        addEdge(u, v);
        string msg = "Edge " + to_string(u) + " " + to_string(v) + " added.\n";
        send(client_fd, msg.c_str(), msg.size(), 0);
    } else if (cmd == "Removeedge") {
        int u, v;
        ss >> u >> v;
        deleteEdge(u, v);
        string msg = "Edge " + to_string(u) + " " + to_string(v) + " removed.\n";
        send(client_fd, msg.c_str(), msg.size(), 0);
    } else if (cmd == "Exit") {
        string msg = "Goodbye!\n";
        send(client_fd, msg.c_str(), msg.size(), 0);
        close(client_fd);
        Reactor::removeFdFromReactor(globalReactor, client_fd);
        return nullptr;
    } else {
        string msg = "Invalid command\n";
        send(client_fd, msg.c_str(), msg.size(), 0);
    }

    return nullptr;
}

