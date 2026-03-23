from enum import Enum

class StepOrder(Enum):
    GRAPH_GENERATION = ("Graph Generation", 1)
    NOISE_INJECTION = ("Noise Injector", 2)
    COMMUNITY_DETECTOR = ("Community Detector", 3)
    EVALUATION_METRICS = ("Evaluation Metrics", 4)

    def __init__(self, step_name: str, order_number: int):
        self.step_name = step_name
        self.order_number = order_number

    @classmethod
    def get_order_number(cls, name: str) -> int:
        for member in cls:
            if member.step_name == name:
                return member.order_number
        raise ValueError(f"Step name '{name}' not found in StepOrder enum.")