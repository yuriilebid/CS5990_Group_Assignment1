import networkx as nx
import pandas as pd
import logging
from concurrent.futures import ThreadPoolExecutor

# Configure logging
logging.basicConfig(level=logging.INFO, format="%(asctime)s - %(levelname)s - %(message)s")

def analyze_network(G, name="Unknown"):
    logging.info(f"Starting analysis of {name} network")

    with ThreadPoolExecutor() as executor:
        logging.info(f"{name}: Submitting task for node count")
        future_n = executor.submit(G.number_of_nodes)

        logging.info(f"{name}: Submitting task for average degree calculation")
        future_degree = executor.submit(lambda: sum(dict(G.degree()).values()) / G.number_of_nodes())

        logging.info(f"{name}: Submitting task for average path length calculation")
        future_path_length = executor.submit(nx.average_shortest_path_length, G)

        logging.info(f"{name}: Submitting task for clustering coefficient calculation")
        future_clustering = executor.submit(nx.average_clustering, G)

        # Retrieve results of parallel tasks
        n = future_n.result()
        logging.info(f"{name}: Node count calculation completed")

        avg_degree = future_degree.result()
        logging.info(f"{name}: Average degree calculation completed")

        avg_path_length = future_path_length.result()
        logging.info(f"{name}: Average path length calculation completed")

        clustering_coefficient = future_clustering.result()
        logging.info(f"{name}: Clustering coefficient calculation completed")

    logging.info(f"Analysis of {name} network completed")
    return n, avg_degree, avg_path_length, clustering_coefficient

def compare_networks(original, ws_model, ba_model, name):
    logging.info("Starting network comparison")

    with ThreadPoolExecutor() as executor:
        logging.info("Submitting analysis for the original network")
        future_orig = executor.submit(analyze_network, original, "Original")

        logging.info("Submitting analysis for the WS model")
        future_ws = executor.submit(analyze_network, ws_model, "WS Model")

        logging.info("Submitting analysis for the BA model")
        future_ba = executor.submit(analyze_network, ba_model, "BA Model")

        # Retrieve results of each network analysis
        n_orig, avg_deg_orig, avg_path_orig, cluster_orig = future_orig.result()
        n_ws, avg_deg_ws, avg_path_ws, cluster_ws = future_ws.result()
        n_ba, avg_deg_ba, avg_path_ba, cluster_ba = future_ba.result()

    logging.info("Network comparison completed")

    # Construct DataFrame with results
    data = {
        "Network": [name],
        "Original Size": [n_orig],
        "Original Avg Degree": [avg_deg_orig],
        "Original Path Length": [avg_path_orig],
        "Original Clustering": [cluster_orig],
        "WS Path Length": [avg_path_ws],
        "WS Clustering": [cluster_ws],
        "BA Path Length": [avg_path_ba],
        "BA Clustering": [cluster_ba]
    }
    return pd.DataFrame(data)
