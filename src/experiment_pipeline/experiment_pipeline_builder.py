from typing import List

from src.experiment_pipeline.steps.pipeline_step import PipelineStep
from src.experiment_pipeline.experiment_pipeline import ExperimentPipeline


class ExperimentPipelineBuilder:
    """
    Construtor que permite acoplar os Steps na ordem desejada.
    """
    def __init__(self, name: str):
        self.name = name
        self.steps: List[PipelineStep] = []

    def add_step(self, step: PipelineStep) -> 'ExperimentPipelineBuilder':
        self.steps.append(step)
        return self

    def build(self) -> ExperimentPipeline:
        if not self.steps:
            raise ValueError("Não é possível construir um pipeline sem steps.")
        return ExperimentPipeline(self.name, self.steps)