import pandas as pd
import networkx as nx
import logging


def load_twitch_data(filename):
    logging.info(f"Starting to load Twitch data from {filename}")

    G = nx.Graph()
    chunk_number = 1

    # Чтение файла частями
    for chunk in pd.read_csv(filename, header=0, chunksize=100000, dtype={'numeric_id_1': int, 'numeric_id_2': int}):
        logging.info(f"Processing chunk {chunk_number} for Twitch data")
        G.add_edges_from(chunk.values)
        chunk_number += 1

    logging.info("Finished loading Twitch data. Extracting the largest connected component.")

    largest_cc = max(nx.connected_components(G), key=len)
    logging.info("Largest connected component extracted.")

    return G.subgraph(largest_cc).copy()


def load_amazon_data(filename):
    logging.info(f"Starting to load Amazon data from {filename}")

    G = nx.Graph()
    chunk_number = 1

    # Чтение файла частями
    for chunk in pd.read_csv(filename, sep='\t', comment='#', chunksize=100000, names=['FromNodeId', 'ToNodeId'],
                             dtype={'FromNodeId': int, 'ToNodeId': int}):
        logging.info(f"Processing chunk {chunk_number} for Amazon data")
        G.add_edges_from(chunk.values)
        chunk_number += 1

    logging.info("Finished loading Amazon data. Extracting the largest connected component.")

    largest_cc = max(nx.connected_components(G), key=len)
    logging.info("Largest connected component extracted.")

    return G.subgraph(largest_cc).copy()
