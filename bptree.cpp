#include "bptree.hpp"

// Internal Node functionality

// gets the next node in the search path
Node* INode::get_next(int key)
{
    // finds proper pointer
    std::size_t idx = keys.size(); // right most pointer by default
    int comp;
    for(std::size_t i=0; i < idx; i++)
    {
        comp = keys[i];
        // search key has to be less than current compare key
        // if so assign key to left pointer of compare key 
        if(key < comp){
            idx = i;
        }
    }

    return pointers[idx];
}

// gets left sibling of a node with internal node as parent
Node* INode::get_left(Node* n)
{
    std::size_t idx = pointers.size(); // default value
    for(std::size_t i=0; i<idx; i++){
        // find matching pointer
        if(n == pointers[i]){
            idx = i;
        }
    }

    // return left pointer if n is not leftmost
    if(idx != 0)
    {
        return pointers[idx-1];
    // else no left sibling with common parent so return nullptr
    }else{
        return nullptr;
    }
}

// gets right sibling of a node with internal node as parent
Node* INode::get_right(Node* n)
{
    std::size_t idx = pointers.size(); // default value
    for(std::size_t i=0; i<idx; i++){
        // find matching pointer
        if(n == pointers[i]){
            idx = i;
        }
    }

    // return right pointer if n is not rightmost
    if(idx != pointers.size()-1)
    {
        return pointers[idx+1];
    // else no right sibling with common parent so return nullptr
    }else{
        return nullptr;
    }
}

// insert a key and new right pointer into internal node
std::pair<int, Node*>  INode::insert(int key, Node* pointer)
{
    std::size_t idx = keys.size(); // default value
    int comp;
    // find proper place for key
    for(std::size_t i=0; i < idx; i++)
    {
        comp = keys[i];
        if(key < comp){
            idx = i;
        }
    }

    //insert new key
    keys.insert(keys.begin()+idx, key);
    // invariant that pointer will always be inserted to right of newly inserted key
    pointers.insert(pointers.begin()+idx+1, pointer);

    // split node if necessary (maximum order violated)
    if (keys.size() > m-1)
    {
        return this->split();
    }
    // else return default pair 
    // signals that no split occurred
    return std::make_pair(-1, nullptr);
}

// splits an internal node
std::pair<int, Node*> INode::split()
{
    // in place splitting
    std::size_t num_pops = keys.size() - std::floor(keys.size()/2); // split in half size
    std::vector<int> new_keys; // new right node keys
    std::vector<Node*> new_pointers; // new right node pointers
    // populate new right node and remove split keys and split pointers from original (left) node (new left node)
    for(size_t i=0; i < num_pops; i++){
        new_keys.insert(keys.begin(), keys.back());
        new_pointers.insert(pointers.begin(), pointers.back());
        keys.pop_back();
        pointers.pop_back();
    }

    // create return value 
    int popped = new_keys.front();
    new_keys.erase(new_keys.begin());

    // return pair that is reference key and new right node
    return std::make_pair(popped, new INode(m, new_keys, new_pointers));
}

// Leaf Node functionality

// finds a value during search
double LNode::find_val(int key)
{
    // find correct key index if it exists
    std::size_t idx = -1; //default value
    for(std::size_t i=0; i < keys.size(); i++){   
        if(keys[i] == key){
            idx = i;
        }
    }

    // return nan if search fails
    if(idx == -1){
        return std::nan("");
    // else return the associated value
    }else{
        return values[idx];
    }

}

// deletes a key and its associated value
// returns deficient?
bool LNode::delete_key_val(int key)
{    
    std::size_t idx = -1; // default value
    // find correct key index if it exists
    for(std::size_t i=0; i < keys.size(); i++){   
        if(keys[i] == key){
            idx = i;
        }
    }

    // delete key and value if key exists
    if(idx != -1){
        keys.erase(keys.begin()+idx);
        values.erase(values.begin()+idx);
    }

    // return true if node is now deficient
    if(keys.size() >= std::ceil(m/2) - 1){
        return false;
    }else{
        return true;
    }
}

// insets a key, value pair into a leaf node
std::pair<int, Node*> LNode::insert(int key, double value)
{
    std::size_t idx = keys.size(); // default value
    bool duplicate = false;  // make sure that a duplicate is not trying to be inserted
    int comp;
    // find correct key index and check duplicate
    for(std::size_t i=0; i < idx; i++){
        comp = keys[i];
        // set duplicate flag if duplicate
        if(key == comp){ 
            duplicate = true; 
        }
        // find correct key insert position
        if(key < comp){
            idx = i;
        }
    }

    // if duplicate return default value
    if(duplicate == true){
      return std::make_pair(-1, nullptr);  
    }

    // insert key, value pair into leaf node at correct position
    keys.insert(keys.begin()+idx,key);
    values.insert(values.begin()+idx,value);

    // split node if necessary (maximum order violated)
    if (keys.size() > m-1)
    {
        return this->split();
    }
    
    // else return default pair 
    // signals that no split occurred
    return std::make_pair(-1, nullptr);
}

// splits a leaf node
std::pair<int, Node*> LNode::split()
{
    // in place splitting
    std::size_t num_pops = keys.size() - std::floor(keys.size()/2); // split in half size
    std::vector<int> new_keys; // new right node keys
    std::vector<double> new_values; // new right node values
    // populate new right node and remove split keys and split pointers from original node (new left node)
    for(size_t i=0; i < num_pops; i++){
        new_keys.insert(keys.begin(), keys.back());
        new_values.insert(values.begin(), values.back());
        keys.pop_back();
        values.pop_back();
    }

    // create return value and return said value
    // return value is a pair that is the reference key and new right node
    return std::make_pair(new_keys.front(), new LNode(m, new_keys, new_values));
}


// B+ Tree functionality

// insert operation
void BPTree::insert(int key, double value)
{
    // if tree is empty create new leaf root with first inserted value
    if(root == nullptr){
        std::vector<int> first_key = {key};
        std::vector<double> first_value = {value};
        root = new LNode(m,first_key,first_value);
    // if root is leaf (special case)
    }else if(root->leaf == true){
        // do leaf insert on root
        std::pair<int, Node*> insert_result = root->insert(key, value);
        // new root if necessary (if split occurs)
        if(insert_result.second != nullptr){
            // new root key is reference key
            std::vector<int> root_key = {insert_result.first};
            // root left child is original root, right child is split node
            std::vector<Node*> root_pointers = {root,insert_result.second};
            // update dll of leaf nodes
            root->next = insert_result.second;
            insert_result.second->prev = root;
            // new root creation
            root = new INode(m,root_key,root_pointers);
        }
    }else{
        // search for insertion node (leaf)
        std::vector<Node*> path; // save the path along the way
        Node* curr = root;
        while(curr->leaf == false){
            path.push_back(curr);
            curr = curr->get_next(key);
        }

        // perform insertion on proper leaf node (we get this in above search)
        std::pair<int, Node*> insert_result = curr->insert(key, value);

        // update dll of leaf nodes
        Node* parent = curr;
        if(insert_result.second != nullptr){
            insert_result.second->next = parent->next;
            insert_result.second->prev = parent;
            parent->next = insert_result.second;
        }

        bool new_root = false; // new root flag
        // recuse up tree splitting as necessary until order is not violated or we create new root
        while(insert_result.second != nullptr || !new_root){ 
            // create new root
            if(path.empty()){
                new_root = true;
                std::vector<int> root_key = {insert_result.first};
                std::vector<Node*> root_pointers = {parent,insert_result.second};
                root = new INode(m,root_key,root_pointers);
            }else{
                // perform inserts up the search path while splitting is necessary
                Node* parent = path.back();
                std::pair<int, Node*> insert_result = parent->insert(insert_result.first,insert_result.second);
                path.pop_back();
            }   
        }   
    }

    return;
}

// delete helper function to handle restructuring internal nodes
void BPTree::inode_rec_del(INode* r, Node* del_node, std::vector<Node*> path)
{
    // find index of pointer to delete from r node (node we are working on)
    std::size_t idx = r->pointers.size();
    for(std::size_t i =0; i < idx; i++){
        if(r->pointers[i] == del_node){
            idx = i;
        }
    }

    // index of key to delete from r node
    std::size_t kidx = idx - 1;
    // delete proper r node key and right pointer of r node
    r->keys.erase(r->keys.begin()+kidx);
    r->pointers.erase(r->pointers.begin()+idx);

    // base case 1
    // check if r is an empty root
    if(r == root && r->keys.size() == 0){
        root = r->pointers[0]; // update root node to single child of deficient root
        delete r; // delete old root and return 
        return; 
    }

    // base case 2
    // check if r is deficient; if not we are done 
    if(r->keys.size() >= std::ceil(m/2) - 1){
        return;
    }

    // fun part!
    // handle deficiencies

    // get parent and left and right siblings of r nodes
    // these are guaranteed to be internal nodes
    INode* parent = dynamic_cast<INode*>(path.back());
    INode* left = dynamic_cast<INode*>(parent->get_left(r));
    INode* right = dynamic_cast<INode*>(parent->get_right(r));

    // try to borrow from left sibling
    if(left != nullptr && left->keys.size() >= std::ceil(m/2)){
        // find correct key index to pull from parent to r node
        std::size_t idx = parent->pointers.size();
        for(std::size_t i =0; i < idx; i++){
            if(parent->pointers[i] == left){
            idx = i;
            }
        }
        // get pulled keys
        int l_tr_key = left->keys.back();
        int p_tr_key = parent->keys[idx];

        // rearrange the keys and pointers properly
        parent->keys[idx] = l_tr_key;
        r->keys.insert(r->keys.begin(), p_tr_key);
        r->pointers.insert(r->pointers.begin(), left->pointers.back());
        // delete borrowed key and pointer
        left->keys.pop_back();
        left->pointers.pop_back();
        return; // we are done
    // else try to borrow from right sibling
    }else if (right != nullptr && right->keys.size() >= std::ceil(m/2)){
        // find key index to pull from parent to r node
        std::size_t idx = parent->pointers.size();
        for(std::size_t i =0; i < idx; i++){
            if(parent->pointers[i] == left){
            idx = i;
            }
        }
        // get pulled keys
        int r_tr_key = right->keys.front();
        int p_tr_key = parent->keys[idx-1];

        //rearrange the keys and pointers properly
        parent->keys[idx-1] = r_tr_key;
        r->keys.push_back(p_tr_key);
        r->pointers.push_back(right->pointers.front());
        // delete borrowed key and pointer
        right->keys.erase(r->keys.begin());
        right->pointers.erase(r->pointers.begin());
        return; // we are done
    // if we can't borrow we need to further restructure up the tree
    // try to merge with left sibling
    }else if(left != nullptr){
        // find key index to pull from parent to left sibling
        std::size_t idx = parent->pointers.size();
        for(std::size_t i =0; i < idx; i++){
            if(parent->pointers[i] == left){
            idx = i;
            }
        }

        // get pulled key
        int p_tr_key = parent->keys[idx];

        // place pulled key in left node and collapse r node into left node
        left->keys.push_back(p_tr_key);
        left->keys.insert(left->keys.end(),r->keys.begin(),r->keys.end());
        left->pointers.insert(left->pointers.end(),r->pointers.begin(), r->pointers.end());
        // delete parent from path
        path.pop_back();
        // recurse up the tree
        return inode_rec_del(parent, r, path);
    }else{ // must have right sibling so merge with right sibling
        // find key index to pull from parent to r node
        std::size_t idx = parent->pointers.size();
        for(std::size_t i =0; i < idx; i++){
            if(parent->pointers[i] == left){
            idx = i;
            }
        }

        // get pulled key
        int p_tr_key = parent->keys[idx-1];
        
        // place pulled key in r node and collapse right node into r node
        r->keys.push_back(p_tr_key);
        r->keys.insert(r->keys.end(), right->keys.begin(), right->keys.end());
        r->pointers.insert(r->pointers.end(), right->pointers.begin() ,right->pointers.end());  
        // delete parent from path
        path.pop_back();
        //recurse up the tree
        return inode_rec_del(parent, right, path);
    }
    return; // default return value
}

// delete operation
void BPTree::del(int key)
{
    // if tree is empty do nothing
    if(root == nullptr){
        return;
    // if tree contains only a root (i.e. root is leaf) just delete the key,value pair from the root
    // no side effects if key does not exist
    }else if(root->leaf == true){
        bool throwaway = root->delete_key_val(key);
    // the complex situations
    }else{
        // search for leaf node to delete at; save path along the way 
        std::vector<Node*> path;
        Node* curr = root;
        while(curr->leaf == false){
            path.push_back(curr);
            curr = curr->get_next(key);
        }

        // delete key and value from leaf node    
        bool def = curr->delete_key_val(key);
        // if underflow does not occur or no such key exists we are done
        if(def == false){
            return;
        }

        // else re-balancing is necessary!

        // get parent of current leaf node and left + right sibling pointers
        // see if we can get away with just borrowing from siblings
        INode* parent = dynamic_cast<INode*>(path.back());
        LNode* n = dynamic_cast<LNode*>(curr);
        LNode* left = dynamic_cast<LNode*>(parent->get_left(n));
        LNode* right = dynamic_cast<LNode*>(parent->get_right(n));

        // check if we can borrow a key,value from left sibling
        if(left != nullptr && left->keys.size() >= std::ceil(m/2)){
            n->keys.insert(n->keys.begin(), left->keys.back()); // borrow key
            n->values.insert(n->values.begin(), left->values.back()); // borrow value
            left->keys.pop_back(); // delete borrowed key from left sibling
            left->values.pop_back(); // delete borrowed value from left sibling
            parent->keys[0] = n->keys.front(); // push proper key up to parent
            return; // we are done
        // else check if we can borrow a key, value from right sibling
        }else if (right != nullptr && right->keys.size() >= std::ceil(m/2))
        {
            n->keys.insert(n->keys.end(), right->keys.front()); // borrow key
            n->values.insert(n->values.end(), right->values.front()); // borrow value
            right->keys.erase(right->keys.begin()); // delete borrowed key from right sibling
            right->values.erase(right->values.begin()); // delete borrow key from right sibling
            parent->keys[0] = n->keys.back(); // push proper key up to parent
            return;
        // if this does not work we need to restructure up the tree
        // try to collapse into left sibling
        }else if(left != nullptr){
            // collapse node we deleted from into left sibling
            left->keys.insert(left->keys.end(),n->keys.begin(),n->keys.end());
            left->values.insert(left->values.end(),n->values.begin(), n->values.end()); 
            // update leaf dll pointer
            left->next = right;
            right->prev = left;
            // remove parent from path
            path.pop_back();
            // recursive delete on internal nodes
            return inode_rec_del(parent, n, path);
        }else{ // must have right sibling to collapse
            // collapse right sibling into node we deleted from
            n->keys.insert(n->keys.end(), right->keys.begin(), right->keys.end());
            n->values.insert(n->values.end(), right->values.begin() ,right->values.end()); 
            // update leaf dll pointer
            n->next = right->next;
            right->next->prev = n;
            // remove parent from path
            path.pop_back();
            //recursive delete on internal nodes
            return inode_rec_del(parent, right, path);
        }
        
    }
    // default void return 
    return;
}

// single key search operation
double BPTree::search(int key)
{

    Node* curr = root;

    // if tree is empty return nan
    if(curr == nullptr){
        return std::nan("");
    }

    // search down tree until we hit a leaf
    while(curr->leaf == false){
        curr = curr->get_next(key);
    }
    
    // get the value from the leaf node
    double val = curr->find_val(key);

    // return the value
    return val;

}

// range search operation
std::vector<double> BPTree::search(int key1, int key2)
{
    // result vector
    std::vector<double> result;
    
    // make sure the range search makes sense else return empty vector
    if(key1 > key2){
        return result;
    }

    Node* curr = root;

    // if tree is empty return empty vector
    if(curr == nullptr){
        return result;
    }

    // search down tree until we hit a leaf
    while(curr->leaf == false){
        curr = curr->get_next(key1);
    }

    // cast hit leaf node to leaf node so we can use some LNode methods
    LNode* l = dynamic_cast<LNode*>(curr);
    bool done = false; // done flag
    while(done != true){
        // push values the fall in range into result vector from current leaf nodes
        for(std::size_t i=0; i<l->keys.size(); i++){
            if (l->keys[i] >= key1 && l->keys[i] <= key2){
                result.push_back(l->values[i]);
            }
        }
        // if current leaf nodes largest key is greater then upper bound of range set done flag to true
        if(l->keys[l->keys.size()-1] > key2)
        {
            done = true;
        }
        
        // see if there exists more leaf nodes to examine
        // if there are set l to the next leaf node
        if(l->next != nullptr){
            LNode* l = dynamic_cast<LNode*>(l->next);
        // else set done flag to true
        }else{
            done = true;
        }
    }

    return result; // return result vector
}
