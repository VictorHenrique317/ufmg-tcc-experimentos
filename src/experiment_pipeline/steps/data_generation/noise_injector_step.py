import networkx as nx
import numpy as np
from scipy.stats import beta
from src.experiment_pipeline.steps.pipeline_step import PipelineStep

class NoiseInjectorStep(PipelineStep):
    def __init__(self, alpha: float = 4.0, beta_param: float = 2.0, random_state: int = None):
        super().__init__(name="Noise Injector")
        self.alpha = alpha
        self.beta_param = beta_param
        self.random_state = random_state


    def run(self, graph: nx.Graph) -> nx.Graph:
        """
        Aplica ruído estocástico aos pesos das arestas utilizando o método de 
        Amostragem por Transformada Inversa (ITS) com base na distribuição Beta.
        Isto converte um grafo booleano num tensor fuzzy [0, 1] ideal para o nclusterbox.

        Parâmetros:
        -----------
        graph : nx.Graph
            O grafo original de entrada (com pesos = 1.0).

        Retorna:
        --------
        nx.Graph
            Uma nova instância do grafo (para evitar mutação do original) com 
            pesos fuzzy transformados (ruidosos).
        """
        return self.apply_beta_its_noise(
            graph=graph, 
            alpha=self.alpha, 
            beta_param=self.beta_param, 
            random_state=self.random_state
        )

    def apply_beta_its_noise(
        self,
        graph: nx.Graph, 
        alpha: float, 
        beta_param: float = 2.0, 
        random_state: int = None
    ) -> nx.Graph:
        """
        Aplica ruído estocástico aos pesos das arestas utilizando o método de 
        Amostragem por Transformada Inversa (ITS) com base na distribuição Beta.
        Isto converte um grafo booleano num tensor fuzzy [0, 1] ideal para o nclusterbox.

        Matemática do ITS:
        ------------------
        Seja U ~ Uniforme(0, 1). 
        Amostra = F^{-1}(U; alpha, beta), onde F^{-1} é a Função Ponto Percentil (PPF)
        (a inversa da Função de Distribuição Acumulada da distribuição Beta).

        Parâmetros:
        -----------
        graph : nx.Graph
            O grafo original de entrada (com pesos = 1.0).
        alpha : float
            O parâmetro de "observações corretas" da distribuição Beta (ex: 1, 2, 4, 8, 16).
            Valores maiores concentram os pesos perto de 1 (menos ruído destrutivo).
        beta_param : float
            O parâmetro que representa as perturbações (fixo por defeito para modular apenas o alpha).
        random_state : int
            Semente aleatória para reprodutibilidade.

        Retorna:
        --------
        nx.Graph
            Uma nova instância do grafo (para evitar mutação do original) com 
            pesos fuzzy transformados (ruidosos).
        """
        if random_state is not None:
            np.random.seed(random_state)
            
        G_noisy = graph.copy()
        
        # Extrai as arestas
        edges = list(G_noisy.edges())
        num_edges = len(edges)
        
        # Passo 1: Gerar U ~ Uniforme(0, 1) para cada aresta
        U = np.random.uniform(0, 1, num_edges)
        
        # Passo 2: Aplicar a Transformada Inversa da Beta
        # Em SciPy, beta.ppf é a inversa da CDF.
        noisy_weights = beta.ppf(U, alpha, beta_param)
        
        # Passo 3: Atualizar o grafo com os novos pesos fuzzy
        for i, (u, v) in enumerate(edges):
            # Arredondamos para 4 casas decimais para manter a escrita/leitura em ficheiros limpa
            G_noisy[u][v]['weight'] = round(float(noisy_weights[i]), 4)
            
        return G_noisy
