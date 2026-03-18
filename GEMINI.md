# Contexto do Repositório: Avaliação de Detecção de Comunidades (nclusterbox vs Louvain)

## 1. Visão Geral do Projeto
Este repositório é um laboratório experimental projetado para comparar a resiliência de algoritmos de detecção de comunidades sob diferentes níveis de ruído estocástico em grafos estáticos (matrizes de adjacência).
Os dois principais algoritmos comparados são:
* **Louvain**: O estado da arte em métodos baseados na otimização da modularidade, gerando partições estritas (hard clustering).
* **nclusterbox**: Uma ferramenta de linha de comando baseada em C++ que modifica padrões contidos num tensor fuzzy para maximizar os seus poderes explicativos. O algoritmo seleciona um subconjunto ordenado de padrões para sumarizar o tensor. A ferramenta espera tuplas associadas a um grau de pertinência no intervalo fechado [0, 1], onde as tuplas não especificadas são associadas a zero.Com a flag `-c` (comunidades), a ferramenta procura nativamente conjuntos simétricos de vértices associados a grafos, ativando suporte a comunidades sobrepostas.

## 2. Arquitetura de Software e Padrões de Projeto
O repositório foi construído utilizando práticas modernas de Engenharia de Software, especificamente focadas na desacoplação e flexibilidade experimental.

### 2.1 Padrão Chain of Responsibility (Data Pipeline)
O fluxo de execução não é monolítico. É baseado numa pipeline de objetos do tipo `PipelineStep` que processam etapas específicas do fluxo.

### 2.2 Padrão Builder
A montagem dos Steps é feita através de um `ExperimentPipelineBuilder`. O orquestrador central (`main.py`) lê as intenções do ficheiro de configuração `config.yaml` e usa o Builder para encadear a ordem exata de execução: Geração -> Ruído -> Algoritmos -> Avaliação.

## 3. Estrutura de Diretórios
```text
poc/
├── data/raw/                 # Grafos de Ground Truth (vazios por padrão no repo)
├── data/processed/           # Grafos ruidosos (gerados em runtime)
├── results/                  # Métricas (CSVs) e Gráficos por experimento
├── config.yaml               # Ficheiro central de hiperparâmetros (YAML)
├── pyproject.toml            # Dependências geridas pelo Poetry
├── src/
