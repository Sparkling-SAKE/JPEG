
#pragma once

#include <vector>
#include <queue>
#include <unordered_map>
#include <memory>
#include <string>

#include "Types.h"

using namespace std;

struct pair_hash
{
    template <class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2>& pair) const {
        return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
    }
};

/************************************************/

//class Node
//{
//    friend class Huffman;
//private:
//    Node() = default;
//    virtual ~Node() = default;
//
//private:
//    shared_ptr<Node> _leftChild         = nullptr;
//    shared_ptr<Node> _rightChild        = nullptr;
//    string          _code               = "";
//    int32             _frequency          = 0;
//};
//
//template <typename T>
//struct NodeCompare
//{
//    bool operator()(const shared_ptr<T>& x, const shared_ptr<T>& y) { return x.get()->_frequency > y->_frequency; }
//};
//
//class AcNode : public Node
//{
//    friend class NodeCompare;
//private:
//    pair<int32, int32>  _symbol = make_pair(-1, -1);
//};
//
//class DcNode : public Node
//{
//    friend class NodeCompare;
//private:
//    int32            _symbol = int32_MAX;
//};


/************************************************/

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
    pair<int32, int32>  _symbol = make_pair(-1, -1);
    int32             _frequency = 0;
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
    int32             _symbol = INT32_MAX;
    int32             _frequency = 0;
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
    void HuffmanCoding(unordered_map<pair<int32, int32>, int32, pair_hash>& ac);
    void HuffmanCoding(unordered_map<int32, int32>& dc);

public:
    const unordered_map<pair<int32, int32>, string, pair_hash>&   getAcSymbol() { return _acSymbolCodeMap; }
    const unordered_map<int32, string>&                         getDcSymbol() { return _dcLengthCodeMap; }
    const unordered_map<string, pair<int32, int32>>&              getAcCode()   { return _acCodeSymbolMap; }
    const unordered_map<string, int32>&                         getDcLength() { return _dcCodeLengthMap; }

private:
    void UpdateCode(AcNode* node, string code = "");
    void UpdateCode(DcNode* node, string code = "");
    void DeleteNodes(AcNode* node);
    void DeleteNodes(DcNode* node);

private:
    AcNode* _acTree;
    DcNode* _dcTree;

    unordered_map<pair<int32, int32>, string, pair_hash>   _acSymbolCodeMap;
    unordered_map<string, pair<int32, int32>>              _acCodeSymbolMap;
    unordered_map<int32, string>                         _dcLengthCodeMap;
    unordered_map<string, int32>                         _dcCodeLengthMap;
};

