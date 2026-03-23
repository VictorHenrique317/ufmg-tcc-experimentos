import os
import json
import glob
import pandas as pd
import matplotlib.pyplot as plt
import re

def parse_experiment_name(name):
    """
    Parses a standardized experiment name to extract parameters.
    Format: {alg}_V{v}_C{c}_N{n}
    Example: louvain_V200_C10_N16
    """
    match = re.match(r'(.*)_V(\d+)_C(\d+)_N([\d.]+)', name)
    if not match:
        return None
    
    algorithm, vertices, communities, noise = match.groups()
    return {
        'algorithm': algorithm,
        'vertices': int(vertices),
        'communities': int(communities),
        'noise': float(noise)
    }

def create_charts():
    """
    Finds all report.json files, aggregates the data, and generates
    line charts comparing algorithm performance across noise levels.
    """
    print("Searching for report files...")
    report_files = glob.glob('results/*/report.json')
    if not report_files:
        print("No report.json files found. Please run experiments first.")
        return

    all_data = []
    for report_file in report_files:
        exp_name_fs = os.path.basename(os.path.dirname(report_file))
        params = parse_experiment_name(exp_name_fs)
        if not params:
            print(f"Warning: Could not parse experiment name '{exp_name_fs}'. Skipping.")
            continue
            
        with open(report_file, 'r') as f:
            report_data = json.load(f)
        
        # Extract mean values for the primary metrics
        mean_jaccard = report_data.get('mean_jaccard', {}).get('mean')
        top_k_mean_jaccard = report_data.get('top_k_mean_jaccard', {}).get('mean')
        
        if mean_jaccard is not None and top_k_mean_jaccard is not None:
            all_data.append({
                **params,
                'mean_jaccard': mean_jaccard,
                'top_k_mean_jaccard': top_k_mean_jaccard
            })

    if not all_data:
        print("No valid data could be extracted from report files.")
        return

    df = pd.DataFrame(all_data)
    
    # Noise levels are inverted (higher number = less noise), so we sort descending
    # to show the effect of *increasing* noise from left to right.
    noise_order = sorted(df['noise'].unique(), reverse=True)
    df['noise'] = pd.Categorical(df['noise'], categories=noise_order, ordered=True)

    output_dir = "charts"
    os.makedirs(output_dir, exist_ok=True)
    print(f"Generating charts in '{output_dir}/'")

    # Group by graph configuration to create a set of charts for each
    for (vertices, communities), group in df.groupby(['vertices', 'communities']):
        fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 12), sharex=True)
        fig.suptitle(f"""Algorithm Performance vs. Noise
(Graph: {vertices} Vertices, {communities} Communities)""", fontsize=16)

        # Plot 1: Mean Jaccard
        for algorithm in group['algorithm'].unique():
            subset = group[group['algorithm'] == algorithm].sort_values('noise')
            ax1.plot(subset['noise'].astype(str), subset['mean_jaccard'], marker='o', linestyle='-', label=algorithm)
        
        ax1.set_title('Mean Jaccard Index')
        ax1.set_ylabel('Jaccard Index (Mean)')
        ax1.legend()
        ax1.grid(True, which='both', linestyle='--', linewidth=0.5)

        # Plot 2: Top-K Mean Jaccard
        for algorithm in group['algorithm'].unique():
            subset = group[group['algorithm'] == algorithm].sort_values('noise')
            ax2.plot(subset['noise'].astype(str), subset['top_k_mean_jaccard'], marker='o', linestyle='-', label=algorithm)

        ax2.set_title(f'Top-{communities} Mean Jaccard Index')
        ax2.set_xlabel('Noise Level (Number of Correct Observations)')
        ax2.set_ylabel('Jaccard Index (Top-K Mean)')
        ax2.legend()
        ax2.grid(True, which='both', linestyle='--', linewidth=0.5)
        
        # Save the combined figure
        chart_filename = f'V{vertices}_C{communities}_performance.png'
        chart_path = os.path.join(output_dir, chart_filename)
        plt.tight_layout(rect=[0, 0, 1, 0.96])
        plt.savefig(chart_path)
        plt.close(fig)
        print(f"  - Saved {chart_path}")

def main():
    create_charts()

if __name__ == "__main__":
    main()
