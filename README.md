# Reddit DSA Web Application

A Reddit clone featuring an authentic Reddit UI built with **React.js & CSS** and a high-performance backend built purely in **C++** implementing 4 core Reddit features using fundamental **Data Structures & Algorithms (DSA)**.

---

## 4 Core Features & DSA Implementations

### 1. Subreddit Hierarchy &rarr; N-ary Tree (`backend/NaryTree.hpp`)
* **Concept**: Subreddits and topics branch out into multiple child communities. Each node can have an arbitrary number ($N$) of child nodes.
* **Operations**:
  * **DFS Traversal**: Used to search for categories and subreddits.
  * **Bottom-Up Traversal**: Traverses upward from any subreddit node to the root via `parent` pointers to compute breadcrumb navigation paths (e.g. `Home > Technology > Programming > r/cpp`).
  * **Dynamic Node Insertion**: Users can create a new sub-community attached under any chosen parent node in the N-ary tree.
* **Complexity**: Search $O(V)$, Breadcrumbs $O(H)$, Insertion $O(V) + O(1)$.

### 2. Karma Ranking &rarr; Max Heap (`backend/MaxHeap.hpp`)
* **Concept**: A Complete Binary Tree stored in a contiguous 0-indexed array where each parent's karma is greater than or equal to its children's karma:
  $$\text{Heap}[\text{parent}] \ge \text{Heap}[\text{child}]$$
  * `Parent(i) = (i - 1) / 2`
  * `LeftChild(i) = 2i + 1`
  * `RightChild(i) = 2i + 2`
* **Operations**:
  * **Peek Max**: Immediate $O(1)$ access to the #1 contributor at root `heap[0]`.
  * **Dynamic siftUp / siftDown**: When upvoting/downvoting a post, the author's karma score is dynamically updated and the node sifts up or down in $O(\log N)$ time.
  * **Top-K Leaderboard**: Extracts top contributors in $O(K \log N)$ time.

### 3. Reddit Search &rarr; Hash Table (`backend/HashTable.hpp`)
* **Concept**: An associative data structure mapping keys to values in $O(1)$ expected time.
* **Operations**:
  * **Hash Function**: DJB2 Polynomial Rolling Hash:
    $$\text{hash} = ((\text{hash} \ll 5) + \text{hash}) + \text{char}$$
  * **Collision Resolution**: Separate Chaining with linked lists in each bucket.
  * **Dynamic Rehashing**: Doubles capacity when Load Factor ($\frac{\text{size}}{\text{capacity}}$) exceeds $0.75$.
  * **Inverted Index**: Words extracted from post titles and content are indexed to lists of post IDs (`keyword -> [post_ids]`), providing instant sub-millisecond search across posts.

### 4. Trending Posts &rarr; Heap / Priority Queue (`backend/PriorityQueue.hpp`)
* **Concept**: A Max-Heap prioritizing posts by their real-time trending momentum using Reddit's time-decay algorithm:
  $$\text{Score} = \frac{(\text{Upvotes} \times 2.0) - (\text{Downvotes} \times 1.5) + (\text{Comments} \times 3.0)}{(\text{Post Age in Hours} + 2.0)^{1.3}}$$
* **Operations**:
  * **Floyd's Heapify Algorithm**: Builds the complete priority queue from $N$ posts in linear $O(N)$ time.
  * **Top Trending Carousel**: Extracts the top 4 highest-priority trending posts in $O(K \log N)$ time.

---

## Project Structure

```
reddit-dsa-app/
├── backend/
│   ├── models.hpp         # UserKarma, Post data models, JSON serialization
│   ├── NaryTree.hpp       # Feature 1: Subreddit Hierarchy (N-ary Tree)
│   ├── MaxHeap.hpp        # Feature 2: Karma Ranking (Max Heap)
│   ├── HashTable.hpp      # Feature 3: Reddit Search (Hash Table Inverted Index)
│   ├── PriorityQueue.hpp  # Feature 4: Trending Posts (Heap / Priority Queue)
│   └── server.cpp         # Winsock REST API HTTP server on port 8080
└── frontend/
    ├── src/
    │   ├── components/
    │   │   ├── Header.jsx                # Reddit header & search bar
    │   │   ├── SidebarLeft.jsx           # N-ary Tree community navigation
    │   │   ├── SidebarRight.jsx          # Max Heap karma leaderboard
    │   │   ├── TrendingBar.jsx           # Priority Queue trending cards
    │   │   ├── PostFeed.jsx              # Reddit feed & upvote pillar
    │   │   ├── CreatePostModal.jsx       # Modal to post & index in Hash Table
    │   │   ├── CreateSubredditModal.jsx  # Modal to insert node into N-ary Tree
    │   │   └── DsaInspectorModal.jsx     # Live visualizer for all 4 structures
    │   ├── api.js                        # REST client connecting to C++ backend
    │   ├── App.jsx                       # Master 3-column Reddit layout
    │   ├── App.css                       # Reddit dark UI styling
    │   └── index.css                     # Global reset & CSS variables
    ├── index.html
    └── package.json
```

---

## Running the Application

### 1. Backend (C++)
```powershell
cd C:\Users\HP\.gemini\antigravity\scratch\reddit-dsa-app\backend
g++ -std=c++14 server.cpp -o server.exe -lws2_32
.\server.exe
```
*Listens on `http://localhost:8080`*

### 2. Frontend (React.js)
```powershell
cd C:\Users\HP\.gemini\antigravity\scratch\reddit-dsa-app\frontend
npm run dev
```
*Accessible in browser at `http://localhost:5173`*
