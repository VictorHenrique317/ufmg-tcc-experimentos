import numpy as np
from typing import List, Set, Dict, Any

from src.experiment_pipeline.shared.pipeline_step import PipelineStep

class MetricsStep(PipelineStep):
    def __init__(self, ground_truth: List[Set[int]]):
        super().__init__(name="Evaluation Metrics")
        self.ground_truth = ground_truth

    def execute(self, detected_communities: List[Set[int]]) -> Dict[str, Any]:
        """
        Compara as comunidades inferidas com a Ground Truth através de um 
        mapeamento de Jaccard bidirecional.
        
        Parâmetros:
        -----------
        detected_communities : List[Set[int]]
            Comunidades encontradas pelo algoritmo (Louvain ou nclusterbox).
            
        Retorna:
        --------
        Dict[str, Any]
            Dicionário contendo os scores médios e os mapeamentos detalhados.
        """
        return self.evaluate_communities(
            detected=detected_communities, 
            ground_truth=self.ground_truth
        )

    def jaccard_index(self, set_a: Set[int], set_b: Set[int]) -> float:
        """
        Calcula o índice de Jaccard entre dois conjuntos de vértices.
        
        Fórmula: |A ∩ B| / |A ∪ B|
        """
        if not set_a and not set_b:
            return 1.0
        if not set_a or not set_b:
            return 0.0
            
        intersection = len(set_a.intersection(set_b))
        union = len(set_a.union(set_b))
        
        return intersection / union

    def evaluate_communities(self, detected: List[Set[int]], ground_truth: List[Set[int]]) -> Dict[str, Any]:
        """
        Compara as comunidades inferidas com a Ground Truth através de um 
        mapeamento de Jaccard bidirecional.
        
        Parâmetros:
        -----------
        detected : List[Set[int]]
            Comunidades encontradas pelo algoritmo (Louvain ou nclusterbox).
        ground_truth : List[Set[int]]
            As comunidades reais em formato de cliques geradas inicialmente.
            
        Retorna:
        --------
        Dict[str, Any]
            Dicionário contendo os scores médios e os mapeamentos detalhados.
        """
        # Trata os casos extremos de falha total do algoritmo
        if not detected:
            return {
                "mean_detected_to_gt": 0.0,
                "mean_gt_to_detected": 0.0,
                "overall_jaccard_score": 0.0
            }

        # 1. Mapeamento Detetado -> Ground Truth (Avalia a Pureza/Precision)
        # Responde a: "Das comunidades que o algoritmo sugeriu, quão reais são elas?"
        det_to_gt_scores = []
        for det_comm in detected:
            best_score = max((jaccard_index(det_comm, gt_comm) for gt_comm in ground_truth), default=0.0)
            det_to_gt_scores.append(best_score)
            
        mean_det_to_gt = np.mean(det_to_gt_scores)

        # 2. Mapeamento Ground Truth -> Detetado (Avalia o Recall/Cobertura)
        # Responde a: "Das comunidades que sabemos que existem, o algoritmo encontrou-as?"
        gt_to_det_scores = []
        for gt_comm in ground_truth:
            best_score = max((jaccard_index(gt_comm, det_comm) for det_comm in detected), default=0.0)
            gt_to_det_scores.append(best_score)
            
        mean_gt_to_det = np.mean(gt_to_det_scores)

        # O Score Global é a média das duas direções (uma harmonia entre Pureza e Cobertura)
        overall_score = (mean_det_to_gt + mean_gt_to_det) / 2.0

        return {
            "mean_detected_to_gt": float(mean_det_to_gt),
            "mean_gt_to_detected": float(mean_gt_to_det),
            "overall_jaccard_score": float(overall_score),
            "details_det_to_gt": det_to_gt_scores,
            "details_gt_to_det": gt_to_det_scores
        }
