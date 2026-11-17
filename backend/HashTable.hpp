#ifndef HASH_TABLE_HPP
#define HASH_TABLE_HPP

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <cctype>
#include <algorithm>
#include "models.hpp"

// ============================================================================
// FEATURE 3: REDDIT SEARCH USING CUSTOM HASH TABLE (SEPARATE CHAINING)
// ============================================================================
//
// DSA Concept:
// A Hash Table is an associative data structure that maps keys to values for 
// direct O(1) average-time complexity access.
//
// Key Components:
// 1. Hash Function (Polynomial Rolling Hash / DJB2 algorithm):
//    Converts an arbitrary string key into an integer index within [0, capacity - 1].
//    Formula: hash = ((hash << 5) + hash) + char_code (i.e. hash * 33 + char)
//
// 2. Collision Resolution (Separate Chaining):
//    When two different keys map to the same bucket index (collision), they are
//    stored in a singly linked list at that bucket.
//
// 3. Dynamic Resizing & Rehashing:
//    When the Load Factor (size / capacity) exceeds the threshold of 0.75,
//    the capacity is doubled, and all elements are re-hashed into the new table.
//    This guarantees that the average chain length remains small, keeping
//    search, insert, and delete operations at O(1) average time.
//
// 4. Inverted Search Index for Reddit:
//    Maps keywords -> List of Post IDs (e.g. "algorithm" -> [1, 4, 9])
//    Enables instant keyword lookups across millions of posts!
//
// Time Complexity:
// - Hash calculation: O(L) where L is the string length
// - Insert: O(1) average, O(N) worst case (all collide)
// - Search / Lookup: O(1) average, O(N) worst case
// - Rehashing: O(N) when load factor threshold is crossed
// ============================================================================

// Linked list node representing a key-value entry in a bucket chain
template <typename K, typename V>
struct HashNode {
    K key;
    V value;
    HashNode* next;

    // Step 1: Node constructor initializes key, value, and sets next to nullptr
    HashNode(const K& k, const V& v) : key(k), value(v), next(nullptr) {}
};

// Generic Hash Table template with Separate Chaining
template <typename V>
class StringHashTable {
private:
    std::vector<HashNode<std::string, V>*> buckets; // Array of bucket pointers
    size_t numElements;                             // Total number of stored key-value pairs
    size_t tableCapacity;                           // Total number of buckets
    const double LOAD_FACTOR_THRESHOLD = 0.75;      // Threshold triggering dynamic rehashing

    // Step 2: Custom Hash Function (DJB2 algorithm by Dan Bernstein)
    // Produces excellent uniform distribution across hash table buckets
    size_t hashFunction(const std::string& key) const {
        unsigned long hash = 5381; // Initial prime seed value
        for (char c : key) {
            // hash * 33 + c
            hash = ((hash << 5) + hash) + static_cast<unsigned char>(c);
        }
        return hash % tableCapacity; // Map hash value to valid bucket index [0, tableCapacity - 1]
    }

    // Step 3: Dynamic Rehashing Function
    // Doubles table capacity and redistributes all existing elements into new buckets
    void rehash(size_t newCapacity) {
        std::cout << "[Hash Table Rehashing]: Load factor exceeded 0.75. Resizing from " 
                  << tableCapacity << " to " << newCapacity << " buckets..." << std::endl;

        std::vector<HashNode<std::string, V>*> oldBuckets = buckets;
        size_t oldCapacity = tableCapacity;

        // Allocate new larger bucket array initialized to nullptr
        tableCapacity = newCapacity;
        buckets.assign(tableCapacity, nullptr);
        numElements = 0; // Will be incremented back during re-insertion

        // Re-insert all nodes from old buckets into new buckets
        for (size_t i = 0; i < oldCapacity; ++i) {
            HashNode<std::string, V>* current = oldBuckets[i];
            while (current != nullptr) {
                insert(current->key, current->value);
                HashNode<std::string, V>* nextNode = current->next;
                delete current; // Free old node memory after re-inserting
                current = nextNode;
            }
        }
    }

public:
    // Step 4: Constructor initializes hash table with default capacity (e.g. 16 buckets)
    StringHashTable(size_t initialCapacity = 16) 
        : numElements(0), tableCapacity(initialCapacity) {
        buckets.assign(tableCapacity, nullptr);
    }

    // Step 5: Destructor cleanly deletes all chained nodes in each bucket
    ~StringHashTable() {
        for (size_t i = 0; i < tableCapacity; ++i) {
            HashNode<std::string, V>* current = buckets[i];
            while (current != nullptr) {
                HashNode<std::string, V>* nextNode = current->next;
                delete current;
                current = nextNode;
            }
            buckets[i] = nullptr;
        }
    }

    // Step 6: Insert or Update a Key-Value pair
    void insert(const std::string& key, const V& value) {
        // Step 6.1: Compute bucket index using hash function
        size_t bucketIndex = hashFunction(key);

        // Step 6.2: Search existing bucket chain to see if key already exists (update case)
        HashNode<std::string, V>* current = buckets[bucketIndex];
        while (current != nullptr) {
            if (current->key == key) {
                current->value = value; // Key exists, update value in place
                return;
            }
            current = current->next;
        }

        // Step 6.3: Key not found; create new node and insert at front of bucket chain (Separate Chaining)
        HashNode<std::string, V>* newNode = new HashNode<std::string, V>(key, value);
        newNode->next = buckets[bucketIndex];
        buckets[bucketIndex] = newNode;
        numElements++;

        // Step 6.4: Check load factor and trigger rehashing if needed
        double currentLoadFactor = static_cast<double>(numElements) / tableCapacity;
        if (currentLoadFactor > LOAD_FACTOR_THRESHOLD) {
            rehash(tableCapacity * 2);
        }
    }

    // Step 7: Search for a key in the Hash Table (O(1) average time)
    bool search(const std::string& key, V& outValue) const {
        // Step 7.1: Compute bucket index in O(1) time
        size_t bucketIndex = hashFunction(key);

        // Step 7.2: Traverse bucket linked list to find matching key
        HashNode<std::string, V>* current = buckets[bucketIndex];
        while (current != nullptr) {
            if (current->key == key) {
                outValue = current->value; // Found match! Copy value to output parameter
                return true;
            }
            current = current->next;
        }

        // Key not found in bucket chain
        return false;
    }

    // Step 8: Check if key exists in Hash Table
    bool contains(const std::string& key) const {
        V dummy;
        return search(key, dummy);
    }

    // Step 9: Get diagnostic stats about buckets, collisions, and load factor
    void getStats(int& outCapacity, int& outElements, double& outLoadFactor, 
                  int& outCollisions, int& outMaxChainLength) const {
        outCapacity = static_cast<int>(tableCapacity);
        outElements = static_cast<int>(numElements);
        outLoadFactor = static_cast<double>(numElements) / tableCapacity;
        outCollisions = 0;
        outMaxChainLength = 0;

        for (size_t i = 0; i < tableCapacity; ++i) {
            int chainLen = 0;
            HashNode<std::string, V>* curr = buckets[i];
            while (curr) {
                chainLen++;
                curr = curr->next;
            }
            if (chainLen > 1) {
                outCollisions += (chainLen - 1); // Extra elements in chain beyond the first count as collisions
            }
            if (chainLen > outMaxChainLength) {
                outMaxChainLength = chainLen;
            }
        }
    }

    // Step 10: Serialize buckets preview for the DSA inspector in frontend
    std::string getBucketsPreviewJson(int maxBucketsToShow = 16) const {
        std::ostringstream ss;
        ss << "[";
        int limit = std::min(static_cast<int>(tableCapacity), maxBucketsToShow);
        for (int i = 0; i < limit; ++i) {
            ss << "{\"bucketIndex\":" << i << ",\"chain\":[";
            HashNode<std::string, V>* curr = buckets[i];
            bool first = true;
            while (curr) {
                if (!first) ss << ",";
                ss << "\"" << escapeJsonString(curr->key) << "\"";
                first = false;
                curr = curr->next;
            }
            ss << "]}";
            if (i + 1 < limit) ss << ",";
        }
        ss << "]";
        return ss.str();
    }

    size_t size() const { return numElements; }
};

// ============================================================================
// REDDIT SEARCH ENGINE USING INVERTED INDEX HASH TABLE
// ============================================================================

class RedditSearchEngine {
private:
    // Inverted Index: Maps lowercase keyword -> List of Post IDs
    // Example: "cpp" -> [101, 104, 108]
    StringHashTable<std::vector<int>> invertedIndex;

    // Direct Post Store: Maps Post ID (as string) -> Post Object
    StringHashTable<Post> postStore;

    // Helper: Normalize word (convert to lowercase and remove punctuation)
    std::string cleanWord(const std::string& input) {
        std::string result = "";
        for (char c : input) {
            if (std::isalnum(static_cast<unsigned char>(c))) {
                result += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            }
        }
        return result;
    }

    // Helper: Tokenize text into distinct keywords
    std::vector<std::string> tokenize(const std::string& text) {
        std::vector<std::string> words;
        std::istringstream stream(text);
        std::string word;
        while (stream >> word) {
            std::string cleaned = cleanWord(word);
            // Ignore very short tokens (e.g. single letters or empty strings)
            if (cleaned.length() >= 2) {
                words.push_back(cleaned);
            }
        }
        return words;
    }

public:
    RedditSearchEngine() {}

    // Step 1: Index a new Reddit Post into the Hash Table
    void indexPost(const Post& post) {
        // Step 1.1: Store post directly in post store hash table
        postStore.insert(std::to_string(post.id), post);

        // Step 1.2: Tokenize post title, content, subreddit, and flair for inverted search
        std::vector<std::string> titleTokens = tokenize(post.title);
        std::vector<std::string> contentTokens = tokenize(post.content);
        std::vector<std::string> subTokens = tokenize(post.subreddit);
        std::vector<std::string> flairTokens = tokenize(post.flair);

        std::vector<std::string> allTokens;
        allTokens.insert(allTokens.end(), titleTokens.begin(), titleTokens.end());
        allTokens.insert(allTokens.end(), contentTokens.begin(), contentTokens.end());
        allTokens.insert(allTokens.end(), subTokens.begin(), subTokens.end());
        allTokens.insert(allTokens.end(), flairTokens.begin(), flairTokens.end());

        // Step 1.3: Map each distinct token to this post's ID in the inverted index
        for (const std::string& token : allTokens) {
            std::vector<int> existingPostIds;
            if (invertedIndex.search(token, existingPostIds)) {
                // If post ID is not already recorded for this token, add it
                if (std::find(existingPostIds.begin(), existingPostIds.end(), post.id) == existingPostIds.end()) {
                    existingPostIds.push_back(post.id);
                    invertedIndex.insert(token, existingPostIds); // Re-insert updated list
                }
            } else {
                // First post containing this keyword
                std::vector<int> newPostIds = { post.id };
                invertedIndex.insert(token, newPostIds);
            }
        }

        std::cout << "[Hash Table Indexed]: Post #" << post.id << " ('" 
                  << post.title.substr(0, std::min<size_t>(post.title.length(), 25)) 
                  << "...') indexed with " << allTokens.size() << " keywords." << std::endl;
    }

    // Step 2: Search posts by query string in O(1) hash lookup per token
    std::vector<Post> searchPosts(const std::string& query) {
        std::vector<Post> matchedPosts;
        std::vector<std::string> queryTokens = tokenize(query);

        if (queryTokens.empty()) {
            return matchedPosts;
        }

        // Map to count how many query keywords each post matched (relevance ranking)
        std::vector<int> candidatePostIds;

        for (const std::string& token : queryTokens) {
            std::vector<int> postIds;
            // O(1) average lookup in hash table for the token!
            if (invertedIndex.search(token, postIds)) {
                for (int pid : postIds) {
                    if (std::find(candidatePostIds.begin(), candidatePostIds.end(), pid) == candidatePostIds.end()) {
                        candidatePostIds.push_back(pid);
                    }
                }
            }
        }

        // Retrieve full Post objects from postStore hash table
        for (int pid : candidatePostIds) {
            Post post;
            if (postStore.search(std::to_string(pid), post)) {
                matchedPosts.push_back(post);
            }
        }

        return matchedPosts;
    }

    // Step 3: Get post by ID from Hash Table
    bool getPostById(int id, Post& outPost) {
        return postStore.search(std::to_string(id), outPost);
    }

    // Step 4: Expose diagnostic stats for the inverted index hash table
    void getStats(int& capacity, int& elements, double& loadFactor, int& collisions, int& maxChain) const {
        invertedIndex.getStats(capacity, elements, loadFactor, collisions, maxChain);
    }

    std::string getBucketsPreviewJson() const {
        return invertedIndex.getBucketsPreviewJson(16);
    }
};

#endif // HASH_TABLE_HPP
