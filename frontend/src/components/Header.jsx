import React, { useState, useEffect, useRef } from 'react';
import { 
  Search, 
  Plus, 
  Flame, 
  Binary, 
  Trophy, 
  FolderTree, 
  Hash, 
  Sparkles,
  ExternalLink,
  ChevronDown
} from 'lucide-react';
import { api } from '../api';

export default function Header({ 
  onSelectSubreddit, 
  activeSubreddit, 
  onOpenCreatePost, 
  onOpenDsaInspector,
  onSearchSelect,
  searchQuery,
  setSearchQuery,
  currentUserKarma
}) {
  const [searchResults, setSearchResults] = useState([]);
  const [isSearching, setIsSearching] = useState(false);
  const [showDropdown, setShowDropdown] = useState(false);
  const searchRef = useRef(null);

  // Feature 3: Reddit Search via Hash Table Inverted Index
  useEffect(() => {
    if (!searchQuery.trim()) {
      setSearchResults([]);
      setShowDropdown(false);
      return;
    }

    const timer = setTimeout(async () => {
      try {
        setIsSearching(true);
        const data = await api.searchPosts(searchQuery.trim());
        setSearchResults(data.results || []);
        setShowDropdown(true);
      } catch (err) {
        console.error('Search error:', err);
      } finally {
        setIsSearching(false);
      }
    }, 200);

    return () => clearTimeout(timer);
  }, [searchQuery]);

  // Close dropdown on outside click
  useEffect(() => {
    function handleClickOutside(e) {
      if (searchRef.current && !searchRef.current.contains(e.target)) {
        setShowDropdown(false);
      }
    }
    document.addEventListener('mousedown', handleClickOutside);
    return () => document.removeEventListener('mousedown', handleClickOutside);
  }, []);

  return (
    <header className="reddit-header">
      {/* Left: Reddit Brand Logo & Subreddit badge */}
      <div className="header-left">
        <div className="reddit-logo" onClick={() => onSelectSubreddit('all')}>
          <div className="snoo-circle">
            <svg viewBox="0 0 20 20" className="snoo-svg" fill="currentColor">
              <circle cx="10" cy="10" r="10" fill="#FF4500" />
              {/* Snoo face silhouette */}
              <circle cx="7" cy="9.5" r="1.5" fill="#FFFFFF" />
              <circle cx="13" cy="9.5" r="1.5" fill="#FFFFFF" />
              <path d="M7 13.5 Q10 16 13 13.5" stroke="#FFFFFF" strokeWidth="1.4" fill="none" strokeLinecap="round" />
              <circle cx="15.5" cy="4" r="1.2" fill="#FFFFFF" />
              <path d="M10 6 L14 4" stroke="#FFFFFF" strokeWidth="1.2" strokeLinecap="round" />
            </svg>
          </div>
          <span className="reddit-title">
            reddit<span className="dsa-pill">DSA</span>
          </span>
        </div>

        {/* Current Subreddit Pill */}
        <div className="current-sub-indicator" onClick={() => onSelectSubreddit('all')}>
          <span className="community-icon">r/</span>
          <span className="community-name">{activeSubreddit || 'all'}</span>
          <ChevronDown size={14} className="chevron-icon" />
        </div>
      </div>

      {/* Center: Reddit Search Bar (Powered by Hash Table) */}
      <div className="header-search-container" ref={searchRef}>
        <div className="search-bar-wrapper">
          <Search size={18} className="search-icon" />
          <input
            type="text"
            className="reddit-search-input"
            placeholder="Search Reddit (Powered by Hash Table O(1) Inverted Index)..."
            value={searchQuery}
            onChange={(e) => setSearchQuery(e.target.value)}
            onFocus={() => {
              if (searchResults.length > 0) setShowDropdown(true);
            }}
          />
          <div className="search-dsa-badge" title="Hash Table separate chaining lookup">
            <Hash size={12} />
            <span>O(1) Hash</span>
          </div>
        </div>

        {/* Search Results Dropdown */}
        {showDropdown && (
          <div className="search-dropdown-menu">
            <div className="dropdown-header">
              <span>Hash Table Matches for "{searchQuery}"</span>
              <span className="dropdown-badge">{searchResults.length} posts found</span>
            </div>

            {isSearching ? (
              <div className="dropdown-loading">Looking up hash buckets...</div>
            ) : searchResults.length === 0 ? (
              <div className="dropdown-empty">No posts match this keyword in the hash table.</div>
            ) : (
              <div className="dropdown-list">
                {searchResults.map((post) => (
                  <div 
                    key={post.id} 
                    className="dropdown-item"
                    onClick={() => {
                      onSearchSelect(post);
                      setShowDropdown(false);
                    }}
                  >
                    <div className="dropdown-item-header">
                      <span className="item-sub">r/{post.subreddit}</span>
                      <span className="item-author">u/{post.author}</span>
                      <span className="item-score">▲ {post.upvotes}</span>
                    </div>
                    <div className="dropdown-item-title">{post.title}</div>
                  </div>
                ))}
              </div>
            )}
          </div>
        )}
      </div>

      {/* Right: Actions, Karma Display, DSA Inspector */}
      <div className="header-right">
        {/* Create Post Button */}
        <button 
          className="btn-create-post" 
          onClick={onOpenCreatePost}
          title="Create a new post"
        >
          <Plus size={16} />
          <span>Create</span>
        </button>

        {/* DSA Live Inspector Button */}
        <button 
          className="btn-dsa-inspector" 
          onClick={onOpenDsaInspector}
          title="Open DSA Visualizer & Diagnostics"
        >
          <Binary size={16} />
          <span>DSA Inspector</span>
        </button>

        {/* User Karma Badge */}
        <div className="user-karma-badge" title="Your Karma (Managed by Max Heap)">
          <div className="user-avatar-mini">🧙‍♂️</div>
          <div className="user-karma-info">
            <span className="user-handle">u/satoshi_dev</span>
            <span className="user-karma-val">
              <Sparkles size={11} className="karma-icon" />
              {currentUserKarma ? currentUserKarma.toLocaleString() : '18,450'} karma
            </span>
          </div>
        </div>
      </div>
    </header>
  );
}
