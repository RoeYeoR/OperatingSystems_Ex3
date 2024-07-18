#include <iostream>
#include <vector>
#include <deque>
#include <limits>

// Define a structure for the linked list nodes
struct ListNode {
    int vertex;
    ListNode* next;
    ListNode(int v) : vertex(v), next(nullptr) {}
};

// Define a structure for the adjacency list
struct AdjList {
    ListNode* head;
    AdjList() : head(nullptr) {}
};

// Function to add an edge to the adjacency list
void addEdge(AdjList* adj, int u, int v) {
    ListNode* newNode = new ListNode(v);
    newNode->next = adj[u].head;
    adj[u].head = newNode;
}

// Function to perform a DFS and fill the deque with vertices in the order of their completion times
void fillOrder(int v, AdjList* adj, std::vector<bool>& visited, std::deque<int>& dq) {
    visited[v] = true;
    ListNode* temp = adj[v].head;
    while (temp) {
        if (!visited[temp->vertex]) {
            fillOrder(temp->vertex, adj, visited, dq);
        }
        temp = temp->next;
    }
    dq.push_front(v);  // Use push_front to emulate stack behavior (LIFO)
}

// Function to perform a DFS and collect the vertices of a strongly connected component
void DFSUtil(int v, AdjList* adjTranspose, std::vector<bool>& visited, std::vector<int>& component) {
    visited[v] = true;
    component.push_back(v);
    ListNode* temp = adjTranspose[v].head;
    while (temp) {
        if (!visited[temp->vertex]) {
            DFSUtil(temp->vertex, adjTranspose, visited, component);
        }
        temp = temp->next;
    }
}

// Function to transpose the graph
AdjList* transposeGraph(int vertices, AdjList* adj) {
    AdjList* adjTranspose = new AdjList[vertices];
    for (int v = 0; v < vertices; ++v) {
        ListNode* temp = adj[v].head;
        while (temp) {
            addEdge(adjTranspose, temp->vertex, v);
            temp = temp->next;
        }
    }
    return adjTranspose;
}

// Function to find and print all strongly connected components using Kosaraju's algorithm
void findSCCs(int vertices, AdjList* adj) {
    std::deque<int> dq;
    std::vector<bool> visited(vertices, false);

    // Fill vertices in the deque according to their finishing times
    for (int i = 0; i < vertices; ++i) {
        if (!visited[i]) {
            fillOrder(i, adj, visited, dq);
        }
    }

    // Transpose the graph
    AdjList* adjTranspose = transposeGraph(vertices, adj);

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

    delete[] adjTranspose; // Clean up the allocated memory for the transposed graph
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

    AdjList* adj = new AdjList[n];
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

    delete[] adj; // Clean up the allocated memory for the adjacency list

    return 0;
}
