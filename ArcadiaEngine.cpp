// ArcadiaEngine.cpp - STUDENT TEMPLATE
// TODO: Implement all the functions below according to the assignment requirements

#include "ArcadiaEngine.h"
#include <algorithm>
#include <queue>
#include <numeric>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <set>

using namespace std;

// =========================================================
// PART A: DATA STRUCTURES (Concrete Implementations)
// =========================================================

// --- 1. PlayerTable (Double Hashing) ---

class ConcretePlayerTable : public PlayerTable
{
private:
    static const int TABLE_SIZE = 101;
    static const int PRIME_NUM = 97;
    // flags
    static const int EMPTY = 0;
    static const int OCCUPIED = 1;
    static const int DELETED = 2;
    // TODO: Define your data structures here
    // Hint: You'll need a hash table with double hashing collision resolution
    struct Player_slot
    {
        int ID;
        string P_name;
        int status;
        Player_slot() : ID(-1), P_name(""), status(EMPTY) {}
    };
    // hash tablevector
    vector<Player_slot> table;
    // Mutliplication methid
    int hash_1(int playerID)
    {
        double A = 0.618033988749895;
        double temp = static_cast<double>(abs(playerID)) * A;
        temp = temp - floor(temp);
        return static_cast<int>(floor(TABLE_SIZE * temp));
    };

    int hash_2(int playerID)
    {
        return 1 + (abs(playerID) % (PRIME_NUM - 1));
    }
    int double_hash_probe(int playerID, int i)
    {
        return (hash_1(playerID) + i * hash_2(playerID)) % TABLE_SIZE;
    }

public:
    // TODO: Initialize your hash table
    ConcretePlayerTable()
    {
        table.resize(TABLE_SIZE);
        for (int i = 0; i < TABLE_SIZE; i++)
        {
            table[i].status = EMPTY;
            table[i].ID = -1;
            table[i].P_name = "";
        }
    }

    // TODO: Implement double hashing insert
    // Remember to handle collisions using h1(key) + i * h2(key)
    void insert(int playerID, string name) override
    {
        int i = 0;
        int index;

        while (i < TABLE_SIZE)
        {
            index = double_hash_probe(playerID, i);
            if (table[index].status == OCCUPIED && table[index].ID == playerID)
            {
                table[index].P_name = name;
                return;
            }
            if (table[index].status == EMPTY || table[index].status == DELETED)
            {
                table[index].ID = playerID;
                table[index].P_name = name;
                table[index].status = OCCUPIED;
                return;
            }
            i++;
        }

        cout << "Table is full" << endl;
    }

    string search(int playerID) override
    {
        // TODO: Implement double hashing search
        // Return "" if player not found
        int i = 0;
        int index;
        while (i < TABLE_SIZE)
        {
            index = double_hash_probe(playerID, i);
            if (table[index].status == OCCUPIED && table[index].ID == playerID)
            {
                return table[index].P_name;
            }
            if (table[index].status == EMPTY)
            {
                return "";
            }
            i++;
        }

        return "";
    }
};

// --- 2. Leaderboard (Skip List) ---

class ConcreteLeaderboard : public Leaderboard
{
private:
    // TODO: Define your skip list node structure and necessary variables
    // Hint: You'll need nodes with multiple forward pointers

public:
    ConcreteLeaderboard()
    {
        // TODO: Initialize your skip list
    }

    void addScore(int playerID, int score) override
    {
        // TODO: Implement skip list insertion
        // Remember to maintain descending order by score
    }

    void removePlayer(int playerID) override
    {
        // TODO: Implement skip list deletion
    }

    vector<int> getTopN(int n) override
    {
        // TODO: Return top N player IDs in descending score order
        return {};
    }
};

// --- 3. AuctionTree (Red-Black Tree) ---

class ConcreteAuctionTree : public AuctionTree
{
private:
    enum Color
    {
        RED,
        BLACK
    };
    // Red black node struct
    struct RBnode
    {
        int itemID;
        int price;
        Color nodeColor;
        RBnode *left;
        RBnode *right;
        RBnode *parent;

        RBnode(int id, int p, RBnode *l = nullptr, RBnode *r = nullptr,
               RBnode *par = nullptr, Color col = RED)
            : itemID(id), price(p), left(l), right(r), parent(par), nodeColor(col) {}

        RBnode *getParent()
        {
            return parent;
        }

        RBnode *getRight()
        {
            return right;
        }

        RBnode *getLeft()
        {
            return left;
        }

        Color getColor()
        {
            return nodeColor;
        }

        void setParent(RBnode *node)
        {
            this->parent = node;
        }

        void setLeft(RBnode *node)
        {
            this->left = node;
        }

        void setRight(RBnode *node)
        {
            this->right = node;
        }
        void setColor(Color col)
        {
            this->nodeColor = col;
        }
    };

    RBnode *root;
    RBnode *NIL;

    // helper methods for insertion

    // to get the grandparent of a node
    RBnode *getGrandparent(RBnode *node)
    {
        if (node == NIL || node->getParent() == NIL)
            return NIL;
        if (node->getParent()->getParent() == NIL)
            return NIL;
        return node->getParent()->getParent();
    }

    // get uncle
    RBnode *getUncle(RBnode *node)
    {
        if (node == NIL)
            return NIL;
        RBnode *grandparent = getGrandparent(node);
        if (grandparent == NIL)
            return NIL;
        if (node->getParent() == grandparent->getRight())
            return grandparent->getLeft();

        return grandparent->getRight();
    }

    void setRoot(RBnode *node)
    {
        this->root = node;
    }

    void leftRotate(RBnode *node)
    {
        if (node == NIL || node->getRight() == NIL)
            return; // can't rotate

        RBnode *nodeOldRight = node->getRight(); // the right child of the node

        // 1 => turn the old right node left subtree into node right subtree
        node->setRight(nodeOldRight->getLeft());

        // 2 => if old right node left child is not nil, update it's parent
        if (nodeOldRight->getLeft() != NIL)
            nodeOldRight->getLeft()->setParent(node);

        // 3 => Link old right node parent to node parent
        nodeOldRight->setParent(node->getParent());

        // 4 => update root or parent's child pointer
        if (node->getParent() == NIL)
            setRoot(nodeOldRight);
        else if (node == node->getParent()->getLeft())
            node->getParent()->setLeft(nodeOldRight);
        else
            node->getParent()->setRight(nodeOldRight);

        // 5 => put node on old right node's left
        nodeOldRight->setLeft(node);
        node->setParent(nodeOldRight);
    }

    void rightRotate(RBnode *node)
    {
        if (node == NIL || node->getLeft() == NIL)
            return;

        RBnode *nodeOldLeft = node->getLeft();

        node->setLeft(nodeOldLeft->getRight());

        if (nodeOldLeft->getRight() != NIL)
            nodeOldLeft->getRight()->setParent(node);

        nodeOldLeft->setParent(node->getParent());

        if (node->getParent() == NIL)
            setRoot(nodeOldLeft);
        else if (node == node->getParent()->getRight())
            node->getParent()->setRight(nodeOldLeft);
        else
            node->getParent()->setLeft(nodeOldLeft);

        nodeOldLeft->setRight(node);
        node->setParent(nodeOldLeft);
    }

   void insertFixup(RBnode* node) {
    while (node != root && node->getParent() != NIL && node->getParent()->getColor() == RED) {
        RBnode* grandparent = getGrandparent(node);
        if (grandparent == NIL) break;  // Safety check
        
        if (node->getParent() == grandparent->getLeft()) {
            RBnode* uncle = grandparent->getRight();
            
            // Case 1: Uncle is RED
            if (uncle != NIL && uncle->getColor() == RED) {
                node->getParent()->setColor(BLACK);
                uncle->setColor(BLACK);
                grandparent->setColor(RED);
                node = grandparent;
            } 
            else {
                // Case 2: node is right child
                if (node == node->getParent()->getRight()) {
                    node = node->getParent();
                    leftRotate(node);
                }
                // Case 3
                node->getParent()->setColor(BLACK);
                grandparent->setColor(RED);
                rightRotate(grandparent);
            }
        } 
        else { 
            // Symmetric case (right side)
            RBnode* uncle = grandparent->getLeft();
            
            if (uncle != NIL && uncle->getColor() == RED) {
                node->getParent()->setColor(BLACK);
                uncle->setColor(BLACK);
                grandparent->setColor(RED);
                node = grandparent;
            } 
            else {
                if (node == node->getParent()->getLeft()) {
                    node = node->getParent();
                    rightRotate(node);
                }
                node->getParent()->setColor(BLACK);
                grandparent->setColor(RED);
                leftRotate(grandparent);
            }
        }
    }
    root->setColor(BLACK);
}

     void destroyTree(RBnode* node) {
        if (node == NIL) return;
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }

public:
    ConcreteAuctionTree()
    {
        NIL = new RBnode(-1, -1, nullptr, nullptr, nullptr, BLACK);
        NIL->left = NIL;
        NIL->right = NIL;
        NIL->parent = NIL;
        root = NIL;
    }

    void insertItem(int itemID, int price) override
    {
        RBnode *newNode = new RBnode(itemID, price, NIL, NIL, NIL, RED);

        if (root == NIL)
        {
            newNode->setColor(BLACK);
            root = newNode;
            return;
        }

        RBnode *current = root;
        RBnode *parent = NIL;

        while (current != NIL)
        {
            parent = current;

            // Composite key comparison: price first, then itemID
            if (price < current->price)
            {
                current = current->left;
            }
            else if (price > current->price)
            {
                current = current->right;
            }
            else
            { // Prices equal, compare itemID
                if (itemID < current->itemID)
                {
                    current = current->left;
                }
                else
                {
                    current = current->right;
                }
            }
        }

        newNode->parent = parent;

        // Same composite key logic to decide left/right child
        if (price < parent->price)
        {
            parent->left = newNode;
        }
        else if (price > parent->price)
        {
            parent->right = newNode;
        }
        else
        { // Prices equal
            if (itemID < parent->itemID)
            {
                parent->left = newNode;
            }
            else
            {
                parent->right = newNode;
            }
        }

        insertFixup(newNode);
    }

    void deleteItem(int itemID) override
    {
        // TODO: Implement Red-Black Tree deletion
        // This is complex - handle all cases carefully
    }

    ~ConcreteAuctionTree() {
        destroyTree(root);
        delete NIL;
    }
};

// =========================================================
// PART B: INVENTORY SYSTEM (Dynamic Programming)
// =========================================================

int InventorySystem::optimizeLootSplit(int n, vector<int> &coins)
{
    // TODO: Implement partition problem using DP
    // Goal: Minimize |sum(subset1) - sum(subset2)|
    // Hint: Use subset sum DP to find closest sum to total/2
     // handle edge cases
    if(n==0){
        return 0;
    }
    if(n==1){
        return coins[0];
    }
    // calculate total sum
   int totalSum =0;
   for(int i=0;i<n;i++){
    totalSum+=coins[i];
   }
   int target=totalSum/2;
    // set up DP array
   vector<bool>flags(target + 1, false);
   flags[0]=true;
    // fill DP table
   for(int i=0;i<n;i++){
     for(int j=target;j>=coins[i];j--){
        if(flags[j-coins[i]]){
            flags[j]=true;
        }
     }
   }
    //find largest bestsum <=target
    int bestsum=0;
    for(int j=target;j>=0;j--){
        if (flags[j]){
        bestsum=j;
        break;
        }
        
    }
    // calc and return min difference
    int mindiff=totalSum-2 * bestsum;
    return mindiff ;
}

int InventorySystem::maximizeCarryValue(int capacity, vector<pair<int, int>> &items)
{
    // TODO: Implement 0/1 Knapsack using DP
    // items = {weight, value} pairs
    // Return maximum value achievable within capacity
    return 0;
}

long long InventorySystem::countStringPossibilities(string s)
{
    // TODO: Implement string decoding DP
    // Rules: "uu" can be decoded as "w" or "uu"
    //        "nn" can be decoded as "m" or "nn"
    // Count total possible decodings
    return 0;
}

// =========================================================
// PART C: WORLD NAVIGATOR (Graphs)
// =========================================================

bool WorldNavigator::pathExists(int n, vector<vector<int>> &edges, int source, int dest)
{
    // TODO: Implement path existence check using BFS or DFS
    // edges are bidirectional
    return false;
}

long long WorldNavigator::minBribeCost(int n, int m, long long goldRate, long long silverRate,
                                       vector<vector<int>> &roadData)
{
    // TODO: Implement Minimum Spanning Tree (Kruskal's or Prim's)
    // roadData[i] = {u, v, goldCost, silverCost}
    // Total cost = goldCost * goldRate + silverCost * silverRate
    // Return -1 if graph cannot be fully connected
    return -1;
}

string WorldNavigator::sumMinDistancesBinary(int n, vector<vector<int>> &roads)
{
    // TODO: Implement All-Pairs Shortest Path (Floyd-Warshall)
    // Sum all shortest distances between unique pairs (i < j)
    // Return the sum as a binary string
    // Hint: Handle large numbers carefully
    return "0";
}

// =========================================================
// PART D: SERVER KERNEL (Greedy)
// =========================================================

int ServerKernel::minIntervals(vector<char> &tasks, int n)
{
    // Count frequency of each task type
    map<char, int> freq;
    for (char task : tasks)
    {
        freq[task]++;
    }

    // Find maximum frequency among all tasks
    int maxFreq = 0;
    for (auto &p : freq)
    {
        maxFreq = max(maxFreq, p.second);
    }

    // Count how many tasks have the maximum frequency
    int maxCount = 0;
    for (auto &p : freq)
    {
        if (p.second == maxFreq)
        {
            maxCount++;
        }
    }

    // Calculate minimum intervals using greedy formula
    // The most frequent task creates (maxFreq - 1) "frames" with (n + 1) slots each
    // Then add the tasks that appear maxFreq times in the last frame
    int minIntervals = (maxFreq - 1) * (n + 1) + maxCount;

    // If we have enough task variety, no idle time is needed
    // Return the maximum of calculated intervals and total task count
    return max((int)tasks.size(), minIntervals);
}

// =========================================================
// FACTORY FUNCTIONS (Required for Testing)
// =========================================================

extern "C"
{
    PlayerTable *createPlayerTable()
    {
        return new ConcretePlayerTable();
    }

    Leaderboard *createLeaderboard()
    {
        return new ConcreteLeaderboard();
    }

    AuctionTree *createAuctionTree()
    {
        return new ConcreteAuctionTree();
    }
}

