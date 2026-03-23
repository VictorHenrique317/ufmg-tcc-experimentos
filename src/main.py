import argparse
import copy
import numpy as np
import os
import json
from src.experiment_configuration.configuration_reader import ConfigurationReader

def main():
    parser = argparse.ArgumentParser(description="Run community detection experiments.")
    parser.add_argument(
        '-e', '--experiments', 
        nargs='+', 
        help='Names of experiments to run. If not specified, all experiments are run.'
    )
    args = parser.parse_args()

    # 1. Ler os experimentos configurados no config.yaml
    reader = ConfigurationReader("config.yaml")
    all_experiments = reader.read_experiments()
    
    if not all_experiments:
        print("Nenhum experimento encontrado no config.yaml.")
        return

    # Filtrar experimentos se nomes forem fornecidos
    experiments_to_run = all_experiments
    if args.experiments:
        experiments_to_run = [
            (p, s) for p, s in all_experiments if s.name in args.experiments
        ]
        if not experiments_to_run:
            print(f"Nenhum experimento encontrado com os nomes: {', '.join(args.experiments)}")
            return

    # 2. Executar cada pipeline
    experiment_summaries = []
    for pipeline, base_state in experiments_to_run:
        print(f"Running experiment: {base_state.name} for {base_state.iterations} iterations")
        iteration_results = []
        base_seed = base_state.random_seed
        
        for i in range(base_state.iterations):
            iter_state = copy.deepcopy(base_state)
            iter_state.current_iteration = i + 1
            iter_state.random_seed = base_seed + i # Vary seed per iteration
            
            print(f"\n--- Iteration {iter_state.current_iteration}/{base_state.iterations} (seed: {iter_state.random_seed}) ---")
            
            final_state = pipeline.run(iter_state)
            iteration_results.append(final_state)
            
        # 3. Gerar sumário agregado para o experimento
        metrics_to_aggregate = {
            'detection_time': [res.detection_time for res in iteration_results if res.detection_time is not None],
            'num_detected_communities': [res.num_detected_communities for res in iteration_results if res.num_detected_communities is not None],
            'mean_jaccard': [res.mean_jaccard for res in iteration_results if res.mean_jaccard is not None],
            'top_k_mean_jaccard': [res.top_k_mean_jaccard for res in iteration_results if res.top_k_mean_jaccard is not None]
        }

        report = {}
        for metric_name, values in metrics_to_aggregate.items():
            if values:
                report[metric_name] = {
                    'mean': np.mean(values),
                    'median': np.median(values),
                    'p25': np.percentile(values, 25),
                    'p75': np.percentile(values, 75),
                    'std_dev': np.std(values),
                    'variance': np.var(values)
                }
        
        # Save report.json
        exp_name_fs = base_state.name.replace(' ', '_').replace('/', '_').replace(':', '')
        output_dir = os.path.join("results", exp_name_fs)
        os.makedirs(output_dir, exist_ok=True) # ensure experiment dir exists
        report_path = os.path.join(output_dir, "report.json")
        with open(report_path, 'w') as f:
            json.dump(report, f, indent=4)
        print(f"\nAggregated report saved to: {report_path}")

        # Store for final summary
        experiment_summaries.append({
            "name": base_state.name,
            "iterations": base_state.iterations,
            "report": report
        })
        
    # 4. Mostrar sumário final
    print("\n" + "="*60)
    print("SUMMARY OF AGGREGATED RESULTS")
    print("="*60)
    for summary in experiment_summaries:
        print(f"Experiment: {summary['name']} ({summary['iterations']} iterations)")
        report = summary.get("report", {})
        if report:
            for metric_name, stats in report.items():
                print(f"  Metric: {metric_name}")
                print(f"    - Mean: {stats['mean']:.4f}, Median: {stats['median']:.4f}")
                print(f"    - Std Dev: {stats['std_dev']:.4f}, Variance: {stats['variance']:.4f}")
                print(f"    - 25th percentile: {stats['p25']:.4f}, 75th percentile: {stats['p75']:.4f}")
        else:
            print("  No metrics were calculated for this experiment.")
        print("-" * 30)

if __name__ == "__main__":
    main()
