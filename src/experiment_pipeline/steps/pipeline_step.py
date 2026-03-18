from abc import ABC, abstractmethod
from src.experiment_pipeline.experiment_state import ExperimentState


class PipelineStep(ABC):
    def __init__(self, name: str):
        self.name = name

    @abstractmethod
    def run(self, state: ExperimentState) -> ExperimentState:
        """
        Executa a etapa utilizando e atualizando o estado fornecido.
        """
        raise NotImplementedError("Subclasses must implement the run method.")
