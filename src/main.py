from src.experiment_configuration.configuration_reader import ConfigurationReader

def main():
    # 1. Ler os experimentos configurados no config.yaml
    reader = ConfigurationReader("config.yaml")
    experiments = reader.read_experiments()
    
    if not experiments:
        print("Nenhum experimento encontrado no config.yaml.")
        return

    # 2. Executar cada pipeline
    results = []
    for pipeline, state in experiments:
        # Apenas para este teste, vamos saltar o nclusterbox se o binário não existir
        if state.community_detection_algorithm == "nclusterbox":
            print(f"\nSaltando {state.name}: Binário nclusterbox necessário.")
            continue
            
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
