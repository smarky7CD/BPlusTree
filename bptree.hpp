#ifndef __BPTREE_H
#define __BPTREE_H

#include <cstddef>
#include <cmath>
#include <math.h> 
#include <utility>
#include <list>
#include <vector>
#include <iterator>

class Node
{
public:     
    //class member variables
    int m; // order
    bool leaf; // leaf node?
    Node* prev = nullptr; // used for dll (only on leaf nodes)
    Node* next = nullptr; // used for dll
    
    std::vector<int> keys;

    // "dirty" polymorphism
    virtual Node* get_next(int key){}
    virtual Node* get_left(Node* n){}
    virtual Node* get_right(Node* n){}
    virtual double find_val(int key){}
    virtual bool delete_key_val(int key){}
    virtual std::pair<int, Node*> insert(int key, Node* pointer){}
    virtual std::pair<int, Node*> insert(int key, double value){}
    virtual std::pair<int, Node*> split(){}
};

// Internal Node "is a" Node 
class INode : public Node
{
public:
    std::vector<Node*> pointers; // vector of pointers to Nodes
    
    // custom constructor that sets order
    INode(int mi)
    {
        m=mi;
        leaf = false; // not a leaf
    }
    // custom constructor that sets order, keys, and pointers
    INode(int mi, std::vector<int> keysi, std::vector<Node*> pointersi)
    {
        m=mi;
        leaf = false; // not a leaf
        keys=keysi;
        pointers=pointersi;
    }
    // gets the next node in a search path
    Node* get_next(int key); 
    // gets left sibling of an internal node
    Node* get_left(Node* n);
    // gets right sibling of an internal node
    Node* get_right(Node* n);
    // inserts a key and a pointer to internal node
    // internally calls split so return type is same 
    std::pair<int, Node*> insert(int key, Node* pointer);
    // called by inserts
    std::pair<int, Node*> split();
};

// Leaf Node "is a" Node
class LNode: public Node
{
public:
    std::vector<double> values; // vector of values (type is double)

    // custom constructor that sets order
    LNode(int m)
    {
        m=m;
        leaf = true; // is a leaf
    }
    // custom constructor that sets order, keys, and values
    LNode(int mi, std::vector<int> keysi, std::vector<double> valuesi)
    {
        m=mi;
        leaf = true; // is a leaf
        keys=keysi;
        values=valuesi;
    }
    // find the value when a key is searched for
    double find_val(int key);
    // deletes a key and corresponding value
    bool delete_key_val(int key);
    // inserts a key, value pair to internal node
    // internally calls split so return type is same 
    std::pair<int, Node*> insert(int key, double value);
    std::pair<int, Node*> split();
};

// B Plus Tree 
class BPTree
{
public:
    int m; // order m which all nodes in tree also have
    Node *root; // Root node pointer

    // custom constructor that sets order and root to nullptr
    BPTree(int m)
    {
        m = m;
        root = nullptr;
    }
    // insert operation
    void insert(int key, double value);
    // delete helper function for recursing on internal nodes
    void inode_rec_del(INode* parent, Node* del_node, std::vector<Node*> path);
    // delete operate
    void del(int key);
    // single key search operation
    double search(int key);
    // range search operation
    std::vector<double> search(int key1, int key2);
};
#endif
