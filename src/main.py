import argparse
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
    results = []
    for pipeline, state in experiments_to_run:
        print(f"Running experiment: {state.name}")
        final_state = pipeline.run(state)
        results.append(final_state)
        
    # 3. Mostrar sumário simples
    print("\n" + "="*60)
    print("SUMÁRIO DOS RESULTADOS")
    print("="*60)
    for res in results:
        print(f"Experimento: {res.name}")
        print(f"  Métricas (Jaccard por comunidade): {res.metrics}")
        if res.metrics:
            print(f"  Média: {sum(res.metrics)/len(res.metrics):.4f}")
        print("-" * 30)

if __name__ == "__main__":
    main()
