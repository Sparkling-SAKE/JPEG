
#include "Huffman.h"

Huffman::~Huffman()
{
    DeleteNodes(_acTree);
    DeleteNodes(_dcTree);
}

void Huffman::UpdateCode(AcNode* node, string code)
{
    if (node == nullptr)
        return;
    node->_code = code;
    if (node->_symbol != make_pair(-1, -1))
    {
        if (code == "")
        {
            _acSymbolCodeMap[node->_symbol] = "0";
            _acCodeSymbolMap["0"] = node->_symbol;
        }
        else
        {
            _acSymbolCodeMap[node->_symbol] = code;
            _acCodeSymbolMap[code] = node->_symbol;
        }
    }

    UpdateCode(node->_leftChild, code + "0");
    UpdateCode(node->_rightChild, code + "1");
}

void Huffman::UpdateCode(DcNode* node, string code)
{
    if (node == nullptr)
        return;
    node->_code = code;
    if (node->_symbol != INT32_MAX)
    {
        if (code == "")
        {
            _dcLengthCodeMap[node->_symbol] = "0";
            _dcCodeLengthMap["0"] = node->_symbol;
        }
        else
        {
            _dcLengthCodeMap[node->_symbol] = code;
            _dcCodeLengthMap[code] = node->_symbol;
        }
    }
    UpdateCode(node->_leftChild, code + "0");
    UpdateCode(node->_rightChild, code + "1");
}

void Huffman::DeleteNodes(AcNode* node)
{
    if (node == nullptr)
        return;
    AcNode* left = node->_leftChild;
    AcNode* right = node->_rightChild;
    if (node)
    {
        delete node;
        node = nullptr;
    }
    DeleteNodes(left);
    DeleteNodes(right);
}

void Huffman::DeleteNodes(DcNode* node)
{
    if (node == nullptr)
        return;
    DcNode* left = node->_leftChild;
    DcNode* right = node->_rightChild;
    if (node)
    {
        delete node;
        node = nullptr;
    }
    DeleteNodes(left);
    DeleteNodes(right);
}

void Huffman::HuffmanCoding(unordered_map<pair<int32, int32>, int32, pair_hash>& ac)
{
    priority_queue<AcNode*, vector<AcNode*>, AcNodeLess> acPriorityQueue;
    for (auto iter = ac.begin(); iter != ac.end(); ++iter)
    {
        AcNode* node = new AcNode;
        node->_frequency = iter->second;
        node->_symbol = iter->first;
        acPriorityQueue.push(node);
    }

    while (acPriorityQueue.size() != 1)
    {
        AcNode* minNode1 = acPriorityQueue.top();
        acPriorityQueue.pop();
        AcNode* minNode2 = acPriorityQueue.top();
        acPriorityQueue.pop();
        AcNode* newNode = new AcNode;
        newNode->_leftChild = minNode1;
        newNode->_rightChild = minNode2;
        newNode->_frequency = minNode1->_frequency + minNode2->_frequency;

        acPriorityQueue.push(newNode);
    }
    _acTree = acPriorityQueue.top();
    acPriorityQueue.pop();

    UpdateCode(_acTree, "");
}

void Huffman::HuffmanCoding(unordered_map<int32, int32>& dc)
{
    priority_queue<DcNode*, vector<DcNode*>, DcNodeLess> dcPriorityQueue;
    for (auto iter = dc.begin(); iter != dc.end(); ++iter)
    {
        DcNode* node = new DcNode;
        node->_frequency = iter->second;
        node->_symbol = iter->first;
        dcPriorityQueue.push(node);
    }

    while (dcPriorityQueue.size() != 1)
    {
        DcNode* minNode1 = dcPriorityQueue.top();
        dcPriorityQueue.pop();
        DcNode* minNode2 = dcPriorityQueue.top();
        dcPriorityQueue.pop();
        DcNode* newNode = new DcNode;
        newNode->_leftChild = minNode1;
        newNode->_rightChild = minNode2;
        newNode->_frequency = minNode1->_frequency + minNode2->_frequency;

        dcPriorityQueue.push(newNode);
    }
    _dcTree = dcPriorityQueue.top();
    dcPriorityQueue.pop();

    UpdateCode(_dcTree, "");
}
