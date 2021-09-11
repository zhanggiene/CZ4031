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

    void insertKeysFromOtherNode(Node * otherNode, int startIndex, int endIndex){
        keys.insert(keys.begin(),otherNode->keys.begin()+startIndex, otherNode->keys.begin()+endIndex);
    }

    void eraseKeys(int startIndex, int endIndex){
        keys.erase(keys.begin()+startIndex, keys.begin()+endIndex);
    }

    void insertValuesFromOtherNode(Node * otherNode, int startIndex, int endIndex){
        children.insert(children.begin(),otherNode->children.begin()+startIndex, otherNode->children.begin()+endIndex);
    }

    void eraseValues(int startIndex, int endIndex){
        children.erase(children.begin()+startIndex, children.begin()+endIndex);
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

    //Meaning no more next leaf node (it's the last of all the leaf nodes)
    bool isTerminalLeafNode(){
        return (getNumKeys() == getNumValues());
    }

    //get next node
    //Eg. [1,3] -> [4, 5] return Node with [4, 5]
    Node * getNextNode(){
        if (isTerminalLeafNode()){
            return NULL;
        }
        else{
            return (Node *) children[getNumValues()-1];
        }
    }

    // To check that the leaf node pointers are right
    // only for dummy example in this file
    // void printAllChildren(){
    //     cout << "|";
    //     for (int i=0;i<getNumKeys();i++){
    //         int * pointerToRecord = (int *)children[i];
    //         cout << *pointerToRecord;
    //         cout << "|";
    //     }
    //     cout << "\t";
    // }

    //For actual data 
    void printAllChildren(){
        cout << "|";
        for (int i=0;i<getNumKeys();i++){
            pair<int,int> * pointerToRecord = (pair<int,int> *)children[i];
            cout <<"<" <<pointerToRecord->first<<","<<pointerToRecord->second<<">";
            cout << "|";
        }
        cout << "\t";
    }

    ~Node(){
        for (auto child : children){
            free(child);
        }
    }

};


class bTree
{
    private: 
        Node* _root;
        int degree;
        int totalNode;
        int numOfNodes = 0;
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
                _root = createNewLeafNode();
                //add 1 key and 1 child to the new root node
                _root->addToKeys(key,0);
                _root->addToValues(value, 0);
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
                    break; //stop adding once it has been added to an empty node
                } else {
                    //Case 3: A leaf node is full;
                    if (nodeToInsertInto->leaf){
                        //Split node [1, 2, 3] + 4 -> [1,2] [3, 4]
                        Node * newLeafNode = splitLeafNode(nodeToInsertInto, key, value);
                        key = newLeafNode->keys[0]; //this will be the smallest
                        value = newLeafNode; //Next node to insert
                        Node * parentNode = getParent(nodeToInsertInto); //get this leaf node's parent
                        //if the parentNode is already a root node
                        if (parentNode == NULL) {
                            parentNode = createNewNonLeafNode(); //no longer leaf
                            parentNode->addToValues(nodeToInsertInto, 0);
                            nodeToInsertInto = parentNode;
                            _root = parentNode;
                        } else {
                            nodeToInsertInto = parentNode;
                        }
                    } 
                    //Case 4: A non-leaf node is full;
                    else { 
                        //Split node [1, 2, 3] + 4 -> [1,2] [3, 4]
                        Node * newNonLeafNode = splitNonLeafNode(nodeToInsertInto, key, value);
                        key = newNonLeafNode->keys[0]; //this will be the smallest (used for root)
                        newNonLeafNode->eraseKeys(0, 1); //Node [1,2] [3, 4] -> Node [1,2] [4]
                        value = newNonLeafNode; //next value to insert
                        Node * parentNode = getParent(nodeToInsertInto); //get this leaf node's parent
                        //if the parentNode is already a root node
                        if (parentNode == NULL) {
                            parentNode = createNewNonLeafNode(); //no longer leaf
                            parentNode->addToValues(nodeToInsertInto, 0);
                            nodeToInsertInto = parentNode;
                            _root = parentNode;
                        } else {
                            nodeToInsertInto = parentNode;
                        }
                    }
                }
            }
       }

        Node * splitLeafNode(Node * oldNode, int key, void * value){
            //Create new leaf node 
            Node * newLeafNode = createNewLeafNode();
            //temporarily insert into the old node (this node will overflow)
            int keyIndexToInsertAt = findKeyIndexToInsert(oldNode, key);
            oldNode->addToKeys(key, keyIndexToInsertAt);
            int valueIndexToInsertAt = findValueIndexToInsert(oldNode, key);
            oldNode->addToValues(value, valueIndexToInsertAt);
            
            //get index to split (everything at & after the index will be shifted to new node)
            //eg. 1 2 [3 4] (degree=3) then return index=2
            //eg. 1 2 [3 4 5] (degree=4) then return index=2
            //eg. 1 2 3 [4 5 6] (degree=5) then return index=3
            int indexToSplit = floor((degree+1)/2);
            // cout << "indexToSplit: "<<indexToSplit<<"\n";

            //Populate the new leaf node with [3, 4] and remove [3, 4] from old leaf node
            newLeafNode->insertKeysFromOtherNode(oldNode, 
                                                indexToSplit, 
                                                oldNode->getNumKeys());
            oldNode->eraseKeys(indexToSplit, oldNode->getNumKeys()) ;
            newLeafNode->insertValuesFromOtherNode(oldNode, 
                                                    indexToSplit, 
                                                    oldNode->getNumValues());
            oldNode->eraseValues(indexToSplit, oldNode->getNumValues());          

            // //add the new leaf node's pointer to the end of old leaf node
            // //[1 2] -> [3 4]
            oldNode->addToValues(newLeafNode, oldNode->getNumValues());

            return newLeafNode;
        }

        //This function splits the values differently than splitLeafNode
        Node * splitNonLeafNode(Node * oldNode, int key, void * value){
            //Create new nonleaf node 
            Node * newNonLeafNode = createNewNonLeafNode();
            //temporarily insert into the old node (this node will overflow)
            int keyIndexToInsertAt = findKeyIndexToInsert(oldNode, key);
            oldNode->addToKeys(key, keyIndexToInsertAt);
            int valueIndexToInsertAt = findValueIndexToInsert(oldNode, key);
            oldNode->addToValues(value, valueIndexToInsertAt);
            //get index to split (everything at & after the index will be shifted to new node)
            //eg. 1 2 [3 4] (degree=3) then return index=2
            //eg. 1 2 [3 4 5] (degree=4) then return index=2
            //eg. 1 2 3 [4 5 6] (degree=5) then return index=3
            int indexToSplit = floor((degree+1)/2);

            //Populate the new leaf node with [3, 4] and remove [3, 4] from old leaf node
            newNonLeafNode->insertKeysFromOtherNode(oldNode, 
                                                indexToSplit, 
                                                oldNode->getNumKeys());
            oldNode->eraseKeys(indexToSplit, oldNode->getNumKeys()) ;
            newNonLeafNode->insertValuesFromOtherNode(oldNode, 
                                                    indexToSplit+1,  //difference is here
                                                    oldNode->getNumValues());
            oldNode->eraseValues(indexToSplit+1, //difference is here
                            oldNode->getNumValues());
            return newNonLeafNode;
        }

        Node * getParent(Node * node){
            int smallestKey = node->keys[0];
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
            cout << "printLastRow:\n";
            Node * node = (Node *) findLeafNodeToInsert(-1);
            while (true){
                node->printAllKeys();
                if (node->isTerminalLeafNode()){
                    break;
                }
                node = node->getNextNode();
            }
        }

        //To check that the leaf node pointers are right
        void printLastRowPointers(){
            cout << "printLastRowPointers:\n";
            Node * node = (Node *) findLeafNodeToInsert(-1);
            while (true){
                node->printAllChildren();
                if (node->isTerminalLeafNode()){
                    break;
                }
                node = node->getNextNode();
            }
        }

        Node * createNewLeafNode(){
            numOfNodes++;
            return (new Node(degree));
        }

        Node * createNewNonLeafNode(){
            numOfNodes++;
            Node * temp = new Node(degree);
            temp->setLeafOrNot(false);
            return (temp);
        }

        int getNumberOfNodes(){
            return numOfNodes;
        }
        int getN(){
            return this->degree;
        }

        int getHeight(){
            int height = 1;
            Node * current_node = _root;
            //if it is not a leaf
            while (!current_node->leaf){
                current_node = (Node *)current_node->children[0];
                height++;
            }
            return height;
        }

        ~bTree(){
            delete _root;
        }

};




// int main()
// {
//     int a=1;
//     int b=2;
//     int c=3;
//     int d=4;
//     bTree tree=bTree(3);

//     tree.insertToBTree(1,&a);
//     tree.printNodeTree();
//     tree.insertToBTree(2,&a);
//     tree.printNodeTree();
//     tree.insertToBTree(3,&b);
//     tree.printNodeTree();

//     tree.insertToBTree(4,&b);
//     tree.printNodeTree();


//     tree.insertToBTree(5,&c);
//     tree.printNodeTree();
//     tree.insertToBTree(6,&c);
//     tree.printNodeTree();
//     tree.insertToBTree(6,&c);
//     tree.printNodeTree();
//     tree.insertToBTree(7,&d);
//     tree.printNodeTree();
//     // tree.insertToBTree(6,&d);
//     // tree.printNodeTree();
//     // tree.insertToBTree(7,&d);
//     // tree.printNodeTree();

//     tree.printLastRow();

//     // tree.printLastRowPointers();

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
//     // tree.insertToBTree(17,&b);
//     // tree.printNodeTree();
//     // tree.insertToBTree(18,&c);
//     // tree.printNodeTree();
//     // tree.insertToBTree(19,&c);
//     // tree.printNodeTree();
//     // tree.insertToBTree(20,&c);
//     // tree.printNodeTree();
//     // tree.insertToBTree(21,&c);
//     // tree.printNodeTree();
//     // tree.insertToBTree(22,&c);
//     // tree.printNodeTree();
//     // tree.insertToBTree(23,&c);
//     // tree.printNodeTree();
//     // tree.insertToBTree(24,&c);
//     // tree.printNodeTree();
//     // tree.insertToBTree(25,&c);
//     // tree.printNodeTree();
//     // tree.insertToBTree(26,&c);
//     // tree.printNodeTree();
//     // tree.insertToBTree(27,&c);
//     // tree.printNodeTree();
//     // tree.insertToBTree(28,&c);
//     // tree.printNodeTree();
//     // tree.insertToBTree(29,&c);
//     // tree.printNodeTree();
//     // tree.insertToBTree(30,&c);
//     // tree.printNodeTree();
//     // tree.insertToBTree(31,&c);
//     // tree.printNodeTree();
//     // tree.insertToBTree(32,&c);
//     // tree.printNodeTree();
//     // tree.insertToBTree(33,&c);
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

// //     // tree.insertToBTree(1,&b);
// //     // tree.printNodeTree();
    
// //     // vector<int> intvec = {1, 2, 3, 4};
// //     // vector<int> newvec;
// //     // newvec.insert(newvec.begin(),intvec.begin()+2, intvec.begin()+3);
// //     // intvec.erase(intvec.begin()+2, intvec.begin()+intvec.size());    
// //     // for (auto i : intvec){
// //     //     cout << i<< "\t";
// //     // }
// //     // cout <<"\n";
// //     // for (auto i : newvec){
// //     //     cout << i<< "\t";
// //     // }
// //     // cout <<"\n";
// //     // tree.insert(3,&c);
// //     // tree.insert(4,&c);
// //     // tree.insert(5,&c);
// //     // tree.insert(6,&c);
// //     // tree.insert(7,&c);
// //     // tree.getRoot()->printThisNode();
// //     //Node* temp=(Node* )tree.getRoot()->children[2];
// //     //temp->printThisNode();
// //     //tree.insert(4,&c);
// //     //tree.insert(5,&c);
// //     //tree.insert(6,&c);
// //     //tree.getRoot()->printAllNodes();
//     return 0;
// }
