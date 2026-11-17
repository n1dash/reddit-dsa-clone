// ============================================================================
// API CLIENT CONNECTING REACT FRONTEND TO C++ BACKEND
// ============================================================================

const API_BASE_URL = 'http://localhost:8080/api';

export const api = {
  // Feature 1: Subreddit Hierarchy (N-ary Tree)
  getSubredditTree: async () => {
    const res = await fetch(`${API_BASE_URL}/subreddits/tree`);
    if (!res.ok) throw new Error('Failed to fetch subreddit tree');
    return res.json();
  },

  getAllSubredditsFlat: async () => {
    const res = await fetch(`${API_BASE_URL}/subreddits/all`);
    if (!res.ok) throw new Error('Failed to fetch flattened subreddits');
    return res.json();
  },

  createSubreddit: async (data) => {
    const res = await fetch(`${API_BASE_URL}/subreddits`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(data),
    });
    if (!res.ok) {
      const err = await res.json().catch(() => ({}));
      throw new Error(err.error || 'Failed to create subreddit');
    }
    return res.json();
  },

  // Feature 2: Karma Ranking (Max Heap)
  getKarmaLeaderboard: async () => {
    const res = await fetch(`${API_BASE_URL}/karma/leaderboard`);
    if (!res.ok) throw new Error('Failed to fetch karma leaderboard');
    return res.json();
  },

  votePost: async (postId, direction) => {
    const res = await fetch(`${API_BASE_URL}/karma/vote`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ postId, direction }),
    });
    if (!res.ok) throw new Error('Failed to register vote');
    return res.json();
  },

  // Feature 3: Reddit Search (Hash Table Inverted Index)
  searchPosts: async (query) => {
    const res = await fetch(`${API_BASE_URL}/search?q=${encodeURIComponent(query)}`);
    if (!res.ok) throw new Error('Failed to perform hash table search');
    return res.json();
  },

  // Feature 4: Trending Posts (Heap / Priority Queue)
  getTrendingPosts: async () => {
    const res = await fetch(`${API_BASE_URL}/posts/trending`);
    if (!res.ok) throw new Error('Failed to fetch trending posts');
    return res.json();
  },

  // General Post Repository
  getPosts: async (subreddit = 'all') => {
    const url = subreddit && subreddit !== 'all' 
      ? `${API_BASE_URL}/posts?subreddit=${encodeURIComponent(subreddit)}`
      : `${API_BASE_URL}/posts`;
    const res = await fetch(url);
    if (!res.ok) throw new Error('Failed to fetch posts');
    return res.json();
  },

  createPost: async (postData) => {
    const res = await fetch(`${API_BASE_URL}/posts`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(postData),
    });
    if (!res.ok) throw new Error('Failed to create post');
    return res.json();
  },

  // Diagnostic Stats for All 4 DSA Structures
  getDsaStats: async () => {
    const res = await fetch(`${API_BASE_URL}/dsa/stats`);
    if (!res.ok) throw new Error('Failed to fetch DSA diagnostics');
    return res.json();
  },
};
