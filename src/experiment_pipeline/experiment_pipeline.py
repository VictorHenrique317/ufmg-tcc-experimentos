from typing import List, Any

from src.experiment_pipeline.steps.pipeline_step import PipelineStep


class ExperimentPipeline:
    """
    O executor do Pipeline. Apenas itera sobre os passos, passando o resultado 
    de um passo como entrada para o próximo.
    """
    def __init__(self, name: str, steps: List[PipelineStep]):
        self.name = name
        self.steps = steps

    def run(self, initial_data: Any = None):
        print(f"\n{'='*60}\nA iniciar Pipeline: {self.name}\n{'='*60}")
        data = initial_data
        
        for step in self.steps:
            data = step.run(data)
            
        print(f"Pipeline '{self.name}' concluído com sucesso!")
        return data
