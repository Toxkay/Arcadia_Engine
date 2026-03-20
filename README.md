# 🎮 Arcadia Engine

A C++ game backend engine implementing core data structures and algorithms across four key subsystems: player management, inventory, world navigation, and server scheduling.

> This is an academic project structured as a hands-on implementation exercise. The header (`ArcadiaEngine.h`) defines the interfaces — students implement the logic in `ArcadiaEngine.cpp`.

---

## 📁 Project Structure

```
Arcadia_Engine/
├── ArcadiaEngine.h           
├── ArcadiaEngine.cpp        
└── main_test_students.cpp    
```

---

## 🧩 Modules

### Part A — Data Structures

| Class | Implementation | Description |
|---|---|---|
| `PlayerTable` | Double Hashing | Maps player IDs to player names with collision resolution |
| `Leaderboard` | Skip List | Maintains ranked scores; ties broken by lower player ID first |
| `AuctionTree` | Red-Black Tree | Stores auction items by ID and price with self-balancing |



### Part B — Inventory System (`InventorySystem`)

Static utility methods using dynamic programming:

| Method | Algorithm | Description |
|---|---|---|
| `optimizeLootSplit` | Partition DP | Minimizes the difference between two coin stacks |
| `maximizeCarryValue` | 0/1 Knapsack | Maximizes item value within a weight capacity |
| `countStringPossibilities` | String DP | Counts the number of ways to decode an encoded string |


### Part C — World Navigator (`WorldNavigator`)

Static graph methods:

| Method | Algorithm | Description |
|---|---|---|
| `pathExists` | BFS/DFS | Returns `true` if a path exists between source and destination |
| `minBribeCost` | MST (Kruskal/Prim) | Minimum cost to connect all nodes; returns `-1` if impossible |
| `sumMinDistancesBinary` | Floyd-Warshall | Sum of all-pairs shortest distances, returned as a binary string |



> **Road data format for `minBribeCost`:** `{u, v, goldCost, silverCost}`

---

### Part D — Server Kernel (`ServerKernel`)

| Method | Algorithm | Description |
|---|---|---|
| `minIntervals` | Greedy Scheduling | Minimum CPU intervals to execute all tasks with cooldown `n` |


## 🔧 Building & Running

### Requirements
- C++17 or later
- A C++ compiler: `g++`, `clang++`, or MSVC

### Compile

```bash
g++ -std=c++17 -o arcadia ArcadiaEngine.cpp main_test_students.cpp
```

### Run Tests

```bash
./arcadia
```

Expected output includes a summary like:
```
==========================================
SUMMARY: Passed: X | Failed: X
==========================================
```

---

## ✅ Test Coverage

The test suite (`main_test_students.cpp`) covers:

- **Happy path** scenarios matching the assignment examples
- **Edge cases**: empty inputs, zero capacity, disconnected graphs, self-loops, duplicate IDs, overflow handling
- **Stress/robustness**: large cycles (no stack overflow), dense graphs, repeated edges, large binary sums

---
