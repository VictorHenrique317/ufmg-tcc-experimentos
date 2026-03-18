import networkx as nx
from typing import List, Set
from .base import BaseDetector

class LouvainDetector(BaseDetector):
    """
    Wrapper para o algoritmo de Louvain, focado em maximização de modularidade.
    Utiliza a implementação estável incorporada no NetworkX 3+.
    """
    
    def __init__(self, resolution: float = 1.0, seed: int = None):
        """
        Parâmetros:
        -----------
        resolution : float
            Parâmetro de resolução. Valores menores que 1.0 favorecem comunidades 
            maiores, enquanto valores maiores favorecem comunidades menores.
        seed : int
            Semente aleatória para o processo iterativo de otimização local.
        """
        self.resolution = resolution
        self.seed = seed
        self.communities_ = []

    def fit(self, graph: nx.Graph, **kwargs) -> 'BaseDetector':
        """
        Aplica o Louvain maximizando a modularidade sobre o grafo ponderado.
        """
        # A função nx.community.louvain_communities assume 'weight' como padrão 
        # para ler a força da aresta, o que combina perfeitamente com nossa injeção de ruído
        comms = nx.community.louvain_communities(
            graph, 
            weight='weight', 
            resolution=self.resolution, 
            seed=self.seed
        )
        
        # Converte para o formato padrão do nosso repositório
        self.communities_ = [set(c) for c in comms]
        
        return self

    def get_communities(self) -> List[Set[int]]:
        return self.communities_
