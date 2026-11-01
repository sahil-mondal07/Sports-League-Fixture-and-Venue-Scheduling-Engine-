"""Build an offline graphify view from the project's markdown documentation."""

from pathlib import Path
import json

from graphify.analyze import god_nodes, suggest_questions, surprising_connections
from graphify.build import build_from_json
from graphify.cluster import cluster, score_all
from graphify.export import generate_html, to_json
from graphify.report import generate


ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "graphify-out"

DOCUMENTS = [
    "PROJECT_OVERVIEW.md",
    "ARCHITECTURE.md",
    "ALGORITHM_MAPPING.md",
    "DATA_STRUCTURES.md",
    "IMPLEMENTATION_TIMELINE.md",
    "TESTING_STRATEGY.md",
]

CONCEPTS = {
    "PROJECT_OVERVIEW.md": [
        "Football Scheduling", "Scheduling Engine", "Algorithm Library", "Data Structures",
        "CLI Interface", "Benchmark Mode", "Fairness Validation", "Travel Cost",
    ],
    "ARCHITECTURE.md": [
        "CLI Interface", "Scheduling Engine", "Data Layer", "Algorithm Library",
        "Utilities", "Benchmark and Testing", "Fixture Generator", "Venue Allocator",
        "Travel Optimizer", "Conflict Resolver", "Fairness Validator",
    ],
    "ALGORITHM_MAPPING.md": [
        "Merge Sort", "Binary Search", "Activity Selection", "Dijkstra", "Prim",
        "Kruskal", "Knapsack", "Bellman-Ford", "Floyd-Warshall", "Bounded TSP",
        "Ford-Fulkerson", "Bipartite Matching", "String Matching", "Chinese Remainder Theorem",
        "Fixture Generation", "Venue Allocation", "Travel Optimization", "Broadcast Capacity",
    ],
    "DATA_STRUCTURES.md": [
        "Team", "Venue", "Fixture", "Travel Graph", "Schedule", "Priority Queue",
        "Union-Find", "Shortest Path Result", "MST Result", "Scheduling Engine",
    ],
    "IMPLEMENTATION_TIMELINE.md": [
        "Foundation", "Graph Algorithms", "Greedy Algorithms", "Dynamic Programming",
        "Divide and Conquer", "Network Flow", "Backtracking", "Branch and Bound",
        "Testing and Documentation", "Dijkstra", "Merge Sort", "Ford-Fulkerson",
    ],
    "TESTING_STRATEGY.md": [
        "Unit Tests", "Integration Tests", "End-to-End Tests", "Benchmark Tests",
        "Merge Sort", "Dijkstra", "Fixture Generation", "Venue Allocation",
        "Travel Optimization", "Fairness Validation", "Scenario Testing",
    ],
}

RELATIONSHIPS = [
    ("Scheduling Engine", "Fixture Generation", "drives"),
    ("Scheduling Engine", "Venue Allocation", "drives"),
    ("Scheduling Engine", "Travel Optimization", "drives"),
    ("Scheduling Engine", "Fairness Validation", "drives"),
    ("Venue Allocation", "Activity Selection", "uses"),
    ("Travel Optimization", "Dijkstra", "uses"),
    ("Travel Optimization", "Bellman-Ford", "compares_with"),
    ("Travel Optimization", "Floyd-Warshall", "uses"),
    ("Travel Optimization", "Bounded TSP", "uses"),
    ("Venue", "Travel Graph", "connects_to"),
    ("Travel Graph", "Priority Queue", "supports"),
    ("Kruskal", "Union-Find", "uses"),
    ("Prim", "Travel Graph", "operates_on"),
    ("Broadcast Capacity", "Knapsack", "constrains"),
    ("Fixture Generation", "Fixture", "produces"),
    ("Fairness Validation", "Schedule", "validates"),
    ("Benchmark Mode", "Benchmark Tests", "measured_by"),
    ("Testing and Documentation", "Unit Tests", "includes"),
    ("Testing and Documentation", "Integration Tests", "includes"),
    ("Testing and Documentation", "End-to-End Tests", "includes"),
]


def node(node_id, label, node_type, source_file, description=""):
    return {
        "id": node_id,
        "label": label,
        "type": node_type,
        "source_file": source_file,
        "source_location": source_file,
        "description": description,
    }


def edge(source, target, relation, source_file, confidence="EXTRACTED"):
    return {
        "source": source,
        "target": target,
        "relation": relation,
        "confidence": confidence,
        "confidence_score": 1.0 if confidence == "EXTRACTED" else 0.8,
        "source_file": source_file,
    }


def main():
    nodes = []
    edges = []
    concept_files = {}

    for document in DOCUMENTS:
        path = ROOT / document
        if not path.exists():
            continue
        doc_id = f"doc:{document}"
        nodes.append(node(doc_id, document.removesuffix(".md"), "document", document))
        for concept in dict.fromkeys(CONCEPTS[document]):
            concept_id = f"concept:{concept.lower().replace(' ', '-') }"
            if concept_id not in concept_files:
                concept_files[concept_id] = document
                nodes.append(node(concept_id, concept, "concept", document))
            edges.append(edge(doc_id, concept_id, "contains", document))

    for source, target, relation in RELATIONSHIPS:
        source_id = f"concept:{source.lower().replace(' ', '-') }"
        target_id = f"concept:{target.lower().replace(' ', '-') }"
        if source_id in concept_files and target_id in concept_files:
            edges.append(edge(source_id, target_id, relation, concept_files[source_id], "INFERRED"))

    extraction = {"nodes": nodes, "edges": edges, "hyperedges": [], "input_tokens": 0, "output_tokens": 0}
    graph = build_from_json(extraction, root=str(ROOT))
    communities = cluster(graph)
    labels = {community: f"Project area {community + 1}" for community in communities}
    cohesion = score_all(graph, communities)
    gods = god_nodes(graph)
    surprises = surprising_connections(graph, communities)
    questions = suggest_questions(graph, communities, labels)

    OUT.mkdir(exist_ok=True)
    to_json(graph, communities, str(OUT / "graph.json"), force=True, community_labels=labels)
    generate_html(graph, communities, str(OUT / "graph.html"), community_labels=labels)

    detection = {
        "scan_root": str(ROOT),
        "total_files": len(DOCUMENTS),
        "total_words": sum(len((ROOT / name).read_text(encoding="utf-8").split()) for name in DOCUMENTS if (ROOT / name).exists()),
        "files": {"document": [str(ROOT / name) for name in DOCUMENTS if (ROOT / name).exists()]},
    }
    report = generate(graph, communities, cohesion, labels, gods, surprises, detection, {"input": 0, "output": 0}, str(ROOT), suggested_questions=questions)
    (OUT / "GRAPH_REPORT.md").write_text(report, encoding="utf-8")
    (OUT / "graph-build.json").write_text(json.dumps({"documents": DOCUMENTS, "nodes": len(nodes), "edges": len(edges)}, indent=2), encoding="utf-8")
    print(f"Graph: {graph.number_of_nodes()} nodes, {graph.number_of_edges()} edges, {len(communities)} communities")
    print(f"Outputs: {OUT / 'graph.html'}, {OUT / 'graph.json'}, {OUT / 'GRAPH_REPORT.md'}")


if __name__ == "__main__":
    main()