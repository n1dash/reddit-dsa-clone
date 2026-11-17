import React, { useState } from 'react';
import { 
  FolderTree, 
  ChevronRight, 
  ChevronDown, 
  Plus, 
  Layers, 
  Compass, 
  Sparkles,
  Users
} from 'lucide-react';

// Recursive Tree Node Item for N-ary Tree rendering
function SubredditTreeNode({ 
  node, 
  activeSubreddit, 
  onSelectSubreddit, 
  depth = 0 
}) {
  const [isExpanded, setIsExpanded] = useState(true);
  const hasChildren = node.children && node.children.length > 0;
  const isSelected = activeSubreddit === node.name;

  return (
    <div className="tree-node-wrapper">
      <div 
        className={`tree-node-row ${isSelected ? 'active' : ''}`}
        style={{ paddingLeft: `${12 + depth * 16}px` }}
        onClick={() => onSelectSubreddit(node.name)}
      >
        {/* Toggle chevron for nodes with children */}
        {hasChildren ? (
          <button 
            className="tree-toggle-btn" 
            onClick={(e) => {
              e.stopPropagation();
              setIsExpanded(!isExpanded);
            }}
          >
            {isExpanded ? <ChevronDown size={14} /> : <ChevronRight size={14} />}
          </button>
        ) : (
          <span className="tree-leaf-bullet">•</span>
        )}

        {/* Node Icon */}
        <span className="node-icon">{node.icon || '💬'}</span>

        {/* Subreddit Name & Info */}
        <div className="node-info">
          <span className="node-name">r/{node.name}</span>
          {depth > 0 && (
            <span className="node-members">
              {node.members > 1000000 
                ? `${(node.members / 1000000).toFixed(1)}M` 
                : `${(node.members / 1000).toFixed(0)}k`}
            </span>
          )}
        </div>
      </div>

      {/* Render N Children recursively */}
      {hasChildren && isExpanded && (
        <div className="tree-children-container">
          {node.children.map((child) => (
            <SubredditTreeNode
              key={child.name}
              node={child}
              activeSubreddit={activeSubreddit}
              onSelectSubreddit={onSelectSubreddit}
              depth={depth + 1}
            />
          ))}
        </div>
      )}
    </div>
  );
}

export default function SidebarLeft({ 
  treeData, 
  activeSubreddit, 
  onSelectSubreddit, 
  onOpenCreateSubreddit,
  treeStats
}) {
  return (
    <aside className="reddit-sidebar-left">
      {/* Sidebar Header & Navigation */}
      <div className="sidebar-section">
        <div className="sidebar-header-row">
          <div className="section-title">
            <Compass size={16} className="section-icon" />
            <span>FEEDS</span>
          </div>
        </div>

        <div className="sidebar-menu-list">
          <button 
            className={`menu-item ${activeSubreddit === 'all' ? 'active' : ''}`}
            onClick={() => onSelectSubreddit('all')}
          >
            <span className="menu-icon">🔥</span>
            <span className="menu-label">r/all (Home Feed)</span>
          </button>
        </div>
      </div>

      {/* Feature 1: Subreddit Hierarchy (N-ary Tree) */}
      <div className="sidebar-section">
        <div className="sidebar-header-row">
          <div className="section-title">
            <FolderTree size={16} className="section-icon" />
            <span>COMMUNITIES (N-ARY TREE)</span>
          </div>

          {/* Button to add new Subreddit node to N-ary tree */}
          <button 
            className="btn-add-community"
            onClick={onOpenCreateSubreddit}
            title="Add a new Subreddit Node to the N-ary Tree"
          >
            <Plus size={14} />
          </button>
        </div>

        {/* N-ary Tree Content */}
        <div className="tree-view-wrapper">
          {treeData ? (
            <SubredditTreeNode
              node={treeData}
              activeSubreddit={activeSubreddit}
              onSelectSubreddit={onSelectSubreddit}
              depth={0}
            />
          ) : (
            <div className="tree-loading">Loading N-ary Tree...</div>
          )}
        </div>
      </div>

      {/* Educational DSA Explainer Card */}
      <div className="dsa-explainer-card">
        <div className="explainer-header">
          <Layers size={14} className="explainer-icon" />
          <span>DSA: N-ary Tree</span>
        </div>
        <p className="explainer-text">
          Subreddits naturally form a multi-way tree where each parent category has an arbitrary number of children.
        </p>
        <div className="explainer-stats">
          <div className="stat-pill">
            <span className="stat-label">Nodes:</span>
            <span className="stat-value">{treeStats?.totalNodes || 13}</span>
          </div>
          <div className="stat-pill">
            <span className="stat-label">Height:</span>
            <span className="stat-value">{treeStats?.treeHeight || 4}</span>
          </div>
        </div>
      </div>
    </aside>
  );
}
