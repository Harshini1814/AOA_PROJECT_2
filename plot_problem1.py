import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Read data
p1 = pd.read_csv('data/flow_p1.csv')
p2 = pd.read_csv('data/flow_p2.csv')
p5 = pd.read_csv('data/flow_p5.csv')

# Group by size and compute mean runtime
p1_grouped = p1.groupby('size')['runtime_ms'].mean().reset_index()
p2_grouped = p2.groupby('size')['runtime_ms'].mean().reset_index()
p5_grouped = p5.groupby('size')['runtime_ms'].mean().reset_index()

# Figure 1: Runtime with Polynomial Fit
fig, ax = plt.subplots(figsize=(10, 6))

# Plot scatter points
ax.scatter(p1_grouped['size'], p1_grouped['runtime_ms'], label='p=0.1', alpha=0.6, s=80)
ax.scatter(p2_grouped['size'], p2_grouped['runtime_ms'], label='p=0.3', alpha=0.6, s=80)
ax.scatter(p5_grouped['size'], p5_grouped['runtime_ms'], label='p=0.5', alpha=0.6, s=80)

# Polynomial curve fitting for p=0.5 (densest, most representative)
x_data = p5_grouped['size'].values
y_data = p5_grouped['runtime_ms'].values

# Try different polynomial degrees and find best fit
degrees = [2, 3, 4]
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

ax.plot(x_smooth, y_smooth, 'r--', linewidth=2, 
        label=f'Polynomial fit (degree {best_degree}, p=0.5)')

# Add text with polynomial info
if best_degree == 2:
    complexity = f'O(n^{best_degree})'
elif best_degree == 3:
    complexity = f'O(n^{best_degree})'
else:
    complexity = f'O(n^{best_degree})'

ax.text(0.05, 0.95, f'Observed complexity: {complexity}',
        transform=ax.transAxes, fontsize=11, verticalalignment='top',
        bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.5))

ax.set_xlabel('Problem Size (n = drivers = requests)', fontsize=12)
ax.set_ylabel('Runtime (milliseconds)', fontsize=12)
ax.set_title('Runtime vs Problem Size for Max Flow Algorithm', fontsize=14, fontweight='bold')
ax.legend(fontsize=10)
ax.grid(True, alpha=0.3)
plt.tight_layout()
plt.savefig('figures/flow_runtime.png', dpi=150, bbox_inches='tight')
print(f"Saved flow_runtime.png with polynomial fit (degree {best_degree})")
plt.close()

# Figure 2: Matching Quality
fig, ax = plt.subplots(figsize=(10, 6))

# Calculate matching percentage
p1_grouped['match_pct'] = (p1.groupby('size')['flow_value'].mean() / p1_grouped['size']) * 100
p2_grouped['match_pct'] = (p2.groupby('size')['flow_value'].mean() / p2_grouped['size']) * 100
p5_grouped['match_pct'] = (p5.groupby('size')['flow_value'].mean() / p5_grouped['size']) * 100

ax.plot(p1_grouped['size'], p1_grouped['match_pct'], 'o-', label='Edge probability p=0.1', linewidth=2, markersize=8)
ax.plot(p2_grouped['size'], p2_grouped['match_pct'], 's-', label='Edge probability p=0.3', linewidth=2, markersize=8)
ax.plot(p5_grouped['size'], p5_grouped['match_pct'], '^-', label='Edge probability p=0.5', linewidth=2, markersize=8)

ax.set_xlabel('Problem Size (n)', fontsize=12)
ax.set_ylabel('Percentage of Requests Matched (%)', fontsize=12)
ax.set_title('Matching Quality for Different Edge Densities', fontsize=14, fontweight='bold')
ax.legend(fontsize=11)
ax.grid(True, alpha=0.3)
ax.set_ylim([0, 105])
plt.tight_layout()
plt.savefig('figures/flow_matching_quality.png', dpi=150, bbox_inches='tight')
print("Saved flow_matching_quality.png")
plt.close()

print("\nProblem 1 plots generated successfully!")
print(f"\nPolynomial Analysis:")
print(f"  Best fit degree: {best_degree}")
print(f"  Observed complexity: {complexity}")
print(f"  Theoretical: O(V^2 * E) where V = 2n+2, E ≈ n^2 * p")
print(f"  For dense graphs (p=0.5): O(n^2 * n^2) = O(n^4)")
print(f"  But Dinic's algorithm performs better in practice")
