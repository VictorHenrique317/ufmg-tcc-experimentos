import networkx as nx
import numpy as np
import itertools
from typing import Tuple, List, Set

from src.experiment_pipeline.steps.pipeline_step import PipelineStep

class GraphGeneratorStep(PipelineStep):
    """
    Etapa responsável por gerar grafos sintéticos com comunidades bem definidas.
    Utiliza a função `generate_synthetic_graph` para criar grafos onde as comunidades
    formam cliques perfeitos, sem arestas intercomunitárias.
    """
    
    def __init__(self, n_nodes: int = 500, n_communities: int = 5, seed: int = 42):
        super().__init__(name="Graph Generation")
        self.n_nodes = n_nodes
        self.n_communities = n_communities
        self.seed = seed

    def execute(self) -> Tuple[nx.Graph, List[Set[int]]]:
        """
        Gera o grafo sintético e retorna tanto o grafo quanto a Ground Truth das comunidades.
        
        Retorna:
        --------
        Tuple[nx.Graph, List[Set[int]]]
            - G: Grafo NetworkX puro (sem pesos).
            - ground_truth: Lista contendo conjuntos de nós, onde cada conjunto é uma comunidade.
        """
        return self.generate_synthetic_graph(
            n_nodes=self.n_nodes, 
            n_communities=self.n_communities, 
            seed=self.seed
        )

    def generate_synthetic_graph(
        self,
        n_nodes: int = 500, 
        n_communities: int = 5, 
        seed: int = 42
    ) -> Tuple[nx.Graph, List[Set[int]]]:
        """
        Gera um grafo estático não-ponderado onde as comunidades formam cliques perfeitos
        e não há nenhuma aresta interligando comunidades diferentes.

        Parâmetros:
        -----------
        n_nodes : int
            Número total de nós (vértices) no grafo.
        n_communities : int
            Número de comunidades (Ground Truth) a serem geradas.
        seed : int
            Semente para o gerador aleatório.

        Retorna:
        --------
        Tuple[nx.Graph, List[Set[int]]]
            - G: Grafo NetworkX puro (sem pesos).
            - ground_truth: Lista contendo conjuntos de nós, onde cada conjunto é uma comunidade.
        """
        np.random.seed(seed)
        
        # Embaralha as IDs dos nós para não ficarem em ordem sequencial óbvia
        nodes = np.random.permutation(n_nodes).tolist()
        
        G = nx.Graph()
        G.add_nodes_from(range(n_nodes))
        
        # Distribui os nós de forma aproximadamente igualitária pelas comunidades
        sizes = [n_nodes // n_communities] * n_communities
        sizes[-1] += n_nodes % n_communities

        ground_truth = []
        current_idx = 0
        for size in sizes:
            community_nodes = set(nodes[current_idx : current_idx + size])
            ground_truth.append(community_nodes)
            
            # Adiciona arestas formando um clique (todas as combinações possíveis entre os nós)
            G.add_edges_from(itertools.combinations(community_nodes, 2))
            
            current_idx += size

        return G, ground_truth
