// ============================================================================
// REDDIT DSA WEB APPLICATION - C++ BACKEND SERVER
// ============================================================================
//
// This server implements 4 core Reddit features strictly using fundamental Data Structures:
// 1. Subreddit Hierarchy  -->  N-ary Tree (NaryTree.hpp)
// 2. Karma Ranking        -->  Max Heap (MaxHeap.hpp)
// 3. Reddit Search        -->  Custom Hash Table with Separate Chaining (HashTable.hpp)
// 4. Trending Posts       -->  Heap / Priority Queue (PriorityQueue.hpp)
//
// The server provides a high-performance HTTP REST API using native Windows Sockets (Winsock2).
// Every step in this code is thoroughly commented to explain its purpose and DSA logic.
// ============================================================================

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <chrono>
#include <cstring>
#include <cstdlib>

// Windows Networking Sockets API
#include <winsock2.h>
#include <ws2tcpip.h>

// Include our custom DSA header files
#include "models.hpp"
#include "NaryTree.hpp"
#include "MaxHeap.hpp"
#include "HashTable.hpp"
#include "PriorityQueue.hpp"

// Port on which the C++ backend will listen for requests from the React frontend
const int SERVER_PORT = 8080;

// ============================================================================
// GLOBAL APPLICATION STATE (HOLDING OUR 4 CORE DATA STRUCTURES)
// ============================================================================

// DSA Feature 1: N-ary Tree managing the entire subreddit category hierarchy
SubredditTree g_subredditTree;

// DSA Feature 2: Max Heap ranking users by their karma score in O(1) top lookup
KarmaMaxHeap g_karmaMaxHeap;

// DSA Feature 3: Hash Table inverted index for instantaneous O(1) keyword search
RedditSearchEngine g_searchEngine;

// DSA Feature 4: Priority Queue (Max-Heap) prioritizing posts by trending momentum
TrendingPriorityQueue g_trendingQueue;

// Master repository of all posts
std::vector<Post> g_allPosts;
int g_nextPostId = 101;

// Helper: Get current Unix epoch timestamp in seconds
long long getCurrentTimestamp() {
    return std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

// ============================================================================
// STEP 1: INITIALIZE AND PRE-SEED REALISTIC REDDIT DATA
// ============================================================================
void seedRedditData() {
    std::cout << "[Init]: Seeding Subreddit Hierarchy (N-ary Tree)..." << std::endl;
    // Step 1.1: Populate N-ary Tree hierarchy
    // Level 1: Categories directly under Root "all"
    g_subredditTree.insertSubreddit("all", "technology", "Technology", 
        "Dedicated to the news and discussions about the creation and use of technology and its surrounding issues.", 14200000, "💻");
    g_subredditTree.insertSubreddit("all", "gaming", "Gaming", 
        "A facility for discussion and sharing news about video games, game hardware, and industry updates.", 34000000, "🎮");
    g_subredditTree.insertSubreddit("all", "science", "Science", 
        "This community is a place to share and discuss new scientific research, breakthroughs, and theories.", 31000000, "🔬");

    // Level 2: Sub-communities under "technology"
    g_subredditTree.insertSubreddit("technology", "programming", "Programming", 
        "Computer Programming discussions, language paradigms, and software development practices.", 5800000, "👨‍💻");
    g_subredditTree.insertSubreddit("technology", "artificial_intelligence", "Artificial Intelligence", 
        "All things AI, LLMs, Neural Networks, Computer Vision, and the future of machine learning.", 4100000, "🤖");

    // Level 3: Specialized sub-topics under "programming"
    g_subredditTree.insertSubreddit("programming", "cpp", "C++ Programming", 
        "Discussions, articles, and questions about the C++ programming language, standards, and DSA.", 325000, "⚡");
    g_subredditTree.insertSubreddit("programming", "webdev", "Web Development", 
        "Dedicated to all things web development: frontend, backend, React, CSS, and modern web tools.", 2150000, "🌐");
    g_subredditTree.insertSubreddit("programming", "algorithms", "Algorithms & DSA", 
        "Data structures, trees, heaps, hash tables, competitive programming, and algorithmic design.", 890000, "🧠");

    // Level 2: Sub-communities under "gaming"
    g_subredditTree.insertSubreddit("gaming", "pcgaming", "PC Gaming", 
        "A community for discussion of PC gaming hardware, game releases, and PC master race culture.", 3200000, "🖥️");
    g_subredditTree.insertSubreddit("gaming", "indiegames", "Indie Games", 
        "Highlighting independent developers, hidden gems, and innovative game mechanics.", 960000, "🕹️");

    // Level 2: Sub-communities under "science"
    g_subredditTree.insertSubreddit("science", "space", "Space Exploration", 
        "Rockets, rovers, astronomical discoveries, cosmology, and humanity's journey to the stars.", 23500000, "🚀");
    g_subredditTree.insertSubreddit("science", "physics", "Physics", 
        "Quantum mechanics, particle physics, general relativity, and fundamental laws of the universe.", 2450000, "⚛️");

    std::cout << "[Init]: Seeding User Karma (Max Heap)..." << std::endl;
    // Step 1.2: Populate Karma Max Heap with users
    g_karmaMaxHeap.insert(UserKarma("satoshi_dev", 18450, 11200, 7250, "🧙‍♂️", "Legend"));
    g_karmaMaxHeap.insert(UserKarma("bjarne_fan", 14320, 8900, 5420, "⚡", "C++ Wizard"));
    g_karmaMaxHeap.insert(UserKarma("algo_guru", 11200, 6800, 4400, "🧠", "DSA Master"));
    g_karmaMaxHeap.insert(UserKarma("react_ninja", 9450, 5800, 3650, "⚛️", "Senior Dev"));
    g_karmaMaxHeap.insert(UserKarma("kernel_hacker", 7890, 4700, 3190, "🐧", "Open Source"));
    g_karmaMaxHeap.insert(UserKarma("web_craftsman", 5420, 3200, 2220, "🎨", "CSS Artist"));
    g_karmaMaxHeap.insert(UserKarma("space_cadet", 3800, 2100, 1700, "🚀", "Cosmonaut"));

    std::cout << "[Init]: Seeding Posts, Hash Table, and Priority Queue..." << std::endl;
    // Step 1.3: Populate sample Reddit Posts
    long long now = getCurrentTimestamp();

    std::vector<Post> initialPosts = {
        Post(101, "Why N-ary Trees are the cleanest data structure for nested Subreddit categories", 
             "When modeling nested communities on Reddit, an N-ary tree allows each parent category (like Technology) to hold an arbitrary number of subreddits (Programming, AI). Traversal in pre-order yields clean nested JSON!", 
             "algo_guru", "algorithms", 342, 12, 54, now - 3600 * 2, "Technical"),

        Post(102, "Building a blazing fast Reddit clone backend purely in C++ with custom DSA", 
             "Forget bulky frameworks! By implementing a Max Heap for karma, an N-ary tree for categories, a Hash Table with separate chaining for search, and a priority queue for trending posts, C++ handles thousands of requests per second effortlessly.", 
             "bjarne_fan", "cpp", 512, 18, 89, now - 3600 * 4, "Showcase"),

        Post(103, "Modern React + CSS styling tricks to match Reddit's sleek dark UI", 
             "How we designed custom Reddit-style post cards, upvote pills, collapsible sidebar trees, and glowing trending badges with pure CSS variables and smooth transitions.", 
             "react_ninja", "webdev", 289, 7, 36, now - 3600 * 6, "Tutorial"),

        Post(104, "Max Heap vs Linear Scan: How we ranked top Reddidors in O(log N) time", 
             "A deep dive into complete binary trees stored in contiguous arrays. By using siftUp on upvotes and siftDown on extracts, leaderboard calculation drops from O(N log N) to O(K log N)!", 
             "algo_guru", "algorithms", 198, 5, 23, now - 3600 * 9, "Deep Dive"),

        Post(105, "James Webb Telescope captures unprecedented view of distant early galaxy", 
             "Astronomers using the NIRCam instrument have confirmed a massive galaxy formed just 350 million years after the Big Bang, defying previous cosmological simulations.", 
             "space_cadet", "space", 1240, 34, 182, now - 3600 * 1, "News"),

        Post(106, "Why Separate Chaining Hash Tables beat Open Addressing for dynamic text search", 
             "In our Reddit search engine, tokens are mapped to post IDs. Separate chaining handles frequent additions smoothly without clustering bottlenecks, maintaining O(1) average lookup.", 
             "satoshi_dev", "cpp", 420, 15, 62, now - 3600 * 5, "Discussion"),

        Post(107, "Indie studio creates entire procedural RPG inside custom C++ game engine", 
             "After 4 years of development, the solo dev team demonstrated their voxel world engine built from scratch in C++ with zero external game engines.", 
             "kernel_hacker", "indiegames", 630, 22, 74, now - 3600 * 3, "Gaming")
    };

    for (const auto& post : initialPosts) {
        g_allPosts.push_back(post);
        // Step 1.4: Index each post into the Hash Table for instant search
        g_searchEngine.indexPost(post);
    }

    // Step 1.5: Rebuild the Trending Priority Queue in O(N) linear time using Floyd's algorithm
    g_trendingQueue.rebuildHeap(g_allPosts, now);

    std::cout << "[Init]: All 4 DSA structures successfully pre-seeded!" << std::endl;
}

// ============================================================================
// STEP 2: HTTP REQUEST PARSING AND ROUTING HELPERS
// ============================================================================

// Structure representing an HTTP Request
struct HttpRequest {
    std::string method;         // GET, POST, OPTIONS
    std::string path;           // /api/posts, /api/search
    std::string queryString;    // q=cpp&limit=10
    std::string body;           // JSON payload for POST
};

// URL-decode a percent-encoded string (e.g. "c%2B%2B" -> "c++")
std::string urlDecode(const std::string& in) {
    std::string out;
    for (size_t i = 0; i < in.length(); ++i) {
        if (in[i] == '+') {
            out += ' ';
        } else if (in[i] == '%' && i + 2 < in.length()) {
            std::string hexStr = in.substr(i + 1, 2);
            char ch = static_cast<char>(std::strtol(hexStr.c_str(), nullptr, 16));
            out += ch;
            i += 2;
        } else {
            out += in[i];
        }
    }
    return out;
}

// Simple JSON field extractor for strings
std::string extractJsonString(const std::string& json, const std::string& key) {
    std::string searchKey = "\"" + key + "\"";
    size_t keyPos = json.find(searchKey);
    if (keyPos == std::string::npos) return "";

    size_t colonPos = json.find(':', keyPos);
    if (colonPos == std::string::npos) return "";

    size_t quoteStart = json.find('"', colonPos);
    if (quoteStart == std::string::npos) return "";

    size_t quoteEnd = json.find('"', quoteStart + 1);
    if (quoteEnd == std::string::npos) return "";

    return json.substr(quoteStart + 1, quoteEnd - quoteStart - 1);
}

// Simple JSON field extractor for integers
int extractJsonInt(const std::string& json, const std::string& key, int defaultVal = 0) {
    std::string searchKey = "\"" + key + "\"";
    size_t keyPos = json.find(searchKey);
    if (keyPos == std::string::npos) return defaultVal;

    size_t colonPos = json.find(':', keyPos);
    if (colonPos == std::string::npos) return defaultVal;

    size_t start = json.find_first_of("-0123456789", colonPos);
    if (start == std::string::npos) return defaultVal;

    size_t end = json.find_first_not_of("-0123456789", start);
    std::string numStr = (end == std::string::npos) ? json.substr(start) : json.substr(start, end - start);
    try {
        return std::stoi(numStr);
    } catch (...) {
        return defaultVal;
    }
}

// Parse raw HTTP request string into HttpRequest struct
HttpRequest parseHttpRequest(const std::string& raw) {
    HttpRequest req;
    std::istringstream stream(raw);
    std::string requestLine;
    if (std::getline(stream, requestLine)) {
        // Strip carriage return if present
        if (!requestLine.empty() && requestLine.back() == '\r') {
            requestLine.pop_back();
        }

        std::istringstream lineStream(requestLine);
        lineStream >> req.method;
        std::string fullPath;
        lineStream >> fullPath;

        // Split path and query string
        size_t questionMark = fullPath.find('?');
        if (questionMark != std::string::npos) {
            req.path = fullPath.substr(0, questionMark);
            req.queryString = fullPath.substr(questionMark + 1);
        } else {
            req.path = fullPath;
            req.queryString = "";
        }
    }

    // Extract HTTP body after empty line \r\n\r\n
    size_t bodyPos = raw.find("\r\n\r\n");
    if (bodyPos != std::string::npos) {
        req.body = raw.substr(bodyPos + 4);
    }

    return req;
}

// Helper to construct HTTP response with full CORS headers
std::string buildResponse(int statusCode, const std::string& statusText, 
                          const std::string& body, const std::string& contentType = "application/json; charset=utf-8") {
    std::ostringstream ss;
    ss << "HTTP/1.1 " << statusCode << " " << statusText << "\r\n";
    ss << "Content-Type: " << contentType << "\r\n";
    ss << "Content-Length: " << body.length() << "\r\n";
    ss << "Access-Control-Allow-Origin: *\r\n";
    ss << "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n";
    ss << "Access-Control-Allow-Headers: Content-Type, Authorization\r\n";
    ss << "Connection: close\r\n\r\n";
    ss << body;
    return ss.str();
}

// ============================================================================
// STEP 3: API REQUEST HANDLER ROUTING TO OUR 4 DSA MODULES
// ============================================================================
std::string handleRequest(const HttpRequest& req) {
    // Step 3.1: Handle CORS Preflight OPTIONS requests from React browser
    if (req.method == "OPTIONS") {
        return buildResponse(204, "No Content", "");
    }

    // ------------------------------------------------------------------------
    // FEATURE 1 ENDPOINTS: SUBREDDIT HIERARCHY (N-ARY TREE)
    // ------------------------------------------------------------------------
    // Endpoint: GET /api/subreddits/tree -> Returns the full hierarchical N-ary tree
    if (req.method == "GET" && req.path == "/api/subreddits/tree") {
        std::string treeJson = g_subredditTree.getTreeJson();
        return buildResponse(200, "OK", treeJson);
    }

    // Endpoint: GET /api/subreddits/all -> Returns flattened list with tree depth levels
    if (req.method == "GET" && req.path == "/api/subreddits/all") {
        std::string flatJson = g_subredditTree.getFlattenedJson();
        return buildResponse(200, "OK", flatJson);
    }

    // Endpoint: POST /api/subreddits -> Adds a new sub-community node into the N-ary tree
    if (req.method == "POST" && req.path == "/api/subreddits") {
        std::string parent = extractJsonString(req.body, "parent");
        std::string name = extractJsonString(req.body, "name");
        std::string title = extractJsonString(req.body, "title");
        std::string description = extractJsonString(req.body, "description");
        std::string icon = extractJsonString(req.body, "icon");
        if (icon.empty()) icon = "💬";

        if (parent.empty() || name.empty() || title.empty()) {
            return buildResponse(400, "Bad Request", "{\"error\":\"Missing required fields: parent, name, title\"}");
        }

        // Insert into N-ary Tree
        bool success = g_subredditTree.insertSubreddit(parent, name, title, description, 1, icon);
        if (success) {
            return buildResponse(201, "Created", "{\"success\":true,\"message\":\"Subreddit inserted into N-ary Tree\"}");
        } else {
            return buildResponse(400, "Bad Request", "{\"error\":\"Failed to insert subreddit. Parent may not exist or name is duplicate.\"}");
        }
    }

    // ------------------------------------------------------------------------
    // FEATURE 2 ENDPOINTS: KARMA RANKING (MAX HEAP)
    // ------------------------------------------------------------------------
    // Endpoint: GET /api/karma/leaderboard -> Returns Top K contributors ranked by Max Heap
    if (req.method == "GET" && req.path == "/api/karma/leaderboard") {
        std::string leaderboardJson = g_karmaMaxHeap.getLeaderboardJson(10);
        return buildResponse(200, "OK", leaderboardJson);
    }

    // Endpoint: POST /api/karma/vote -> Upvote or Downvote a post
    // This dynamically updates author karma in the Max Heap & updates trending priority queue!
    if (req.method == "POST" && req.path == "/api/karma/vote") {
        int postId = extractJsonInt(req.body, "postId", 0);
        std::string direction = extractJsonString(req.body, "direction"); // "up" or "down"

        if (postId == 0 || (direction != "up" && direction != "down")) {
            return buildResponse(400, "Bad Request", "{\"error\":\"Invalid postId or direction ('up' or 'down')\"}");
        }

        int delta = (direction == "up") ? 1 : -1;

        // Find post in allPosts
        Post* targetPost = nullptr;
        for (auto& p : g_allPosts) {
            if (p.id == postId) {
                targetPost = &p;
                break;
            }
        }

        if (!targetPost) {
            return buildResponse(404, "Not Found", "{\"error\":\"Post not found\"}");
        }

        // Apply vote to post
        if (direction == "up") targetPost->upvotes += 1;
        else targetPost->downvotes += 1;

        // Step: Update Author Karma dynamically in the Max Heap (O(log N) sift operation)
        g_karmaMaxHeap.updateKarma(targetPost->author, delta * 10, true);

        // Step: Re-index in Hash Table
        g_searchEngine.indexPost(*targetPost);

        // Step: Re-heapify Trending Priority Queue
        g_trendingQueue.rebuildHeap(g_allPosts, getCurrentTimestamp());

        std::ostringstream ss;
        ss << "{\"success\":true,"
           << "\"postId\":" << targetPost->id << ","
           << "\"upvotes\":" << targetPost->upvotes << ","
           << "\"downvotes\":" << targetPost->downvotes << ","
           << "\"author\":\"" << escapeJsonString(targetPost->author) << "\""
           << "}";
        return buildResponse(200, "OK", ss.str());
    }

    // ------------------------------------------------------------------------
    // FEATURE 3 ENDPOINTS: REDDIT SEARCH (HASH TABLE INVERTED INDEX)
    // ------------------------------------------------------------------------
    // Endpoint: GET /api/search?q=keyword
    if (req.method == "GET" && req.path == "/api/search") {
        std::string query = "";
        size_t qPos = req.queryString.find("q=");
        if (qPos != std::string::npos) {
            size_t ampPos = req.queryString.find('&', qPos);
            std::string rawQ = (ampPos == std::string::npos) 
                             ? req.queryString.substr(qPos + 2) 
                             : req.queryString.substr(qPos + 2, ampPos - (qPos + 2));
            query = urlDecode(rawQ);
        }

        // Step: Search using Hash Table Inverted Index in O(1) average lookup per word
        std::vector<Post> matched = g_searchEngine.searchPosts(query);

        std::ostringstream ss;
        ss << "{\"query\":\"" << escapeJsonString(query) << "\",\"results\":[";
        for (size_t i = 0; i < matched.size(); ++i) {
            ss << postToJson(matched[i]);
            if (i + 1 < matched.size()) ss << ",";
        }
        ss << "]}";
        return buildResponse(200, "OK", ss.str());
    }

    // ------------------------------------------------------------------------
    // FEATURE 4 ENDPOINTS: TRENDING POSTS (HEAP / PRIORITY QUEUE)
    // ------------------------------------------------------------------------
    // Endpoint: GET /api/posts/trending -> Returns Top Trending posts from Max-Heap
    if (req.method == "GET" && req.path == "/api/posts/trending") {
        std::string trendingJson = g_trendingQueue.getTopTrendingJson(10);
        return buildResponse(200, "OK", trendingJson);
    }

    // ------------------------------------------------------------------------
    // GENERAL POSTS ENDPOINTS (FILTER BY SUBREDDIT VIA N-ARY TREE)
    // ------------------------------------------------------------------------
    // Endpoint: GET /api/posts
    if (req.method == "GET" && req.path == "/api/posts") {
        std::string subFilter = "";
        size_t subPos = req.queryString.find("subreddit=");
        if (subPos != std::string::npos) {
            size_t ampPos = req.queryString.find('&', subPos);
            subFilter = (ampPos == std::string::npos) 
                      ? req.queryString.substr(subPos + 10) 
                      : req.queryString.substr(subPos + 10, ampPos - (subPos + 10));
            subFilter = urlDecode(subFilter);
        }

        std::ostringstream ss;
        ss << "[";
        bool first = true;
        for (const auto& post : g_allPosts) {
            // If subreddit filter is specified and not "all", filter posts
            if (!subFilter.empty() && subFilter != "all" && post.subreddit != subFilter) {
                continue;
            }
            if (!first) ss << ",";
            ss << postToJson(post);
            first = false;
        }
        ss << "]";
        return buildResponse(200, "OK", ss.str());
    }

    // Endpoint: POST /api/posts -> Create a new post
    // Adds post to repository, indexes in Hash Table, inserts into Trending Heap!
    if (req.method == "POST" && req.path == "/api/posts") {
        std::string title = extractJsonString(req.body, "title");
        std::string content = extractJsonString(req.body, "content");
        std::string author = extractJsonString(req.body, "author");
        std::string subreddit = extractJsonString(req.body, "subreddit");
        std::string flair = extractJsonString(req.body, "flair");
        if (author.empty()) author = "curious_redditor";
        if (subreddit.empty()) subreddit = "programming";
        if (flair.empty()) flair = "Discussion";

        if (title.empty() || content.empty()) {
            return buildResponse(400, "Bad Request", "{\"error\":\"Title and content are required\"}");
        }

        // Create new Post instance
        int newId = g_nextPostId++;
        long long now = getCurrentTimestamp();
        Post newPost(newId, title, content, author, subreddit, 1, 0, 0, now, flair);

        // Step: Save to global repository
        g_allPosts.insert(g_allPosts.begin(), newPost);

        // Step: Index post into Hash Table Inverted Index
        g_searchEngine.indexPost(newPost);

        // Step: Rebuild / Push into Trending Priority Queue
        g_trendingQueue.rebuildHeap(g_allPosts, now);

        // Step: Award author +10 karma in the Max Heap
        g_karmaMaxHeap.updateKarma(author, 10, true);

        return buildResponse(201, "Created", postToJson(newPost));
    }

    // ------------------------------------------------------------------------
    // DSA INSPECTOR STATS ENDPOINT
    // ------------------------------------------------------------------------
    // Endpoint: GET /api/dsa/stats -> Diagnostic data for all 4 structures
    if (req.method == "GET" && req.path == "/api/dsa/stats") {
        int hashCapacity = 0, hashElements = 0, hashCollisions = 0, hashMaxChain = 0;
        double hashLoadFactor = 0.0;
        g_searchEngine.getStats(hashCapacity, hashElements, hashLoadFactor, hashCollisions, hashMaxChain);

        UserKarma topKarmaUser;
        g_karmaMaxHeap.peekMax(topKarmaUser);

        Post topTrendingPost;
        g_trendingQueue.top(topTrendingPost);

        std::ostringstream ss;
        ss << "{"
           << "\"naryTree\":{"
           << "\"totalNodes\":" << g_subredditTree.getNodeCount() << ","
           << "\"treeHeight\":" << g_subredditTree.getTreeHeight()
           << "},"
           << "\"maxHeap\":{"
           << "\"heapSize\":" << g_karmaMaxHeap.size() << ","
           << "\"topUser\":\"" << escapeJsonString(topKarmaUser.username) << "\","
           << "\"topKarma\":" << topKarmaUser.karma << ","
           << "\"heapArray\":" << g_karmaMaxHeap.getHeapArrayJson()
           << "},"
           << "\"hashTable\":{"
           << "\"capacity\":" << hashCapacity << ","
           << "\"elements\":" << hashElements << ","
           << "\"loadFactor\":" << std::fixed << std::setprecision(2) << hashLoadFactor << ","
           << "\"collisions\":" << hashCollisions << ","
           << "\"maxChainLength\":" << hashMaxChain << ","
           << "\"bucketsPreview\":" << g_searchEngine.getBucketsPreviewJson()
           << "},"
           << "\"priorityQueue\":{"
           << "\"queueSize\":" << g_trendingQueue.size() << ","
           << "\"topPostTitle\":\"" << escapeJsonString(topTrendingPost.title) << "\","
           << "\"topTrendingScore\":" << std::fixed << std::setprecision(2) << topTrendingPost.trendingScore << ","
           << "\"queuePreview\":" << g_trendingQueue.getHeapPreviewJson()
           << "}"
           << "}";
        return buildResponse(200, "OK", ss.str());
    }

    // Default 404 handler
    return buildResponse(404, "Not Found", "{\"error\":\"Route not found\"}");
}

// ============================================================================
// STEP 4: SERVER ENTRY POINT (WINSOCK SOCKET LISTENER)
// ============================================================================
int main() {
    std::cout << "=======================================================" << std::endl;
    std::cout << "   REDDIT CLONE BACKEND (POWERED BY C++ & DSA)        " << std::endl;
    std::cout << "=======================================================" << std::endl;

    // Step 4.1: Seed all 4 core data structures with realistic Reddit data
    seedRedditData();

    // Step 4.2: Initialize Windows Sockets API (WSAStartup)
    WSADATA wsaData;
    int wsaResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsaResult != 0) {
        std::cerr << "[Fatal Error]: WSAStartup failed with code: " << wsaResult << std::endl;
        return 1;
    }
    std::cout << "[Winsock]: Initialized successfully." << std::endl;

    // Step 4.3: Create server listening socket (AF_INET = IPv4, SOCK_STREAM = TCP)
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "[Fatal Error]: Could not create socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // Step 4.4: Allow socket address reuse (SO_REUSEADDR)
    BOOL optVal = TRUE;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&optVal, sizeof(optVal));

    // Step 4.5: Bind socket to local IP address and target port (8080)
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY; // Accept connections on any local network interface
    serverAddr.sin_port = htons(SERVER_PORT);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "[Fatal Error]: Bind failed on port " << SERVER_PORT << ": " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Step 4.6: Start listening for incoming connections
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "[Fatal Error]: Listen failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "\n>>> C++ DSA Backend Server running on http://localhost:" << SERVER_PORT << " <<<" << std::endl;
    std::cout << "Waiting for requests from React Frontend...\n" << std::endl;

    // Step 4.7: Main connection acceptance loop
    char buffer[16384];

    while (true) {
        sockaddr_in clientAddr;
        int clientLen = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientLen);

        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "[Server Warning]: accept() failed: " << WSAGetLastError() << std::endl;
            continue;
        }

        // Read request from client
        std::memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

        if (bytesReceived > 0) {
            std::string rawRequest(buffer, bytesReceived);

            // Parse HTTP Request
            HttpRequest request = parseHttpRequest(rawRequest);

            std::cout << "[API Request]: " << request.method << " " << request.path;
            if (!request.queryString.empty()) std::cout << "?" << request.queryString;
            std::cout << std::endl;

            // Generate HTTP Response via our DSA router
            std::string response = handleRequest(request);

            // Send response back to browser client
            send(clientSocket, response.c_str(), static_cast<int>(response.length()), 0);
        }

        // Close connection for this HTTP/1.1 request
        closesocket(clientSocket);
    }

    // Step 4.8: Clean up socket and Winsock resources upon shutdown
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
