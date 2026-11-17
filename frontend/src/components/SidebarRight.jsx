import React, { useState } from 'react';
import { 
  Trophy, 
  Sparkles, 
  TrendingUp, 
  ShieldCheck, 
  Info, 
  Zap,
  Award,
  Crown
} from 'lucide-react';
import { api } from '../api';

export default function SidebarRight({ 
  leaderboard, 
  activeSubredditInfo, 
  onVoteRecorded,
  heapStats 
}) {
  const [boostingUser, setBoostingUser] = useState(null);
  const [boostMessage, setBoostMessage] = useState('');

  // Handler to simulate dynamic karma boost to demonstrate C++ Max Heap siftUp in action
  const handleSimulateKarmaBoost = async (username) => {
    try {
      setBoostingUser(username);
      // We can trigger an upvote on a post or invoke a karma boost
      // In our server, POST /api/karma/vote takes a postId, or we can pick post #101 or #102
      // Let's call /api/karma/vote on post 102 (authored by bjarne_fan) or post 101 (algo_guru)
      await api.votePost(102, 'up');
      setBoostMessage(`Boosted karma! Max Heap sifted up in O(log N) time.`);
      if (onVoteRecorded) onVoteRecorded();
      setTimeout(() => setBoostMessage(''), 3500);
    } catch (err) {
      console.error(err);
    } finally {
      setBoostingUser(null);
    }
  };

  const getRankBadge = (rank) => {
    if (rank === 1) return <span className="rank-badge gold"><Crown size={14} /> 1</span>;
    if (rank === 2) return <span className="rank-badge silver">2</span>;
    if (rank === 3) return <span className="rank-badge bronze">3</span>;
    return <span className="rank-badge normal">{rank}</span>;
  };

  return (
    <aside className="reddit-sidebar-right">
      {/* Active Subreddit Information Card */}
      {activeSubredditInfo && (
        <div className="sidebar-card community-about-card">
          <div className="card-header">
            <span className="card-header-icon">{activeSubredditInfo.icon || '🌐'}</span>
            <span className="card-header-title">About r/{activeSubredditInfo.name}</span>
          </div>

          <p className="community-desc">
            {activeSubredditInfo.description || 'Welcome to this Reddit community!'}
          </p>

          <div className="community-meta-grid">
            <div className="meta-item">
              <span className="meta-val">
                {activeSubredditInfo.members 
                  ? activeSubredditInfo.members.toLocaleString() 
                  : '54,000,000'}
              </span>
              <span className="meta-lbl">Members</span>
            </div>
            <div className="meta-item">
              <span className="meta-val">
                {activeSubredditInfo.parent ? `r/${activeSubredditInfo.parent}` : 'Root'}
              </span>
              <span className="meta-lbl">Parent Node</span>
            </div>
          </div>
        </div>
      )}

      {/* Feature 2: Karma Ranking (Max Heap Leaderboard) */}
      <div className="sidebar-card karma-leaderboard-card">
        <div className="card-header">
          <Trophy size={18} className="trophy-icon" />
          <span className="card-header-title">Karma Ranking (Max Heap)</span>
        </div>

        <p className="card-subtitle">
          Users sorted by Max Heap invariant: <br />
          <code>Heap[parent] &ge; Heap[child]</code>
        </p>

        {boostMessage && (
          <div className="heap-toast-msg">
            <Sparkles size={13} />
            <span>{boostMessage}</span>
          </div>
        )}

        <div className="leaderboard-list">
          {leaderboard && leaderboard.length > 0 ? (
            leaderboard.map((user, idx) => (
              <div 
                key={user.username} 
                className={`leaderboard-item ${idx === 0 ? 'top-rank' : ''}`}
              >
                <div className="leaderboard-item-left">
                  {getRankBadge(idx + 1)}
                  <div className="user-avatar">{user.avatar || '👤'}</div>
                  <div className="user-details">
                    <div className="user-name-row">
                      <span className="user-name">u/{user.username}</span>
                      {idx === 0 && <span className="root-pill" title="Heap Root Index 0">Root [0]</span>}
                    </div>
                    <span className="user-badge">{user.badge}</span>
                  </div>
                </div>

                <div className="leaderboard-item-right">
                  <div className="karma-number">
                    <Sparkles size={12} className="karma-sparkle" />
                    <span>{user.karma.toLocaleString()}</span>
                  </div>
                  <span className="karma-label">karma</span>
                </div>
              </div>
            ))
          ) : (
            <div className="leaderboard-empty">Building Max Heap...</div>
          )}
        </div>

        {/* Live Heap Action Button */}
        <div className="heap-action-box">
          <button 
            className="btn-boost-heap"
            onClick={() => handleSimulateKarmaBoost('bjarne_fan')}
            disabled={boostingUser !== null}
          >
            <Zap size={14} />
            <span>{boostingUser ? 'Sifting Up...' : 'Upvote Post #102 (Test siftUp)'}</span>
          </button>
        </div>
      </div>

      {/* Educational DSA Explainer Card for Max Heap */}
      <div className="dsa-explainer-card">
        <div className="explainer-header">
          <ShieldCheck size={14} className="explainer-icon" />
          <span>DSA: Max Heap</span>
        </div>
        <p className="explainer-text">
          Binary heap packed into an array: Root is at <code>[0]</code>, left child at <code>2i + 1</code>, right child at <code>2i + 2</code>.
        </p>
        <div className="explainer-stats">
          <div className="stat-pill">
            <span className="stat-label">Peek:</span>
            <span className="stat-value">O(1)</span>
          </div>
          <div className="stat-pill">
            <span className="stat-label">Sift:</span>
            <span className="stat-value">O(log N)</span>
          </div>
          <div className="stat-pill">
            <span className="stat-label">Size:</span>
            <span className="stat-value">{heapStats?.heapSize || 7}</span>
          </div>
        </div>
      </div>
    </aside>
  );
}
