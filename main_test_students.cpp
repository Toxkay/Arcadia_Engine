/**
 * main_test_student.cpp
 * Basic "Happy Path" Test Suite for ArcadiaEngine
 * Use this to verify your basic logic against the assignment examples.
 */

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <functional>
#include "ArcadiaEngine.h" 

using namespace std;

// ==========================================
// FACTORY FUNCTIONS (LINKING)
// ==========================================
// These link to the functions at the bottom of your .cpp file
extern "C" {
    PlayerTable* createPlayerTable();
    Leaderboard* createLeaderboard();
    AuctionTree* createAuctionTree();
}

// ==========================================
// TEST UTILITIES
// ==========================================
class StudentTestRunner {
	int count = 0;
    int passed = 0;
    int failed = 0;

public:
    void runTest(string testName, bool condition) {
		count++;
        cout << "TEST: " << left << setw(50) << testName;
        if (condition) {
            cout << "[ PASS ]";
            passed++;
        } else {
            cout << "[ FAIL ]";
            failed++;
        }
        cout << endl;
    }

    void printSummary() {
        cout << "\n==========================================" << endl;
        cout << "SUMMARY: Passed: " << passed << " | Failed: " << failed << endl;
        cout << "==========================================" << endl;
		cout << "TOTAL TESTS: " << count << endl;
        if (failed == 0) {
            cout << "Great job! All basic scenarios passed." << endl;
            cout << "Now make sure to handle edge cases (empty inputs, collisions, etc.)!" << endl;
        } else {
            cout << "Some basic tests failed. Check your logic against the PDF examples." << endl;
        }
    }
};

StudentTestRunner runner;

// ==========================================
// PART A: DATA STRUCTURES
// ==========================================

void test_PartA_DataStructures() {
    cout << "\n--- Part A: Data Structures ---" << endl;

    // 1. PlayerTable (Double Hashing)
    // Requirement: Basic Insert and Search
    PlayerTable* table = createPlayerTable();
    runner.runTest("PlayerTable: Insert 'Alice' and Search", [&]() {
        table->insert(101, "Alice");
        return table->search(101) == "Alice";
    }());
    delete table;

    // 2. Leaderboard (Skip List)
    Leaderboard* board = createLeaderboard();

    // Test A: Basic High Score
    runner.runTest("Leaderboard: Add Scores & Get Top 1", [&]() {
        board->addScore(1, 100);
        board->addScore(2, 200); // 2 is Leader
        vector<int> top = board->getTopN(1);
        return (!top.empty() && top[0] == 2);
    }());

    // Test B: Tie-Breaking Visual Example (Crucial!)
    // PDF Visual Example: Player A (ID 10) 500pts, Player B (ID 20) 500pts.
    // Correct Order: ID 10 then ID 20.
    runner.runTest("Leaderboard: Tie-Break (ID 10 before ID 20)", [&]() {
        board->addScore(10, 500);
        board->addScore(20, 500);
        vector<int> top = board->getTopN(2);
        // We expect {10, 20} NOT {20, 10}
        if (top.size() < 2) return false;
        return (top[0] == 10 && top[1] == 20); 
    }());
    
    delete board;

    // 3. AuctionTree (Red-Black Tree)
    // Requirement: Insert items without crashing
    AuctionTree* tree = createAuctionTree();
    runner.runTest("AuctionTree: Insert Items", [&]() {
        tree->insertItem(1, 100);
        tree->insertItem(2, 50);
        return true; // Pass if no crash
    }());
    delete tree;
}

// ==========================================
// PART B: INVENTORY SYSTEM
// ==========================================

void test_PartB_Inventory() {
    cout << "\n--- Part B: Inventory System ---" << endl;

    // 1. Loot Splitting (Partition)
    // PDF Example: coins = {1, 2, 4} -> Best split {4} vs {1,2} -> Diff 1
    runner.runTest("LootSplit: {1, 2, 4} -> Diff 1", [&]() {
        vector<int> coins = {1, 2, 4};
        return InventorySystem::optimizeLootSplit(3, coins) == 1;
    }());

    // 2. Inventory Packer (Knapsack)
    // PDF Example: Cap=10, Items={{1,10}, {2,20}, {3,30}}. All fit. Value=60.
    runner.runTest("Knapsack: Cap 10, All Fit -> Value 60", [&]() {
        vector<pair<int, int>> items = {{1, 10}, {2, 20}, {3, 30}};
        return InventorySystem::maximizeCarryValue(10, items) == 60;
    }());

    // 3. Chat Autocorrect (String DP)
    // PDF Example: "uu" -> "uu" or "w" -> 2 possibilities
    runner.runTest("ChatDecorder: 'uu' -> 2 Possibilities", [&]() {
        return InventorySystem::countStringPossibilities("uu") == 2;
    }());
}

// ==========================================
// PART C: WORLD NAVIGATOR
// ==========================================

void test_PartC_Navigator() {
    cout << "\n--- Part C: World Navigator ---" << endl;

    // 1. Safe Passage (Path Exists)
    // PDF Example: 0-1, 1-2. Path 0->2 exists.
    runner.runTest("PathExists: 0->1->2 -> True", [&]() {
        vector<vector<int>> edges = {{0, 1}, {1, 2}};
        return WorldNavigator::pathExists(3, edges, 0, 2) == true;
    }());

    // 2. The Bribe (MST)
    // PDF Example: 3 Nodes. Roads: {0,1,10}, {1,2,5}, {0,2,20}. Rate=1.
    // MST should pick 10 and 5. Total 15.
    runner.runTest("MinBribeCost: Triangle Graph -> Cost 15", [&]() {
        vector<vector<int>> roads = {
            {0, 1, 10, 0}, 
            {1, 2, 5, 0}, 
            {0, 2, 20, 0}
        };
        // n=3, m=3, goldRate=1, silverRate=1
        return WorldNavigator::minBribeCost(3, 3, 1, 1, roads) == 15;
    }());

    // 3. Teleporter (Binary Sum APSP)
    // PDF Example: 0-1 (1), 1-2 (2). Distances: 1, 2, 3. Sum=6 -> "110"
    runner.runTest("BinarySum: Line Graph -> '110'", [&]() {
        vector<vector<int>> roads = {
            {0, 1, 1},
            {1, 2, 2}
        };
        return WorldNavigator::sumMinDistancesBinary(3, roads) == "110";
    }());
}

// ==========================================
// PART D: SERVER KERNEL
// ==========================================

void test_PartD_Kernel() {
    cout << "\n--- Part D: Server Kernel ---" << endl;

    // 1. Task Scheduler
    // PDF Example: Tasks={A, A, B}, n=2.
    // Order: A -> B -> idle -> A. Total intervals: 4.
    runner.runTest("Scheduler: {A, A, B}, n=2 -> 4 Intervals", [&]() {
        vector<char> tasks = {'A', 'A', 'B'};
        return ServerKernel::minIntervals(tasks, 2) == 4;
    }());
}

void test_AuctionTree_Advanced() {
    cout << "\n--- Advanced AuctionTree Tests ---" << endl;
    
    AuctionTree* tree = createAuctionTree();
    
    // Test 1: Insert with same price, different IDs
    runner.runTest("AuctionTree: Same price diff IDs ordering", [&]() {
        tree->insertItem(1, 100);
        tree->insertItem(2, 100);  // Same price, different ID
        tree->insertItem(3, 50);
        return true;  // No crash
    }());
    
    // Test 2: Delete non-existent item
    runner.runTest("AuctionTree: Delete non-existent (no crash)", [&]() {
        tree->deleteItem(999);  // Should not crash
        return true;
    }());
    
    // Test 3: Insert, delete, insert again
    runner.runTest("AuctionTree: Insert-Delete-Reinsert", [&]() {
        tree->insertItem(10, 200);
        tree->deleteItem(10);
        tree->insertItem(10, 200);  // Reinsert
        return true;
    }());
    
    // Test 4: Multiple deletions
    runner.runTest("AuctionTree: Multiple deletions", [&]() {
        tree->insertItem(100, 300);
        tree->insertItem(101, 250);
        tree->insertItem(102, 350);
        tree->deleteItem(101);
        tree->deleteItem(100);
        return true;
    }());
    
    // Test 5: Delete all items
    runner.runTest("AuctionTree: Delete all items", [&]() {
        tree->deleteItem(102);
        tree->deleteItem(10);
        tree->deleteItem(3);
        tree->deleteItem(1);
        tree->deleteItem(2);
        return true;
    }());
    
    delete tree;
}

void test_Knapsack_EdgeCases() {
    cout << "\n--- Knapsack Edge Cases ---" << endl;
    
    // Test 1: Empty items
    runner.runTest("Knapsack: Empty items -> 0", [&]() {
        vector<pair<int, int>> items;
        return InventorySystem::maximizeCarryValue(10, items) == 0;
    }());
    
    // Test 2: Zero capacity
    runner.runTest("Knapsack: Zero capacity -> 0", [&]() {
        vector<pair<int, int>> items = {{1, 10}, {2, 20}};
        return InventorySystem::maximizeCarryValue(0, items) == 0;
    }());
    
    // Test 3: All items too heavy
    runner.runTest("Knapsack: All items too heavy -> 0", [&]() {
        vector<pair<int, int>> items = {{15, 100}, {20, 200}};
        return InventorySystem::maximizeCarryValue(10, items) == 0;
    }());
    
    // Test 4: Exact capacity fit
    runner.runTest("Knapsack: Exact capacity fit", [&]() {
        vector<pair<int, int>> items = {{3, 30}, {7, 70}, {10, 100}};
        return InventorySystem::maximizeCarryValue(10, items) == 100;
    }());
}

void test_HashTable_EdgeCases() {
    cout << "\n--- HashTable Edge Cases ---" << endl;
    
    PlayerTable* table = createPlayerTable();
    
    // Test 1: Search non-existent
    runner.runTest("HashTable: Search non-existent -> empty", [&]() {
        return table->search(999) == "";
    }());
    
    // Test 2: Update existing player
    runner.runTest("HashTable: Update existing player", [&]() {
        table->insert(1, "Alice");
        table->insert(1, "Bob");  // Update
        return table->search(1) == "Bob";
    }());
    
    // Test 3: Delete and search
    // Note: Your implementation doesn't have delete, but test insert collisions
    runner.runTest("HashTable: Handle collisions", [&]() {
        table->insert(1, "One");
        table->insert(102, "Collision");  // 102 % 101 = 1, same as ID 1
        return table->search(102) == "Collision";
    }());
    
    delete table;
}

void test_Leaderboard_EdgeCases() {
    cout << "\n--- Leaderboard Edge Cases ---" << endl;
    
    Leaderboard* board = createLeaderboard();
    
    // Test 1: Get top N more than players
    runner.runTest("Leaderboard: GetTopN more than players", [&]() {
        board->addScore(1, 100);
        board->addScore(2, 200);
        vector<int> top = board->getTopN(5);
        return top.size() == 2 && top[0] == 2 && top[1] == 1;
    }());
    
    // Test 2: Remove non-existent player
    runner.runTest("Leaderboard: Remove non-existent (no crash)", [&]() {
        board->removePlayer(999);
        return true;
    }());
    
    // Test 3: Empty leaderboard
    runner.runTest("Leaderboard: Empty getTopN", [&]() {
        Leaderboard* emptyBoard = createLeaderboard();
        vector<int> top = emptyBoard->getTopN(3);
        delete emptyBoard;
        return top.empty();
    }());
    
    // Test 4: Same score different IDs ordering
    runner.runTest("Leaderboard: Same score ordering (ID 5 before 10)", [&]() {
        board->addScore(10, 500);
        board->addScore(5, 500);  // Same score, lower ID
        vector<int> top = board->getTopN(2);
        return top[0] == 5 && top[1] == 10;
    }());
    
    delete board;
}

int main() {
    cout << "Arcadia Engine - Comprehensive Tests" << endl;
    cout << "-----------------------------------" << endl;

    test_PartA_DataStructures();
    test_HashTable_EdgeCases();
    test_Leaderboard_EdgeCases();
    test_AuctionTree_Advanced();
    
    test_PartB_Inventory();
    test_Knapsack_EdgeCases();
    
    test_PartC_Navigator();
    test_PartD_Kernel();

    runner.printSummary();

    return 0;
}