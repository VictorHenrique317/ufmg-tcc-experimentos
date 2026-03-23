from dataclasses import dataclass, field
from typing import List, Set, Dict, Any, Optional
import networkx as nx

@dataclass
class ExperimentState:
    """
    Mantém o estado completo de um experimento, incluindo configurações
    e dados gerados durante a execução do pipeline.
    """
    # Identificação
    name: str
    
    # Configurações de Geração de Ground Truth
    random_seed: int
    number_of_vertices: int
    number_of_ground_truth_communities: int
    
    # Configurações de Injeção de Ruído
    # alpha na distribuição Beta (ITS)
    number_of_correct_observations: float 
    
    # Configurações de Detecção
    community_detection_algorithm: str # 'louvain' ou 'nclusterbox'
    timeout: Optional[int] = None
    
    # Dados de Execução (Runtime)
    original_graph: Optional[nx.Graph] = field(default=None, repr=False)
    noisy_graph: Optional[nx.Graph] = field(default=None, repr=False)
    ground_truth: Optional[List[Set[int]]] = field(default=None, repr=False)
    detected_communities: Optional[List[List[int]]] = field(default=None, repr=False)
    
    # Métricas de Avaliação
    num_detected_communities: Optional[int] = field(default=None)
    mean_jaccard: Optional[float] = field(default=None)
    top_k_mean_jaccard: Optional[float] = field(default=None)
    detection_time: Optional[float] = field(default=None)

    def to_dict(self) -> Dict[str, Any]:
        """Retorna um sumário do estado (sem o grafo completo)."""
        return {
            "name": self.name,
            "config": {
                "seed": self.random_seed,
                "n_vertices": self.number_of_vertices,
                "n_communities": self.number_of_ground_truth_communities,
                "alpha": self.number_of_correct_observations,
                "algorithm": self.community_detection_algorithm,
                "timeout": self.timeout
            },
            "metrics": {
                "num_detected_communities": self.num_detected_communities,
                "mean_jaccard": self.mean_jaccard,
                "top_k_mean_jaccard": self.top_k_mean_jaccard,
                "detection_time": self.detection_time
            }
        }
