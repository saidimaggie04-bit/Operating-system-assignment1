import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import numpy as np

COLORS = ['#FF6B6B','#4ECDC4','#45B7D1','#96CEB4',
          '#FFEAA7','#DDA0DD','#98D8C8','#F7DC6F']

def plot_gantt(schedule, processes, algo_name, save_path=None):
    """Plot Gantt chart for a scheduling algorithm"""
    fig, ax = plt.subplots(figsize=(14, 4))
    proc_map = {p['pid']: p for p in processes}
    color_map = {}
    color_idx = 0
    for pid, start, end in schedule:
        if pid not in color_map:
            color_map[pid] = COLORS[color_idx % len(COLORS)]
            color_idx += 1
        ax.barh(0, end - start, left=start, height=0.5,
                color=color_map[pid], edgecolor='black', linewidth=0.5)
        ax.text((start + end) / 2, 0, f"P{pid}",
                ha='center', va='center', fontsize=8, fontweight='bold')
    max_time = max(end for _, _, end in schedule)
    ax.set_xlim(0, max_time)
    ax.set_xticks(range(0, max_time + 1))
    ax.set_yticks([])
    ax.set_xlabel('Time Units')
    ax.set_title(f'Gantt Chart - {algo_name}')
    patches = [mpatches.Patch(color=color_map[pid],
               label=f"P{pid}: {proc_map[pid]['name']}")
               for pid in color_map]
    ax.legend(handles=patches, loc='upper right', fontsize=7)
    plt.tight_layout()
    if save_path:
        plt.savefig(save_path, dpi=100, bbox_inches='tight')
        print(f"Gantt chart saved to {save_path}")
    plt.close()

def plot_comparison(all_metrics, save_path=None):
    """Plot comparison bar charts for all algorithms"""
    algos = list(all_metrics.keys())
    awt = [all_metrics[a]['avg_waiting_time'] for a in algos]
    atat = [all_metrics[a]['avg_turnaround_time'] for a in algos]
    cpu = [all_metrics[a]['cpu_utilization'] for a in algos]
    x = np.arange(len(algos))
    width = 0.25
    fig, ax = plt.subplots(figsize=(12, 6))
    ax.bar(x - width, awt, width, label='Avg WT', color='#FF6B6B')
    ax.bar(x, atat, width, label='Avg TAT', color='#4ECDC4')
    ax.bar(x + width, cpu, width, label='CPU Util%', color='#45B7D1')
    ax.set_xlabel('Algorithm')
    ax.set_ylabel('Value')
    ax.set_title('Scheduling Algorithm Comparison')
    ax.set_xticks(x)
    ax.set_xticklabels(algos, rotation=15)
    ax.legend()
    plt.tight_layout()
    if save_path:
        plt.savefig(save_path, dpi=100, bbox_inches='tight')
        print(f"Comparison chart saved to {save_path}")
    plt.close()
