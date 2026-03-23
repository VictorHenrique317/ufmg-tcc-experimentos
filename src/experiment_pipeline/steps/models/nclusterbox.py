import os
import tempfile
import subprocess
from typing import List, Set
import networkx as nx

from .base import BaseDetector
from src.experiment_pipeline.experiment_state import ExperimentState

class NclusterboxDetector(BaseDetector):
    """
    Wrapper para o algoritmo nclusterbox.
    """
    
    def __init__(self, executable_path: str = "libs/nclusterbox/nclusterbox", max_patterns: int = None, jobs: int = None):
        super().__init__(name="Nclusterbox Detector")
        self.executable_path = executable_path
        self.max_patterns = max_patterns
        self.jobs = jobs
        self.communities_ = []

    def fit(self, graph: nx.Graph, **kwargs) -> 'BaseDetector':
        self.communities_ = []
        timeout = kwargs.get('timeout')
        
        with tempfile.NamedTemporaryFile(mode='w', delete=False, suffix='.txt') as tmp:
            for u, v, data in graph.edges(data=True):
                weight = data.get('weight', 1.0)
                if weight > 0.0:
                    tmp.write(f"{u} {v} {weight}\n")
            tmp_path = tmp.name

        try:
            cmd = [self.executable_path, "-c", "1"]
            if self.max_patterns is not None:
                cmd.extend(["--mss", str(self.max_patterns)])
            if self.jobs is not None:
                cmd.extend(["-j", str(self.jobs)])
            cmd.append(tmp_path)

            try:
                result = subprocess.run(cmd, capture_output=True, text=True, check=True, timeout=timeout)
            except subprocess.TimeoutExpired:
                print(f"nclusterbox excedeu o tempo limite de {timeout}s.")
                return self
            except subprocess.CalledProcessError as e:
                print(f"Erro ao executar nclusterbox. STDERR:\n{e.stderr}")
                raise
            except FileNotFoundError:
                raise FileNotFoundError(f"Binário '{self.executable_path}' não encontrado.")

            output_lines = result.stdout.strip().split('\n')
            for line in output_lines:
                line = line.strip()
                if not line: continue
                parts = line.split(' ')
                nodes_str = parts[0]
                try:
                    nodes = {int(n) for n in nodes_str.split(',')}
                    self.communities_.append(nodes)
                except ValueError:
                    continue
        finally:
            if os.path.exists(tmp_path):
                os.remove(tmp_path)
        return self



    def get_communities(self) -> List[Set[int]]:
        return self.communities_
