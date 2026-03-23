import networkx as nx
import numpy as np
import itertools
from typing import Tuple, List, Set

from src.experiment_pipeline.steps.pipeline_step import PipelineStep
from src.experiment_pipeline.experiment_state import ExperimentState

class GraphGeneratorStep(PipelineStep):
    """
    Etapa responsável por gerar grafos sintéticos com comunidades bem definidas.
    """
    
    def __init__(self):
        super().__init__(name="Graph Generation")

    def run(self, state: ExperimentState) -> ExperimentState:
        """
        Gera o grafo sintético utilizando as configurações presentes no estado.
        """
        graph, gt = self.generate_synthetic_graph(
            n_nodes=state.number_of_vertices, 
            n_communities=state.number_of_ground_truth_communities, 
            seed=state.random_seed
        )
        state.original_graph = graph
        state.ground_truth = gt
        return state

    def generate_synthetic_graph(
        self,
        n_nodes: int, 
        n_communities: int, 
        seed: int
    ) -> Tuple[nx.Graph, List[Set[int]]]:
        """
        Gera um grafo estático não-ponderado onde as comunidades formam cliques perfeitos.
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
