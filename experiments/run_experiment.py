import os
import subprocess
import csv

INSTANCE_DIR = "instancias/2l-cvrp-0"
SOLUTION_DIR = os.path.join(INSTANCE_DIR, "soluciones")
INSTANCE_FILES = [f for f in os.listdir(INSTANCE_DIR) if f.endswith('.dat') or f.endswith('.DAT')]

HEURISTICS = ["cw", "ni", "grasp"]
LOCAL_SEARCH = ["none", "swap", "relocate", "both"]

CSV_FILE = "experiments/results/output.csv"

def parse_output(output):
    result = {}
    for line in output.splitlines():
        if ':' in line:
            key, value = line.split(':', 1)
            result[key.strip()] = value.strip()
    for k in ["capacity", "total_demand", "cost", "num_routes", "time"]:
        if k in result:
            try:
                if k in ["capacity", "total_demand", "num_routes"]:
                    result[k] = int(float(result[k]))
                else:
                    result[k] = float(result[k])
            except Exception:
                pass
    return result

def find_solution_file(instance_file):
    # Busca un archivo de solución que coincida con el nombre base de la instancia
    base = os.path.splitext(os.path.basename(instance_file))[0].lower()
    for fname in os.listdir(SOLUTION_DIR):
        if fname.lower().startswith(base):
            return os.path.join(SOLUTION_DIR, fname)
    return None

def get_best_cost(instance_file):
    sol_file = find_solution_file(instance_file)
    if not sol_file:
        return None
    with open(sol_file) as f:
        for line in f:
            if line.strip().upper().startswith("COST"):
                # Puede ser: COST    : 723.541  o  COST    :723.541
                parts = line.split(":")
                if len(parts) > 1:
                    try:
                        return float(parts[1].strip())
                    except Exception:
                        pass
                # Alternativamente, buscar el primer número en la línea
                for token in line.split():
                    try:
                        return float(token)
                    except Exception:
                        continue
    return None

def run_experiment(instance_file, heuristic, local_search):
    args = ["./build/bin/main_experiment", os.path.join(INSTANCE_DIR, instance_file), heuristic]
    if heuristic != "grasp":
        args.append(local_search)
    else:
        args.append("none")
        args.append("100")  # Iteraciones
        args.append("5")   # rcl_size
    proc = subprocess.run(
        args,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        timeout=120
    )
    output = proc.stdout.decode()
    return output

def main():
    results = []
    for instance_file in INSTANCE_FILES:
        best = get_best_cost(instance_file)
        for heuristic in HEURISTICS:
            if heuristic == "grasp":
                local_searches = ["none"]
            else:
                local_searches = LOCAL_SEARCH
            for local_search in local_searches:
                print(f"Running {instance_file} with {heuristic} + {local_search}")
                output = run_experiment(instance_file, heuristic, local_search)
                data = parse_output(output)
                data["heuristic"] = heuristic
                data["local_search"] = local_search
                if best and data.get("cost"):
                    data["gap"] = 100 * (data["cost"] - best) / best
                else:
                    data["gap"] = ""
                data["best_known"] = best if best is not None else ""
                results.append(data)

    with open(CSV_FILE, "w", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=[
            "instance", "capacity", "total_demand", "heuristic", "local_search",
            "cost", "num_routes", "time", "gap", "best_known", "status", "msg"
        ])
        writer.writeheader()
        for row in results:
            filtered_row = {k: row.get(k, "") for k in writer.fieldnames}
            writer.writerow(filtered_row)

if __name__ == "__main__":
    main()