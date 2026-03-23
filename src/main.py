import argparse
import copy
import numpy as np
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
        summary = {
            "name": base_state.name,
            "iterations": base_state.iterations,
            "number_of_ground_truth_communities": base_state.number_of_ground_truth_communities,
            "aggregated_metrics": {}
        }
        
        # Collect metrics from all iterations
        all_mean_jaccards = [res.mean_jaccard for res in iteration_results if res.mean_jaccard is not None]
        all_top_k_jaccards = [res.top_k_mean_jaccard for res in iteration_results if res.top_k_mean_jaccard is not None]
        all_detection_times = [res.detection_time for res in iteration_results if res.detection_time is not None]
        
        if all_mean_jaccards:
            summary["aggregated_metrics"]["avg_mean_jaccard"] = np.mean(all_mean_jaccards)
            summary["aggregated_metrics"]["std_mean_jaccard"] = np.std(all_mean_jaccards)
            
        if all_top_k_jaccards:
            summary["aggregated_metrics"]["avg_top_k_mean_jaccard"] = np.mean(all_top_k_jaccards)
            summary["aggregated_metrics"]["std_top_k_mean_jaccard"] = np.std(all_top_k_jaccards)
            
        if all_detection_times:
            summary["aggregated_metrics"]["avg_detection_time"] = np.mean(all_detection_times)

        experiment_summaries.append(summary)
        
    # 4. Mostrar sumário final
    print("\n" + "="*60)
    print("SUMMARY OF AGGREGATED RESULTS")
    print("="*60)
    for summary in experiment_summaries:
        print(f"Experiment: {summary['name']} ({summary['iterations']} iterations)")
        metrics = summary.get("aggregated_metrics", {})
        if metrics:
            k = summary['number_of_ground_truth_communities']
            print(f"  Avg. Mean Jaccard: {metrics.get('avg_mean_jaccard', 0.0):.4f} (std: {metrics.get('std_mean_jaccard', 0.0):.4f})")
            print(f"  Avg. Top-{k} Jaccard: {metrics.get('avg_top_k_mean_jaccard', 0.0):.4f} (std: {metrics.get('std_top_k_mean_jaccard', 0.0):.4f})")
            if 'avg_detection_time' in metrics:
                print(f"  Avg. Detection Time: {metrics.get('avg_detection_time', 0.0):.4f} seconds")
        else:
            print("  No metrics were calculated for this experiment.")
        print("-" * 30)

if __name__ == "__main__":
    main()
