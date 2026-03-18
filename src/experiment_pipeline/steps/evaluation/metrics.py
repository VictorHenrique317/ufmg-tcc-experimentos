import numpy as np
from typing import List, Set

from src.experiment_pipeline.steps.pipeline_step import PipelineStep
from src.experiment_pipeline.experiment_state import ExperimentState

class MetricsStep(PipelineStep):
    def __init__(self):
        super().__init__(name="Evaluation Metrics")

    def run(self, state: ExperimentState) -> ExperimentState:
        """
        Calcula o índice de Jaccard para cada comunidade detectada em relação à 
        melhor correspondência na Ground Truth.
        """
        if state.detected_communities is None or state.ground_truth is None:
            state.metrics = []
            return state

        scores = []
        for det_comm in state.detected_communities:
            det_set = set(det_comm)
            # Para cada comunidade detectada, encontra o maior Jaccard na GT
            best_score = max((self.jaccard_index(det_set, gt_comm) for gt_comm in state.ground_truth), default=0.0)
            scores.append(float(best_score))
            
        state.metrics = scores
        return state

    def jaccard_index(self, set_a: Set[int], set_b: Set[int]) -> float:
        if not set_a and not set_b: return 1.0
        if not set_a or not set_b: return 0.0
        intersection = len(set_a.intersection(set_b))
        union = len(set_a.union(set_b))
        return intersection / union
