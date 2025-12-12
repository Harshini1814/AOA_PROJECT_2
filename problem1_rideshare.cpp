/*
 * Problem 1: Ride-Share Driver Assignment via Maximum Flow
 * =========================================================
 * 
 * This program implements the reduction from the ride-share driver assignment
 * problem to maximum flow using Dinic's algorithm.
 * 
 * Author: Your Name
 * Date: December 11, 2025
 */

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <cstring>
#include <chrono>
#include <random>
#include <fstream>

using namespace std;
using namespace std::chrono;

const int INF = 1e9;

// Dinic's Algorithm for Maximum Flow
class MaxFlow {
private:
    struct Edge {
        int to, rev;
        int cap;
    };
    
    vector<vector<Edge>> graph;
    vector<int> level, iter;
    int n;
    
    void bfs(int s) {
        fill(level.begin(), level.end(), -1);
        queue<int> q;
        level[s] = 0;
        q.push(s);
        
        while (!q.empty()) {
            int v = q.front();
            q.pop();
            for (auto& e : graph[v]) {
                if (e.cap > 0 && level[e.to] < 0) {
                    level[e.to] = level[v] + 1;
                    q.push(e.to);
                }
            }
        }
    }
    
    int dfs(int v, int t, int f) {
        if (v == t) return f;
        for (int& i = iter[v]; i < graph[v].size(); i++) {
            Edge& e = graph[v][i];
            if (e.cap > 0 && level[v] < level[e.to]) {
                int d = dfs(e.to, t, min(f, e.cap));
                if (d > 0) {
                    e.cap -= d;
                    graph[e.to][e.rev].cap += d;
                    return d;
                }
            }
        }
        return 0;
    }
    
public:
    MaxFlow(int n) : n(n), graph(n), level(n), iter(n) {}
    
    void add_edge(int from, int to, int cap) {
        graph[from].push_back({to, (int)graph[to].size(), cap});
        graph[to].push_back({from, (int)graph[from].size() - 1, 0});
    }
    
    int max_flow(int s, int t) {
        int flow = 0;
        while (true) {
            bfs(s);
            if (level[t] < 0) return flow;
            fill(iter.begin(), iter.end(), 0);
            int f;
            while ((f = dfs(s, t, INF)) > 0) {
                flow += f;
            }
        }
    }
    
    // Get flow on edge from -> to
    int get_flow(int from, int to) {
        for (auto& e : graph[from]) {
            if (e.to == to) {
                // Find reverse edge to get actual flow
                for (auto& re : graph[to]) {
                    if (re.to == from) {
                        return re.cap;  // Reverse edge capacity = flow
                    }
                }
            }
        }
        return 0;
    }
};

// Ride-Share Assignment Problem
class RideShareAssignment {
private:
    int num_drivers;
    int num_requests;
    vector<int> driver_capacities;
    vector<vector<bool>> feasible_edges;
    MaxFlow* flow_graph;
    
    int source;
    int sink;
    int driver_offset;
    int request_offset;
    
public:
    RideShareAssignment(int m, int n) 
        : num_drivers(m), num_requests(n) {
        driver_capacities.resize(m, 1);
        feasible_edges.resize(m, vector<bool>(n, false));
        
        // Node numbering: 0=source, 1..m=drivers, m+1..m+n=requests, m+n+1=sink
        source = 0;
        driver_offset = 1;
        request_offset = driver_offset + num_drivers;
        sink = request_offset + num_requests;
        
        flow_graph = nullptr;
    }
    
    ~RideShareAssignment() {
        if (flow_graph) delete flow_graph;
    }
    
    void set_driver_capacity(int driver_id, int capacity) {
        driver_capacities[driver_id] = capacity;
    }
    
    void add_feasible_assignment(int driver_id, int request_id) {
        feasible_edges[driver_id][request_id] = true;
    }
    
    void build_flow_network() {
        int total_nodes = num_drivers + num_requests + 2;
        flow_graph = new MaxFlow(total_nodes);
        
        // Source to drivers
        for (int i = 0; i < num_drivers; i++) {
            flow_graph->add_edge(source, driver_offset + i, driver_capacities[i]);
        }
        
        // Drivers to requests
        for (int i = 0; i < num_drivers; i++) {
            for (int j = 0; j < num_requests; j++) {
                if (feasible_edges[i][j]) {
                    flow_graph->add_edge(driver_offset + i, request_offset + j, 1);
                }
            }
        }
        
        // Requests to sink
        for (int j = 0; j < num_requests; j++) {
            flow_graph->add_edge(request_offset + j, sink, 1);
        }
    }
    
    pair<int, vector<pair<int, int>>> solve() {
        if (!flow_graph) {
            build_flow_network();
        }
        
        int max_flow_value = flow_graph->max_flow(source, sink);
        
        // Extract assignments
        vector<pair<int, int>> assignments;
        for (int i = 0; i < num_drivers; i++) {
            for (int j = 0; j < num_requests; j++) {
                if (feasible_edges[i][j]) {
                    int flow = flow_graph->get_flow(driver_offset + i, request_offset + j);
                    if (flow > 0) {
                        assignments.push_back({i, j});
                    }
                }
            }
        }
        
        return {max_flow_value, assignments};
    }
    
    void print_stats() {
        int num_edges = 0;
        for (int i = 0; i < num_drivers; i++) {
            for (int j = 0; j < num_requests; j++) {
                if (feasible_edges[i][j]) num_edges++;
            }
        }
        
        int total_nodes = num_drivers + num_requests + 2;
        int total_edges = num_drivers + num_edges + num_requests;
        
        cout << "Flow Network Statistics:" << endl;
        cout << "  Nodes: " << total_nodes << endl;
        cout << "  Edges: " << total_edges << endl;
        cout << "  Drivers: " << num_drivers << endl;
        cout << "  Requests: " << num_requests << endl;
        cout << "  Feasible edges: " << num_edges << endl;
    }
};

// Generate random instance
RideShareAssignment* generate_random_instance(int num_drivers, int num_requests, 
                                              double edge_probability, 
                                              mt19937& rng) {
    RideShareAssignment* problem = new RideShareAssignment(num_drivers, num_requests);
    
    // Set driver capacities (all 1 for simplicity)
    for (int i = 0; i < num_drivers; i++) {
        problem->set_driver_capacity(i, 1);
    }
    
    // Generate feasible edges randomly
    uniform_real_distribution<double> dist(0.0, 1.0);
    for (int i = 0; i < num_drivers; i++) {
        for (int j = 0; j < num_requests; j++) {
            if (dist(rng) < edge_probability) {
                problem->add_feasible_assignment(i, j);
            }
        }
    }
    
    return problem;
}

// Benchmark runtime
void benchmark_runtime(vector<int>& sizes, double edge_probability, 
                      int trials, const string& output_file) {
    ofstream out(output_file);
    out << "size,runtime_ms,flow_value,num_edges" << endl;
    
    random_device rd;
    mt19937 rng(rd());
    
    for (int size : sizes) {
        cout << "Benchmarking size " << size << "..." << endl;
        
        for (int trial = 0; trial < trials; trial++) {
            auto problem = generate_random_instance(size, size, edge_probability, rng);
            
            auto start = high_resolution_clock::now();
            auto result = problem->solve();
            auto end = high_resolution_clock::now();
            
            auto duration = duration_cast<microseconds>(end - start);
            double runtime_ms = duration.count() / 1000.0;
            
            out << size << "," << runtime_ms << "," 
                << result.first << "," << result.second.size() << endl;
            
            delete problem;
        }
    }
    
    out.close();
    cout << "Results saved to " << output_file << endl;
}

// Demonstration example
void demo_example() {
    cout << "\n============================================================" << endl;
    cout << "Demonstration: Small Example" << endl;
    cout << "============================================================\n" << endl;
    
    // Create small instance: 3 drivers, 4 requests
    RideShareAssignment problem(3, 4);
    
    // Set capacities
    for (int i = 0; i < 3; i++) {
        problem.set_driver_capacity(i, 1);
    }
    
    // Define feasible assignments
    problem.add_feasible_assignment(0, 0);
    problem.add_feasible_assignment(0, 1);
    problem.add_feasible_assignment(1, 1);
    problem.add_feasible_assignment(1, 2);
    problem.add_feasible_assignment(2, 2);
    problem.add_feasible_assignment(2, 3);
    
    cout << "Problem Setup:" << endl;
    cout << "  Drivers: 3 (each with capacity 1)" << endl;
    cout << "  Requests: 4" << endl;
    cout << "  Feasible assignments:" << endl;
    cout << "    Driver 0 → Requests {0, 1}" << endl;
    cout << "    Driver 1 → Requests {1, 2}" << endl;
    cout << "    Driver 2 → Requests {2, 3}" << endl;
    
    // Solve
    auto result = problem.solve();
    int flow_value = result.first;
    auto assignments = result.second;
    
    cout << "\nSolution:" << endl;
    cout << "  Maximum flow value: " << flow_value << endl;
    cout << "  Assignments:" << endl;
    for (auto& p : assignments) {
        cout << "    Driver " << p.first << " → Request " << p.second << endl;
    }
    
    cout << endl;
    problem.print_stats();
}

int main(int argc, char* argv[]) {
    cout << "============================================================" << endl;
    cout << "Problem 1: Ride-Share Driver Assignment Experiments" << endl;
    cout << "============================================================\n" << endl;
    
    // Run demonstration
    demo_example();
    
    // Run benchmarks if requested
    if (argc > 1 && string(argv[1]) == "--benchmark") {
        cout << "\n============================================================" << endl;
        cout << "Running Benchmarks" << endl;
        cout << "============================================================\n" << endl;
        
        vector<int> sizes = {50, 100, 200, 300, 400, 500, 750, 1000};
        vector<double> edge_probs = {0.1, 0.3, 0.5};
        int trials = 5;
        
        for (double p : edge_probs) {
            cout << "\nEdge probability p = " << p << endl;
            string filename = "data/flow_p" + to_string((int)(p * 10)) + ".csv";
            benchmark_runtime(sizes, p, trials, filename);
        }
        
        cout << "\n============================================================" << endl;
        cout << "Benchmarks completed!" << endl;
        cout << "Run Python script to generate plots from CSV data." << endl;
        cout << "============================================================" << endl;
    } else {
        cout << "\nTo run benchmarks, use: " << argv[0] << " --benchmark" << endl;
    }
    
    return 0;
}
