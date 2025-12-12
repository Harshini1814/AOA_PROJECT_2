# AOA PROJECT2

This project implements and analyzes two algorithmic problems from real-world applications:
1. **Ride-Share Driver Assignment** (reduces to Maximum Flow)
2. **School Bus Stop Placement** (reduces to Set Cover - NP-complete)

# Authors
1. Bhoomika Mudi - 99602766
2. Harshini Sangem - 50167090



## Project Structure

```
project2_bhoomi/
├── main.tex                    # Complete LaTeX report (ACM format)
├── problem1_rideshare.cpp      # Max Flow implementation (Dinic's algorithm)
├── problem2_busstop.cpp        # Set Cover implementation (Greedy algorithm)
├── plot_problem1.py            # Plotting script for Problem 1
├── plot_problem2.py            # Plotting script for Problem 2
├── data/                       # Generated benchmark data (CSV files)
│   ├── flow_p1.csv
│   ├── flow_p2.csv
│   ├── flow_p5.csv
│   ├── setcover_runtime.csv
│   └── ...
└── figures/                    # Generated plots (PNG files)
    ├── flow_runtime.png
    ├── flow_matching_quality.png
    ├── setcover_runtime.png
    ├── setcover_stops.png
    └── setcover_visualization.png
```

## Problem 1: Ride-Share Driver Assignment

**Real-world problem**: Match drivers to ride requests based on location/time feasibility.

**Reduction**: Converts to Maximum Flow problem using bipartite graph:
- Source → Drivers (capacity = driver capacity)
- Drivers → Requests (edge if feasible)
- Requests → Sink (capacity = 1)

**Algorithm**: Dinic's algorithm for Maximum Flow
- **Complexity**: O(V² × E) where V = O(n), E = O(n²)
- **Observed**: O(n⁴) for dense graphs (matches theory)

### Compilation and Execution

```powershell
# Compile
g++ -std=c++17 -O2 -o problem1_rideshare.exe problem1_rideshare.cpp

# Run demo
.\problem1_rideshare.exe

# Run benchmarks (generates CSV data)
.\problem1_rideshare.exe --benchmark
```

**Demo Output**: Shows a small 3-driver, 4-request example with optimal assignment.

**Benchmark Output**: Tests problem sizes from 50 to 1000 with edge probabilities 0.1, 0.3, 0.5.

## Problem 2: School Bus Stop Placement

**Real-world problem**: Place minimum bus stops so all students can walk to a stop.

**Reduction**: Converts to Set Cover problem:
- Universe = Students
- Sets = Coverage of each candidate stop
- Goal: Minimum stops to cover all students

**Algorithm**: Greedy Set Cover
- **Complexity**: O(|U| × |S|) where |U| = students, |S| = candidate stops
- **Approximation**: ln(n) worst-case, observed ~0.56 (much better!)

### Compilation and Execution

```powershell
# Compile
g++ -std=c++17 -O2 -o problem2_busstop.exe problem2_busstop.cpp

# Run demo
.\problem2_busstop.exe

# Run benchmarks (generates CSV data)
.\problem2_busstop.exe --benchmark
```

**Demo Output**: Shows a 6-student, 4-stop example with greedy solution.

**Benchmark Output**: Tests problem sizes from 100 to 1000 students with coordinate-based instances.

## Generating Plots

**Requirements**: Python 3 with pandas, matplotlib, numpy

```powershell
# Install dependencies (if needed)
pip install pandas matplotlib numpy

# Generate Problem 1 plots
python plot_problem1.py

# Generate Problem 2 plots
python plot_problem2.py
```

**Generated Figures**:

### Problem 1
- `flow_runtime.png`: Runtime vs problem size with polynomial fit (O(n⁴))
- `flow_matching_quality.png`: Matching percentage vs edge density

### Problem 2
- `setcover_runtime.png`: Runtime vs problem size with polynomial fit (O(n³))
- `setcover_stops.png`: Approximation quality analysis (greedy vs optimal)
- `setcover_visualization.png`: 2D visualization of bus stop placement

## Compiling the LaTeX Report

**Option 1: Overleaf (Recommended)**
1. Upload `main.tex` and all PNG files from `figures/` directory
2. Compile with pdfLaTeX
3. Download the PDF

**Option 2: Local LaTeX Installation**
```powershell
pdflatex main.tex
bibtex main
pdflatex main.tex
pdflatex main.tex
```

## Key Results

### Problem 1: Maximum Flow
- **Observed Complexity**: O(n⁴) for dense graphs (p=0.5)
- **Theoretical Match**: O(V² × E) = O(n² × n²) = O(n⁴) ✓
- **Matching Quality**: 95-100% for p ≥ 0.3

### Problem 2: Set Cover
- **Observed Complexity**: O(n³) for greedy algorithm
- **Approximation Ratio**: 0.56 ± 0.18 (5-10× better than ln(n) worst-case!)
- **Practical Performance**: Greedy finds near-optimal solutions efficiently

## Implementation Highlights

### Problem 1 Features
- Custom Dinic's algorithm implementation
- BFS for level graph construction
- DFS for blocking flow computation
- Benchmark mode with configurable edge densities

### Problem 2 Features
- Coordinate-based instance generation
- Greedy set cover with coverage tracking
- Solution verification
- Visualization data export


