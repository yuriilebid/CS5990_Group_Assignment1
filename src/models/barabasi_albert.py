import networkx as nx
import random
import logging
import time


def barabasi_albert_graph(n, m):
    logging.info(f"Starting Barabasi-Albert model simulation with {n} nodes and {m} edges per new node.")

    # Время начала
    start_time = time.time()

    # Инициализация графа как полного графа из m узлов
    G = nx.complete_graph(m)
    logging.info(f"Initial complete graph created with {m} nodes.")

    # Список узлов для выбора
    targets = list(G.nodes())
    repeated_nodes = targets[:]

    # Добавляем узлы по одному и создаем m новых рёбер
    for i in range(m, n):
        if i % 100 == 0:  # Логировать прогресс каждые 100 узлов
            logging.info(
                f"Adding node {i} with {m} edges. Current graph has {G.number_of_nodes()} nodes and {G.number_of_edges()} edges.")

        G.add_node(i)
        new_edges = random.sample(repeated_nodes, m)
        G.add_edges_from((i, target) for target in new_edges)

        # Обновляем список узлов
        repeated_nodes.extend(new_edges)
        repeated_nodes.append(i)

    logging.info(
        f"Finished Barabasi-Albert model simulation. Final graph has {G.number_of_nodes()} nodes and {G.number_of_edges()} edges.")
    logging.info(f"Time taken for Barabasi-Albert simulation: {time.time() - start_time:.2f} seconds")

    return G
