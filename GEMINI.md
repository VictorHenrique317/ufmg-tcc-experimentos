# Contexto do Repositório: Avaliação de Detecção de Comunidades (nclusterbox vs Louvain)

## 1. Visão Geral do Projeto
Este repositório é um laboratório experimental projetado para comparar a resiliência de algoritmos de detecção de comunidades sob diferentes níveis de ruído estocástico em grafos estáticos (matrizes de adjacência).
Os dois principais algoritmos comparados são:
* **Louvain**: O estado da arte em métodos baseados na otimização da modularidade, gerando partições estritas (hard clustering).
* **nclusterbox**: Uma ferramenta de linha de comando baseada em C++ que modifica padrões contidos num tensor fuzzy para maximizar os seus poderes explicativos. O algoritmo seleciona um subconjunto ordenado de padrões para sumarizar o tensor. A ferramenta espera tuplas associadas a um grau de pertinência no intervalo fechado [0, 1], onde as tuplas não especificadas são associadas a zero. Com a flag `-c` (comunidades), a ferramenta procura nativamente conjuntos simétricos de vértices associados a grafos, ativando suporte a comunidades sobrepostas.

## 2. Arquitetura de Software e Padrões de Projeto
O repositório foi construído utilizando práticas modernas de Engenharia de Software, especificamente focadas na desacoplação e flexibilidade experimental.

### 2.1 Padrão Chain of Responsibility (Data Pipeline)
O fluxo de execução não é monolítico. É baseado numa pipeline de objetos do tipo `PipelineStep` que processam etapas específicas do fluxo. Cada passo recebe os dados do passo anterior e retorna os dados processados para o próximo: Grafo -> Grafo Ruidoso -> Comunidades (List[List[int]]) -> Métricas (List[float]).

### 2.2 Padrão Builder
A montagem dos Steps é feita através de um `ExperimentPipelineBuilder`. O orquestrador central (idealmente `main.py` ou scripts de execução) lê as intenções do ficheiro de configuração `config.yaml` e usa o Builder para encadear a ordem exata de execução: Geração -> Ruído -> Algoritmos -> Avaliação.

## 3. Estrutura de Diretórios
```text
poc/
├── data/
│   ├── raw/                 # Grafos de Ground Truth (vazios por padrão no repo)
│   └── processed/           # Grafos ruidosos (gerados em runtime)
├── results/
│   ├── communities/         # Saídas dos algoritmos
│   └── metrics/             # Métricas (CSVs) e Gráficos por experimento
├── src/
│   └── experiment_pipeline/
│       ├── steps/
│       │   ├── data_generation/ # Geradores de grafos e injetores de ruído
│       │   ├── evaluation/      # Cálculo de métricas (Jaccard, etc.)
│       │   └── models/          # Wrappers para Louvain e nclusterbox
│       ├── experiment_pipeline.py
│       └── experiment_pipeline_builder.py
├── config.yaml               # Ficheiro central de hiperparâmetros (YAML)
├── pyproject.toml            # Dependências geridas pelo Poetry (ou pip)
└── README.md                 # Documentação básica de uso
```

## 4. Componentes Principais

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
