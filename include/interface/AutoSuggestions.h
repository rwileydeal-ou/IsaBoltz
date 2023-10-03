#include <iostream>

class AutoSuggestions
{
private:
public:
    struct TrieNode;
    struct TrieNode *getNode(void);
    void insert(struct TrieNode *root,  const std::string key);
    bool search(struct TrieNode *root, const std::string key);
    bool isLastNode(struct TrieNode* root);
    void suggestionsRec(struct TrieNode* root, std::string currPrefix);
    int printAutoSuggestions(TrieNode* root, const std::string query);
    AutoSuggestions(/* args */);
    ~AutoSuggestions();
};
