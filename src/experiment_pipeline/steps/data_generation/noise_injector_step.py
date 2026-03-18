import networkx as nx
import numpy as np
from scipy.stats import beta
from src.experiment_pipeline.steps.pipeline_step import PipelineStep
from src.experiment_pipeline.experiment_state import ExperimentState

class NoiseInjectorStep(PipelineStep):
    def __init__(self, beta_param: float = 2.0):
        super().__init__(name="Noise Injector")
        self.beta_param = beta_param

    def run(self, state: ExperimentState) -> ExperimentState:
        """
        Aplica ruído estocástico aos pesos das arestas utilizando o método de 
        Amostragem por Transformada Inversa (ITS) com base na distribuição Beta.
        """
        if state.graph is None:
            raise ValueError("O grafo no ExperimentState está vazio.")
            
        state.graph = self.apply_beta_its_noise(
            graph=state.graph, 
            alpha=state.number_of_correct_observations, 
            beta_param=self.beta_param, 
            random_state=state.random_seed
        )
        return state

    def apply_beta_its_noise(
        self,
        graph: nx.Graph, 
        alpha: float, 
        beta_param: float = 2.0, 
        random_state: int = None
    ) -> nx.Graph:
        if random_state is not None:
            np.random.seed(random_state)
            
        G_noisy = graph.copy()
        
        # Extrai as arestas
        edges = list(G_noisy.edges())
        num_edges = len(edges)
        
        if num_edges == 0:
            return G_noisy
            
        # Passo 1: Gerar U ~ Uniforme(0, 1) para cada aresta
        U = np.random.uniform(0, 1, num_edges)
        
        # Passo 2: Aplicar a Transformada Inversa da Beta
        noisy_weights = beta.ppf(U, alpha, beta_param)
        
        # Passo 3: Atualizar o grafo com os novos pesos fuzzy
        for i, (u, v) in enumerate(edges):
            G_noisy[u][v]['weight'] = round(float(noisy_weights[i]), 4)
            
        return G_noisy
