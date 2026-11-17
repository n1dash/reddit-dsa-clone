#ifndef MAX_HEAP_HPP
#define MAX_HEAP_HPP

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "models.hpp"

// ============================================================================
// FEATURE 2: KARMA RANKING USING MAX HEAP
// ============================================================================
//
// DSA Concept:
// A Max Heap is a specialized Complete Binary Tree where the value of each node 
// is greater than or equal to the values of its children (Max Heap Invariant):
//      Heap[parent] >= Heap[child]
// 
// Storage representation:
// Instead of linked pointers, a complete binary tree can be efficiently packed 
// into a contiguous 0-indexed array / vector:
// - For any element at index 'i':
//     * Parent index       = (i - 1) / 2
//     * Left child index   = 2 * i + 1
//     * Right child index  = 2 * i + 2
//
// In Reddit, user karma reflects appreciation. Using a Max Heap allows us to:
// 1. Immediately access the #1 highest karma user at O(1) time (root at index 0).
// 2. Extract Top K contributors in O(K log N) time.
// 3. Dynamically insert or update user karma on upvotes/downvotes in O(log N) time.
//
// Time Complexity:
// - Peek Top Karma: O(1)
// - Insertion (Push + Sift-Up): O(log N)
// - Extract Max (Pop + Sift-Down): O(log N)
// - Karma Update: O(N) search + O(log N) sift
// ============================================================================

class KarmaMaxHeap {
private:
    // Internal vector storing the complete binary tree
    std::vector<UserKarma> heap;

    // Step 1: Helper function to calculate parent index in complete binary tree
    int getParentIndex(int childIndex) const {
        return (childIndex - 1) / 2;
    }

    // Step 2: Helper function to calculate left child index
    int getLeftChildIndex(int parentIndex) const {
        return (2 * parentIndex) + 1;
    }

    // Step 3: Helper function to calculate right child index
    int getRightChildIndex(int parentIndex) const {
        return (2 * parentIndex) + 2;
    }

    // Step 4: Sift-Up (Bubble-Up / Percolate-Up) Operation
    // When a node's karma is increased or a new node is inserted at the bottom,
    // it may be larger than its parent, violating the Max Heap property.
    // We swap it upward until Heap[parent] >= Heap[current] or it becomes the root.
    void siftUp(int index) {
        // Continue while node is not the root and its karma is greater than its parent's karma
        while (index > 0) {
            int parentIdx = getParentIndex(index);

            // Compare karma values
            if (heap[index].karma > heap[parentIdx].karma) {
                // Violation detected: Child has higher karma than parent. Swap them!
                std::swap(heap[index], heap[parentIdx]);

                // Move index pointer up to parent's position to continue checking
                index = parentIdx;
            } else {
                // Max Heap property satisfied for this subtree; stop sifting up
                break;
            }
        }
    }

    // Step 5: Sift-Down (Bubble-Down / Percolate-Down) Operation
    // When the root is extracted or a node's karma is decreased,
    // it may be smaller than its children, violating the Max Heap property.
    // We find the largest child and swap downward until Heap[current] >= both children.
    void siftDown(int index) {
        int size = static_cast<int>(heap.size());

        while (true) {
            int largest = index;
            int leftChild = getLeftChildIndex(index);
            int rightChild = getRightChildIndex(index);

            // Check if left child exists and has greater karma than current largest
            if (leftChild < size && heap[leftChild].karma > heap[largest].karma) {
                largest = leftChild;
            }

            // Check if right child exists and has greater karma than current largest
            if (rightChild < size && heap[rightChild].karma > heap[largest].karma) {
                largest = rightChild;
            }

            // If the largest is not the current index, swap with the largest child
            if (largest != index) {
                std::swap(heap[index], heap[largest]);

                // Continue sifting down in the swapped subtree
                index = largest;
            } else {
                // Node is larger than both children; Max Heap invariant restored
                break;
            }
        }
    }

public:
    // Step 6: Constructor
    KarmaMaxHeap() {}

    // Step 7: Insert a new user into the Max Heap
    void insert(const UserKarma& user) {
        // Step 7.1: Append new user to the end of the heap vector (at bottom of binary tree)
        heap.push_back(user);

        // Step 7.2: Restore Max Heap property by sifting up from the newly inserted position
        int newlyAddedIndex = static_cast<int>(heap.size()) - 1;
        siftUp(newlyAddedIndex);

        std::cout << "[Max Heap Insert]: User @" << user.username 
                  << " added with " << user.karma << " karma. Heap size: " << heap.size() << std::endl;
    }

    // Step 8: Peek at the user with the highest karma without removing them (O(1) time)
    bool peekMax(UserKarma& outUser) const {
        if (heap.empty()) return false;
        outUser = heap[0]; // Root element always holds the maximum karma
        return true;
    }

    // Step 9: Extract the user with the highest karma from the heap (O(log N) time)
    bool extractMax(UserKarma& outUser) {
        if (heap.empty()) return false;

        // Step 9.1: Save root element (the maximum karma user)
        outUser = heap[0];

        // Step 9.2: Move the last element of the complete binary tree to the root position
        heap[0] = heap.back();

        // Step 9.3: Remove the duplicate last element
        heap.pop_back();

        // Step 9.4: Restore Max Heap invariant by sifting down from root if heap is not empty
        if (!heap.empty()) {
            siftDown(0);
        }

        return true;
    }

    // Step 10: Update a user's karma dynamically (e.g. after upvote or downvote)
    bool updateKarma(const std::string& username, int deltaKarma, bool isPost = true) {
        // Find user index in heap array
        int targetIndex = -1;
        for (size_t i = 0; i < heap.size(); ++i) {
            if (heap[i].username == username) {
                targetIndex = static_cast<int>(i);
                break;
            }
        }

        if (targetIndex == -1) {
            std::cerr << "[Max Heap Warning]: User @" << username << " not found for karma update." << std::endl;
            return false;
        }

        // Apply karma modification
        heap[targetIndex].karma += deltaKarma;
        if (isPost) {
            heap[targetIndex].postKarma += deltaKarma;
        } else {
            heap[targetIndex].commentKarma += deltaKarma;
        }

        std::cout << "[Max Heap Karma Update]: User @" << username << " karma changed by " 
                  << (deltaKarma >= 0 ? "+" : "") << deltaKarma << " -> New Karma: " 
                  << heap[targetIndex].karma << std::endl;

        // Step 10.1: If karma increased, user may need to bubble up
        if (deltaKarma > 0) {
            siftUp(targetIndex);
        } 
        // Step 10.2: If karma decreased, user may need to trickle down
        else if (deltaKarma < 0) {
            siftDown(targetIndex);
        }

        return true;
    }

    // Step 11: Get Top K users ranked by Karma using a cloned heap (O(K log N) time)
    std::vector<UserKarma> getTopK(int k) const {
        std::vector<UserKarma> result;
        if (heap.empty() || k <= 0) return result;

        // Make a copy of the current Max Heap so the original data is preserved
        KarmaMaxHeap tempHeap = *this;

        // Successively extract the max element K times
        int count = std::min(k, static_cast<int>(tempHeap.size()));
        for (int i = 0; i < count; ++i) {
            UserKarma topUser;
            if (tempHeap.extractMax(topUser)) {
                result.push_back(topUser);
            }
        }
        return result;
    }

    // Step 12: Serialize the Top K users to JSON for frontend leaderboard
    std::string getLeaderboardJson(int k = 10) const {
        std::vector<UserKarma> topUsers = getTopK(k);
        std::ostringstream ss;
        ss << "[";
        for (size_t i = 0; i < topUsers.size(); ++i) {
            ss << userKarmaToJson(topUsers[i], static_cast<int>(i + 1));
            if (i + 1 < topUsers.size()) ss << ",";
        }
        ss << "]";
        return ss.str();
    }

    // Step 13: Serialize the raw array representation of the Max Heap for DSA visualizer
    std::string getHeapArrayJson() const {
        std::ostringstream ss;
        ss << "[";
        for (size_t i = 0; i < heap.size(); ++i) {
            ss << "{"
               << "\"index\":" << i << ","
               << "\"username\":\"" << escapeJsonString(heap[i].username) << "\","
               << "\"karma\":" << heap[i].karma << ","
               << "\"parentIndex\":" << (i == 0 ? -1 : getParentIndex(static_cast<int>(i))) << ","
               << "\"leftIndex\":" << getLeftChildIndex(static_cast<int>(i)) << ","
               << "\"rightIndex\":" << getRightChildIndex(static_cast<int>(i))
               << "}";
            if (i + 1 < heap.size()) ss << ",";
        }
        ss << "]";
        return ss.str();
    }

    // Step 14: Heap Size and Status Check
    size_t size() const { return heap.size(); }
    bool empty() const { return heap.empty(); }
};

#endif // MAX_HEAP_HPP
