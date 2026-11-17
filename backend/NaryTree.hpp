#ifndef NARY_TREE_HPP
#define NARY_TREE_HPP

#include <string>
#include <vector>
#include <queue>
#include <sstream>
#include <iostream>
#include "models.hpp"

// ============================================================================
// FEATURE 1: SUBREDDIT HIERARCHY USING N-ARY TREE
// ============================================================================
//
// DSA Concept:
// An N-ary Tree is a hierarchical tree data structure where each node can have 
// at most N child nodes (an arbitrary number of children). 
// In Reddit, subreddits and communities naturally form a hierarchy:
// - Root: "All / Reddit"
//   - Category: "Technology"
//       - Community: "Programming"
//           - Sub-topic: "C++"
//           - Sub-topic: "WebDev"
//       - Community: "Artificial Intelligence"
//   - Category: "Gaming"
//       - Community: "PC Gaming"
//       - Community: "RPG"
//
// Time Complexity:
// - Insertion: O(V) where V is the number of nodes (to find parent) + O(1) to append child
// - Search (DFS): O(V) time complexity, visiting each node once
// - Breadcrumb Path (node to root): O(H) where H is the height of the tree (typically O(log V))
// - JSON Serialization (Pre-order Traversal): O(V)
// ============================================================================

// Structure representing an individual node in the N-ary Tree
struct SubredditNode {
    std::string name;          // Unique subreddit key (e.g. "cpp", "gaming")
    std::string title;         // Display title (e.g. "C++ Programming Community")
    std::string description;   // Detailed summary of the community
    int members;               // Active members count
    std::string icon;          // Emoji or icon identifier
    SubredditNode* parent;     // Pointer to parent node (allows upward traversal for breadcrumbs)
    std::vector<SubredditNode*> children; // Pointers to N child subreddits

    // Step 1: Constructor initializes node values and sets parent to nullptr by default
    SubredditNode(std::string n, std::string t, std::string d, int m, std::string ic = "🌐")
        : name(n), title(t), description(d), members(m), icon(ic), parent(nullptr) {}
};

// Class managing the N-ary Tree of Subreddits
class SubredditTree {
private:
    SubredditNode* root; // Root node of the hierarchy (e.g., "all")
    int totalNodes;      // Total count of subreddits in the tree

    // Helper: Recursive Depth-First Search (DFS) to find a node by its unique name
    SubredditNode* findDFS(SubredditNode* current, const std::string& name) {
        // Base case: If current node is null, return null
        if (!current) return nullptr;

        // Base case: If current node matches target name, return current node
        if (current->name == name) return current;

        // Recursive case: Traverse through each child in the N-ary children list
        for (SubredditNode* child : current->children) {
            SubredditNode* found = findDFS(child, name);
            if (found != nullptr) {
                return found; // Found target in subtree, propagate result up
            }
        }

        // Target not found in this subtree
        return nullptr;
    }

    // Helper: Recursive Post-Order Traversal for safely deleting all nodes (destructor)
    void destroyTree(SubredditNode* node) {
        if (!node) return;

        // First recursively delete all children before deleting the parent node
        for (SubredditNode* child : node->children) {
            destroyTree(child);
        }

        // Finally delete the current node to avoid memory leaks
        delete node;
    }

    // Helper: Recursive Pre-Order Traversal to serialize the tree into JSON
    void serializeToJson(SubredditNode* node, std::ostringstream& ss) {
        if (!node) return;

        ss << "{";
        ss << "\"name\":\"" << escapeJsonString(node->name) << "\",";
        ss << "\"title\":\"" << escapeJsonString(node->title) << "\",";
        ss << "\"description\":\"" << escapeJsonString(node->description) << "\",";
        ss << "\"members\":" << node->members << ",";
        ss << "\"icon\":\"" << escapeJsonString(node->icon) << "\",";
        ss << "\"parent\":\"" << (node->parent ? escapeJsonString(node->parent->name) : "") << "\",";
        ss << "\"children\":[";

        for (size_t i = 0; i < node->children.size(); ++i) {
            serializeToJson(node->children[i], ss);
            if (i + 1 < node->children.size()) {
                ss << ",";
            }
        }
        ss << "]}";
    }

    // Helper: Collect all nodes with their depth level (for flattened lists / selectors)
    void collectFlattened(SubredditNode* node, int depth, std::vector<std::pair<SubredditNode*, int>>& list) {
        if (!node) return;
        list.push_back({node, depth});
        for (SubredditNode* child : node->children) {
            collectFlattened(child, depth + 1, list);
        }
    }

    // Helper: Calculate the maximum height (depth) of the N-ary tree
    int calculateHeight(SubredditNode* node) {
        if (!node) return 0;
        int maxChildHeight = 0;
        for (SubredditNode* child : node->children) {
            int h = calculateHeight(child);
            if (h > maxChildHeight) maxChildHeight = h;
        }
        return 1 + maxChildHeight;
    }

public:
    // Step 2: Constructor initializes the root community ("all")
    SubredditTree() {
        root = new SubredditNode("all", "Reddit All", "The front page of everything on Reddit", 54000000, "🔥");
        totalNodes = 1;
    }

    // Step 3: Destructor frees all allocated nodes via post-order traversal
    ~SubredditTree() {
        destroyTree(root);
    }

    // Step 4: Insert a new subreddit under a specified parent in the hierarchy
    bool insertSubreddit(const std::string& parentName, const std::string& name,
                         const std::string& title, const std::string& description,
                         int members, const std::string& icon = "🌐") {
        // Step 4.1: Find the parent node in the N-ary tree using DFS
        SubredditNode* parentNode = findDFS(root, parentName);
        if (!parentNode) {
            // Parent not found, insertion cannot proceed
            std::cerr << "[N-ary Tree Error]: Parent subreddit '" << parentName << "' not found." << std::endl;
            return false;
        }

        // Step 4.2: Check if a subreddit with this name already exists in the tree
        if (findDFS(root, name) != nullptr) {
            std::cerr << "[N-ary Tree Error]: Subreddit '" << name << "' already exists." << std::endl;
            return false;
        }

        // Step 4.3: Allocate a new SubredditNode on the heap
        SubredditNode* newNode = new SubredditNode(name, title, description, members, icon);

        // Step 4.4: Establish bi-directional tree links:
        // Set child's parent pointer to parentNode
        newNode->parent = parentNode;

        // Append the new node into the parent's dynamic children vector
        parentNode->children.push_back(newNode);

        // Step 4.5: Increment the total node count
        totalNodes++;

        std::cout << "[N-ary Tree Success]: Added 'r/" << name << "' under 'r/" << parentName 
                  << "' (Children count of parent: " << parentNode->children.size() << ")" << std::endl;
        return true;
    }

    // Step 5: Search for a subreddit by its unique name
    SubredditNode* findSubreddit(const std::string& name) {
        return findDFS(root, name);
    }

    // Step 6: Generate breadcrumb path from target node up to the root (Bottom-Up Traversal)
    // Example: "all" -> "technology" -> "programming" -> "cpp"
    std::vector<std::string> getBreadcrumbPath(const std::string& name) {
        std::vector<std::string> path;
        SubredditNode* curr = findDFS(root, name);
        if (!curr) return path;

        // Traverse upwards using parent pointers
        while (curr != nullptr) {
            path.push_back(curr->name);
            curr = curr->parent;
        }

        // Reverse to get path from Root to Target
        std::vector<std::string> reversedPath;
        for (int i = (int)path.size() - 1; i >= 0; --i) {
            reversedPath.push_back(path[i]);
        }
        return reversedPath;
    }

    // Step 7: Serialize the entire N-ary Tree to JSON format for React frontend rendering
    std::string getTreeJson() {
        std::ostringstream ss;
        serializeToJson(root, ss);
        return ss.str();
    }

    // Step 8: Get flattened list of all subreddits with their tree depth level
    std::string getFlattenedJson() {
        std::vector<std::pair<SubredditNode*, int>> list;
        collectFlattened(root, 0, list);

        std::ostringstream ss;
        ss << "[";
        for (size_t i = 0; i < list.size(); ++i) {
            SubredditNode* node = list[i].first;
            int depth = list[i].second;
            ss << "{"
               << "\"name\":\"" << escapeJsonString(node->name) << "\","
               << "\"title\":\"" << escapeJsonString(node->title) << "\","
               << "\"description\":\"" << escapeJsonString(node->description) << "\","
               << "\"members\":" << node->members << ","
               << "\"icon\":\"" << escapeJsonString(node->icon) << "\","
               << "\"depth\":" << depth << ","
               << "\"parent\":\"" << (node->parent ? escapeJsonString(node->parent->name) : "") << "\""
               << "}";
            if (i + 1 < list.size()) ss << ",";
        }
        ss << "]";
        return ss.str();
    }

    // Get DSA diagnostic stats for the N-ary tree
    int getNodeCount() const { return totalNodes; }
    int getTreeHeight() { return calculateHeight(root); }
};

#endif // NARY_TREE_HPP
