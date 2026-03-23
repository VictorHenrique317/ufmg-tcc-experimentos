import yaml
from typing import List, Tuple
from src.experiment_pipeline.experiment_state import ExperimentState
from src.experiment_pipeline.experiment_pipeline import ExperimentPipeline
from src.experiment_pipeline.steps.data_generation.graph_generator_step import GraphGeneratorStep
from src.experiment_pipeline.steps.data_generation.noise_injector_step import NoiseInjectorStep
from src.experiment_pipeline.steps.models.louvain import LouvainDetector
from src.experiment_pipeline.steps.models.nclusterbox import NclusterboxDetector
from src.experiment_pipeline.steps.evaluation.metrics import MetricsStep

class ConfigurationReader:
    """
    Lê o config.yaml e retorna pares de (Pipeline, State) prontos para execução.
    """
    def __init__(self, config_path: str = "config.yaml"):
        self.config_path = config_path

    def read_experiments(self) -> List[Tuple[ExperimentPipeline, ExperimentState]]:
        with open(self.config_path, 'r') as f:
            config = yaml.safe_load(f)
            
        if not config or 'experiments' not in config:
            return []
            
        experiment_list = []
        for exp_config in config['experiments']:
            # Extração de parâmetros organizada por temas
            gt_gen = exp_config.get('ground_truth_generation', {})
            noise_inj = exp_config.get('noise_injection', {})
            detection = exp_config.get('community_detection', {})
            
            # Criação do Estado
            state = ExperimentState(
                name=exp_config.get('name', 'Unnamed Experiment'),
                iterations=exp_config.get('iterations', 1),
                random_seed=gt_gen.get('random_seed', 42),
                number_of_vertices=gt_gen.get('number_of_vertices', 500),
                number_of_ground_truth_communities=gt_gen.get('number_of_ground_truth_communities', 5),
                number_of_correct_observations=noise_inj.get('number_of_correct_observations', 4.0),
                community_detection_algorithm=detection.get('algorithm', 'louvain'),
                timeout=detection.get('timeout', None)
            )
            
            # Construção do Pipeline (Steps fixos, comportamento dinâmico via State)
            steps = [
                GraphGeneratorStep(),
                NoiseInjectorStep(),
                LouvainDetector(),
                NclusterboxDetector(), # Ambos os detectores são adicionados, mas filtram via algorithm no run()
                MetricsStep()
            ]
            
            pipeline = ExperimentPipeline(steps=steps)
            experiment_list.append((pipeline, state))
            
        return experiment_list
