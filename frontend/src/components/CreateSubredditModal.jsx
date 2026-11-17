import React, { useState } from 'react';
import { X, FolderTree, Plus, Sparkles } from 'lucide-react';
import { api } from '../api';

export default function CreateSubredditModal({ 
  isOpen, 
  onClose, 
  onSubredditCreated, 
  subredditsList 
}) {
  const [name, setName] = useState('');
  const [title, setTitle] = useState('');
  const [description, setDescription] = useState('');
  const [parent, setParent] = useState('technology');
  const [icon, setIcon] = useState('🚀');
  const [isSubmitting, setIsSubmitting] = useState(false);
  const [error, setError] = useState('');

  if (!isOpen) return null;

  const handleSubmit = async (e) => {
    e.preventDefault();
    const cleanName = name.trim().toLowerCase().replace(/[^a-z0-9_]/g, '');
    if (!cleanName || !title.trim()) {
      setError('Please provide a valid subreddit name and title.');
      return;
    }

    try {
      setIsSubmitting(true);
      setError('');
      await api.createSubreddit({
        parent,
        name: cleanName,
        title: title.trim(),
        description: description.trim() || `The official r/${cleanName} community`,
        icon: icon.trim() || '🌐'
      });

      if (onSubredditCreated) {
        onSubredditCreated(cleanName);
      }
      onClose();
    } catch (err) {
      setError(err.message || 'Failed to create subreddit');
    } finally {
      setIsSubmitting(false);
    }
  };

  return (
    <div className="modal-overlay" onClick={onClose}>
      <div className="reddit-modal-box" onClick={(e) => e.stopPropagation()}>
        <div className="modal-header">
          <div className="modal-title-row">
            <FolderTree size={20} className="modal-icon-tree" />
            <h3>Create a Subreddit (N-ary Tree Node)</h3>
          </div>
          <button className="btn-modal-close" onClick={onClose}><X size={18} /></button>
        </div>

        <div className="modal-dsa-callout">
          <Sparkles size={14} className="callout-icon" />
          <span>
            This attaches a new child node to the selected parent in the C++ <strong>N-ary Tree</strong>. The tree updates dynamically!
          </span>
        </div>

        {error && <div className="modal-error-msg">{error}</div>}

        <form onSubmit={handleSubmit} className="modal-form">
          <div className="form-group">
            <label>Select Parent Category / Subreddit in N-ary Tree:</label>
            <select 
              value={parent} 
              onChange={(e) => setParent(e.target.value)}
              className="form-select"
            >
              {subredditsList && subredditsList.map((sub) => (
                <option key={sub.name} value={sub.name}>
                  {'—'.repeat(sub.depth)} r/{sub.name} ({sub.title})
                </option>
              ))}
            </select>
          </div>

          <div className="form-row-duo">
            <div className="form-group">
              <label>Subreddit Key Name (r/):</label>
              <input 
                type="text" 
                className="form-input" 
                placeholder="e.g. rust, machinelearning"
                value={name} 
                onChange={(e) => setName(e.target.value)} 
                required
              />
            </div>

            <div className="form-group">
              <label>Icon / Emoji:</label>
              <input 
                type="text" 
                className="form-input" 
                placeholder="e.g. 🦀, 🤖, 🎮"
                value={icon} 
                onChange={(e) => setIcon(e.target.value)} 
              />
            </div>
          </div>

          <div className="form-group">
            <label>Community Display Title:</label>
            <input 
              type="text" 
              className="form-input" 
              placeholder="e.g. Rust Programming & Systems Architecture"
              value={title} 
              onChange={(e) => setTitle(e.target.value)} 
              required
            />
          </div>

          <div className="form-group">
            <label>Description:</label>
            <textarea 
              rows={3} 
              className="form-textarea" 
              placeholder="What is this community about?"
              value={description} 
              onChange={(e) => setDescription(e.target.value)} 
            />
          </div>

          <div className="modal-actions">
            <button type="button" className="btn-cancel" onClick={onClose}>
              Cancel
            </button>
            <button type="submit" className="btn-submit" disabled={isSubmitting}>
              <Plus size={15} />
              <span>{isSubmitting ? 'Linking...' : 'Add Node to N-ary Tree'}</span>
            </button>
          </div>
        </form>
      </div>
    </div>
  );
}
