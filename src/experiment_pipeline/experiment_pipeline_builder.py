class ExperimentPipelineBuilder:
    """
    Construtor que permite acoplar os Steps na ordem desejada.
    """
    def __init__(self, name: str):
        self.name = name
        self.steps: List[BaseStep] = []

    def add_step(self, step: BaseStep) -> 'PipelineBuilder':
        self.steps.append(step)
        return self

    def build(self) -> ExperimentPipeline:
        if not self.steps:
            raise ValueError("Não é possível construir um pipeline sem steps.")
        return ExperimentPipeline(self.name, self.steps)