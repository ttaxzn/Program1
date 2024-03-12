#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <stack>
#include <algorithm>
#include <ctime>

using namespace std;

// Define a structure to represent a city
struct City {
    string name;
    double latitude;
    double longitude;

    // Constructor to initialize members
    City(const string& n, double lat, double lon) : name(n), latitude(lat), longitude(lon) {}
};

// Define a structure to represent an edge between two cities
struct Edge {
    string from;
    string to;
};

// Function to parse the cities data file and populate the cities vector
void parseCitiesFile(const string& filename, vector<City>& cities) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Unable to open cities file." << endl;
        exit(1);
    }

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string name;
        double lat, lon;
        ss >> name >> lat >> lon;
        cities.push_back(City(name, lat, lon)); // Use parentheses instead of braces
    }

    file.close();
}

void parseAdjacenciesFile(const string& filename, unordered_map<string, vector<string> >& adjacencyMap) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Unable to open adjacencies file." << endl;
        exit(1);
    }

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string from, to;
        ss >> from >> to;

        // Add the edge from 'from' to 'to'
        adjacencyMap[from].push_back(to);

        // Add the edge from 'to' to 'from' (ensure symmetry)
        adjacencyMap[to].push_back(from);
    }

    file.close();
}


vector<string> breadthFirstSearch(const unordered_map<string, vector<string> >& adjacencyMap, const string& start, const string& end) {
    // Queue to store nodes to be visited
    queue<string> q;

    // Map to keep track of visited nodes and their parents
    unordered_map<string, bool> visited;
    unordered_map<string, string> parent;

    // Start BFS from the start node
    q.push(start);
    visited[start] = true;
    parent[start] = ""; // Parent of start node is empty

    while (!q.empty()) {
        string current = q.front();
        q.pop();

        // Goal check
        if (current == end) {
            // Reconstruct the path and return it
            vector<string> path;
            while (current != "") {
                path.push_back(current);
                current = parent[current];
            }
            reverse(path.begin(), path.end());
            return path;
        }

        // Explore neighbors of the current node
        for (const string& neighbor : adjacencyMap.at(current)) {
            if (!visited[neighbor]) {
                q.push(neighbor);
                visited[neighbor] = true;
                parent[neighbor] = current; // Update parent of neighbor
            }
        }
    }

    // If no path found, return an empty vector
    return vector<string>();
}


vector<string> depthFirstSearch(const unordered_map<string, vector<string> >& adjacencyMap, const string& start, const string& end) {
    // Use a stack to implement DFS
    stack<string> stack;
    unordered_map<string, bool> visited;
    unordered_map<string, string> parent;

    // Initialize data structures
    for (const auto& pair : adjacencyMap) {
        visited[pair.first] = false;
        parent[pair.first] = "";
    }

    // Start DFS
    stack.push(start);
    visited[start] = true;

    while (!stack.empty()) {
        string current = stack.top();
        stack.pop();

        // Goal check
        if (current == end) {
            // Reconstruct the path and return it
            vector<string> path;
            while (current != "") {
                path.push_back(current);
                current = parent[current];
            }
            reverse(path.begin(), path.end());
            return path;
        }

        // Explore neighbors
        for (const string& neighbor : adjacencyMap.at(current)) {
            if (!visited[neighbor]) {
                stack.push(neighbor);
                visited[neighbor] = true;
                parent[neighbor] = current;
            }
        }
    }

    // If no path found, return an empty vector
    return vector<string>();
}


// Depth-limited DFS function used by iterativeDeepeningDFS
vector<string> depthLimitedDFS(const unordered_map<string, vector<string> >& adjacencyMap, const string& start, const string& end, int depthLimit) {
    // Use a stack to implement DFS
    stack<pair<string, int> > stack; // Pair of node and depth
    unordered_map<string, bool> visited;
    unordered_map<string, string> parent;

    // Initialize data structures
    for (const auto& pair : adjacencyMap) {
        visited[pair.first] = false;
        parent[pair.first] = "";
    }

    // Start DFS with start node and depth 0
    stack.push(make_pair(start, 0));

    visited[start] = true;

    while (!stack.empty()) {
        auto [current, depth] = stack.top();
        stack.pop();

        // Goal check
        if (current == end) {
            // Reconstruct the path and return it
            vector<string> path;
            while (current != "") {
                path.push_back(current);
                current = parent[current];
            }
            reverse(path.begin(), path.end());
            return path;
        }

        // Explore neighbors if depth limit not exceeded
        if (depth < depthLimit) {
            for (const string& neighbor : adjacencyMap.at(current)) {
                if (!visited[neighbor]) {
                    stack.push(make_pair(neighbor, depth + 1));
                    visited[neighbor] = true;
                    parent[neighbor] = current;
                }
            }
        }
    }

    // If no path found within depth limit, return an empty vector
    return vector<string>();
}

vector<string> iterativeDeepeningDFS(const unordered_map<string, vector<string> >& adjacencyMap, const string& start, const string& end) {
    // Perform depth-limited DFS with increasing depth limits
    for (int depthLimit = 0; ; ++depthLimit) {
        vector<string> path = depthLimitedDFS(adjacencyMap, start, end, depthLimit);
        if (!path.empty()) {
            return path; // Return the path if found
        }
    }
}

vector<string> bestFirstSearch(const unordered_map<string, vector<string> >& adjacencyMap, const string& start, const string& end) {
    priority_queue<pair<double, string>, vector<pair<double, string> >, greater<pair<double, string> > > pq; // Min-heap
    unordered_map<string, bool> visited;
    unordered_map<string, string> parent;
    unordered_map<string, double> heuristic;

    pq.push(make_pair(0, start));
    parent[start] = "";
    visited[start] = true;

    while (!pq.empty()) {
        auto [cost, node] = pq.top();
        pq.pop();

        if (node == end) {
            // Reconstruct the path and return it
            vector<string> path;
            while (node != "") {
                path.push_back(node);
                node = parent[node];
            }
            reverse(path.begin(), path.end());
            return path;
        }

        for (const auto& neighbor : adjacencyMap.at(node)) {
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                parent[neighbor] = node;
                pq.push(make_pair(heuristic[neighbor], neighbor));

            }
        }
    }

    return vector<string>(); // No path found
}

// Function to perform A* search
vector<string> aStarSearch(const unordered_map<string, vector<string> >& adjacencyMap, const string& start, const string& end) {
    // Implement A* search here
}

int main() {
    // Parse cities and adjacencies data files
    vector<City> cities;
    unordered_map<string, vector<string> > adjacencyMap;
    parseCitiesFile("coordinates.csv", cities);
    parseAdjacenciesFile("Adjacencies.txt", adjacencyMap);

    // Display menu and prompt user for input
    string start, end;
    cout << "Enter starting town: ";
    cin >> start;
    cout << "Enter ending town: ";
    cin >> end;

    // Select search method
    int choice;
    do {
        cout << "\nSelect search method:" << endl;
        cout << "1. Breadth-first search\n2. Depth-first search\n3. Iterative deepening depth-first search\n4. Best-first search\n5. A* search\n";
        cout << "Enter your choice (1-5): ";
        cin >> choice;

        vector<string> route;
        clock_t startTime, endTime;
        double totalTime;

        switch (choice) {
            case 1:
                startTime = clock();
                route = breadthFirstSearch(adjacencyMap, start, end);
                endTime = clock();
                totalTime = double(endTime - startTime) / CLOCKS_PER_SEC;
                break;
            case 2:
                startTime = clock();
                route = depthFirstSearch(adjacencyMap, start, end);
                endTime = clock();
                totalTime = double(endTime - startTime) / CLOCKS_PER_SEC;
                break;
            case 3:
                startTime = clock();
                route = iterativeDeepeningDFS(adjacencyMap, start, end);
                endTime = clock();
                totalTime = double(endTime - startTime) / CLOCKS_PER_SEC;
                break;
            case 4:
                startTime = clock();
                route = bestFirstSearch(adjacencyMap, start, end);
                endTime = clock();
                totalTime = double(endTime - startTime) / CLOCKS_PER_SEC;
                break;
            case 5:
                startTime = clock();
                route = aStarSearch(adjacencyMap, start, end);
                endTime = clock();
                totalTime = double(endTime - startTime) / CLOCKS_PER_SEC;
                break;
            default:
                cout << "Invalid choice. Please enter a number between 1 and 5." << endl;
                continue;
        }

        // Display route and statistics
        if (!route.empty()) {
            cout << "\nRoute found:" << endl;
            for (const string& city : route) {
                cout << city << " -> ";
            }
            cout << "Destination reached." << endl;
            cout << "Total time: " << totalTime << " seconds" << endl;
            // Calculate and display total distance
            // Optional: Determine total memory used
        } else {
            cout << "No route found." << endl;
        }

        // Ask user if they want to try another search method
        cout << "\nDo you want to try another search method? (1: Yes, 0: No): ";
        cin >> choice;
    } while (choice == 1);

    return 0;
}
