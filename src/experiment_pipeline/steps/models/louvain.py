import networkx as nx
from typing import List, Set
from .base import BaseDetector
from src.experiment_pipeline.experiment_state import ExperimentState

class LouvainDetector(BaseDetector):
    """
    Wrapper para o algoritmo de Louvain.
    """
    
    def __init__(self, resolution: float = 1.0):
        super().__init__(name="Community Detector")
        self.resolution = resolution
        self.communities_ = []

    @property
    def algorithm_type(self) -> str:
        return "louvain"

    def fit(self, graph: nx.Graph, **kwargs) -> 'BaseDetector':
        # Nota: Usamos a semente que vier no kwargs ou deixamos None
        seed = kwargs.get('seed')
        
        comms = nx.community.louvain_communities(
            graph, 
            weight='weight', 
            resolution=self.resolution, 
            seed=seed
        )
        self.communities_ = [set(c) for c in comms]
        return self



    def get_communities(self) -> List[Set[int]]:
        return self.communities_
