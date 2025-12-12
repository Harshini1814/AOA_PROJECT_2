/*
 * Problem 2: School Bus Stop Placement via Greedy Set Cover
 * ==========================================================
 * 
 * This program implements the greedy approximation algorithm for the school
 * bus stop placement problem, which reduces to the NP-complete Set Cover problem.
 * 
 * Author: Your Name
 * Date: December 11, 2025
 */

#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>
#include <fstream>
#include <iomanip>

using namespace std;
using namespace std::chrono;

// Represent a 2D point
struct Point {
    double x, y;
    
    double distance(const Point& other) const {
        double dx = x - other.x;
        double dy = y - other.y;
        return sqrt(dx * dx + dy * dy);
    }
};

// Bus Stop Placement Problem
class BusStopPlacement {
public:
    int num_students;
    int num_candidates;
    vector<set<int>> coverage;  // coverage[i] = students covered by stop i
    vector<Point> student_positions;
    vector<Point> stop_positions;
    
public:
    BusStopPlacement(int n_students, int n_candidates)
        : num_students(n_students), num_candidates(n_candidates) {
        coverage.resize(n_candidates);
        student_positions.resize(n_students);
        stop_positions.resize(n_candidates);
    }
    
    void set_coverage(int stop_id, const set<int>& covered_students) {
        coverage[stop_id] = covered_students;
    }
    
    void set_student_position(int student_id, Point pos) {
        student_positions[student_id] = pos;
    }
    
    void set_stop_position(int stop_id, Point pos) {
        stop_positions[stop_id] = pos;
    }
    
    // Greedy Set Cover Algorithm
    vector<int> greedy_set_cover() {
        vector<int> selected_stops;
        set<int> uncovered_students;
        
        // Initialize all students as uncovered
        for (int i = 0; i < num_students; i++) {
            uncovered_students.insert(i);
        }
        
        while (!uncovered_students.empty()) {
            int best_stop = -1;
            set<int> best_coverage;
            
            // Find stop that covers most uncovered students
            for (int j = 0; j < num_candidates; j++) {
                set<int> new_coverage;
                set_intersection(coverage[j].begin(), coverage[j].end(),
                               uncovered_students.begin(), uncovered_students.end(),
                               inserter(new_coverage, new_coverage.begin()));
                
                if (new_coverage.size() > best_coverage.size()) {
                    best_coverage = new_coverage;
                    best_stop = j;
                }
            }
            
            // If no stop covers any remaining students, problem is infeasible
            if (best_stop == -1 || best_coverage.empty()) {
                cerr << "Error: No feasible solution - some students cannot be covered" << endl;
                return vector<int>();
            }
            
            // Add best stop to solution
            selected_stops.push_back(best_stop);
            
            // Remove covered students from uncovered set
            for (int student : best_coverage) {
                uncovered_students.erase(student);
            }
        }
        
        return selected_stops;
    }
    
    // Verify solution covers all students
    bool verify_solution(const vector<int>& selected_stops) {
        set<int> covered;
        for (int stop_id : selected_stops) {
            covered.insert(coverage[stop_id].begin(), coverage[stop_id].end());
        }
        return covered.size() == num_students;
    }
    
    void print_stats(const vector<int>& solution) {
        cout << "Solution Statistics:" << endl;
        cout << "  Students: " << num_students << endl;
        cout << "  Candidate stops: " << num_candidates << endl;
        cout << "  Stops selected: " << solution.size() << endl;
        cout << "  Coverage verified: " << (verify_solution(solution) ? "Yes" : "No") << endl;
    }
    
    void save_visualization_data(const vector<int>& solution, const string& filename) {
        ofstream out(filename);
        
        // Save students
        out << "STUDENTS" << endl;
        for (int i = 0; i < num_students; i++) {
            out << student_positions[i].x << "," << student_positions[i].y << endl;
        }
        
        // Save selected stops
        out << "SELECTED_STOPS" << endl;
        for (int stop_id : solution) {
            out << stop_positions[stop_id].x << "," << stop_positions[stop_id].y << endl;
        }
        
        // Save unselected stops
        out << "UNSELECTED_STOPS" << endl;
        set<int> selected_set(solution.begin(), solution.end());
        for (int i = 0; i < num_candidates; i++) {
            if (selected_set.find(i) == selected_set.end()) {
                out << stop_positions[i].x << "," << stop_positions[i].y << endl;
            }
        }
        
        out.close();
    }
};

// Generate coordinate-based instance
BusStopPlacement* generate_coordinate_based_instance(
    int num_students, int num_candidates, double walking_distance,
    double grid_size, bool use_grid_stops, mt19937& rng) {
    
    BusStopPlacement* problem = new BusStopPlacement(num_students, num_candidates);
    
    uniform_real_distribution<double> dist(0.0, grid_size);
    
    // Generate random student positions
    for (int i = 0; i < num_students; i++) {
        Point p{dist(rng), dist(rng)};
        problem->set_student_position(i, p);
    }
    
    // Generate candidate stop positions
    if (use_grid_stops) {
        // Grid placement - ensure dense enough coverage
        int grid_dim = max((int)ceil(sqrt(num_candidates)), 
                          (int)ceil(grid_size / (walking_distance * 0.7)));
        
        vector<Point> all_grid_stops;
        for (int i = 0; i < grid_dim; i++) {
            for (int j = 0; j < grid_dim; j++) {
                double x = i * grid_size / (grid_dim - 1);
                double y = j * grid_size / (grid_dim - 1);
                all_grid_stops.push_back({x, y});
            }
        }
        
        // Use all grid stops or limit to num_candidates
        int actual_candidates = min((int)all_grid_stops.size(), num_candidates);
        for (int i = 0; i < actual_candidates; i++) {
            problem->set_stop_position(i, all_grid_stops[i]);
        }
        num_candidates = actual_candidates;
    } else {
        // Random placement
        for (int i = 0; i < num_candidates; i++) {
            Point p{dist(rng), dist(rng)};
            problem->set_stop_position(i, p);
        }
    }
    
    // Compute coverage based on Euclidean distance
    for (int j = 0; j < num_candidates; j++) {
        set<int> covered_students;
        Point stop_pos = problem->stop_positions[j];
        
        for (int i = 0; i < num_students; i++) {
            Point student_pos = problem->student_positions[i];
            if (stop_pos.distance(student_pos) <= walking_distance) {
                covered_students.insert(i);
            }
        }
        
        problem->set_coverage(j, covered_students);
    }
    
    return problem;
}

// Benchmark runtime
void benchmark_runtime(vector<int>& sizes, double walking_distance, int trials, 
                      const string& output_file) {
    ofstream out(output_file);
    out << "size,runtime_ms,num_stops" << endl;
    
    random_device rd;
    mt19937 rng(rd());
    
    for (int size : sizes) {
        cout << "Benchmarking size " << size << "..." << endl;
        
        for (int trial = 0; trial < trials; trial++) {
            int num_candidates = max(size, 100);  // At least as many candidates as students
            
            auto problem = generate_coordinate_based_instance(
                size, num_candidates, walking_distance, 1000.0, true, rng);
            
            auto start = high_resolution_clock::now();
            auto solution = problem->greedy_set_cover();
            auto end = high_resolution_clock::now();
            
            auto duration = duration_cast<microseconds>(end - start);
            double runtime_ms = duration.count() / 1000.0;
            
            if (!solution.empty()) {
                out << size << "," << runtime_ms << "," << solution.size() << endl;
            }
            
            delete problem;
        }
    }
    
    out.close();
    cout << "Results saved to " << output_file << endl;
}

// Compare walking distances
void compare_walking_distances(int size, vector<double>& walking_distances, 
                               int trials, const string& output_file) {
    ofstream out(output_file);
    out << "walking_distance,num_stops" << endl;
    
    random_device rd;
    mt19937 rng(rd());
    
    for (double distance : walking_distances) {
        cout << "Testing walking distance " << distance << "..." << endl;
        
        for (int trial = 0; trial < trials; trial++) {
            int num_candidates = max((int)(3 * sqrt(size)), 30);
            
            auto problem = generate_coordinate_based_instance(
                size, num_candidates, distance, 1000.0, true, rng);
            
            auto solution = problem->greedy_set_cover();
            
            if (!solution.empty()) {
                out << distance << "," << solution.size() << endl;
            }
            
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
    
    // Create small instance manually
    BusStopPlacement problem(6, 4);
    
    // Define coverage manually
    problem.set_coverage(0, {0, 1, 2});
    problem.set_coverage(1, {1, 3, 4});
    problem.set_coverage(2, {2, 4, 5});
    problem.set_coverage(3, {3, 5});
    
    cout << "Problem Setup:" << endl;
    cout << "  Students: 6" << endl;
    cout << "  Candidate stops: 4" << endl;
    cout << "  Coverage:" << endl;
    cout << "    Stop 0 → Students {0, 1, 2}" << endl;
    cout << "    Stop 1 → Students {1, 3, 4}" << endl;
    cout << "    Stop 2 → Students {2, 4, 5}" << endl;
    cout << "    Stop 3 → Students {3, 5}" << endl;
    
    // Solve
    auto solution = problem.greedy_set_cover();
    
    cout << "\nGreedy Solution:" << endl;
    cout << "  Number of stops: " << solution.size() << endl;
    cout << "  Selected stops: ";
    for (int i = 0; i < solution.size(); i++) {
        if (i > 0) cout << ", ";
        cout << solution[i];
    }
    cout << endl;
    
    cout << "\nNote: For this instance, the optimal solution uses 2 stops." << endl;
    cout << "      For example: Stops {0, 1} or Stops {0, 3}" << endl;
    cout << "      Greedy may not always find optimal (expected for NP-hard problem)\n" << endl;
    
    problem.print_stats(solution);
}

int main(int argc, char* argv[]) {
    cout << "============================================================" << endl;
    cout << "Problem 2: School Bus Stop Placement Experiments" << endl;
    cout << "============================================================\n" << endl;
    
    // Run demonstration
    demo_example();
    
    // Run benchmarks if requested
    if (argc > 1 && string(argv[1]) == "--benchmark") {
        cout << "\n============================================================" << endl;
        cout << "Running Benchmarks" << endl;
        cout << "============================================================\n" << endl;
        
        // Runtime analysis
        cout << "\nRuntime analysis..." << endl;
        vector<int> sizes = {100, 200, 300, 400, 500, 750, 1000};
        double walking_distance = 200.0;
        benchmark_runtime(sizes, walking_distance, 5, "data/setcover_runtime.csv");
        
        // Walking distance comparison
        cout << "\nWalking distance comparison..." << endl;
        vector<double> distances = {150, 200, 250, 300};
        vector<int> test_sizes = {100, 150, 200};
        
        for (int size : test_sizes) {
            string filename = "data/setcover_distance_n" + to_string(size) + ".csv";
            compare_walking_distances(size, distances, 10, filename);
        }
        
        // Generate visualization data
        cout << "\nGenerating visualization data..." << endl;
        random_device rd;
        mt19937 rng(rd());
        auto vis_problem = generate_coordinate_based_instance(80, 80, 250.0, 1000.0, true, rng);
        auto vis_solution = vis_problem->greedy_set_cover();
        vis_problem->save_visualization_data(vis_solution, "data/visualization.csv");
        delete vis_problem;
        
        cout << "\n============================================================" << endl;
        cout << "Benchmarks completed!" << endl;
        cout << "Run Python script to generate plots from CSV data." << endl;
        cout << "============================================================" << endl;
    } else {
        cout << "\nTo run benchmarks, use: " << argv[0] << " --benchmark" << endl;
    }
    
    return 0;
}
