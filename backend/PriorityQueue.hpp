#ifndef PRIORITY_QUEUE_HPP
#define PRIORITY_QUEUE_HPP

#include <vector>
#include <iostream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include "models.hpp"

// ============================================================================
// FEATURE 4: TRENDING POSTS USING HEAP / PRIORITY QUEUE
// ============================================================================
//
// DSA Concept:
// A Priority Queue is an abstract data type where each element has an associated 
// "priority". Elements with higher priority are served before elements with lower priority.
//
// Binary Heap Implementation:
// - Implemented using a Max-Heap keyed on 'trendingScore'.
// - The post with the highest trending score is always at the root (index 0).
//
// Reddit Trending Score Formula:
// Engagement increases trending momentum, while time decay gradually cools down older posts:
//
//               (Upvotes * 2.0) - (Downvotes * 1.5) + (Comments * 3.0)
//   Score =  -------------------------------------------------------------
//                          (Post Age in Hours + 2.0) ^ 1.3
//
// Time Complexity:
// - Peek Top Trending: O(1)
// - Push Post (Sift-Up): O(log N)
// - Extract Top Trending (Sift-Down): O(log N)
// - Floyd's Heapify (build from scratch): O(N) linear time
// - Top-K Trending Extraction: O(K log N)
// ============================================================================

class TrendingPriorityQueue {
private:
    // Internal array representation of complete binary tree
    std::vector<Post> heap;

    // Step 1: Parent index in 0-indexed complete binary tree
    int getParentIndex(int index) const {
        return (index - 1) / 2;
    }

    // Step 2: Left child index in complete binary tree
    int getLeftChildIndex(int index) const {
        return (2 * index) + 1;
    }

    // Step 3: Right child index in complete binary tree
    int getRightChildIndex(int index) const {
        return (2 * index) + 2;
    }

    // Step 4: Sift-Up Operation based on Trending Score
    // Restores Max-Heap property by bubbling higher-scoring posts up towards the root
    void siftUp(int index) {
        while (index > 0) {
            int parentIdx = getParentIndex(index);

            // Compare trending scores
            if (heap[index].trendingScore > heap[parentIdx].trendingScore) {
                // Child has higher trending score than parent: Swap!
                std::swap(heap[index], heap[parentIdx]);

                // Continue up the tree
                index = parentIdx;
            } else {
                // Max Heap property satisfied
                break;
            }
        }
    }

    // Step 5: Sift-Down Operation based on Trending Score
    // Restores Max-Heap property by trickling lower-scoring posts down
    void siftDown(int index) {
        int size = static_cast<int>(heap.size());

        while (true) {
            int largest = index;
            int leftChild = getLeftChildIndex(index);
            int rightChild = getRightChildIndex(index);

            // Check if left child has higher trending score
            if (leftChild < size && heap[leftChild].trendingScore > heap[largest].trendingScore) {
                largest = leftChild;
            }

            // Check if right child has higher trending score
            if (rightChild < size && heap[rightChild].trendingScore > heap[largest].trendingScore) {
                largest = rightChild;
            }

            // If largest is not the current element, swap with largest child
            if (largest != index) {
                std::swap(heap[index], heap[largest]);

                // Move down to the swapped position and continue
                index = largest;
            } else {
                // Both children are smaller; heap property satisfied
                break;
            }
        }
    }

public:
    TrendingPriorityQueue() {}

    // Step 6: Compute Trending Score using Reddit engagement and time decay formula
    static double computeTrendingScore(const Post& post, long long currentTimeSeconds) {
        // Step 6.1: Calculate age in hours (minimum 0.0)
        double ageHours = (currentTimeSeconds > post.createdAt) 
                        ? static_cast<double>(currentTimeSeconds - post.createdAt) / 3600.0 
                        : 0.0;

        // Step 6.2: Calculate weighted engagement points
        double engagement = (post.upvotes * 2.0) - (post.downvotes * 1.5) + (post.commentsCount * 3.0);
        if (engagement < 1.0) engagement = 1.0; // Prevent non-positive scores

        // Step 6.3: Apply polynomial time decay factor
        double timeDecay = std::pow(ageHours + 2.0, 1.3);

        // Step 6.4: Return normalized trending score
        return (engagement / timeDecay) * 10.0;
    }

    // Step 7: Push a post into the Priority Queue
    void push(Post post, long long currentTimeSeconds) {
        // Step 7.1: Compute and assign the latest trending score
        post.trendingScore = computeTrendingScore(post, currentTimeSeconds);

        // Step 7.2: Append to the end of the heap vector
        heap.push_back(post);

        // Step 7.3: Bubble the post up into its correct heap position
        siftUp(static_cast<int>(heap.size()) - 1);

        std::cout << "[Trending Priority Queue Push]: Post #" << post.id 
                  << " added with Trending Score: " << post.trendingScore << std::endl;
    }

    // Step 8: Peek at the #1 Trending Post without removing it (O(1) time)
    bool top(Post& outPost) const {
        if (heap.empty()) return false;
        outPost = heap[0]; // Root element has the highest priority score
        return true;
    }

    // Step 9: Pop the top trending post from the Priority Queue (O(log N) time)
    bool pop(Post& outPost) {
        if (heap.empty()) return false;

        // Step 9.1: Save root post
        outPost = heap[0];

        // Step 9.2: Move last leaf element to the root position
        heap[0] = heap.back();
        heap.pop_back();

        // Step 9.3: Sift down to restore Max-Heap property
        if (!heap.empty()) {
            siftDown(0);
        }

        return true;
    }

    // Step 10: Rebuild Priority Queue from a list of posts in O(N) linear time (Floyd's Heapify)
    // Starting from the lowest non-leaf node (N/2 - 1) down to 0, call siftDown on each node.
    void rebuildHeap(std::vector<Post>& posts, long long currentTimeSeconds) {
        heap.clear();

        // Step 10.1: Calculate updated trending scores for all posts
        for (auto& post : posts) {
            post.trendingScore = computeTrendingScore(post, currentTimeSeconds);
            heap.push_back(post);
        }

        int n = static_cast<int>(heap.size());
        if (n <= 1) return;

        // Step 10.2: Bottom-Up Heap Construction (Floyd's Algorithm - O(N) time)
        // Nodes from index n/2 to n-1 are already leaves (trivially valid heaps of size 1).
        // We only need to siftDown from the last internal node (n/2 - 1) up to the root (0).
        for (int i = (n / 2) - 1; i >= 0; --i) {
            siftDown(i);
        }

        std::cout << "[Trending Heap Rebuilt]: " << n << " posts re-heapified in O(N) time." << std::endl;
    }

    // Step 11: Get Top K Trending Posts in descending order (O(K log N) time)
    std::vector<Post> getTopTrending(int k) const {
        std::vector<Post> result;
        if (heap.empty() || k <= 0) return result;

        // Clone heap so the original priority queue is intact
        TrendingPriorityQueue tempQueue = *this;

        int count = std::min(k, static_cast<int>(tempQueue.heap.size()));
        for (int i = 0; i < count; ++i) {
            Post topPost;
            if (tempQueue.pop(topPost)) {
                result.push_back(topPost);
            }
        }
        return result;
    }

    // Step 12: Serialize Top K Trending Posts to JSON
    std::string getTopTrendingJson(int k = 10) const {
        std::vector<Post> trendingList = getTopTrending(k);
        std::ostringstream ss;
        ss << "[";
        for (size_t i = 0; i < trendingList.size(); ++i) {
            ss << postToJson(trendingList[i]);
            if (i + 1 < trendingList.size()) ss << ",";
        }
        ss << "]";
        return ss.str();
    }

    // Step 13: Serialize Priority Queue preview for DSA Inspector
    std::string getHeapPreviewJson() const {
        std::ostringstream ss;
        ss << "[";
        for (size_t i = 0; i < heap.size(); ++i) {
            ss << "{"
               << "\"index\":" << i << ","
               << "\"id\":" << heap[i].id << ","
               << "\"title\":\"" << escapeJsonString(heap[i].title) << "\","
               << "\"score\":" << std::fixed << std::setprecision(2) << heap[i].trendingScore << ","
               << "\"upvotes\":" << heap[i].upvotes << ","
               << "\"parentIndex\":" << (i == 0 ? -1 : getParentIndex(static_cast<int>(i)))
               << "}";
            if (i + 1 < heap.size()) ss << ",";
        }
        ss << "]";
        return ss.str();
    }

    size_t size() const { return heap.size(); }
    bool empty() const { return heap.empty(); }
};

#endif // PRIORITY_QUEUE_HPP
