import React from 'react';
import { Flame, MessageSquare, ArrowBigUp, Clock, Sparkles } from 'lucide-react';

export default function TrendingBar({ trendingPosts, onSelectPost }) {
  if (!trendingPosts || trendingPosts.length === 0) {
    return null;
  }

  // Show top 4 trending cards
  const top4 = trendingPosts.slice(0, 4);

  return (
    <div className="trending-bar-container">
      <div className="trending-bar-header">
        <div className="header-badge-group">
          <Flame size={18} className="flame-icon animated" />
          <span className="trending-title">Trending Today</span>
        </div>
        <div className="trending-dsa-indicator" title="Managed by C++ Max-Heap Priority Queue">
          <Sparkles size={12} />
          <span>Priority Queue (Time-Decay Formula)</span>
        </div>
      </div>

      <div className="trending-cards-grid">
        {top4.map((post, idx) => (
          <div 
            key={post.id} 
            className="trending-card"
            onClick={() => onSelectPost && onSelectPost(post)}
          >
            <div className="trending-card-top">
              <span className="trending-rank-pill">
                #{idx + 1} Trending
              </span>
              <span className="trending-sub">r/{post.subreddit}</span>
            </div>

            <h4 className="trending-card-title">{post.title}</h4>

            <div className="trending-card-footer">
              <div className="trending-score-pill">
                <Flame size={13} className="mini-flame" />
                <span>Score: {post.trendingScore ? post.trendingScore.toFixed(0) : '0'}</span>
              </div>
              <div className="trending-stats">
                <span>▲ {post.upvotes}</span>
                <span>💬 {post.commentsCount}</span>
              </div>
            </div>
          </div>
        ))}
      </div>
    </div>
  );
}
