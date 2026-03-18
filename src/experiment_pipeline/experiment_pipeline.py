from abc import ABC, abstractmethod
from typing import List, Dict, Any
import networkx as nx

class ExperimentPipeline:
    """
    O executor do Pipeline. Apenas itera sobre os passos.
    """
    def __init__(self, name: str, steps: List[BaseStep]):
        self.name = name
        self.steps = steps

    def run(self):
        print(f"\n{'='*60}\nA iniciar Pipeline: {self.name}\n{'='*60}")
        context = PipelineContext(self.name)
        
        for step in self.steps:
            context = step.process(context)
            
        print(f"Pipeline '{self.name}' concluído com sucesso!")
        return context

