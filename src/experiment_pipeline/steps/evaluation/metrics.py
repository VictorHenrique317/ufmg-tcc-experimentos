import numpy as np
from typing import List, Set

from src.experiment_pipeline.steps.pipeline_step import PipelineStep
from src.experiment_pipeline.experiment_state import ExperimentState

class MetricsStep(PipelineStep):
    def __init__(self):
        super().__init__(name="Evaluation Metrics")

    def run(self, state: ExperimentState) -> ExperimentState:
        """
        Calcula métricas de avaliação agregadas (Jaccard) e as armazena no estado.
        """
        if not state.detected_communities or not state.ground_truth:
            return state

        scores = []
        for det_comm in state.detected_communities:
            det_set = set(det_comm)
            # Para cada comunidade detectada, encontra o maior Jaccard na GT
            best_score = max((self.jaccard_index(det_set, gt_comm) for gt_comm in state.ground_truth), default=0.0)
            scores.append(float(best_score))
        
        if not scores:
            return state

        # Calcular métricas agregadas
        state.num_detected_communities = len(scores)
        state.mean_jaccard = np.mean(scores)
        
        # Calcular a média do top-k
        k = state.number_of_ground_truth_communities
        scores.sort(reverse=True)
        top_k_scores = scores[:k]
        state.top_k_mean_jaccard = np.mean(top_k_scores)
        
        return state

    def jaccard_index(self, set_a: Set[int], set_b: Set[int]) -> float:
        if not set_a and not set_b: return 1.0
        if not set_a or not set_b: return 0.0
        intersection = len(set_a.intersection(set_b))
        union = len(set_a.union(set_b))
        return intersection / union
