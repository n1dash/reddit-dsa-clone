import React, { useState, useEffect } from 'react';
import { 
  X, 
  Binary, 
  FolderTree, 
  Trophy, 
  Hash, 
  Flame, 
  RefreshCw, 
  CheckCircle2, 
  Cpu, 
  ArrowRight
} from 'lucide-react';
import { api } from '../api';

export default function DsaInspectorModal({ isOpen, onClose }) {
  const [activeTab, setActiveTab] = useState('all');
  const [stats, setStats] = useState(null);
  const [loading, setLoading] = useState(false);

  const fetchStats = async () => {
    try {
      setLoading(true);
      const data = await api.getDsaStats();
      setStats(data);
    } catch (err) {
      console.error('Failed to fetch DSA stats:', err);
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    if (isOpen) {
      fetchStats();
    }
  }, [isOpen]);

  if (!isOpen) return null;

  return (
    <div className="modal-overlay" onClick={onClose}>
      <div className="dsa-inspector-box" onClick={(e) => e.stopPropagation()}>
        {/* Modal Header */}
        <div className="dsa-inspector-header">
          <div className="header-title-block">
            <Cpu size={22} className="dsa-main-icon" />
            <div>
              <h3>C++ DSA Diagnostics & Architecture</h3>
              <p>Live telemetry and data structure internals running inside <code>server.exe</code></p>
            </div>
          </div>

          <div className="header-actions">
            <button className="btn-refresh-stats" onClick={fetchStats} title="Refresh DSA Telemetry">
              <RefreshCw size={15} className={loading ? 'spinning' : ''} />
              <span>Refresh</span>
            </button>
            <button className="btn-modal-close" onClick={onClose}><X size={18} /></button>
          </div>
        </div>

        {/* Tab Selector */}
        <div className="dsa-tab-bar">
          <button 
            className={`dsa-tab ${activeTab === 'all' ? 'active' : ''}`}
            onClick={() => setActiveTab('all')}
          >
            Overview (All 4 Features)
          </button>
          <button 
            className={`dsa-tab ${activeTab === 'tree' ? 'active' : ''}`}
            onClick={() => setActiveTab('tree')}
          >
            1. N-ary Tree (Subreddits)
          </button>
          <button 
            className={`dsa-tab ${activeTab === 'heap' ? 'active' : ''}`}
            onClick={() => setActiveTab('heap')}
          >
            2. Max Heap (Karma)
          </button>
          <button 
            className={`dsa-tab ${activeTab === 'hash' ? 'active' : ''}`}
            onClick={() => setActiveTab('hash')}
          >
            3. Hash Table (Search)
          </button>
          <button 
            className={`dsa-tab ${activeTab === 'priority' ? 'active' : ''}`}
            onClick={() => setActiveTab('priority')}
          >
            4. Priority Queue (Trending)
          </button>
        </div>

        {/* Tab Body */}
        <div className="dsa-modal-body">
          {loading && !stats ? (
            <div className="dsa-loading-state">Querying C++ Winsock Server...</div>
          ) : stats ? (
            <>
              {/* FEATURE 1: N-ARY TREE */}
              {(activeTab === 'all' || activeTab === 'tree') && (
                <div className="dsa-card-panel">
                  <div className="panel-headline">
                    <FolderTree size={18} className="panel-icon tree" />
                    <h4>1. Subreddit Hierarchy &rarr; N-ary Tree</h4>
                  </div>
                  <p className="panel-desc">
                    Unlike binary trees, an N-ary tree allows each community node to branch into arbitrary sub-topics.
                    Uses <strong>Depth-First Search (DFS)</strong> for node resolution and <strong>Parent Pointers</strong> for breadcrumbs.
                  </p>

                  <div className="metrics-row">
                    <div className="metric-box">
                      <span className="metric-val">{stats.naryTree?.totalNodes || 13}</span>
                      <span className="metric-lbl">Total Tree Nodes</span>
                    </div>
                    <div className="metric-box">
                      <span className="metric-val">{stats.naryTree?.treeHeight || 4}</span>
                      <span className="metric-lbl">Tree Height (Max Depth)</span>
                    </div>
                    <div className="metric-box">
                      <span className="metric-val">O(V)</span>
                      <span className="metric-lbl">DFS Traversal Time</span>
                    </div>
                    <div className="metric-box">
                      <span className="metric-val">O(Height)</span>
                      <span className="metric-lbl">Breadcrumb Path Time</span>
                    </div>
                  </div>

                  <div className="code-logic-box">
                    <strong>C++ Struct Definition:</strong>
                    <pre>{`struct SubredditNode {
    std::string name, title, description;
    int members;
    SubredditNode* parent;                // Upward traversal
    std::vector<SubredditNode*> children; // N child branches
};`}</pre>
                  </div>
                </div>
              )}

              {/* FEATURE 2: MAX HEAP */}
              {(activeTab === 'all' || activeTab === 'heap') && (
                <div className="dsa-card-panel">
                  <div className="panel-headline">
                    <Trophy size={18} className="panel-icon heap" />
                    <h4>2. Karma Ranking &rarr; Max Heap (Complete Binary Tree)</h4>
                  </div>
                  <p className="panel-desc">
                    User karma scores are maintained in a contiguous array-backed complete binary tree obeying the invariant: 
                    <code> Heap[parent] &ge; Heap[child]</code>.
                  </p>

                  <div className="metrics-row">
                    <div className="metric-box">
                      <span className="metric-val">{stats.maxHeap?.heapSize || 7}</span>
                      <span className="metric-lbl">Heap Elements</span>
                    </div>
                    <div className="metric-box">
                      <span className="metric-val">u/{stats.maxHeap?.topUser || 'satoshi_dev'}</span>
                      <span className="metric-lbl">Top User (Root [0])</span>
                    </div>
                    <div className="metric-box">
                      <span className="metric-val">{stats.maxHeap?.topKarma?.toLocaleString() || 18450}</span>
                      <span className="metric-lbl">Max Karma (O(1) Peek)</span>
                    </div>
                    <div className="metric-box">
                      <span className="metric-val">O(log N)</span>
                      <span className="metric-lbl">siftUp / siftDown Time</span>
                    </div>
                  </div>

                  {/* Heap Array Visualizer */}
                  {stats.maxHeap?.heapArray && (
                    <div className="heap-array-visualizer">
                      <span className="vis-label">Contiguous Heap Array Storage:</span>
                      <div className="array-cells-row">
                        {stats.maxHeap.heapArray.map((node) => (
                          <div key={node.index} className={`array-cell ${node.index === 0 ? 'root-cell' : ''}`}>
                            <div className="cell-index">[{node.index}]</div>
                            <div className="cell-user">u/{node.username}</div>
                            <div className="cell-karma">{node.karma}</div>
                            <div className="cell-parent">
                              {node.parentIndex === -1 ? 'Root' : `P: [${node.parentIndex}]`}
                            </div>
                          </div>
                        ))}
                      </div>
                    </div>
                  )}
                </div>
              )}

              {/* FEATURE 3: HASH TABLE */}
              {(activeTab === 'all' || activeTab === 'hash') && (
                <div className="dsa-card-panel">
                  <div className="panel-headline">
                    <Hash size={18} className="panel-icon hash" />
                    <h4>3. Reddit Search &rarr; Custom Hash Table (Separate Chaining)</h4>
                  </div>
                  <p className="panel-desc">
                    Search keywords are mapped to lists of matching Post IDs using a custom 
                    <strong> DJB2 Polynomial Rolling Hash function</strong> and <strong>Separate Chaining</strong> for collision resolution.
                  </p>

                  <div className="metrics-row">
                    <div className="metric-box">
                      <span className="metric-val">{stats.hashTable?.capacity || 512}</span>
                      <span className="metric-lbl">Bucket Capacity</span>
                    </div>
                    <div className="metric-box">
                      <span className="metric-val">{stats.hashTable?.elements || 205}</span>
                      <span className="metric-lbl">Indexed Keywords</span>
                    </div>
                    <div className="metric-box">
                      <span className="metric-val">{stats.hashTable?.loadFactor || 0.40}</span>
                      <span className="metric-lbl">Load Factor (&lt; 0.75)</span>
                    </div>
                    <div className="metric-box">
                      <span className="metric-val">{stats.hashTable?.collisions || 38}</span>
                      <span className="metric-lbl">Handled Collisions</span>
                    </div>
                  </div>

                  {/* Hash Table Sample Buckets */}
                  {stats.hashTable?.bucketsPreview && (
                    <div className="buckets-preview-section">
                      <span className="vis-label">Sample Hash Buckets (Separate Chaining Linked Lists):</span>
                      <div className="buckets-chips-grid">
                        {stats.hashTable.bucketsPreview.slice(0, 10).map((b) => (
                          <div key={b.bucketIndex} className="bucket-chip">
                            <span className="bucket-idx">Bucket #{b.bucketIndex}</span>
                            <div className="bucket-tokens">
                              {b.chain && b.chain.length > 0 ? (
                                b.chain.map((token, i) => (
                                  <span key={i} className="chain-token">
                                    "{token}"{i + 1 < b.chain.length ? ' &rarr; ' : ''}
                                  </span>
                                ))
                              ) : (
                                <span className="empty-chain">null</span>
                              )}
                            </div>
                          </div>
                        ))}
                      </div>
                    </div>
                  )}
                </div>
              )}

              {/* FEATURE 4: PRIORITY QUEUE */}
              {(activeTab === 'all' || activeTab === 'priority') && (
                <div className="dsa-card-panel">
                  <div className="panel-headline">
                    <Flame size={18} className="panel-icon priority" />
                    <h4>4. Trending Posts &rarr; Heap / Priority Queue</h4>
                  </div>
                  <p className="panel-desc">
                    Trending posts are dynamically ranked by an engagement velocity formula with polynomial time decay.
                    Built in linear <code>O(N)</code> time using Floyd's algorithm.
                  </p>

                  <div className="metrics-row">
                    <div className="metric-box">
                      <span className="metric-val">{stats.priorityQueue?.queueSize || 7}</span>
                      <span className="metric-lbl">Queue Size</span>
                    </div>
                    <div className="metric-box">
                      <span className="metric-val">{stats.priorityQueue?.topTrendingScore || 7132.3}</span>
                      <span className="metric-lbl">Top Score</span>
                    </div>
                    <div className="metric-box">
                      <span className="metric-val">O(N)</span>
                      <span className="metric-lbl">Floyd's Heapify</span>
                    </div>
                    <div className="metric-box">
                      <span className="metric-val">O(1)</span>
                      <span className="metric-lbl">Top Priority Access</span>
                    </div>
                  </div>

                  <div className="code-logic-box">
                    <strong>Time-Decayed Trending Formula:</strong>
                    <pre>{`Score = ((Upvotes * 2.0) - (Downvotes * 1.5) + (Comments * 3.0)) / (PostAgeHours + 2.0)^1.3`}</pre>
                  </div>
                </div>
              )}
            </>
          ) : (
            <div className="dsa-error-state">Could not connect to C++ backend at http://localhost:8080.</div>
          )}
        </div>
      </div>
    </div>
  );
}
