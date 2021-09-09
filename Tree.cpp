#include <vector>
#include <list>
#include <iterator>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>

using namespace std;


// https://www.youtube.com/watch?v=kjBI0rimo-w&t=1401s&ab_channel=Mayopepeweezy
// https://www.cs.usfca.edu/~galles/visualization/BPlusTree.html
struct Node
{

    /* data */
    vector<int> keys;
    vector< void*> children;
    bool leaf;
    int fullSize;
    // if it is leave, point to iterator in double linkedlist 
    // if it is inner node, point  to the next node. 
    /*
    |*|*|*|      3 keys
    /  \ \ \     4 children
      */
    Node(int n)
    {
        fullSize=n;
        leaf=true;    // when it is created it is true;
    }

    bool isFull(){
        return (keys.size()>=fullSize);
    }

    int getNumKeys(){
        return keys.size();
    }
    int getNumValues(){
        return children.size();
    }

    void addToKeys(int key, int index){
        keys.insert(keys.begin() + index , key);
    }

    void addToValues(void * value, int index){
        children.insert(children.begin() + index , value);
    }

    void setLeafOrNot(bool leaf){
        this->leaf = leaf;
    }

    void printAllKeys(){
        cout << "|";
        for (auto key : keys){
            cout << key << "|";
        }
        cout << "\t";
    }

};


class bTree
{
    private: Node* _root;
    int degree;
    int totalNode;
    public:
        bTree(int n)
        {
            _root=NULL;
            degree=n;
            totalNode=0;
        }

        Node * getRoot()
        {
            return _root;
        }

        /*vector<void*> getbyIndex(int i){
            // to some search and return vector of pointer
        }
        */

       void insertToBTree(int key, void * value){
            //Case 1: Empty tree 
            if (_root == NULL){
                _root = new Node(degree);
                //add 1 key and 1 child to the new root node
                _root->keys.push_back(key);
                _root->children.push_back(value);
                return;
            }
            //For all other cases, find the leaf node we are inserting into 
            Node * nodeToInsertInto = findLeafNodeToInsert(key);
            //starting from this leaf node
            while (true){ 
                
                //Case 2: Current node is not full
                if (!nodeToInsertInto->isFull()){
                    int keyIndexToInsertAt = findKeyIndexToInsert(nodeToInsertInto, key);
                    nodeToInsertInto->addToKeys(key, keyIndexToInsertAt);
                    int valueIndexToInsertAt = findValueIndexToInsert(nodeToInsertInto, key);
                    nodeToInsertInto->addToValues(value, valueIndexToInsertAt);
                    break;
                } else {
                    //Case 3: A leaf node is full;
                    if (nodeToInsertInto->leaf){
                        //Create new leaf node 
                        Node * newLeafNode = new Node(degree);
                        //temporarily insert into the old node (this node will overflow)
                        int keyIndexToInsertAt = findKeyIndexToInsert(nodeToInsertInto, key);
                        nodeToInsertInto->addToKeys(key, keyIndexToInsertAt);
                        int valueIndexToInsertAt = findValueIndexToInsert(nodeToInsertInto, key);
                        nodeToInsertInto->addToValues(value, valueIndexToInsertAt);
                        //get index to split (everything at & after the index will be shifted to new node)
                        //eg. 1 2 [3 4] (degree=3) then return index=2
                        //eg. 1 2 [3 4 5] (degree=4) then return index=2
                        //eg. 1 2 3 [4 5 6] (degree=5) then return index=3
                        int indexToSplit = floor((degree+1)/2);
                        //eg. 1 2 [3 4] -> remove 3 add to new node
                        //              -> remove 4 add to new node
                        //Populate the new leaf node with [3, 4] and remove [3, 4] from old leaf node
                        newLeafNode->keys.insert(newLeafNode->keys.begin(),nodeToInsertInto->keys.begin()+indexToSplit, nodeToInsertInto->keys.end());
                        nodeToInsertInto->keys.erase(nodeToInsertInto->keys.begin()+indexToSplit, nodeToInsertInto->keys.begin()+nodeToInsertInto->keys.size());   
                        newLeafNode->children.insert(newLeafNode->children.begin(),nodeToInsertInto->children.begin()+indexToSplit, nodeToInsertInto->children.end());
                        nodeToInsertInto->children.erase(nodeToInsertInto->children.begin()+indexToSplit, nodeToInsertInto->children.begin()+nodeToInsertInto->keys.size());   

                        // //add the new leaf node's pointer to the end of old leaf node
                        // //[1 2] -> [3 4]
                        nodeToInsertInto->children.push_back(newLeafNode);
                        key = newLeafNode->keys[0]; //this will be the smallest
                        value = newLeafNode;
                        Node * parentNode = getParent(nodeToInsertInto); //get this leaf node's parent
  
                        //this is already root node
                        if (parentNode == NULL) {
                            parentNode = new Node(degree);
                            parentNode->setLeafOrNot(false); //no longer leaf
                            parentNode->children.push_back(nodeToInsertInto);
                            nodeToInsertInto = parentNode;
                            _root = parentNode;
                        } else {
                            nodeToInsertInto = parentNode;
                        }
                    } 
                    //Case 4: A non-leaf node is full;
                    else { 
                        //Create new nonleaf node 
                        Node * newNonLeafNode = new Node(degree);
                        newNonLeafNode->setLeafOrNot(false);
                        //temporarily insert into the old node (this node will overflow)
                        int keyIndexToInsertAt = findKeyIndexToInsert(nodeToInsertInto, key);
                        nodeToInsertInto->addToKeys(key, keyIndexToInsertAt);
                        int valueIndexToInsertAt = findValueIndexToInsert(nodeToInsertInto, key);
                        nodeToInsertInto->addToValues(value, valueIndexToInsertAt);
                        //get index to split (everything at & after the index will be shifted to new node)
                        //eg. 1 2 [3 4] (degree=3) then return index=2
                        //eg. 1 2 [3 4 5] (degree=4) then return index=2
                        //eg. 1 2 3 [4 5 6] (degree=5) then return index=3
                        int indexToSplit = floor((degree+1)/2);
                        //eg. 1 2 [3 4] -> remove 3 add to new node
                        //              -> remove 4 add to new node
                        //Populate the new leaf node with [3, 4] and remove [3, 4] from old leaf node
                        newNonLeafNode->keys.insert(newNonLeafNode->keys.begin(),nodeToInsertInto->keys.begin()+indexToSplit, nodeToInsertInto->keys.end());
                        nodeToInsertInto->keys.erase(nodeToInsertInto->keys.begin()+indexToSplit, nodeToInsertInto->keys.begin()+nodeToInsertInto->keys.size());   
                        newNonLeafNode->children.insert(newNonLeafNode->children.begin(),nodeToInsertInto->children.begin()+indexToSplit+1, nodeToInsertInto->children.end());
                        nodeToInsertInto->children.erase(nodeToInsertInto->children.begin()+indexToSplit+1, nodeToInsertInto->children.end());   

                        // // //add the new leaf node's pointer to the end of old leaf node
                        // // //[1 2] -> [3 4]
                        
                        key = newNonLeafNode->keys[0]; //this will be the smallest
                        newNonLeafNode->keys.erase(newNonLeafNode->keys.begin(), newNonLeafNode->keys.begin()+1);
                        value = newNonLeafNode;
                        Node * parentNode = getParent(nodeToInsertInto); //get this leaf node's parent
                        //this is already root node
                        if (parentNode == NULL) {
                            parentNode = new Node(degree);
                            parentNode->setLeafOrNot(false); //no longer leaf
                            parentNode->children.push_back(nodeToInsertInto);
                            nodeToInsertInto = parentNode;
                            _root = parentNode;
                            Node * temp =(Node *) value;
                        } else {
                            nodeToInsertInto = parentNode;

                        }

                    }

                }
            }
       }

        Node * getParent(Node * node){
            //return null if this is already the root node
            Node * current_node = _root;
            if (node == _root){
                return NULL;
            } 
            else {
                while (true){
                    //if the current node has this node as a child
                    for (void * value : current_node->children){
                        if (value == node){
                            return current_node;
                        }
                    }
                    //if the current node does not contain this node as a child
                    int smallestKey = current_node->keys[0];
                    int  valueIndex = findKeyIndexToInsert(current_node,smallestKey);
                    current_node = (Node *)current_node->children[valueIndex];
                }
            }
        }

        //find index in keys of node to insert into
        int findKeyIndexToInsert(Node * nodeToInsert, int key){
            //compare the key with the keys of the current node
            for (int i=0;i<nodeToInsert->keys.size();i++){
                //imagine keys 2, 3, 5
                //insert 4
                if (nodeToInsert->keys[i] > key){
                    return i;
                }
            }
            //if the key is bigger than all the keys in current node
            //imagine keys 2, 3, 4
            //insert 5
            return (nodeToInsert->getNumKeys());
        }

        //find index in values of node to insert into
        int findValueIndexToInsert(Node * nodeToInsert, int key){
            //compare the key with the keys of the current node
            for (int i=0;i<nodeToInsert->keys.size();i++){
                //imagine keys 2, 3, 5
                //insert 4
                if (nodeToInsert->keys[i] > key){
                    return i;
                }
            }
            //if the key is bigger than all the keys in current node
            //imagine keys 2, 3, 4
            //insert 5
            return (nodeToInsert->getNumValues());
        }

        Node * findLeafNodeToInsert(int key){
            //traverse the tree downwards until we find the leaf node
                Node * current_node = _root;
                //if it is not a leaf
                
                while (!current_node->leaf){
                    //compare the key with the keys of the current node
                    for (int i=0;i<current_node->keys.size();i++){
                        //imagine keys 2, 3, 5
                        //insert 4
                        if (current_node->keys[i] > key){
                            //go to the left of the key
                            current_node = (Node *)current_node->children[i]; 
                            break;
                        }
                        //if the key is bigger than all the keys in current node
                        //imagine keys 2, 3, 4
                        //insert 5
                        if (i==current_node->keys.size()-1){
                            current_node = (Node *) current_node->children[current_node->getNumValues()-1];
                            break;
                        }
                    }
                }
                return current_node;
        }

        //bfs print from node
        void printNodeTree(){
            if (_root == NULL){
                cout << "No nodes yet";
                return;
            }
            list<Node *> parent_queue;
            list<Node *> child_queue;
            parent_queue.push_back(_root);
            _root->printAllKeys();
            cout << "\n";
            //bfs
            while (true){
                Node * parent_node = (Node * )parent_queue.front();
                parent_queue.pop_front();
                if (parent_node->leaf){ //stop once we reach the leaf
                    break;
                }
                //for all children in parent node
                for (int j=0;j<parent_node->getNumValues();j++){
                    Node * child_node = (Node * ) parent_node->children[j];
                    child_queue.push_back(child_node);
                    child_node->printAllKeys();
                }
                if (parent_queue.size()==0){
                    parent_queue = child_queue;
                    child_queue = {};
                    cout << "\n";
                }
                
                
            }
        }

        //To check
        void printLastRow(){
            Node * node = (Node *) findLeafNodeToInsert(-1);
            while (node != NULL){
                node->printAllKeys();
                node = (Node *) node->children[node->getNumValues()-1];
            }
        }

        ~bTree(){
            delete _root;
        }
};




// int main()
// {
//     int a=1;
//     int b=2;
//     // int c=3;
//     bTree tree=bTree(3);

//     // tree.insertToBTree(1,&a);
//     // tree.printNodeTree();
//     // tree.insertToBTree(2,&b);
//     // tree.printNodeTree();
//     // tree.insertToBTree(3,&b);
//     // tree.printNodeTree();
//     // tree.insertToBTree(4,&b);
//     // tree.printNodeTree();

//     // tree.insertToBTree(5,&b);
//     // tree.printNodeTree();
//     // tree.insertToBTree(6,&b);
//     // tree.printNodeTree();
//     // tree.insertToBTree(7,&b);
//     // tree.printNodeTree();
//     // tree.insertToBTree(8,&b);
//     // tree.printNodeTree();
//     // tree.insertToBTree(9,&b);
//     // tree.printNodeTree();

//     // tree.insertToBTree(10,&b);
//     // tree.printNodeTree();
//     // tree.insertToBTree(11,&b);
//     // tree.printNodeTree();
//     // tree.insertToBTree(12,&b);
//     // tree.printNodeTree();
//     // tree.insertToBTree(13,&b);
//     // tree.printNodeTree();
//     // tree.insertToBTree(14,&b);
//     // tree.printNodeTree();

//     // tree.insertToBTree(15,&b);
//     // tree.printNodeTree();
//     // tree.insertToBTree(16,&b);
//     // tree.printNodeTree();
//     // tree.insertToBTree(16,&a);
//     // tree.printNodeTree();   
//     // tree.insertToBTree(15,&a);
//     // tree.printNodeTree();
//     // tree.insertToBTree(14,&a);
//     // tree.printNodeTree();
//     // tree.insertToBTree(13,&a);
//     // tree.printNodeTree();
//     // tree.insertToBTree(12,&a);
//     // tree.printNodeTree();
//     // tree.insertToBTree(11,&a);
//     // tree.printNodeTree();
//     // tree.insertToBTree(10,&a);
//     // tree.printNodeTree();
//     // tree.insertToBTree(9,&b);
//     // tree.printNodeTree();
//     // tree.insertToBTree(8,&b);
//     // tree.printNodeTree();
//     // tree.insertToBTree(7,&b);
//     // tree.printNodeTree();

//     // tree.insertToBTree(6,&b);
//     // tree.printNodeTree();
//     // tree.insertToBTree(5,&b);
//     // tree.printNodeTree(); //<-
//     // tree.insertToBTree(4,&b);
//     // tree.printNodeTree();
//     // tree.insertToBTree(3,&b);
//     // tree.printNodeTree();
//     // tree.insertToBTree(2,&b);
//     // tree.printNodeTree();

//     // tree.insertToBTree(1,&b);
//     // tree.printNodeTree();
    
//     // vector<int> intvec = {1, 2, 3, 4};
//     // vector<int> newvec;
//     // newvec.insert(newvec.begin(),intvec.begin()+2, intvec.begin()+3);
//     // intvec.erase(intvec.begin()+2, intvec.begin()+intvec.size());    
//     // for (auto i : intvec){
//     //     cout << i<< "\t";
//     // }
//     // cout <<"\n";
//     // for (auto i : newvec){
//     //     cout << i<< "\t";
//     // }
//     // cout <<"\n";
//     // tree.insert(3,&c);
//     // tree.insert(4,&c);
//     // tree.insert(5,&c);
//     // tree.insert(6,&c);
//     // tree.insert(7,&c);
//     // tree.getRoot()->printThisNode();
//     //Node* temp=(Node* )tree.getRoot()->children[2];
//     //temp->printThisNode();
//     //tree.insert(4,&c);
//     //tree.insert(5,&c);
//     //tree.insert(6,&c);
//     //tree.getRoot()->printAllNodes();
//     return 0;
// }
