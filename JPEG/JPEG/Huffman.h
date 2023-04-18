
#pragma once

#include <vector>
#include <queue>
#include <unordered_map>
#include <string>

using namespace std;

struct pair_hash
{
    template <class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2>& pair) const {
        return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
    }
};

class AcNode
{
    friend class Huffman;
    friend struct AcNodeLess;
private:
    AcNode()    = default;
    ~AcNode()   = default;
private:
    AcNode*         _leftChild = nullptr;
    AcNode*         _rightChild = nullptr;
    string          _code = "";
    pair<int, int>  _symbol = make_pair(-1, -1);
    int             _frequency = 0;
};

struct AcNodeLess
{
    bool operator()(const AcNode* x, const AcNode* y) { return x->_frequency > y->_frequency; }
};

class DcNode
{
    friend class Huffman;
    friend struct DcNodeLess;
private:
    DcNode() = default;
    ~DcNode() = default;
private:
    DcNode*         _leftChild = nullptr;
    DcNode*         _rightChild = nullptr;
    string          _code = "";
    int             _symbol = INT_MAX;
    int             _frequency = 0;
};

struct DcNodeLess
{
    bool operator()(const DcNode* x, const DcNode* y) { return x->_frequency > y->_frequency; }
};

class Huffman
{
public:
    Huffman() = default;
    ~Huffman();

public:
    void HuffmanCoding(unordered_map<pair<int, int>, int, pair_hash>& ac);
    void HuffmanCoding(unordered_map<int, int>& dc);

public:
    const unordered_map<pair<int, int>, string, pair_hash>&   getAcSymbol() { return _acSymbolCodeMap; }
    const unordered_map<int, string>&                         getDcSymbol() { return _dcLengthCodeMap; }
    const unordered_map<string, pair<int, int>>&              getAcCode()   { return _acCodeSymbolMap; }
    const unordered_map<string, int>&                         getDcLength() { return _dcCodeLengthMap; }

private:
    void UpdateCode(AcNode* node, string code = "");
    void UpdateCode(DcNode* node, string code = "");
    void DeleteNodes(AcNode* node);
    void DeleteNodes(DcNode* node);

private:
    AcNode* _acTree;
    DcNode* _dcTree;

    unordered_map<pair<int, int>, string, pair_hash>   _acSymbolCodeMap;
    unordered_map<string, pair<int, int>>              _acCodeSymbolMap;
    unordered_map<int, string>                         _dcLengthCodeMap;
    unordered_map<string, int>                         _dcCodeLengthMap;
};

