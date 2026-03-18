from abc import ABC, abstractmethod
import networkx as nx
from typing import List, Set

from src.experiment_pipeline.steps.pipeline_step import PipelineStep

class BaseDetector(PipelineStep):
    """
    Interface base genérica para todos os algoritmos de detecção de comunidades.
    Garante que todos os modelos expõem a mesma API para o pipeline de avaliação.
    """
    
    @abstractmethod
    def fit(self, graph: nx.Graph, **kwargs) -> 'BaseDetector':
        """
        Executa o algoritmo sobre o grafo fornecido.
        
        Parâmetros:
        -----------
        graph : nx.Graph
            O grafo (possivelmente ruidoso e ponderado) a ser analisado.
            
        Retorna:
        --------
        self : Retorna a própria instância para encadeamento.
        """
        pass

    @abstractmethod
    def get_communities(self) -> List[Set[int]]:
        """
        Recupera as comunidades detectadas após o uso do método `fit`.
        
        Retorna:
        --------
        List[Set[int]]
            Uma lista contendo conjuntos de nós. Cada conjunto representa uma comunidade.
        """
        pass

    def run(self, graph: nx.Graph, **kwargs) -> List[Set[int]]:
        """
        Método de conveniência para executar o fit e obter as comunidades em um único passo.
        """
        return self.fit(graph, **kwargs).get_communities()