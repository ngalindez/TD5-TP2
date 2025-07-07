import subprocess
import csv
import os
import shutil

script_dir = os.path.dirname(os.path.abspath(__file__))

# Configurable
instances_dir = os.path.join(script_dir, "../benchmarks")
# Try multiple possible solver locations
possible_executables = [
    os.path.join(script_dir, "../build/bin/cvrp_solver"),
    os.path.join(script_dir, "../build/cvrp_solver"),
    os.path.join(script_dir, "../bin/cvrp_solver")
]
executable = None
for path in possible_executables:
    if os.path.isfile(path) and os.access(path, os.X_OK):
        executable = path
        break
if executable is None:
    run_sh_path = os.path.join(script_dir, "../run.sh")
    if os.path.isfile(run_sh_path) and os.access(run_sh_path, os.X_OK):
        executable = run_sh_path
        use_run_sh = True
    else:
        print("ERROR: Could not find cvrp_solver executable or run.sh script. Please build the project.")
        exit(1)
else:
    use_run_sh = False
results_path = os.path.join(script_dir, "results/output.csv")

# Experiments definition
experiments = [
    {
        "instance": "E051.dat",
        "heuristic": "1",  # Clarke & Wright
        "operator": "3",   # Both Swap + Relocate
    },
    {
        "instance": "E051.dat",
        "heuristic": "2",  # Nearest Insertion
        "operator": "3",
    },
    {
        "instance": "E051.dat",
        "heuristic": "3",  # GRASP
        "grasp_iter": "50",
        "grasp_rcl": "3",
    }
]

# Prepare CSV
os.makedirs(os.path.dirname(results_path), exist_ok=True)
with open(results_path, "w", newline="") as csvfile:
    writer = csv.writer(csvfile)
    writer.writerow(["Instance", "Heuristic", "Operator", "Cost", "Time"])

    for exp in experiments:
        print(f"Running {exp['instance']} heuristic {exp['heuristic']}...")

        # Build the input string as if the user typed it interactively
        inputs = []
        inputs.append(os.path.join(instances_dir, exp["instance"]))
        inputs.append(exp["heuristic"])

        if exp["heuristic"] == "3":
            # GRASP requires extra inputs
            inputs.append(exp["grasp_iter"])
            inputs.append(exp["grasp_rcl"])
        else:
            # For heuristics 1 or 2, select operator
            inputs.append(exp["operator"])
            inputs.append("4")  # Exit immediately after operator

        # Join all inputs as lines separated by \n
        input_str = "\n".join(inputs) + "\n"

        # Run the subprocess
        if use_run_sh:
            # Use run.sh run
            result = subprocess.run(
                [executable, "run"],
                input=input_str,
                capture_output=True,
                text=True
            )
        else:
            result = subprocess.run(
                [executable],
                input=input_str,
                capture_output=True,
                text=True
            )

        # Output raw for debugging
        print(result.stdout)

        # Extract COST and TIME
        cost = None
        time_ = None
        for line in result.stdout.splitlines():
            if line.startswith("COST:"):
                cost = line.split(":")[1].strip()
            if line.startswith("TIME:"):
                time_ = line.split(":")[1].strip()

        if cost is None or time_ is None:
            print("WARNING: Could not find COST or TIME in output.")
        
        writer.writerow([
            exp["instance"],
            exp["heuristic"],
            exp.get("operator", "-"),
            cost,
            time_
        ])
