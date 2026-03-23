# POC: Community Detection Evaluation (nclusterbox vs Louvain)

This project is an experimental laboratory designed to compare the resilience of community detection algorithms under different levels of stochastic noise in static graphs (adjacency matrices).

## 🚀 Quick Start

### Prerequisites
- Python 3.12+
- [Poetry](https://python-poetry.org/docs/#installation)

For nclusterbox:

```bash
sudo apt-get update
sudo apt-get install -y clang libboost-dev libboost-program-options-dev libjemalloc-dev help2man
```

### Installation

1. Install dependencies and create the virtual environment:
   ```bash
   poetry install
   ```

### Running Experiments

The project is structured to be executed as a Python module. To run the experiments defined in `config.yaml`, use the following command:

```bash
poetry run python -m src.main
```

To run a subset of experiments, use the `-e` flag followed by the experiment names:
```bash
poetry run python -m src.main -e "Experiment Name 1" "Experiment Name 2"
```

## ⚙️ Configuration

Experiments are defined in the `config.yaml` file at the root of the project. You can declare multiple experiments with different parameters:

```yaml
experiments:
  - name: "Louvain Low Noise"
    ground_truth_generation:
      random_seed: 42
      number_of_vertices: 200
      number_of_ground_truth_communities: 4
    noise_injection:
      number_of_correct_observations: 16.0 # Higher = less noise
    community_detection:
      algorithm: "louvain" # or "nclusterbox"
      timeout: 30 # optional timeout in seconds
```

## 🏗️ Architecture

- **Pipeline Pattern**: The execution flow follows a `PipelineStep` architecture (Generation -> Noise -> Algorithm -> Evaluation).
- **ExperimentState**: A central object that carries configuration and runtime data (graph, ground truth, detected communities) through each step of the pipeline.
- **Metrics**: Accuracy is measured using the Jaccard Index, comparing each detected community against the best matching community in the Ground Truth.
