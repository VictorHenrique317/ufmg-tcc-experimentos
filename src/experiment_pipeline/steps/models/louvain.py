import networkx as nx
from typing import List, Set
from .base import BaseDetector
from src.experiment_pipeline.experiment_state import ExperimentState

class LouvainDetector(BaseDetector):
    """
    Wrapper para o algoritmo de Louvain.
    """
    
    def __init__(self, resolution: float = 1.0):
        super().__init__(name="Louvain Detector")
        self.resolution = resolution
        self.communities_ = []

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

    def run(self, state: ExperimentState) -> ExperimentState:
        if state.community_detection_algorithm != "louvain":
            return state
            
        self.fit(state.graph, seed=state.random_seed)
        state.detected_communities = [list(c) for c in self.get_communities()]
        return state

    def get_communities(self) -> List[Set[int]]:
        return self.communities_
