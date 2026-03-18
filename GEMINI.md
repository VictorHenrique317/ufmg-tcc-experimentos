# Contexto do Repositório: Avaliação de Detecção de Comunidades (nclusterbox vs Louvain)

## 1. Visão Geral do Projeto
Este repositório é um laboratório experimental projetado para comparar a resiliência de algoritmos de detecção de comunidades sob diferentes níveis de ruído estocástico em grafos estáticos (matrizes de adjacência).
Os dois principais algoritmos comparados são:
* **Louvain**: O estado da arte em métodos baseados na otimização da modularidade, gerando partições estritas (hard clustering).
* **nclusterbox**: Uma ferramenta de linha de comando baseada em C++ que modifica padrões contidos num tensor fuzzy para maximizar os seus poderes explicativos. O algoritmo seleciona um subconjunto ordenado de padrões para sumarizar o tensor. A ferramenta espera tuplas associadas a um grau de pertinência no intervalo fechado [0, 1], onde as tuplas não especificadas são associadas a zero. Com a flag `-c` (comunidades), a ferramenta procura nativamente conjuntos simétricos de vértices associados a grafos, ativando suporte a comunidades sobrepostas.

## 2. Arquitetura de Software e Padrões de Projeto
O repositório foi construído utilizando práticas modernas de Engenharia de Software, especificamente focadas na desacoplação e flexibilidade experimental.

### 2.1 Padrão Chain of Responsibility (Data Pipeline)
O fluxo de execução não é monolítico. É baseado numa pipeline de objetos do tipo `PipelineStep` que processam um objeto `ExperimentState`. Cada passo utiliza as configurações do estado e atualiza os seus dados de execução (grafo, comunidades, métricas).

### 2.2 Padrão Builder e Configuração
A montagem dos experimentos é automatizada pela classe `ConfigurationReader`, que lê o `config.yaml`. Para cada entrada em `experiments`, o leitor:
1. Cria um `ExperimentState` com os parâmetros específicos (seed, vértices, alpha, algoritmo, etc.).
2. Constrói um `ExperimentPipeline` com a sequência padrão de steps.
3. Retorna o par pronto para execução via `pipeline.run(state)`.

## 3. Estrutura de Diretórios
```text
poc/
├── data/                    # Dados temporários ou processados
├── results/                 # Saídas dos experimentos
├── src/
│   ├── experiment_configuration/
│   │   └── configuration_reader.py # Orquestração via YAML
│   ├── experiment_pipeline/
│   │   ├── steps/           # Implementações de PipelineStep
│   │   ├── experiment_pipeline.py
│   │   └── experiment_state.py # Objeto central de dados e config
│   └── main.py              # Script principal de execução
├── config.yaml               # Declaração de múltiplos experimentos
├── pyproject.toml            # Dependências geridas pelo Poetry
└── (outros ficheiros de sistema)
```
...
## 4. Componentes Principais

### 0. Estado do Experimento (`ExperimentState`)
Objeto que centraliza todas as configurações (temas: `ground_truth_generation`, `noise_injection`, `community_detection`) e os resultados intermediários da execução.


### 4.1 Geração de Dados (`GraphGeneratorStep`)
Gera grafos sintéticos onde as comunidades são cliques perfeitos. Permite configurar o número de nós, número de comunidades e semente aleatória.

### 4.2 Injeção de Ruído (`NoiseInjectorStep`)
Aplica ruído estocástico aos pesos das arestas usando a distribuição Beta via Amostragem por Transformada Inversa (ITS). Isso transforma o grafo booleano num tensor fuzzy [0, 1], permitindo testar a robustez dos algoritmos.

### 4.3 Detetores de Comunidades (`models/`)
* **LouvainDetector**: Wrapper para o algoritmo de Louvain do NetworkX.
* **NclusterboxDetector**: Interface com o binário `nclusterbox` (C++). Requer que o binário esteja no PATH ou que o caminho seja especificado.

### 4.4 Avaliação (`MetricsStep`)
Calcula o Índice de Jaccard para cada comunidade detetada em relação à melhor correspondência na Ground Truth. Retorna uma lista de scores (`List[float]`), preservando a ordem das comunidades detetadas.

## 5. Fluxo de Trabalho Recomendado
1. **Configuração**: Definir os parâmetros do experimento no `config.yaml`.
2. **Construção**: Usar o `ExperimentPipelineBuilder` para instanciar os steps necessários.
3. **Execução**: Chamar o método `run()` do pipeline construído.
4. **Análise**: Verificar os resultados na pasta `results/`.

## 6. Requisitos Técnicos
* **Python 3.12+**
* **NetworkX**, **NumPy**, **SciPy** (geridos via `pyproject.toml`)
* **nclusterbox binary**: O executável `nclusterbox` deve estar instalado e acessível no sistema para utilizar o `NclusterboxDetector`.
