from enum import Enum

class StepType(Enum):
    GRAPH_GENERATION = ("Graph Generation", 1)
    NOISE_INJECTION = ("Noise Injector", 2)
    COMMUNITY_DETECTOR = ("Community Detector", 3)
    EVALUATION_METRICS = ("Evaluation Metrics", 4)

    def __init__(self, step_name: str, order_number: int):
        self._value_ = (step_name, order_number)

    @property
    def step_name(self) -> str:
        return self._value_[0]

    @property
    def order_number(self) -> int:
        return self._value_[1]

    @classmethod
    def from_step_name(cls, name: str):
        for member in cls:
            if member.step_name == name:
                return member
        raise ValueError(f"Step name '{name}' not found in StepType enum.")