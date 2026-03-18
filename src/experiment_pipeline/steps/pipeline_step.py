from abc import ABC


class PipelineStep(ABC):
    def __init__(self, name: str):
        self.name = name

    def run(self, data):
        raise NotImplementedError("Subclasses must implement the run method.")