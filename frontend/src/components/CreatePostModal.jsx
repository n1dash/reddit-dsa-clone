import React, { useState } from 'react';
import { X, Hash, Sparkles, Send } from 'lucide-react';
import { api } from '../api';

export default function CreatePostModal({ 
  isOpen, 
  onClose, 
  onPostCreated, 
  subredditsList,
  activeSubreddit 
}) {
  const [title, setTitle] = useState('');
  const [content, setContent] = useState('');
  const [subreddit, setSubreddit] = useState(activeSubreddit || 'programming');
  const [flair, setFlair] = useState('Discussion');
  const [author, setAuthor] = useState('curious_redditor');
  const [isSubmitting, setIsSubmitting] = useState(false);
  const [error, setError] = useState('');

  if (!isOpen) return null;

  const handleSubmit = async (e) => {
    e.preventDefault();
    if (!title.trim() || !content.trim()) {
      setError('Please provide both title and content.');
      return;
    }

    try {
      setIsSubmitting(true);
      setError('');
      const newPost = await api.createPost({
        title: title.trim(),
        content: content.trim(),
        subreddit,
        flair,
        author: author.trim() || 'curious_redditor'
      });

      if (onPostCreated) {
        onPostCreated(newPost);
      }
      onClose();
    } catch (err) {
      setError(err.message || 'Failed to create post');
    } finally {
      setIsSubmitting(false);
    }
  };

  return (
    <div className="modal-overlay" onClick={onClose}>
      <div className="reddit-modal-box" onClick={(e) => e.stopPropagation()}>
        <div className="modal-header">
          <div className="modal-title-row">
            <span className="modal-icon">✍️</span>
            <h3>Create a Post</h3>
          </div>
          <button className="btn-modal-close" onClick={onClose}><X size={18} /></button>
        </div>

        {/* DSA Info Banner */}
        <div className="modal-dsa-callout">
          <Hash size={14} className="callout-icon" />
          <span>
            Upon posting, your text is tokenized into keywords and indexed in the C++ <strong>Hash Table</strong> in O(1) time and added to the <strong>Trending Priority Queue</strong>!
          </span>
        </div>

        {error && <div className="modal-error-msg">{error}</div>}

        <form onSubmit={handleSubmit} className="modal-form">
          <div className="form-row-duo">
            <div className="form-group">
              <label>Choose Subreddit (from N-ary Tree):</label>
              <select 
                value={subreddit} 
                onChange={(e) => setSubreddit(e.target.value)}
                className="form-select"
              >
                {subredditsList && subredditsList.length > 0 ? (
                  subredditsList.map((sub) => (
                    <option key={sub.name} value={sub.name}>
                      {'—'.repeat(sub.depth)} r/{sub.name} ({sub.title})
                    </option>
                  ))
                ) : (
                  <option value="programming">r/programming</option>
                )}
              </select>
            </div>

            <div className="form-group">
              <label>Flair:</label>
              <select 
                value={flair} 
                onChange={(e) => setFlair(e.target.value)}
                className="form-select"
              >
                <option value="Discussion">Discussion</option>
                <option value="Showcase">Showcase</option>
                <option value="Technical">Technical</option>
                <option value="Tutorial">Tutorial</option>
                <option value="News">News</option>
              </select>
            </div>
          </div>

          <div className="form-group">
            <label>Author Username:</label>
            <input 
              type="text" 
              className="form-input" 
              placeholder="e.g. cpp_coder"
              value={author} 
              onChange={(e) => setAuthor(e.target.value)} 
            />
          </div>

          <div className="form-group">
            <label>Title:</label>
            <input 
              type="text" 
              className="form-input" 
              placeholder="An interesting title about C++, DSA, or tech..."
              value={title} 
              onChange={(e) => setTitle(e.target.value)} 
              required
            />
          </div>

          <div className="form-group">
            <label>Post Content:</label>
            <textarea 
              rows={4} 
              className="form-textarea" 
              placeholder="Write your post here. Words used here will be searchable via the Hash Table Inverted Index..."
              value={content} 
              onChange={(e) => setContent(e.target.value)} 
              required
            />
          </div>

          <div className="modal-actions">
            <button type="button" className="btn-cancel" onClick={onClose}>
              Cancel
            </button>
            <button type="submit" className="btn-submit" disabled={isSubmitting}>
              <Send size={15} />
              <span>{isSubmitting ? 'Indexing...' : 'Post to Reddit'}</span>
            </button>
          </div>
        </form>
      </div>
    </div>
  );
}
