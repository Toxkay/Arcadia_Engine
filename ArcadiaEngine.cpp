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
    static const int PRIME_NUM = 7;
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

    int hash_1(int playerID)
    {
        return ((playerID % TABLE_SIZE) + TABLE_SIZE) % TABLE_SIZE;
    };

    int hash_2(int playerID)
    {
        return PRIME_NUM - (((playerID % PRIME_NUM) + PRIME_NUM) % PRIME_NUM);
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
    static const int MAX_LEVELS = 16;

    struct skipNode
    {
        int playerID;
        int score;
        vector<skipNode *> forward;

        skipNode(int id, int sc, int level)
            : playerID(id), score(sc), forward(level + 1, nullptr) {}
    };
    skipNode *header;
    int currentLevel;

    int randomlevel()
    {
        int level = 0;
        while (level < MAX_LEVELS && (rand() % 2) == 0)
        {
            level++;
        }
        return level;
    }

    // Higher score comes first, same score → lower ID comes first
    bool shouldcomebefore(int score1, int id1, int score2, int id2)
    {
        if (score1 != score2)
        {
            return score1 > score2;
        }
        return id1 < id2;
    }

public:
    ConcreteLeaderboard()
    {
        currentLevel = 0;
        header = new skipNode(-1, INT_MIN, MAX_LEVELS);
    }
    void addScore(int playerID, int score) override {
    // First remove existing player in O(N)
    removePlayer(playerID);
    
    //  insert new score
    vector<skipNode*> update(MAX_LEVELS + 1, nullptr);
    skipNode* current = header;
    
    // Find insertion point
    for (int i = currentLevel; i >= 0; i--) {
        while (current->forward[i] != nullptr &&
               shouldcomebefore(current->forward[i]->score,
                                current->forward[i]->playerID,
                                score, playerID)) {
            current = current->forward[i];
        }
        update[i] = current;
    }
    
    int newLevel = randomlevel();
    if (newLevel > currentLevel) {
        for (int i = currentLevel + 1; i <= newLevel; i++) {
            update[i] = header;
        }
        currentLevel = newLevel;
    }
    
    skipNode* newNode = new skipNode(playerID, score, newLevel);
    for (int i = 0; i <= newLevel; i++) {
        newNode->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = newNode;
    }
}

    void removePlayer(int playerID) override
    {
        vector<skipNode *> update(MAX_LEVELS + 1, nullptr);
        skipNode *current = header;

        // We need to find the node with the exact playerID
        // First, we need to find its score to do proper comparison
        skipNode *targetNode = nullptr;
        skipNode *temp = header->forward[0];
        while (temp != nullptr)
        {
            if (temp->playerID == playerID)
            {
                targetNode = temp;
                break;
            }
            temp = temp->forward[0];
        }

        if (targetNode == nullptr)
            return; // Player not found

        // Now search using the same comparison as addScore
        for (int i = currentLevel; i >= 0; i--)
        {
            while (current->forward[i] != nullptr &&
                   shouldcomebefore(current->forward[i]->score,
                                    current->forward[i]->playerID,
                                    targetNode->score,
                                    targetNode->playerID))
            {
                current = current->forward[i];
            }
            update[i] = current;
        }

        // The next node should be our target
        current = current->forward[0];

        // Verify it's the right node
        if (current != nullptr && current->playerID == playerID)
        {
            // Remove from all levels
            for (int i = 0; i <= currentLevel; i++)
            {
                if (update[i]->forward[i] != current)
                {
                    break;
                }
                update[i]->forward[i] = current->forward[i];
            }

            delete current;

            // Update currentLevel if needed
            while (currentLevel > 0 && header->forward[currentLevel] == nullptr)
            {
                currentLevel--;
            }
        }
    }

    vector<int>
    getTopN(int n) override
    {
        vector<int> result;
        skipNode *current = header->forward[0];
        while (current != nullptr && result.size() < n)
        {
            result.push_back(current->playerID);
            current = current->forward[0];
        }
        return result;
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

        int getItemID()
        {
            return itemID;
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

    void insertFixup(RBnode *node)
    {
        while (node != root && node->getParent() != NIL && node->getParent()->getColor() == RED)
        {
            RBnode *grandparent = getGrandparent(node);
            if (grandparent == NIL)
                break; // Safety check

            if (node->getParent() == grandparent->getLeft())
            {
                RBnode *uncle = grandparent->getRight();

                // Case 1: Uncle is RED
                if (uncle != NIL && uncle->getColor() == RED)
                {
                    node->getParent()->setColor(BLACK);
                    uncle->setColor(BLACK);
                    grandparent->setColor(RED);
                    node = grandparent;
                }
                else
                {
                    // Case 2: node is right child
                    if (node == node->getParent()->getRight())
                    {
                        node = node->getParent();
                        leftRotate(node);
                    }
                    // Case 3
                    node->getParent()->setColor(BLACK);
                    grandparent->setColor(RED);
                    rightRotate(grandparent);
                }
            }
            else
            {
                // Symmetric case (right side)
                RBnode *uncle = grandparent->getLeft();

                if (uncle != NIL && uncle->getColor() == RED)
                {
                    node->getParent()->setColor(BLACK);
                    uncle->setColor(BLACK);
                    grandparent->setColor(RED);
                    node = grandparent;
                }
                else
                {
                    if (node == node->getParent()->getLeft())
                    {
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

    void destroyTree(RBnode *node)
    {
        if (node == NIL)
            return;
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
                else if (itemID > current->itemID)
                {
                    current = current->right;
                }
                else
                {
                    // Same itemID AND Same price -> ignore
                    delete newNode; // Clean up the node we created
                    return;         // Exit without inserting
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

    // delete helper functions:
    RBnode *findMin(RBnode *node)
    {
        if (node == NIL)
            return NIL;
        RBnode *Min = node;
        while (Min->getLeft() != NIL)
        {
            Min = Min->getLeft();
        }
        return Min;
    }

    void transplant(RBnode *u, RBnode *v)
    { // attach subtree v to u parent
        //  Update u's parent to point to v instead of u
        if (u->getParent() == NIL)
        {
            // u was root, so v becomes new root
            root = v;
        }
        else if (u == u->getParent()->getLeft())
        {
            // u was left child, make v the new left child
            u->getParent()->setLeft(v);
        }
        else
        {
            // u was right child, make v the new right child
            u->getParent()->setRight(v);
        }

        // Step 2: Update v's parent pointer (if v is not NIL)
        if (v != NIL)
        {
            v->setParent(u->getParent());
        }
    }

    RBnode *findNodeByID(RBnode *node, int itemID)
    {
        if (node == NIL)
            return NIL;

        // Check current node
        if (node->getItemID() == itemID)
            return node;

        // Search left and right subtrees
        RBnode *leftResult = findNodeByID(node->getLeft(), itemID);
        if (leftResult != NIL)
            return leftResult;

        return findNodeByID(node->getRight(), itemID);
    }

    void deleteFixup(RBnode *fixupNode)
    {
        if (fixupNode == NIL)
        {
            root->setColor(BLACK);
            return;
        }
        while (fixupNode != root && fixupNode != NIL && fixupNode->getColor() == BLACK)
        {
            if (fixupNode == fixupNode->getParent()->getLeft())
            {
                RBnode *sibling = fixupNode->getParent()->getRight();

                // Case 1: Sibling is RED
                if (sibling->getColor() == RED)
                {
                    sibling->setColor(BLACK);
                    fixupNode->getParent()->setColor(RED);
                    leftRotate(fixupNode->getParent());
                    sibling = fixupNode->getParent()->getRight();
                }

                // Case 2: Sibling BLACK with two BLACK children
                if (sibling->getLeft()->getColor() == BLACK && sibling->getRight()->getColor() == BLACK)
                {
                    sibling->setColor(RED);
                    fixupNode = fixupNode->getParent(); // Move problem up
                }
                else
                {
                    // Case 3: Sibling's right child is BLACK
                    if (sibling->getRight()->getColor() == BLACK)
                    {
                        sibling->getLeft()->setColor(BLACK);
                        sibling->setColor(RED);
                        rightRotate(sibling);
                        sibling = fixupNode->getParent()->getRight();
                    }

                    // Case 4: Sibling's right child is RED
                    sibling->setColor(fixupNode->getParent()->getColor());
                    fixupNode->getParent()->setColor(BLACK);
                    sibling->getRight()->setColor(BLACK);
                    leftRotate(fixupNode->getParent());
                    fixupNode = root; // Problem solved
                }
            }
            else
            {
                // Symmetric: fixupNode is RIGHT child

                RBnode *sibling = fixupNode->getParent()->getLeft();

                // Case 1 (symmetric)
                if (sibling->getColor() == RED)
                {
                    sibling->setColor(BLACK);
                    fixupNode->getParent()->setColor(RED);
                    rightRotate(fixupNode->getParent());
                    sibling = fixupNode->getParent()->getLeft();
                }

                // Case 2 (symmetric)
                if (sibling->getRight()->getColor() == BLACK && sibling->getLeft()->getColor() == BLACK)
                {
                    sibling->setColor(RED);
                    fixupNode = fixupNode->getParent();
                }
                else
                {
                    // Case 3 (symmetric): Sibling's left child is BLACK
                    if (sibling->getLeft()->getColor() == BLACK)
                    {
                        sibling->getRight()->setColor(BLACK);
                        sibling->setColor(RED);
                        leftRotate(sibling);
                        sibling = fixupNode->getParent()->getLeft();
                    }

                    // Case 4 (symmetric): Sibling's left child is RED
                    sibling->setColor(fixupNode->getParent()->getColor());
                    fixupNode->getParent()->setColor(BLACK);
                    sibling->getLeft()->setColor(BLACK);
                    rightRotate(fixupNode->getParent());
                    fixupNode = root;
                }
            }
        }

        fixupNode->setColor(BLACK);
    }

    void deleteItem(int itemID) override
    {
        // Step 1: Find node by itemID (O(n) traversal)
        RBnode *targetNode = findNodeByID(root, itemID);
        if (targetNode == NIL)
            return; // Item not found

        // Step 2: Standard Red-Black deletion
        RBnode *nodeToDelete = targetNode;
        RBnode *replacementChild;
        Color deletedColor = nodeToDelete->getColor();

        // Case 1: No left child
        if (targetNode->left == NIL)
        {
            replacementChild = targetNode->right;
            transplant(targetNode, targetNode->right);
        }
        // Case 2: No right child
        else if (targetNode->right == NIL)
        {
            replacementChild = targetNode->left;
            transplant(targetNode, targetNode->left);
        }
        // Case 3: Two children
        else
        {
            nodeToDelete = findMin(targetNode->right);
            deletedColor = nodeToDelete->getColor();
            replacementChild = nodeToDelete->right;

            if (nodeToDelete->parent == targetNode)
            {
                if (replacementChild != NIL)
                    replacementChild->parent = nodeToDelete;
            }
            else
            {
                transplant(nodeToDelete, nodeToDelete->right);
                nodeToDelete->right = targetNode->right;
                nodeToDelete->right->parent = nodeToDelete;
            }

            transplant(targetNode, nodeToDelete);
            nodeToDelete->left = targetNode->left;
            nodeToDelete->left->parent = nodeToDelete;
            nodeToDelete->setColor(targetNode->getColor());
        }

        delete targetNode;

        if (deletedColor == BLACK)
        {
            deleteFixup(replacementChild);
        }
    }

    ~ConcreteAuctionTree()
    {
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
    if (n == 0)
    {
        return 0;
    }
    if (n == 1)
    {
        return coins[0];
    }
    // calculate total sum
    int totalSum = 0;
    for (int i = 0; i < n; i++)
    {
        totalSum += coins[i];
    }
    int target = totalSum / 2;
    // set up DP array
    vector<bool> flags(target + 1, false);
    flags[0] = true;
    // fill DP table
    for (int i = 0; i < n; i++)
    {
        for (int j = target; j >= coins[i]; j--)
        {
            if (flags[j - coins[i]])
            {
                flags[j] = true;
            }
        }
    }
    // find largest bestsum <=target
    int bestsum = 0;
    for (int j = target; j >= 0; j--)
    {
        if (flags[j])
        {
            bestsum = j;
            break;
        }
    }
    // calc and return min difference
    int mindiff = totalSum - 2 * bestsum;
    return mindiff;
}
// ======================== knapsack ===========================
int InventorySystem::maximizeCarryValue(int capacity, vector<pair<int, int>> &items)
{
    int n = items.size();
    vector<vector<int>> dp(n + 1, vector<int>(capacity + 1, -1));

    // Initialize base cases
    for (int w = 0; w <= capacity; w++)
    {
        dp[0][w] = 0; // 0 items = 0 value
    }
    for (int i = 0; i <= n; i++)
    {
        dp[i][0] = 0; // 0 capacity = 0 value
    }

    // Fill DP table bottom-up
    for (int i = 1; i <= n; i++)
    {
        int weight = items[i - 1].first;
        int value = items[i - 1].second;

        for (int w = 1; w <= capacity; w++)
        {
            // Can't take this item
            dp[i][w] = dp[i - 1][w];

            // Can take this item
            if (weight <= w)
            {
                dp[i][w] = max(dp[i][w], value + dp[i - 1][w - weight]);
            }
        }
    }

    return dp[n][capacity];
    return 0;
}

long long InventorySystem::countStringPossibilities(string s)
{
    const long long MOD = 1000000007;
    int n = s.length();

    if (n == 0)
        return 1;

    vector<long long> dp(n + 1, 0);
    dp[0] = 1;

    for (int i = 1; i <= n; i++)
    {
        dp[i] = dp[i - 1];

        if (i >= 2)
        {
            string sub = s.substr(i - 2, 2);
            if (sub == "uu" || sub == "nn")
            {
                dp[i] = (dp[i] + dp[i - 2]) % MOD;
            }
        }
    }
    return dp[n];
}

// =========================================================
// PART C: WORLD NAVIGATOR (Graphs)
// =========================================================

bool WorldNavigator::pathExists(int n, vector<vector<int>> &edges, int source, int dest)
{
    // TODO: Implement path existence check using BFS or DFS
    // edges are bidirectional
    if (source == dest)
    {
        return true;
    }

    vector<vector<int>> adj(n);
    for (int i = 0; i < edges.size(); i++)
    {
        int u = edges[i][0];
        int v = edges[i][1];
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    vector<bool> visited(n, false);
    queue<int> q;
    q.push(source);
    visited[source] = true;

    while (!q.empty())
    {
        int current = q.front();
        q.pop();
        for (int neighbor : adj[current])
        {
            if (neighbor == dest)
            {
                return true;
            }
            if (!visited[neighbor])
            {
                visited[neighbor] = true;
                q.push(neighbor);
            }
        }
    }
    return false;
}

class UnionFind
{
private:
    vector<int> parent;
    vector<int> rank;

public:
    UnionFind(int n)
    {
        parent.resize(n);
        rank.resize(n, 0);
        for (int i = 0; i < n; i++)
        {
            parent[i] = i;
        }
    }

    int find(int x)
    {
        if (parent[x] != x)
        {
            parent[x] = find(parent[x]);
        }
        return parent[x];
    }

    bool unite(int x, int y)
    {
        int px = find(x);
        int py = find(y);

        if (px == py)
        {
            return false;
        }

        if (rank[px] < rank[py])
        {
            swap(px, py);
        }

        parent[py] = px;
        if (rank[px] == rank[py])
        {
            rank[px]++;
        }
        return true;
    }
};

long long WorldNavigator::minBribeCost(int n, int m, long long goldRate, long long silverRate,
                                       vector<vector<int>> &roadData)
{
    // TODO: Implement Minimum Spanning Tree (Kruskal's or Prim's)
    // roadData[i] = {u, v, goldCost, silverCost}
    // Total cost = goldCost * goldRate + silverCost * silverRate
    // Return -1 if graph cannot be fully connected
    if (n == 1)
    {
        return 0;
    }

    vector<vector<long long>> edges;

    for (int i = 0; i < roadData.size(); i++)
    {
        int u = roadData[i][0];
        int v = roadData[i][1];
        long long goldCost = roadData[i][2];
        long long silverCost = roadData[i][3];

        long long totalCost = goldCost * goldRate + silverCost * silverRate;
        edges.push_back({totalCost, u, v});
    }

    sort(edges.begin(), edges.end());

    UnionFind uf(n);

    long long totalCost = 0;
    int edgesUsed = 0;

    for (int i = 0; i < edges.size(); i++)
    {
        long long cost = edges[i][0];
        int u = edges[i][1];
        int v = edges[i][2];

        if (uf.unite(u, v))
        {
            totalCost += cost;
            edgesUsed++;

            if (edgesUsed == n - 1)
            {
                break;
            }
        }
    }

    return (edgesUsed == n - 1) ? totalCost : -1;
}

string WorldNavigator::sumMinDistancesBinary(int n, vector<vector<int>> &roads)
{
    // TODO: Implement All-Pairs Shortest Path (Floyd-Warshall)
    // Sum all shortest distances between unique pairs (i < j)
    // Return the sum as a binary string
    // Hint: Handle large numbers carefully
    const long long INF = LLONG_MAX / 4;

    vector<vector<long long>> dist(n, vector<long long>(n, INF));

    for (int i = 0; i < n; i++)
    {
        dist[i][i] = 0;
    }

    for (int i = 0; i < roads.size(); i++)
    {
        int u = roads[i][0];
        int v = roads[i][1];
        long long w = roads[i][2];
        dist[u][v] = min(dist[u][v], w);
        dist[v][u] = min(dist[v][u], w);
    }

    for (int k = 0; k < n; k++)
    {
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                if (dist[i][k] < INF && dist[k][j] < INF)
                {
                    dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j]);
                }
            }
        }
    }

    long long totalSum = 0;

    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            if (dist[i][j] < INF)
            {
                totalSum += dist[i][j];
            }
        }
    }

    if (totalSum == 0)
    {
        return "0";
    }

    string result = "";
    while (totalSum > 0)
    {
        result = (char)('0' + (totalSum & 1)) + result;
        totalSum >>= 1;
    }
    return result;
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
