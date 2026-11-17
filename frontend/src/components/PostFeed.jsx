import React, { useState } from 'react';
import { 
  ArrowBigUp, 
  ArrowBigDown, 
  MessageSquare, 
  Share2, 
  Bookmark, 
  Flame, 
  Sparkles, 
  Hash, 
  FolderTree,
  ChevronRight,
  Filter,
  Check
} from 'lucide-react';
import { api } from '../api';

export default function PostFeed({ 
  posts, 
  activeSubreddit, 
  activeFilter, 
  setActiveFilter, 
  onVoteSuccess, 
  onOpenCreatePost,
  searchActiveQuery,
  onClearSearch
}) {
  const [votingMap, setVotingMap] = useState({}); // postId -> 'up' | 'down'
  const [toastMessage, setToastMessage] = useState('');

  // Handle upvote / downvote
  const handleVote = async (post, direction) => {
    try {
      // Toggle if already voted the same direction
      const currentVote = votingMap[post.id];
      if (currentVote === direction) return;

      setVotingMap(prev => ({ ...prev, [post.id]: direction }));

      const res = await api.votePost(post.id, direction);

      const karmaDelta = direction === 'up' ? '+10' : '-10';
      setToastMessage(`Vote applied! Author @${post.author} karma updated (${karmaDelta}) in C++ Max Heap!`);
      setTimeout(() => setToastMessage(''), 3000);

      if (onVoteSuccess) {
        onVoteSuccess(post.id, direction);
      }
    } catch (err) {
      console.error('Failed to vote:', err);
    }
  };

  const formatTimeAgo = (timestamp) => {
    if (!timestamp) return 'recently';
    const now = Math.floor(Date.now() / 1000);
    const diff = Math.max(0, now - timestamp);
    if (diff < 60) return `${diff}s ago`;
    if (diff < 3600) return `${Math.floor(diff / 60)}m ago`;
    if (diff < 86400) return `${Math.floor(diff / 3600)}h ago`;
    return `${Math.floor(diff / 86400)}d ago`;
  };

  return (
    <div className="reddit-feed-column">
      {/* Toast Alert for Live DSA Karma Updates */}
      {toastMessage && (
        <div className="feed-dsa-toast">
          <Sparkles size={16} className="toast-icon" />
          <span>{toastMessage}</span>
        </div>
      )}

      {/* Breadcrumb Bar (Tree Lineage) */}
      <div className="feed-breadcrumbs">
        <span className="breadcrumb-item" onClick={() => onClearSearch && onClearSearch()}>
          Home
        </span>
        <ChevronRight size={14} className="breadcrumb-sep" />
        <span className="breadcrumb-item active">
          r/{activeSubreddit}
        </span>
        {searchActiveQuery && (
          <>
            <ChevronRight size={14} className="breadcrumb-sep" />
            <span className="breadcrumb-item search-tag">
              Search: "{searchActiveQuery}"
            </span>
            <button className="btn-clear-search" onClick={onClearSearch}>✕ Clear</button>
          </>
        )}
      </div>

      {/* Quick Create Post Input Trigger */}
      <div className="quick-create-bar" onClick={onOpenCreatePost}>
        <div className="quick-avatar">🧙‍♂️</div>
        <input 
          type="text" 
          readOnly 
          placeholder="Create a Reddit post (indexed automatically into Hash Table)..." 
          className="quick-input"
        />
        <button className="btn-quick-create">Post</button>
      </div>

      {/* Feed Filter Sort Tabs */}
      <div className="feed-filter-tabs">
        <button 
          className={`filter-tab ${activeFilter === 'trending' ? 'active' : ''}`}
          onClick={() => setActiveFilter('trending')}
          title="Sorted by C++ Heap / Priority Queue"
        >
          <Flame size={16} />
          <span>Trending (Priority Queue)</span>
        </button>

        <button 
          className={`filter-tab ${activeFilter === 'community' ? 'active' : ''}`}
          onClick={() => setActiveFilter('community')}
          title="Filtered by Subreddit N-ary Tree"
        >
          <FolderTree size={16} />
          <span>r/{activeSubreddit} (N-ary Tree)</span>
        </button>

        <button 
          className={`filter-tab ${activeFilter === 'all' ? 'active' : ''}`}
          onClick={() => setActiveFilter('all')}
        >
          <span>All Posts</span>
        </button>
      </div>

      {/* Post List */}
      <div className="feed-posts-container">
        {posts && posts.length > 0 ? (
          posts.map((post) => {
            const userVote = votingMap[post.id];
            const netScore = (post.upvotes - post.downvotes) + 
              (userVote === 'up' ? 1 : userVote === 'down' ? -1 : 0);

            return (
              <article key={post.id} className="reddit-post-card">
                {/* Left Vote Pillar */}
                <div className="post-vote-pillar">
                  <button 
                    className={`vote-btn upvote ${userVote === 'up' ? 'voted' : ''}`}
                    onClick={() => handleVote(post, 'up')}
                    title="Upvote (Awards Author +10 Karma in Max Heap)"
                  >
                    <ArrowBigUp size={24} />
                  </button>

                  <span className={`vote-score ${userVote === 'up' ? 'upvoted' : userVote === 'down' ? 'downvoted' : ''}`}>
                    {netScore}
                  </span>

                  <button 
                    className={`vote-btn downvote ${userVote === 'down' ? 'voted' : ''}`}
                    onClick={() => handleVote(post, 'down')}
                    title="Downvote (Deducts Author 10 Karma in Max Heap)"
                  >
                    <ArrowBigDown size={24} />
                  </button>
                </div>

                {/* Right Post Body */}
                <div className="post-main-content">
                  {/* Post Metadata Header */}
                  <div className="post-meta-row">
                    <span className="post-subreddit-badge">
                      r/{post.subreddit}
                    </span>
                    <span className="meta-separator">•</span>
                    <span className="post-author-link">
                      Posted by u/{post.author}
                    </span>
                    <span className="meta-separator">•</span>
                    <span className="post-timestamp">
                      {formatTimeAgo(post.createdAt)}
                    </span>
                    {post.flair && (
                      <span className="post-flair-tag">{post.flair}</span>
                    )}
                  </div>

                  {/* Post Title */}
                  <h3 className="post-title">{post.title}</h3>

                  {/* Post Content */}
                  <p className="post-body-text">{post.content}</p>

                  {/* Post Footer & DSA Metadata */}
                  <div className="post-footer-row">
                    <div className="action-buttons-group">
                      <button className="post-action-btn">
                        <MessageSquare size={16} />
                        <span>{post.commentsCount} Comments</span>
                      </button>

                      <button className="post-action-btn">
                        <Share2 size={16} />
                        <span>Share</span>
                      </button>

                      <button className="post-action-btn">
                        <Bookmark size={16} />
                        <span>Save</span>
                      </button>
                    </div>

                    {/* DSA Properties Tag */}
                    <div className="post-dsa-tags">
                      <span className="dsa-tag hash-tag" title="Indexed in Hash Table">
                        <Hash size={11} /> ID #{post.id}
                      </span>
                      {post.trendingScore > 0 && (
                        <span className="dsa-tag trending-tag" title="Computed via Priority Queue Formula">
                          <Flame size={11} /> Score: {post.trendingScore.toFixed(0)}
                        </span>
                      )}
                    </div>
                  </div>
                </div>
              </article>
            );
          })
        ) : (
          <div className="feed-empty-state">
            <div className="empty-icon">📭</div>
            <h4>No posts found in this view</h4>
            <p>Try switching to another subreddit in the N-ary tree or clear your search.</p>
          </div>
        )}
      </div>
    </div>
  );
}
