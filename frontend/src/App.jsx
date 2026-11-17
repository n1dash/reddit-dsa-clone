import React, { useState, useEffect } from 'react';
import './App.css';
import Header from './components/Header';
import SidebarLeft from './components/SidebarLeft';
import SidebarRight from './components/SidebarRight';
import TrendingBar from './components/TrendingBar';
import PostFeed from './components/PostFeed';
import CreatePostModal from './components/CreatePostModal';
import CreateSubredditModal from './components/CreateSubredditModal';
import DsaInspectorModal from './components/DsaInspectorModal';
import { api } from './api';

export default function App() {
  const [treeData, setTreeData] = useState(null);
  const [subredditsFlat, setSubredditsFlat] = useState([]);
  const [activeSubreddit, setActiveSubreddit] = useState('all');
  const [activeSubredditInfo, setActiveSubredditInfo] = useState(null);

  const [leaderboard, setLeaderboard] = useState([]);
  const [trendingPosts, setTrendingPosts] = useState([]);
  const [posts, setPosts] = useState([]);

  const [activeFilter, setActiveFilter] = useState('trending'); // 'trending', 'community', 'all'
  const [searchQuery, setSearchQuery] = useState('');
  const [searchActiveQuery, setSearchActiveQuery] = useState('');

  const [dsaStats, setDsaStats] = useState(null);
  const [currentUserKarma, setCurrentUserKarma] = useState(18450);

  const [isCreatePostOpen, setIsCreatePostOpen] = useState(false);
  const [isCreateSubredditOpen, setIsCreateSubredditOpen] = useState(false);
  const [isDsaInspectorOpen, setIsDsaInspectorOpen] = useState(false);

  // Load Tree, Leaderboard, Trending, and Subreddits
  const loadInitialData = async () => {
    try {
      const [tree, flat, board, trending, stats] = await Promise.all([
        api.getSubredditTree(),
        api.getAllSubredditsFlat(),
        api.getKarmaLeaderboard(),
        api.getTrendingPosts(),
        api.getDsaStats()
      ]);

      setTreeData(tree);
      setSubredditsFlat(flat);
      setLeaderboard(board);
      setTrendingPosts(trending);
      setDsaStats(stats);

      if (board && board.length > 0) {
        setCurrentUserKarma(board[0].karma);
      }
    } catch (err) {
      console.error('Error loading initial data from C++ server:', err);
    }
  };

  // Load Posts based on Active Subreddit and Filter
  const loadPosts = async () => {
    try {
      if (activeFilter === 'trending') {
        const trending = await api.getTrendingPosts();
        setTrendingPosts(trending);
        setPosts(trending);
      } else {
        const subPosts = await api.getPosts(activeSubreddit);
        setPosts(subPosts);
      }
    } catch (err) {
      console.error('Error fetching posts:', err);
    }
  };

  useEffect(() => {
    loadInitialData();
  }, []);

  useEffect(() => {
    loadPosts();

    // Update active subreddit info
    if (subredditsFlat.length > 0) {
      const found = subredditsFlat.find(s => s.name === activeSubreddit);
      setActiveSubredditInfo(found || { name: activeSubreddit, title: activeSubreddit, members: 54000000 });
    }
  }, [activeSubreddit, activeFilter]);

  // Handle selecting a subreddit in the N-ary tree
  const handleSelectSubreddit = (name) => {
    setActiveSubreddit(name);
    setSearchActiveQuery('');
    setSearchQuery('');
    if (name === 'all') {
      setActiveFilter('trending');
    } else {
      setActiveFilter('community');
    }
  };

  // Handle vote completion (re-fetches Max Heap leaderboard and Priority Queue)
  const handleVoteSuccess = async () => {
    try {
      const [board, trending, stats] = await Promise.all([
        api.getKarmaLeaderboard(),
        api.getTrendingPosts(),
        api.getDsaStats()
      ]);
      setLeaderboard(board);
      setTrendingPosts(trending);
      setDsaStats(stats);
      if (board && board.length > 0) {
        setCurrentUserKarma(board[0].karma);
      }

      // If active filter is trending, update feed posts
      if (activeFilter === 'trending') {
        setPosts(trending);
      }
    } catch (err) {
      console.error('Error updating after vote:', err);
    }
  };

  // Handle new post created
  const handlePostCreated = async (newPost) => {
    await loadInitialData();
    loadPosts();
  };

  // Handle new subreddit created in N-ary tree
  const handleSubredditCreated = async (newSubName) => {
    const [tree, flat] = await Promise.all([
      api.getSubredditTree(),
      api.getAllSubredditsFlat()
    ]);
    setTreeData(tree);
    setSubredditsFlat(flat);
    handleSelectSubreddit(newSubName);
  };

  // Handle selecting a search result from Hash Table dropdown
  const handleSearchSelect = (post) => {
    setPosts([post]);
    setSearchActiveQuery(post.title);
  };

  const handleClearSearch = () => {
    setSearchActiveQuery('');
    setSearchQuery('');
    loadPosts();
  };

  return (
    <div className="reddit-app-wrapper">
      {/* Reddit Header */}
      <Header
        onSelectSubreddit={handleSelectSubreddit}
        activeSubreddit={activeSubreddit}
        onOpenCreatePost={() => setIsCreatePostOpen(true)}
        onOpenDsaInspector={() => setIsDsaInspectorOpen(true)}
        onSearchSelect={handleSearchSelect}
        searchQuery={searchQuery}
        setSearchQuery={setSearchQuery}
        currentUserKarma={currentUserKarma}
      />

      {/* Main Reddit Layout with 3 Columns: Left Sidebar, Main Feed, Right Sidebar */}
      <div className="reddit-main-layout">
        {/* Left Column: Subreddit Hierarchy (N-ary Tree) */}
        <SidebarLeft
          treeData={treeData}
          activeSubreddit={activeSubreddit}
          onSelectSubreddit={handleSelectSubreddit}
          onOpenCreateSubreddit={() => setIsCreateSubredditOpen(true)}
          treeStats={dsaStats?.naryTree}
        />

        {/* Center Column: Trending Bar + Post Feed */}
        <main className="reddit-center-feed">
          {/* Feature 4: Trending Posts (Heap / Priority Queue) */}
          <TrendingBar
            trendingPosts={trendingPosts}
            onSelectPost={(p) => handleSearchSelect(p)}
          />

          {/* Post Feed with Voting & DSA Tags */}
          <PostFeed
            posts={posts}
            activeSubreddit={activeSubreddit}
            activeFilter={activeFilter}
            setActiveFilter={setActiveFilter}
            onVoteSuccess={handleVoteSuccess}
            onOpenCreatePost={() => setIsCreatePostOpen(true)}
            searchActiveQuery={searchActiveQuery}
            onClearSearch={handleClearSearch}
          />
        </main>

        {/* Right Column: Karma Leaderboard (Max Heap) + Community Info */}
        <SidebarRight
          leaderboard={leaderboard}
          activeSubredditInfo={activeSubredditInfo}
          onVoteRecorded={handleVoteSuccess}
          heapStats={dsaStats?.maxHeap}
        />
      </div>

      {/* Modals */}
      <CreatePostModal
        isOpen={isCreatePostOpen}
        onClose={() => setIsCreatePostOpen(false)}
        onPostCreated={handlePostCreated}
        subredditsList={subredditsFlat}
        activeSubreddit={activeSubreddit}
      />

      <CreateSubredditModal
        isOpen={isCreateSubredditOpen}
        onClose={() => setIsCreateSubredditOpen(false)}
        onSubredditCreated={handleSubredditCreated}
        subredditsList={subredditsFlat}
      />

      <DsaInspectorModal
        isOpen={isDsaInspectorOpen}
        onClose={() => setIsDsaInspectorOpen(false)}
      />
    </div>
  );
}
