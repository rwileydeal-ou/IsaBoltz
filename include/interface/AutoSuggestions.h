#include <iostream>

class AutoSuggestions
{
private:
public:
    struct TrieNode;
    TrieNode *getNode();
    void insert(TrieNode *root,  const std::string key);
    void deleteTrie(TrieNode* node);
    bool search(TrieNode *root, const std::string key);
    bool isLastNode(TrieNode* root);
    void suggestionsRec(TrieNode* root, std::string currPrefix);
    int printAutoSuggestions(TrieNode* root, const std::string query);
    AutoSuggestions(/* args */);
    ~AutoSuggestions();
};
