import os
import json
import networkx as nx
from typing import List

from src.experiment_pipeline.steps.pipeline_step import PipelineStep
from src.experiment_pipeline.experiment_state import ExperimentState
from src.experiment_pipeline.step_type import StepType # Updated import
from src.experiment_pipeline.steps.models.base import BaseDetector


class ExperimentPipeline:
    """
    O executor do Pipeline. Apenas itera sobre os passos,
    passando e atualizando o objeto ExperimentState.
    """
    def __init__(self, steps: List[PipelineStep]):
        self.steps = steps

    def _save_step_result(self, state: ExperimentState, step: PipelineStep):
        """Saves the result of a pipeline step to a file."""
        step_type = StepType.from_step_name(step.name)
        step_order_number = step_type.order_number
        step_name_fs = f"{step_order_number}_{step_type.step_name.replace(' ', '_').lower()}"

        exp_name_fs = state.name.replace(' ', '_').replace('/', '_').replace(':', '')
        output_dir = os.path.join("results", exp_name_fs, step_name_fs)
        os.makedirs(output_dir, exist_ok=True)
        
        file_path = os.path.join(output_dir, f"iter_{state.current_iteration}.json")
        
        config = {}
        step_output = {}
        
        if step.name == StepType.GRAPH_GENERATION.step_name:
            config = {
                "random_seed": state.random_seed,
                "number_of_vertices": state.number_of_vertices,
                "number_of_ground_truth_communities": state.number_of_ground_truth_communities
            }

            step_output['graph'] = nx.node_link_data(state.original_graph)
            step_output['ground_truth'] = [list(c) for c in state.ground_truth] if state.ground_truth else None

        elif step.name == StepType.NOISE_INJECTION.step_name:
            config = {
                "number_of_correct_observations": state.number_of_correct_observations
            }

            step_output['weighted_graph'] = nx.node_link_data(state.noisy_graph)

        elif step.name == StepType.COMMUNITY_DETECTOR.step_name:
            config = {
                "algorithm": state.community_detection_algorithm,
                "timeout": state.timeout
            }
            step_output['detected_communities'] = state.detected_communities
            step_output['detection_time'] = state.detection_time

        elif step.name == StepType.EVALUATION_METRICS.step_name:
            config = {}
            step_output = {
                "num_detected_communities": state.num_detected_communities,
                "mean_jaccard": state.mean_jaccard,
                "top_k_mean_jaccard": state.top_k_mean_jaccard,
                "detection_time": state.detection_time,
                "raw_jaccard_scores": state.raw_jaccard_scores
            }

        data_to_save = {"config": config, "step_output": step_output}

        if data_to_save["config"] or data_to_save["step_output"]:
            with open(file_path, 'w') as f:
                json.dump(data_to_save, f, indent=4)
            print(f"  Resultados guardados em: {file_path}")

    def run(self, state: ExperimentState) -> ExperimentState:
        print(f"\n{'='*60}\nA iniciar Pipeline: {state.name}\n{'='*60}")
        
        for step in self.steps:
            # Execute only the selected detector based on the state
            if isinstance(step, BaseDetector):
                if step.algorithm_type != state.community_detection_algorithm:
                    continue

            print(f"Executando Step: {step.name}")
            state = step.run(state)
            self._save_step_result(state, step) # Save step results
            
        print(f"Pipeline '{state.name}' concluído com sucesso!")
        return state
