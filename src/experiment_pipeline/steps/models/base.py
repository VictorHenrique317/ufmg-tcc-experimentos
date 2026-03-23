from abc import ABC, abstractmethod
import networkx as nx
import time
from typing import List, Set

from src.experiment_pipeline.steps.pipeline_step import PipelineStep
from src.experiment_pipeline.experiment_state import ExperimentState

class BaseDetector(PipelineStep):
    """
    Interface base genérica para todos os algoritmos de detecção de comunidades.
    """
    
    @property
    @abstractmethod
    def algorithm_type(self) -> str:
        """Returns the string identifier for the specific detection algorithm (e.g., 'louvain', 'nclusterbox')."""
        pass

    @abstractmethod
    def fit(self, graph: nx.Graph, **kwargs) -> 'BaseDetector':
        pass

    @abstractmethod
    def get_communities(self) -> List[Set[int]]:
        pass

    def run(self, state: ExperimentState) -> ExperimentState:
        """
        Executa o algoritmo sobre o grafo do estado e armazena os resultados.
        """
        if state.noisy_graph is None:
            raise ValueError("Grafo ruidoso não encontrado no estado.")
            
        start_time = time.time()
        # Pass seed and timeout directly to fit, allowing specific detectors to use them
        self.fit(state.noisy_graph, seed=state.random_seed, timeout=state.timeout)
        state.detected_communities = [list(c) for c in self.get_communities()]
        end_time = time.time()
        state.detection_time = end_time - start_time
        
        return state
