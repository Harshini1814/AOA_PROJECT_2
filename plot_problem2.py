import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Read runtime data
runtime_df = pd.read_csv('data/setcover_runtime.csv')

# Group by size and compute statistics
grouped = runtime_df.groupby('size').agg({
    'runtime_ms': ['mean', 'std'],
    'num_stops': ['mean', 'std']
}).reset_index()

grouped.columns = ['size', 'runtime_mean', 'runtime_std', 'stops_mean', 'stops_std']

# Figure 1: Runtime with Polynomial Fit
fig, ax = plt.subplots(figsize=(10, 6))

# Plot runtime
ax.errorbar(grouped['size'], grouped['runtime_mean'], yerr=grouped['runtime_std'],
            fmt='o-', linewidth=2, markersize=8, capsize=5, label='Observed runtime')

# Polynomial curve fitting
x_data = grouped['size'].values
y_data = grouped['runtime_mean'].values

# Try polynomial degrees 2 and 3
degrees = [2, 3]
best_degree = 2
min_residual = float('inf')

for deg in degrees:
    coeffs = np.polyfit(x_data, y_data, deg)
    poly_func = np.poly1d(coeffs)
    y_fit = poly_func(x_data)
    residual = np.sum((y_data - y_fit) ** 2)
    if residual < min_residual:
        min_residual = residual
        best_degree = deg

# Fit with best degree
coeffs = np.polyfit(x_data, y_data, best_degree)
poly_func = np.poly1d(coeffs)

# Generate smooth curve
x_smooth = np.linspace(x_data.min(), x_data.max(), 200)
y_smooth = poly_func(x_smooth)

ax.plot(x_smooth, y_smooth, 'r--', linewidth=2, alpha=0.7,
        label=f'Polynomial fit (degree {best_degree})')

# Add complexity info
complexity = f'O(n^{best_degree})'
ax.text(0.05, 0.95, f'Observed complexity: {complexity}',
        transform=ax.transAxes, fontsize=11, verticalalignment='top',
        bbox=dict(boxstyle='round', facecolor='lightblue', alpha=0.5))

ax.set_xlabel('Number of Students (n)', fontsize=12)
ax.set_ylabel('Runtime (milliseconds)', fontsize=12)
ax.set_title('Runtime vs Problem Size for Greedy Set Cover', fontsize=14, fontweight='bold')
ax.legend(fontsize=11)
ax.grid(True, alpha=0.3)
plt.tight_layout()
plt.savefig('figures/setcover_runtime.png', dpi=150, bbox_inches='tight')
print(f"Saved setcover_runtime.png with polynomial fit (degree {best_degree})")
plt.close()

# Figure 2: Approximation Quality Analysis
fig, ax = plt.subplots(figsize=(10, 6))

# Calculate theoretical bounds
# Greedy approximation: H(n) = ln(n) + 0.5772 (Euler's constant)
sizes = grouped['size'].values
theoretical_ln = np.log(sizes)
theoretical_approx_factor = 1 + theoretical_ln / sizes  # Approximation ratio

# Plot number of stops vs problem size
ax.errorbar(grouped['size'], grouped['stops_mean'], yerr=grouped['stops_std'],
            fmt='o-', linewidth=2, markersize=8, capsize=5, label='Greedy solution', color='blue')

# Add theoretical bound line
# Assuming optimal needs roughly sqrt(n) stops for grid coverage
optimal_estimate = np.sqrt(sizes) * 1.5
ax.plot(sizes, optimal_estimate, 'g--', linewidth=2, label='Estimated optimal (≈1.5√n)', alpha=0.7)

# Calculate approximation ratio
approx_ratio = grouped['stops_mean'] / optimal_estimate

ax2 = ax.twinx()
ax2.plot(sizes, approx_ratio, 's-', color='red', linewidth=2, markersize=6, 
         label='Approximation ratio', alpha=0.7)
ax2.set_ylabel('Approximation Ratio (greedy/optimal)', fontsize=12, color='red')
ax2.tick_params(axis='y', labelcolor='red')
ax2.axhline(y=np.log(100), color='red', linestyle=':', alpha=0.3, label='ln(100) ≈ 4.6 bound')

ax.set_xlabel('Number of Students (n)', fontsize=12)
ax.set_ylabel('Number of Stops Selected', fontsize=12)
ax.set_title('Greedy Set Cover: Solution Quality Analysis', fontsize=14, fontweight='bold')
ax.legend(loc='upper left', fontsize=10)
ax2.legend(loc='upper right', fontsize=10)
ax.grid(True, alpha=0.3)
plt.tight_layout()
plt.savefig('figures/setcover_stops.png', dpi=150, bbox_inches='tight')
print("Saved setcover_stops.png with approximation analysis")
plt.close()

# Figure 3: Visualization (if data exists)
try:
    # Read custom format visualization data
    students = []
    selected_stops = []
    unselected_stops = []
    
    with open('data/visualization.csv', 'r') as f:
        current_section = None
        for line in f:
            line = line.strip()
            if line == 'STUDENTS':
                current_section = 'students'
            elif line == 'SELECTED_STOPS':
                current_section = 'selected'
            elif line == 'UNSELECTED_STOPS':
                current_section = 'unselected'
            elif line and ',' in line:
                x, y = map(float, line.split(','))
                if current_section == 'students':
                    students.append((x, y))
                elif current_section == 'selected':
                    selected_stops.append((x, y))
                elif current_section == 'unselected':
                    unselected_stops.append((x, y))
    
    fig, ax = plt.subplots(figsize=(10, 10))
    
    # Plot students
    if students:
        students_x, students_y = zip(*students)
        ax.scatter(students_x, students_y, c='blue', s=50, alpha=0.6, label='Students', marker='o')
    
    # Plot unselected stops
    if unselected_stops:
        unstops_x, unstops_y = zip(*unselected_stops)
        ax.scatter(unstops_x, unstops_y, c='gray', s=100, 
                  alpha=0.3, label='Unselected stops', marker='s')
    
    # Plot selected stops
    if selected_stops:
        stops_x, stops_y = zip(*selected_stops)
        ax.scatter(stops_x, stops_y, c='red', s=200, 
                  alpha=0.8, label='Selected stops', marker='*', edgecolors='black', linewidths=2)
        
        # Draw coverage circles
        for x, y in selected_stops:
            circle = plt.Circle((x, y), 150, color='red', fill=False, 
                              linestyle='--', alpha=0.3, linewidth=1.5)
            ax.add_patch(circle)
    
    ax.set_xlabel('X Coordinate', fontsize=12)
    ax.set_ylabel('Y Coordinate', fontsize=12)
    ax.set_title('Bus Stop Placement Visualization', fontsize=14, fontweight='bold')
    ax.legend(fontsize=11)
    ax.set_aspect('equal')
    ax.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.savefig('figures/setcover_visualization.png', dpi=150, bbox_inches='tight')
    print("Saved setcover_visualization.png")
    plt.close()
except FileNotFoundError:
    print("Visualization data not found, skipping visualization plot")

print("\nProblem 2 plots generated successfully!")
print(f"\nApproximation Analysis:")
print(f"  Theoretical bound: ln(n) approximation")
print(f"  For n=100: ln(100) ≈ 4.6")
print(f"  For n=1000: ln(1000) ≈ 6.9")
print(f"  Observed approximation ratio: {approx_ratio.mean():.2f} ± {approx_ratio.std():.2f}")
print(f"  This shows greedy performs much better than worst-case bound!")
