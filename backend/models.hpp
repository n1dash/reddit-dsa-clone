#ifndef MODELS_HPP
#define MODELS_HPP

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <chrono>

// ============================================================================
// DATA MODELS FOR REDDIT APPLICATION
// ============================================================================

// Structure representing a Reddit User and their Karma
struct UserKarma {
    std::string username;
    int karma;             // Overall karma score
    int postKarma;         // Karma earned from posts
    int commentKarma;      // Karma earned from comments
    std::string avatar;    // Avatar URL / identifier
    std::string badge;     // Special community badge

    // Default constructor
    UserKarma() : username(""), karma(0), postKarma(0), commentKarma(0), avatar(""), badge("Member") {}

    // Parameterized constructor
    UserKarma(std::string u, int k, int pk, int ck, std::string av, std::string b = "Member")
        : username(u), karma(k), postKarma(pk), commentKarma(ck), avatar(av), badge(b) {}
};

// Structure representing a Reddit Post
struct Post {
    int id;
    std::string title;
    std::string content;
    std::string author;
    std::string subreddit; // Subreddit name (e.g. "cpp", "technology")
    int upvotes;
    int downvotes;
    int commentsCount;
    long long createdAt;   // Unix timestamp in seconds
    double trendingScore;  // Computed via Priority Queue algorithm
    std::string flair;     // Post flair (e.g. "Discussion", "Tutorial", "News")

    Post() : id(0), title(""), content(""), author(""), subreddit(""),
             upvotes(0), downvotes(0), commentsCount(0), createdAt(0), trendingScore(0.0), flair("General") {}

    Post(int pid, std::string t, std::string c, std::string a, std::string sub,
         int up, int down, int comm, long long created, std::string f = "General")
        : id(pid), title(t), content(c), author(a), subreddit(sub),
          upvotes(up), downvotes(down), commentsCount(comm), createdAt(created), trendingScore(0.0), flair(f) {}
};

// Helper function to safely escape strings for JSON format
inline std::string escapeJsonString(const std::string& input) {
    std::ostringstream ss;
    for (char c : input) {
        switch (c) {
            case '"':  ss << "\\\""; break;
            case '\\': ss << "\\\\"; break;
            case '\b': ss << "\\b";  break;
            case '\f': ss << "\\f";  break;
            case '\n': ss << "\\n";  break;
            case '\r': ss << "\\r";  break;
            case '\t': ss << "\\t";  break;
            default:
                if ('\x00' <= c && c <= '\x1f') {
                    ss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)c;
                } else {
                    ss << c;
                }
        }
    }
    return ss.str();
}

// Helper to convert Post to JSON string
inline std::string postToJson(const Post& post) {
    std::ostringstream ss;
    ss << "{"
       << "\"id\":" << post.id << ","
       << "\"title\":\"" << escapeJsonString(post.title) << "\","
       << "\"content\":\"" << escapeJsonString(post.content) << "\","
       << "\"author\":\"" << escapeJsonString(post.author) << "\","
       << "\"subreddit\":\"" << escapeJsonString(post.subreddit) << "\","
       << "\"upvotes\":" << post.upvotes << ","
       << "\"downvotes\":" << post.downvotes << ","
       << "\"commentsCount\":" << post.commentsCount << ","
       << "\"createdAt\":" << post.createdAt << ","
       << "\"trendingScore\":" << std::fixed << std::setprecision(2) << post.trendingScore << ","
       << "\"flair\":\"" << escapeJsonString(post.flair) << "\""
       << "}";
    return ss.str();
}

// Helper to convert UserKarma to JSON string
inline std::string userKarmaToJson(const UserKarma& user, int rank = 0) {
    std::ostringstream ss;
    ss << "{"
       << "\"rank\":" << rank << ","
       << "\"username\":\"" << escapeJsonString(user.username) << "\","
       << "\"karma\":" << user.karma << ","
       << "\"postKarma\":" << user.postKarma << ","
       << "\"commentKarma\":" << user.commentKarma << ","
       << "\"avatar\":\"" << escapeJsonString(user.avatar) << "\","
       << "\"badge\":\"" << escapeJsonString(user.badge) << "\""
       << "}";
    return ss.str();
}

#endif // MODELS_HPP
