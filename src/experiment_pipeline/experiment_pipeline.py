from abc import ABC, abstractmethod
from typing import List, Dict, Any
from dataclasses import dataclass
import networkx as nx

from src.experiment_pipeline.steps.pipeline_step import PipelineStep


@dataclass
class PipelineContext:
    """
    Contexto executado através do pipeline, passado de step para step.
    """
    name: str


class ExperimentPipeline:
    """
    O executor do Pipeline. Apenas itera sobre os passos.
    """
    def __init__(self, name: str, steps: List[PipelineStep]):
        self.name = name
        self.steps = steps

    def run(self):
        print(f"\n{'='*60}\nA iniciar Pipeline: {self.name}\n{'='*60}")
        context = PipelineContext(self.name)
        
        for step in self.steps:
            context = step.run(context)
            
        print(f"Pipeline '{self.name}' concluído com sucesso!")
        return context

