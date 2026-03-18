import os
import tempfile
import subprocess
import networkx as nx
from typing import List, Set
from .base import BaseDetector

class NclusterboxDetector(BaseDetector):
    """
    Wrapper para o algoritmo nclusterbox, focado na fatoração de tensores fuzzy.
    Faz a ponte comunicando-se com o executável binário.
    """
    
    def __init__(self, executable_path: str = "nclusterbox", max_patterns: int = None, jobs: int = None):
        """
        Parâmetros:
        -----------
        executable_path : str
            Caminho para o binário do nclusterbox (padrão assume que está no PATH).
        max_patterns : int
            Limite de padrões para sumarização (opção --mss do nclusterbox). 
            Se definido, para a extração ao atingir o limite.
        jobs : int
            Número de threads concorrentes para a busca de padrões (opção -j).
        """
        self.executable_path = executable_path
        self.max_patterns = max_patterns
        self.jobs = jobs
        self.communities_ = []

    def fit(self, graph: nx.Graph, **kwargs) -> 'BaseDetector':
        self.communities_ = []
        
        # 1. Escrever o grafo num ficheiro temporário no formato "tensor de 2 dimensões"
        # Formato: "nó_A nó_B grau_de_pertinência"
        with tempfile.NamedTemporaryFile(mode='w', delete=False, suffix='.txt') as tmp:
            for u, v, data in graph.edges(data=True):
                weight = data.get('weight', 1.0)
                # Como o nclusterbox assume ausência de arestas como 0, 
                # evitamos escrever arestas cujo peso caiu para zero absoluto pelo ruído
                if weight > 0.0:
                    tmp.write(f"{u} {v} {weight}\n")
            tmp_path = tmp.name

        try:
            # 2. Construir o comando
            # A flag '-c 1' diz ao nclusterbox para tratar as dimensões adjacentes a partir da 1
            # (ou seja, nós de origem e nós de destino) como um grafo estático, ativando
            # simetria e auto-loops para encontrar comunidades.
            cmd = [self.executable_path, "-c", "1"]
            
            if self.max_patterns is not None:
                cmd.extend(["--mss", str(self.max_patterns)])
                
            if self.jobs is not None:
                cmd.extend(["-j", str(self.jobs)])
                
            cmd.append(tmp_path)

            # 3. Executar o binário do nclusterbox
            try:
                result = subprocess.run(cmd, capture_output=True, text=True, check=True)
            except subprocess.CalledProcessError as e:
                print(f"Erro ao executar nclusterbox. STDERR:\n{e.stderr}")
                raise
            except FileNotFoundError:
                raise FileNotFoundError(
                    f"O binário '{self.executable_path}' não foi encontrado. "
                    "Certifique-se de que o nclusterbox está compilado e adicionado ao seu PATH."
                )

            # 4. Parsing da Saída
            # De acordo com a documentação, o formato de saída padrão separa dimensões com ' '
            # e elementos com ','. Num tensor de 2 dimensões sob '-c 1', teremos algo como:
            # "node1,node2,node3 node1,node2,node3 0.854"
            # O primeiro token é o conjunto de vértices da comunidade.
            output_lines = result.stdout.strip().split('\n')
            
            for line in output_lines:
                line = line.strip()
                if not line:
                    continue
                    
                parts = line.split(' ') # Separa as dimensões e densidade
                
                # parts[0] contém os elementos da primeira dimensão (vértices da comunidade) separados por vírgula
                nodes_str = parts[0]
                
                # Converte para inteiros e adiciona ao conjunto
                try:
                    nodes = {int(n) for n in nodes_str.split(',')}
                    self.communities_.append(nodes)
                except ValueError:
                    # Caso encontre algum cabeçalho inesperado ou aviso misturado
                    continue

        finally:
            # 5. Limpar ficheiro temporário
            if os.path.exists(tmp_path):
                os.remove(tmp_path)
                
        return self

    def get_communities(self) -> List[Set[int]]:
        return self.communities_
    
    