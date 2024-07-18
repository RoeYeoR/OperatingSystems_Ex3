#include <iostream>
#include <vector>
#include <deque>
#include <limits>

// Function to add an edge to the adjacency list
void addEdge(std::vector<std::vector<int>>& adj, int u, int v) {
    adj[u].push_back(v);
}

// Function to perform a DFS and fill the deque with vertices in the order of their completion times
void fillOrder(int v, std::vector<std::vector<int>>& adj, std::vector<bool>& visited, std::deque<int>& dq) {
    visited[v] = true;
    for (int i : adj[v]) {
        if (!visited[i]) {
            fillOrder(i, adj, visited, dq);
        }
    }
    dq.push_front(v);  // Use push_front to emulate stack behavior (LIFO)
}

// Function to perform a DFS and collect the vertices of a strongly connected component
void DFSUtil(int v, std::vector<std::vector<int>>& adjTranspose, std::vector<bool>& visited, std::vector<int>& component) {
    visited[v] = true;
    component.push_back(v);
    for (int i : adjTranspose[v]) {
        if (!visited[i]) {
            DFSUtil(i, adjTranspose, visited, component);
        }
    }
}

// Function to transpose the graph
std::vector<std::vector<int>> transposeGraph(int vertices, std::vector<std::vector<int>>& adj) {
    std::vector<std::vector<int>> adjTranspose(vertices);
    for (int v = 0; v < vertices; ++v) {
        for (int i : adj[v]) {
            adjTranspose[i].push_back(v);
        }
    }
    return adjTranspose;
}

// Function to find and print all strongly connected components using Kosaraju's algorithm
void findSCCs(int vertices, std::vector<std::vector<int>>& adj) {
    std::deque<int> dq;
    std::vector<bool> visited(vertices, false);

    // Fill vertices in the deque according to their finishing times
    for (int i = 0; i < vertices; ++i) {
        if (!visited[i]) {
            fillOrder(i, adj, visited, dq);
        }
    }

    // Transpose the graph
    std::vector<std::vector<int>> adjTranspose = transposeGraph(vertices, adj);

    // Mark all vertices as not visited for the second DFS
    std::fill(visited.begin(), visited.end(), false);

    // Process all vertices in the order defined by the deque
    while (!dq.empty()) {
        int v = dq.front();
        dq.pop_front();

        if (!visited[v]) {
            std::vector<int> component;
            DFSUtil(v, adjTranspose, visited, component);

            // Print the strongly connected component
            std::cout << "Strongly Connected Component: ";
            for (int i : component) {
                std::cout << i << " ";
            }
            std::cout << std::endl;
        }
    }
}

int main() {
    int n; // vertices
    int m; // edges

    std::cout << "Enter number of vertices and edges: " << std::endl;
    while (true) {
        std::cin >> n >> m;
        if (std::cin.fail() || n < 0 || m < 0) {
            std::cin.clear(); // clear the error flag
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // discard invalid input
            std::cout << "Invalid input. Please enter two non-negative integers: " << std::endl;
        } else {
            break; // valid input, exit the loop
        }
    }

    std::vector<std::vector<int>> adj(n);
    std::cout << "Enter " << m << " pairs of vertices (each pair separated by a space): " << std::endl;
    for (int i = 0; i < m; ++i) {
        int u, v;
        while (true) {
            std::cin >> u >> v;
            if (std::cin.fail()) {
                std::cin.clear(); // clear the error flag
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // discard invalid input
                std::cout << "Invalid input. Please enter two integers separated by a space: " << std::endl;
            } else {
                addEdge(adj, u, v);
                break; // valid input, exit the loop
            }
        }
    }

    findSCCs(n, adj);

    return 0;
}
