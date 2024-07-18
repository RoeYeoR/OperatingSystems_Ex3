#include <iostream>
#include <vector>
#include <stack>
#include <limits>

// Function to add an edge to the adjacency list
void addEdge(std::vector<std::vector<long>>& adj, long u, long v) {
    adj[u].push_back(v);
}

// Function to remove an edge from the adjacency list
void removeEdge(std::vector<std::vector<long>>& adj, long u, long v) {
    for (auto it = adj[u].begin(); it != adj[u].end(); ++it) {
        if (*it == v) {
            adj[u].erase(it);
            break;
        }
    }
}

// Function to perform a DFS and fill the stack with vertices in the order of their completion times
void fillOrder(long v, std::vector<std::vector<long>>& adj, std::vector<bool>& visited, std::stack<long>& Stack) {
    visited[v] = true;
    for (long i : adj[v]) {
        if (!visited[i]) {
            fillOrder(i, adj, visited, Stack);
        }
    }
    Stack.push(v);
}

// Function to perform a DFS and collect the vertices of a strongly connected component
void DFSUtil(long v, std::vector<std::vector<long>>& adjTranspose, std::vector<bool>& visited, std::vector<long>& component) {
    visited[v] = true;
    component.push_back(v);
    for (long i : adjTranspose[v]) {
        if (!visited[i]) {
            DFSUtil(i, adjTranspose, visited, component);
        }
    }
}

// Function to transpose the graph
std::vector<std::vector<long>> transposeGraph(long vertices, std::vector<std::vector<long>>& adj) {
    std::vector<std::vector<long>> adjTranspose(vertices);
    for (long v = 0; v < vertices; ++v) {
        for (long i : adj[v]) {
            adjTranspose[i].push_back(v);
        }
    }
    return adjTranspose;
}

// Function to find and print all strongly connected components using Kosaraju's algorithm
void findSCCs(long vertices, std::vector<std::vector<long>>& adj) {
    std::stack<long> Stack;
    std::vector<bool> visited(vertices, false);

    // Fill vertices in the stack according to their finishing times
    for (long i = 0; i < vertices; ++i) {
        if (!visited[i]) {
            fillOrder(i, adj, visited, Stack);
        }
    }

    // Transpose the graph
    std::vector<std::vector<long>> adjTranspose = transposeGraph(vertices, adj);

    // Mark all vertices as not visited for the second DFS
    std::fill(visited.begin(), visited.end(), false);

    // Process all vertices in the order defined by the stack
    while (!Stack.empty()) {
        long v = Stack.top();
        Stack.pop();

        if (!visited[v]) {
            std::vector<long> component;
            DFSUtil(v, adjTranspose, visited, component);

            // Print the strongly connected component
            std::cout << "Strongly Connected Component: ";
            for (long i : component) {
                std::cout << i << " ";
            }
            std::cout << std::endl;
        }
    }
}

int main() {
    long n = 0; // vertices
    long m = 0; // edges

    std::vector<std::vector<long>> adj;

    std::string command;
    while (std::cin >> command) {
        if (command == "Newgraph") {
            // Read n and m
            std::cin >> n >> m;

            if (n <= 0 || m < 0 ) {
                std::cout << "Invalid number of vertices or edges. Please enter positive number of vertices and non-negative number of edges." << std::endl;
                continue;
            }

            adj.assign(n, std::vector<long>());
              std::cout << "Enter " << m << " pairs of vertices (each pair separated by a space): " << std::endl;
            // Read m pairs of vertices (edges)
            for (long i = 0; i < m; ++i) {
            long u, v;
            while (true) {
                std::cin >> u >> v;
                if (std::cin.fail()) {
                    std::cin.clear(); // clear the error flag
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // discard invalid input
                    std::cout << "Invalid input. Please enter two longegers separated by a space: " << std::endl;
                } else {
                    addEdge(adj, u, v);
                    break; // valid input, exit the loop
                }
            }
            }

            std::cout << "Graph created successfully with " << n << " vertices and " << m << " edges." << std::endl;
        } else if (command == "Kosaraju") {
            // Compute Kosaraju's algorithm
            findSCCs(n, adj);
        } else if (command == "Newedge") {
            // Add an edge between i and j
            long u, v;
            std::cin >> u >> v;
            if (u < 0 || u >= n || v < 0 || v >= n) {
                std::cout << "Invalid vertices for edge addition. Vertices should be within the range [0, " << n-1 << "]." << std::endl;
                continue;
            }
            addEdge(adj, u, v);
            std::cout << "Edge (" << u << "," << v << ") added successfully." << std::endl;
        } else if (command == "Removeedge") {
            // Remove an edge between i and j
            long u, v;
            std::cin >> u >> v;
            if (u < 0 || u >= n || v < 0 || v >= n) {
                std::cout << "Invalid vertices for edge removal. Vertices should be within the range [0, " << n-1 << "]." << std::endl;
                continue;
            }
            removeEdge(adj, u, v);
            std::cout << "Edge (" << u << "," << v << ") removed successfully." << std::endl;
        } else {
            std::cout << "Unknown command: " << command << std::endl;
        }
    }

    return 0;
}
