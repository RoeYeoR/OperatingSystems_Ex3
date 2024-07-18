#include <iostream>
#include <vector>
#include <stack>
#include <limits>

// Function to add an edge to the adjacency list
void addEdge(std::vector<std::vector<long>>& adj, long u, long v) {
    adj[u].push_back(v);
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

// Function to find and prlong all strongly connected components using Kosaraju's algorithm
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

            // Prlong the strongly connected component
            std::cout << "Strongly Connected Component: ";
            for (long i : component) {
                std::cout << i << " ";
            }
            std::cout << std::endl;
        }
    }
}

int main() {
    long n; // vertices
    long m; // edges

    std::cout << "Enter number of vertices and edges: " << std::endl;
    while (true) {
        std::cin >> n >> m;
        if (std::cin.fail() || n < 0 || m < 0) {
            std::cin.clear(); // clear the error flag
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // discard invalid input
            std::cout << "Invalid input. Please enter two non-negative longegers: " << std::endl;
        } else {
            break; // valid input, exit the loop
        }
    }

    std::vector<std::vector<long>> adj(n);
    std::cout << "Enter " << m << " pairs of vertices (each pair separated by a space): " << std::endl;
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

    findSCCs(n, adj);

    return 0;
}
