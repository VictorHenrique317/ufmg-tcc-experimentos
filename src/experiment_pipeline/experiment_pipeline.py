from typing import List

from src.experiment_pipeline.steps.pipeline_step import PipelineStep
from src.experiment_pipeline.experiment_state import ExperimentState


class ExperimentPipeline:
    """
    O executor do Pipeline. Apenas itera sobre os passos,
    passando e atualizando o objeto ExperimentState.
    """
    def __init__(self, steps: List[PipelineStep]):
        self.steps = steps

    def run(self, state: ExperimentState) -> ExperimentState:
        print(f"\n{'='*60}\nA iniciar Pipeline: {state.name}\n{'='*60}")
        
        for step in self.steps:
            print(f"Executando Step: {step.name}")
            state = step.run(state)
            
        print(f"Pipeline '{state.name}' concluído com sucesso!")
        return state
