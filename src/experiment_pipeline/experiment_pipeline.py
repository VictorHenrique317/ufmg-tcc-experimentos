import os
import json
import networkx as nx
from typing import List

from src.experiment_pipeline.steps.pipeline_step import PipelineStep
from src.experiment_pipeline.experiment_state import ExperimentState
from src.experiment_pipeline.step_order import StepOrder
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
        step_order_number = StepOrder.get_order_number(step.name)
        step_name_fs = f"{step_order_number}_{step.name.replace(' ', '_').lower()}"
        output_dir = os.path.join("results", step_name_fs)
        os.makedirs(output_dir, exist_ok=True)
        
        exp_name_fs = state.name.replace(' ', '_').replace('/', '_').replace(':', '')
        file_path = os.path.join(output_dir, f"{exp_name_fs}.json")
        
        config = {}
        step_output = {}
        
        if step.name == "Graph Generation":
            config = {
                "random_seed": state.random_seed,
                "number_of_vertices": state.number_of_vertices,
                "number_of_ground_truth_communities": state.number_of_ground_truth_communities
            }
            if state.graph:
                step_output['graph'] = nx.node_link_data(state.graph)
            if state.ground_truth:
                step_output['ground_truth'] = [list(c) for c in state.ground_truth]
        
        elif step.name == "Noise Injector":
            config = {
                "number_of_correct_observations": state.number_of_correct_observations
            }
            if state.graph:
                step_output['weighted_graph'] = nx.node_link_data(state.graph)

        elif step.name == "Community Detector":
            config = {
                "algorithm": state.community_detection_algorithm,
                "timeout": state.timeout
            }
            if state.detected_communities is not None:
                step_output['detected_communities'] = state.detected_communities
            if state.detection_time is not None:
                step_output['detection_time'] = state.detection_time
                
        elif step.name == "Evaluation Metrics":
            config = {
                "number_of_ground_truth_communities": state.number_of_ground_truth_communities
            }
            step_output = {
                "num_detected_communities": state.num_detected_communities,
                "mean_jaccard": state.mean_jaccard,
                "top_k_mean_jaccard": state.top_k_mean_jaccard,
                "detection_time": state.detection_time,
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
