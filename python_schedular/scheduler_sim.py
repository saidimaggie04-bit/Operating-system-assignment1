import argparse
import random
import json
import csv
import os
from copy import deepcopy

# ── Scheduling Algorithms ──────────────────────────────────────────

def fcfs(processes):
    """First Come First Served - non-preemptive"""
    procs = sorted(deepcopy(processes), key=lambda p: (p['arrival_time'], p['pid']))
    schedule = []
    time = 0
    for p in procs:
        if time < p['arrival_time']:
            time = p['arrival_time']
        start = time
        end = time + p['burst_time']
        schedule.append((p['pid'], start, end))
        time = end
    return schedule

def sjf(processes):
    """Shortest Job First - non-preemptive"""
    procs = deepcopy(processes)
    schedule = []
    time = 0
    remaining = list(procs)
    while remaining:
        available = [p for p in remaining if p['arrival_time'] <= time]
        if not available:
            time = min(p['arrival_time'] for p in remaining)
            available = [p for p in remaining if p['arrival_time'] <= time]
        shortest = min(available, key=lambda p: (p['burst_time'], p['pid']))
        start = time
        end = time + shortest['burst_time']
        schedule.append((shortest['pid'], start, end))
        time = end
        remaining.remove(shortest)
    return schedule

def priority_scheduling(processes):
    """Priority Scheduling with ageing - non-preemptive"""
    procs = deepcopy(processes)
    for p in procs:
        p['wait_time'] = 0
    schedule = []
    time = 0
    remaining = list(procs)
    while remaining:
        available = [p for p in remaining if p['arrival_time'] <= time]
        if not available:
            time = min(p['arrival_time'] for p in remaining)
            available = [p for p in remaining if p['arrival_time'] <= time]
        # Ageing: every 3 time units waiting adds +1 priority
        for p in available:
            age_bonus = p['wait_time'] // 3
            p['effective_priority'] = max(0, p['priority'] - age_bonus)
        best = min(available, key=lambda p: (p['effective_priority'], p['pid']))
        start = time
        end = time + best['burst_time']
        schedule.append((best['pid'], start, end))
        # Update wait times
        for p in available:
            if p['pid'] != best['pid']:
                p['wait_time'] += best['burst_time']
        time = end
        remaining.remove(best)
    return schedule

def round_robin(processes, quantum=3):
    """Round Robin - preemptive with user-defined quantum"""
    procs = deepcopy(processes)
    for p in procs:
        p['remaining'] = p['burst_time']
    schedule = []
    time = 0
    queue = []
    remaining = sorted(procs, key=lambda p: p['arrival_time'])
    arrived = []
    while remaining or queue:
        # Add newly arrived processes
        while remaining and remaining[0]['arrival_time'] <= time:
            queue.append(remaining.pop(0))
        if not queue:
            time = remaining[0]['arrival_time']
            queue.append(remaining.pop(0))
        p = queue.pop(0)
        run = min(quantum, p['remaining'])
        start = time
        end = time + run
        schedule.append((p['pid'], start, end))
        time = end
        p['remaining'] -= run
        # Add newly arrived during this slice
        while remaining and remaining[0]['arrival_time'] <= time:
            queue.append(remaining.pop(0))
        if p['remaining'] > 0:
            queue.append(p)
    return schedule

# ── Metrics Calculation ────────────────────────────────────────────

def calculate_metrics(processes, schedule):
    """Calculate per-process and aggregate metrics"""
    proc_map = {p['pid']: p for p in processes}
    results = {}
    for pid, start, end in schedule:
        if pid not in results:
            results[pid] = {
                'pid': pid,
                'arrival_time': proc_map[pid]['arrival_time'],
                'burst_time': proc_map[pid]['burst_time'],
                'first_start': start,
                'completion_time': end
            }
        else:
            results[pid]['completion_time'] = end
    rows = []
    for pid, r in results.items():
        tat = r['completion_time'] - r['arrival_time']
        wt = tat - r['burst_time']
        rt = r['first_start'] - r['arrival_time']
        rows.append({
            'pid': pid,
            'arrival_time': r['arrival_time'],
            'burst_time': r['burst_time'],
            'completion_time': r['completion_time'],
            'turnaround_time': tat,
            'waiting_time': wt,
            'response_time': rt
        })
    total_time = max(end for _, _, end in schedule)
    total_burst = sum(p['burst_time'] for p in processes)
    avg_wt = sum(r['waiting_time'] for r in rows) / len(rows)
    avg_tat = sum(r['turnaround_time'] for r in rows) / len(rows)
    avg_rt = sum(r['response_time'] for r in rows) / len(rows)
    cpu_util = (total_burst / total_time) * 100 if total_time > 0 else 0
    throughput = len(processes) / total_time if total_time > 0 else 0
    return rows, {
        'avg_waiting_time': round(avg_wt, 2),
        'avg_turnaround_time': round(avg_tat, 2),
        'avg_response_time': round(avg_rt, 2),
        'cpu_utilization': round(cpu_util, 2),
        'throughput': round(throughput, 4)
    }

# ── Input / Output ─────────────────────────────────────────────────

def generate_random_processes(n, seed=None):
    if seed is not None:
        random.seed(seed)
    processes = []
    for i in range(1, n + 1):
        processes.append({
            'pid': i,
            'name': f'process_{i}',
            'arrival_time': random.randint(0, 10),
            'burst_time': random.randint(1, 15),
            'priority': random.randint(0, 5),
            'memory_req_kb': random.randint(64, 512),
            'owner_id': 1
        })
    return processes

def load_from_csv(filepath):
    processes = []
    with open(filepath, 'r') as f:
        reader = csv.DictReader(f)
        for row in reader:
            processes.append({
                'pid': int(row['pid']),
                'name': row.get('name', f"process_{row['pid']}"),
                'arrival_time': int(row['arrival_time']),
                'burst_time': int(row['burst_time']),
                'priority': int(row.get('priority', 0)),
                'memory_req_kb': int(row.get('memory_req_kb', 128)),
                'owner_id': int(row.get('owner_id', 1))
            })
    return processes

def load_from_json(filepath):
    with open(filepath, 'r') as f:
        data = json.load(f)
    processes = []
    for p in data:
        processes.append({
            'pid': p['pid'],
            'name': p.get('name', f"process_{p['pid']}"),
            'arrival_time': p.get('arrival_time', 0),
            'burst_time': p.get('burst_time', 5),
            'priority': p.get('priority', 0),
            'memory_req_kb': p.get('memory_req_kb', 128),
            'owner_id': p.get('owner_id', 1)
        })
    return processes

def print_results_table(rows, aggregates, algo_name):
    print(f"\n{'='*70}")
    print(f"Algorithm: {algo_name}")
    print(f"{'='*70}")
    print(f"{'PID':<6}{'Arrival':<10}{'Burst':<8}{'Finish':<10}{'TAT':<8}{'WT':<8}{'RT':<6}")
    print(f"{'-'*70}")
    for r in rows:
        print(f"{r['pid']:<6}{r['arrival_time']:<10}{r['burst_time']:<8}"
              f"{r['completion_time']:<10}{r['turnaround_time']:<8}"
              f"{r['waiting_time']:<8}{r['response_time']:<6}")
    print(f"{'-'*70}")
    print(f"Avg WT: {aggregates['avg_waiting_time']}  "
          f"Avg TAT: {aggregates['avg_turnaround_time']}  "
          f"CPU Util: {aggregates['cpu_utilization']}%  "
          f"Throughput: {aggregates['throughput']}")

# ── Main ───────────────────────────────────────────────────────────

def main():
    parser = argparse.ArgumentParser(description='EduOS Scheduler Simulator')
    parser.add_argument('--random', type=int, help='Generate N random processes')
    parser.add_argument('--seed', type=int, default=42, help='Random seed')
    parser.add_argument('--file', type=str, help='Load from CSV or JSON file')
    parser.add_argument('--quantum', type=int, default=3, help='Round Robin quantum')
    args = parser.parse_args()

    # Load processes
    if args.random:
        processes = generate_random_processes(args.random, args.seed)
        print(f"Generated {args.random} random processes (seed={args.seed})")
    elif args.file:
        if args.file.endswith('.csv'):
            processes = load_from_csv(args.file)
        else:
            processes = load_from_json(args.file)
        print(f"Loaded {len(processes)} processes from {args.file}")
    else:
        print("Please use --random N or --file <path>")
        return

    # Run all algorithms
    algorithms = {
        'FCFS': fcfs(processes),
        'SJF': sjf(processes),
        'Priority': priority_scheduling(processes),
        f'RoundRobin(q={args.quantum})': round_robin(processes, args.quantum)
    }

    all_metrics = {}
    for name, schedule in algorithms.items():
        rows, aggregates = calculate_metrics(processes, schedule)
        print_results_table(rows, aggregates, name)
        all_metrics[name] = aggregates

    # Print comparison table
    print(f"\n{'='*70}")
    print("COMPARISON TABLE")
    print(f"{'='*70}")
    print(f"{'Algorithm':<25}{'Avg WT':<12}{'Avg TAT':<12}{'CPU Util%':<12}{'Throughput'}")
    print(f"{'-'*70}")
    for name, m in all_metrics.items():
        print(f"{name:<25}{m['avg_waiting_time']:<12}{m['avg_turnaround_time']:<12}"
              f"{m['cpu_utilization']:<12}{m['throughput']}")

if __name__ == '__main__':
    main()