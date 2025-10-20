#include <interface/AutoSuggestions.h>

using namespace std;

AutoSuggestions::AutoSuggestions(/* args */)
{
}

AutoSuggestions::~AutoSuggestions()
{
}

// Alphabet size (# of symbols)
#define ALPHABET_SIZE (90)
 
// Converts key current character into index
// uses ASCII
#define CHAR_TO_INDEX(c) ((int)c - (int)'!')
 
// trie node
struct AutoSuggestions::TrieNode
{
    AutoSuggestions::TrieNode *children[ALPHABET_SIZE];
 
    // isWordEnd is true if the node represents
    // end of a word
    bool isWordEnd;
};
 
// Returns new trie node (initialized to NULLs)
AutoSuggestions::TrieNode *AutoSuggestions::getNode()
{
    AutoSuggestions::TrieNode *pNode = new AutoSuggestions::TrieNode;
    pNode->isWordEnd = false;
 
    for (int i = 0; i < ALPHABET_SIZE; i++)
        pNode->children[i] = NULL;
 
    return pNode;
}
 
// If not present, inserts key into trie.  If the
// key is prefix of trie node, just marks leaf node
void AutoSuggestions::insert(AutoSuggestions::TrieNode *root,  const string key)
{
    AutoSuggestions::TrieNode *pCrawl = root;
 
    for (size_t level = 0; level < key.length(); level++)
    {
        int index = CHAR_TO_INDEX(key[level]);
        if (!pCrawl->children[index])
            pCrawl->children[index] = getNode();
 
        pCrawl = pCrawl->children[index];
    }
 
    // mark last node as leaf
    pCrawl->isWordEnd = true;
}
 

// Recursive function to delete all nodes in the trie
void AutoSuggestions::deleteTrie(TrieNode* node) {
    if (!node) return;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (node->children[i])
            deleteTrie(node->children[i]);
    }
    delete node;
}

// Returns true if key presents in trie, else false
bool AutoSuggestions::search(TrieNode *root, const string key)
{
    int length = key.length();
    AutoSuggestions::TrieNode *pCrawl = root;
    for (int level = 0; level < length; level++)
    {
        int index = CHAR_TO_INDEX(key[level]);
 
        if (!pCrawl->children[index])
            return false;
 
        pCrawl = pCrawl->children[index];
    }
 
    return (pCrawl != NULL && pCrawl->isWordEnd);
}
 
// Returns 0 if current node has a child
// If all children are NULL, return 1.
bool AutoSuggestions::isLastNode(AutoSuggestions::TrieNode* root)
{
    for (int i = 0; i < ALPHABET_SIZE; i++)
        if (root->children[i])
            return 0;
    return 1;
}
 
// Recursive function to print auto-suggestions for given
// node.
void AutoSuggestions::suggestionsRec(AutoSuggestions::TrieNode* root, string currPrefix)
{
    // found a string in Trie with the given prefix
    if (root->isWordEnd)
    {
        cout << currPrefix << "\t";
    }
 
    // All children struct node pointers are NULL
    if (isLastNode(root))
        return;
 
    for (int i = 0; i < ALPHABET_SIZE; i++)
    {
        if (root->children[i])
        {
            // append current character to currPrefix string
            currPrefix.push_back(33 + i);
 
            // recur over the rest
            suggestionsRec(root->children[i], currPrefix);
            // remove last character
            currPrefix.pop_back();
        }
    }
}
 
// print suggestions for given query prefix.
int AutoSuggestions::printAutoSuggestions(AutoSuggestions::TrieNode* root, const string query)
{
    AutoSuggestions::TrieNode* pCrawl = root;
 
    // Check if prefix is present and find the
    // the node (of last level) with last character
    // of given string.
    int level;
    int n = query.length();
    for (level = 0; level < n; level++)
    {
        int index = CHAR_TO_INDEX(query[level]);
 
        // no string in the Trie has this prefix
        if (!pCrawl->children[index])
            return 0;
 
        pCrawl = pCrawl->children[index];
    }
 
    // If prefix is present as a word.
    bool isWord = (pCrawl->isWordEnd == true);
 
    // If prefix is last node of tree (has no
    // children)
    bool isLast = isLastNode(pCrawl);
 
    // If prefix is present as a word, but
    // there is no subtree below the last
    // matching node.
    if (isWord && isLast)
    {
        cout << query << "\t";
        cout << endl;
        return -1;
    }
 
    // If there are are nodes below last
    // matching character.
    if (!isLast)
    {
        string prefix = query;
        suggestionsRec(pCrawl, prefix);
        cout << endl;
        return 1;
    } 

    return -1;
}

