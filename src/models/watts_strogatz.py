import networkx as nx
import random
import logging
import time


def watts_strogatz_graph(n, k, beta):
    logging.info(
        f"Starting Watts-Strogatz model simulation with {n} nodes, {k} neighbors per node, and rewiring probability {beta}.")

    start_time = time.time()

    # Инициализация графа и создание кольцевого ланцюга
    G = nx.Graph()
    nodes = list(range(n))

    # Создание связи каждого узла с ближайшими соседями
    logging.info("Creating ring lattice...")
    for i in range(n):
        for j in range(1, k // 2 + 1):
            G.add_edge(i, (i + j) % n)
            G.add_edge(i, (i - j) % n)
    logging.info(f"Ring lattice created with {G.number_of_edges()} edges.")

    # Перестановка рёбер с вероятностью beta
    logging.info("Starting edge rewiring...")
    rewired_edges = 0
    edges = list(G.edges())

    for idx, edge in enumerate(edges):
        if random.random() < beta:
            u, v = edge
            # Выбираем новую вершину для соединения с u, не совпадающую с соседями u
            new_v = random.choice(list(set(nodes) - {u} - set(G.neighbors(u))))
            G.remove_edge(u, v)
            G.add_edge(u, new_v)
            rewired_edges += 1

            if rewired_edges % 100 == 0:  # Логировать каждые 100 перестановок
                logging.info(f"Rewired {rewired_edges} edges so far.")

    logging.info(f"Edge rewiring completed. Total rewired edges: {rewired_edges}.")
    logging.info(f"Final graph has {G.number_of_nodes()} nodes and {G.number_of_edges()} edges.")
    logging.info(f"Time taken for Watts-Strogatz simulation: {time.time() - start_time:.2f} seconds")

    return G
