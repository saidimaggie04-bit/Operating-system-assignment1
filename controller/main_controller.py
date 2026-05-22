import subprocess
import json
import os
import sys
import time
from datetime import datetime

def run_c_simulator():
    print("[Controller] Starting C simulator...")
    c_binary = os.path.join(os.path.dirname(__file__), '..', 'c_core', 'eduos')
    if not os.path.exists(c_binary):
        print(f"[Controller] ERROR: C binary not found")
        print("[Controller] Please run 'make all' in c_core/ first")
        return None
    try:
        process = subprocess.Popen([c_binary], stdout=subprocess.PIPE,
                                   stderr=subprocess.PIPE, text=True)
        stdout, stderr = process.communicate(timeout=30)
        print("[Controller] C simulator output:")
        print(stdout)
        return stdout
    except FileNotFoundError:
        print("[Controller] ERROR: Could not find C binary")
        return None

def load_pcb_snapshot():
    snapshot_path = os.path.join(os.path.dirname(__file__), '..', 'c_core', 'pcb_snapshot.json')
    if not os.path.exists(snapshot_path):
        print("[Controller] PCB snapshot not found, using sample data")
        return None
    with open(snapshot_path, 'r') as f:
        data = json.load(f)
    print(f"[Controller] Loaded {len(data)} processes from PCB snapshot")
    return data

def run_python_scheduler(processes):
    print("\n[Controller] Starting Python scheduler...")
    scheduler_path = os.path.join(os.path.dirname(__file__), '..', 'python_schedular', 'scheduler_sim.py')
    temp_json = os.path.join(os.path.dirname(__file__), '..', 'pcb_input.json')
    with open(temp_json, 'w') as f:
        json.dump(processes, f)
    try:
        result = subprocess.run([sys.executable, scheduler_path, '--file', temp_json],
                                capture_output=True, text=True, timeout=60)
        print(result.stdout)
        return result.stdout
    except subprocess.TimeoutExpired:
        print("[Controller] ERROR: Scheduler timed out")
        return None

def generate_report(scheduler_output):
    report = {
        'timestamp': datetime.now().isoformat(),
        'simulation_id': f"sim_{int(time.time())}",
        'status': 'completed',
        'scheduler_output': scheduler_output
    }
    report_path = os.path.join(os.path.dirname(__file__), '..', 'simulation_report.json')
    with open(report_path, 'w') as f:
        json.dump(report, f, indent=2)
    print(f"\n[Controller] Report saved to simulation_report.json")
    return report

def main():
    print("=" * 60)
    print("  EduOS Main Controller")
    print("=" * 60)
    c_output = run_c_simulator()
    processes = load_pcb_snapshot()
    if not processes:
        processes = [
            {'pid': 1, 'name': 'calculator', 'arrival_time': 0, 'burst_time': 8, 'priority': 2, 'memory_req_kb': 128, 'owner_id': 1},
            {'pid': 2, 'name': 'text_editor', 'arrival_time': 1, 'burst_time': 4, 'priority': 1, 'memory_req_kb': 256, 'owner_id': 1},
            {'pid': 3, 'name': 'file_manager', 'arrival_time': 2, 'burst_time': 9, 'priority': 3, 'memory_req_kb': 512, 'owner_id': 1},
            {'pid': 4, 'name': 'web_browser', 'arrival_time': 3, 'burst_time': 5, 'priority': 2, 'memory_req_kb': 1024, 'owner_id': 1},
        ]
        print("[Controller] Using default sample processes")
    scheduler_output = run_python_scheduler(processes)
    generate_report(scheduler_output)
    print("\n" + "=" * 60)
    print("  EduOS Simulation Complete!")
    print("=" * 60)

if __name__ == '__main__':
    main()
