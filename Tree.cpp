#include <vector>
#include <list>
#include <iterator>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <math.h>


using namespace std;

// https://www.youtube.com/watch?v=kjBI0rimo-w&t=1401s&ab_channel=Mayopepeweezy
// https://www.cs.usfca.edu/~galles/visualization/BPlusTree.html
struct Node
//class Node
{

    /* data */
    //
    //------------------------------------------------------------------------
    int *key; //array of keys
    int t; // min degree
    Node **Child; //array of child pointers
    int n; // number of keys currently
    //------------------------------------------------------------------------
    //
    vector<int> keys;
    vector<void*> children;
    bool leaf;
    int fullSize;
    Node* previousLeaf;
    Node* nextLeaf;
    // it is must to store NULL when it is the end 

    // if it is leave, point to iterator in double linkedlist 
    // if it is inner node, point  to the next node. 
    /*
    |*|*|*|      3 keys
    /  \ \ \     4 children
      */
    
    //Construction of the Node
    Node(int n)
    {
        fullSize=n;
        leaf=true;    // when it is created it is true;
        previousLeaf=NULL;
        nextLeaf=NULL;
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

    int getMiniNoKeys(){
        return floor((this->fullSize+1) / 2);
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

    void eraseKey(int idx) {
        keys.erase(keys.begin()+idx);
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
        return (nextLeaf==NULL);
    }

    //get next node
    //Eg. [1,3] -> [4, 5] return Node with [4, 5]
    Node * getNextNode(){
        if (isTerminalLeafNode()){
            return NULL;
        }
        else{
            return (Node *) nextLeaf;
        }
    }

    //
    //
    //--------------------------------------------------------------------------------
    void traverse (); //traverse all nodes in the subtree

    Node *search1(int k); 

    int findKey(int k);

    void insertNonFull(int k);
    
    void splitChild(int i, Node *y);
  
    void remove(int k);
 
    void removeFromLeaf(int idx);
 
    void removeFromNonLeaf(int idx);
 
    int getPred(int idx);
  
    int getSucc(int idx);
 
    void fill(int idx);

    void borrowFromPrev(int idx);
  
    void borrowFromNext(int idx);
  
    void merge(int idx);
  
    friend class bTree;
    //--------------------------------------------------------------------------------
    //
    //
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
        
        // Construction
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

        //
        //------------------------------------------------------------------------
        void traverse()
        {
            if (_root != NULL) _root->traverse();
        }

        Node * search1(int k)
        {
            return (_root == NULL)? NULL : _root->search1(k);
        }

        void deleteFromBTree(int k);
        //------------------------------------------------------------------------
        //


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
            vector<Node *> traversedPath = traversalPathOfInsertion(key);
            Node * nodeToInsertInto = traversedPath.back();
            traversedPath.pop_back();

            //starting from this leaf node
            while (true){ 
                //Case 2: Current node is not full
                if (!nodeToInsertInto->isFull()){
                    int keyIndexToInsertAt = findKeyIndexToInsert(nodeToInsertInto, key);
                    int valueIndexToInsertAt = findValueIndexToInsert(nodeToInsertInto, key);
                    nodeToInsertInto->addToKeys(key, keyIndexToInsertAt);
                    nodeToInsertInto->addToValues(value, valueIndexToInsertAt);
                    break; //stop adding once it has been added to an empty node
                } else {
                    //Case 3: A leaf node is full;
                    if (nodeToInsertInto->leaf){
                        //Split node [1, 2, 3] + 4 -> [1,2] [3, 4]
                        Node * newLeafNode = splitLeafNode(nodeToInsertInto, key, value);

                        key = newLeafNode->keys[0]; //this will be the smallest
                        value = newLeafNode; //Next node to insert
    
                        //if there is no parent (this is root)
                        if (traversedPath.empty()) {
                            Node * parentNode = createNewNonLeafNode(); //no longer leaf
                            parentNode->addToValues(nodeToInsertInto, 0);
                            nodeToInsertInto = parentNode;
                            _root = parentNode;
                        } else {
                            Node * parentNode = traversedPath.back();//get this leaf node's parent
                            traversedPath.pop_back(); 
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
                       
                        
                        //if there is no parent (this is root)
                        if (traversedPath.empty()) {
                            Node * parentNode = createNewNonLeafNode(); //no longer leaf
                            parentNode->addToValues(nodeToInsertInto, 0);
                            nodeToInsertInto = parentNode;
                            _root = parentNode;
                        } else {
                             Node * parentNode = traversedPath.back();//get this non leaf node's parent
                            traversedPath.pop_back(); 
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
            int valueIndexToInsertAt = findValueIndexToInsert(oldNode, key);
            oldNode->addToKeys(key, keyIndexToInsertAt);
            oldNode->addToValues(value, valueIndexToInsertAt);
            
            //get index to split (everything at & after the index will be shifted to new node)
            //eg. 1 2 [3 4] (degree=3) then return index=2
            //eg. 1 2 [3 4 5] (degree=4) then return index=2
            //eg. 1 2 3 [4 5 6] (degree=5) then return index=3
            int indexToSplit = floor((degree+1)/2);

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
            // //[1 2] -> [3 4]  old<->new
            Node * prevNextLeafNode = oldNode->nextLeaf;
            newLeafNode->previousLeaf=oldNode;
            oldNode->nextLeaf=newLeafNode;
            // //[1 2] -> [3 4]  new <-> prevNextNode
            if (prevNextLeafNode!=NULL){
                newLeafNode->nextLeaf = prevNextLeafNode;
                prevNextLeafNode->previousLeaf = newLeafNode->previousLeaf;
            } 


            return newLeafNode;
        }

        //This function splits the values differently than splitLeafNode
        Node * splitNonLeafNode(Node * oldNode, int key, void * value){
            //Create new nonleaf node 
            Node * newNonLeafNode = createNewNonLeafNode();
            //temporarily insert into the old node (this node will overflow)
            
            int keyIndexToInsertAt = findKeyIndexToInsert(oldNode, key);
            int valueIndexToInsertAt = findValueIndexToInsert(oldNode, key);;
            oldNode->addToKeys(key, keyIndexToInsertAt);
            oldNode->addToValues(value, keyIndexToInsertAt+1);
            
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
            if (nodeToInsert->leaf){
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
            } else {
                for (int i=0;i<nodeToInsert->keys.size();i++){
                    //imagine keys 2, 3, 5
                    //insert 4
                    if (nodeToInsert->keys[i] > key){
                        return i+1;
                    }
                }
                //if the key is bigger than all the keys in current node
                //imagine keys 2, 3, 4
                //insert 5
                return (nodeToInsert->getNumValues());
            }
            
        }

        //vector<Node*> (traversal path to get the parents of all)
        //[root,nonleaf,leaf]
        vector<Node*> traversalPathOfInsertion(int key){
            //traverse the tree downwards until we find the leaf node
                vector <Node*> traversalNodes;
                Node * current_node = _root;
                traversalNodes.push_back(current_node);
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
                        if (i==current_node->getNumKeys()-1){
                            current_node = (Node *) current_node->children[current_node->getNumValues()-1];
                            break;
                        }
                    }
                    traversalNodes.push_back(current_node);
                }
                return traversalNodes;
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
                    child_queue.clear();
                    cout << "\n";
                }
                
                
            }
        }

        //To check
        void printLastRow(){
            cout << "printLastRow:\n";
            vector<Node *> traversedPath = traversalPathOfInsertion(-1);
            Node * node = (Node *) traversedPath.back();
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
            vector<Node *> traversedPath = traversalPathOfInsertion(-1);
            Node * node = (Node *) traversedPath.back();
            while (true){
                node->printAllChildren();
                if (node->isTerminalLeafNode()){
                    break;
                }
                node = node->getNextNode();
            }
        }

        vector<void*> returnLastRowPointers(){
            vector<void*> children;
            vector<Node *> traversedPath = traversalPathOfInsertion(-1);
            Node * node = (Node *) traversedPath.back();
            while (true){
                children.insert(children.end(), node->children.begin(), node->children.end());
                if (node->isTerminalLeafNode()){
                    break;
                }
                node = node->getNextNode();
            }
            return children;
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

         vector<pair<int,int> > search(int numVotes)
        {
            // use of binary search
            // print all the content inside the data blocks, even if the numBVotes is not equal
            // return vector of directory pointer.
            Node * current_node = _root;
            vector<pair<int,int> > result;
            int counterIndex=5;
            int counterData=5;
            int indexNodeNumber=0;
            int dataNodeNumber=0;
            cout<<"Top 5 content of the index node: "<<endl;

            while(!current_node->leaf)
            {
                indexNodeNumber+=1;
                if (true){ //first 5
                    current_node->printAllKeys();
                }
                if (counterIndex>0) {counterIndex-=1;}
                int childrenIndex=upper_bound(current_node->keys.begin(),current_node->keys.end(),numVotes)-current_node->keys.begin();
                current_node=(Node* )current_node->children[childrenIndex];
            }
            cout<<"\nTotal number of index nodes: "<<indexNodeNumber<<endl;

            // now reach leaf node
            // keep traversing to the left 23 33 33 33
            //current_node->printAllKeys();
            
            while(current_node!=NULL && current_node->keys[0]==numVotes)
            {
                //current_node->printAllKeys();
                current_node=current_node->previousLeaf;

            } 
            cout<<"_________________________________________\n";
            cout<<"Top 5 content of the data nodes: "<<endl;
             //current_node->printAllKeys();
            while(current_node!=NULL && current_node->keys[0]<=numVotes)
            {
                dataNodeNumber+=1;
                if (counterData>0){
                    current_node->printAllKeys();
                    current_node->printAllChildren();
                    counterData-=1;
                }

                for (int i=0;i<current_node->getNumKeys();i++){
                if (current_node->keys[i]==numVotes)
                
                {  
                pair<int,int> pointerToRecord = *(pair<int,int> *)current_node->children[i];
                result.push_back(pointerToRecord);}
        }
            current_node=current_node->nextLeaf;
            }

            cout<<"\nTotal number of data nodes: "<<dataNodeNumber<<endl;
            return result;


        }


        vector<pair<int,int> > searchRange(int lower,int higher)
        {
            // use of binary search
            // print all the content inside the data blocks, even if the numBVotes is not equal
            // return vector of directory pointer.
            Node * current_node = _root;
            vector<pair<int,int> > result;
            int counterIndex=5;
            int counterData=5;
            int indexNodeNumber=0;
            int dataNodeNumber=0;
            cout<<"Top 5 content of the index Node: "<<endl;

            while(!current_node->leaf)
            {
                //current_node->printAllKeys();
                indexNodeNumber+=1;
                if (counterIndex>0) {current_node->printAllKeys();counterIndex-=1;}
                int childrenIndex=upper_bound(current_node->keys.begin(),current_node->keys.end(),lower)-current_node->keys.begin();
                current_node=(Node* )current_node->children[childrenIndex];
            }
            cout<<"Total number of index nodes: "<<indexNodeNumber<<endl;

            cout<<endl;
            cout<<"Top 5 content of the dataNode: "<<endl;
            // now reach leaf node
            // keep traversing to the left 23 33 33 33
            //current_node->printAllKeys();
            
            while(current_node!=NULL && current_node->keys[0]==lower)
            {
                //current_node->printAllKeys();
                current_node=current_node->previousLeaf;

            } 
             //current_node->printAllKeys();
            while(current_node!=NULL && current_node->keys[0]<=higher)
            {
                dataNodeNumber+=1;
                if (counterData>0){
                    current_node->printAllKeys();
                    current_node->printAllChildren();
                    counterData-=1;
                }

                for (int i=0;i<current_node->getNumKeys();i++){
                if (current_node->keys[i]<=higher && current_node->keys[i]>=lower)
                
                {  
                pair<int,int> pointerToRecord = *(pair<int,int> *)current_node->children[i];
                result.push_back(pointerToRecord);}
        }
            current_node=current_node->nextLeaf;
            }

            cout<<"Total number of data nodes: "<<dataNodeNumber<<endl;
            return result;


        }

        //
        //------------------------------------------------------------------------
        Node(int t0, bool leaf0)
        {       
            t = t0;
            leaf = leaf0;
            key = new int[2*t-1];
            Child = new Node *[2*t];
            n = 0;
        }
  
        int Node::findKey(int k)
        {
            int idx=0;
            while (idx<n && keys[idx] < k)
                ++idx;
            return idx;
        }

        int Node::getPred(int idx)
        {
            BTreeNode *cur=C[idx];
            while (!cur->leaf)
                cur = cur->C[cur->n];
            return cur->keys[cur->n-1];
        }
        
        int Node::getSucc(int idx)
        {
            Node *cur = Child[idx+1];
            while (!cur->leaf)
                cur = cur->Child[0];
            return cur->key[0];
        }

        void Node::fill(int idx)
        {

            if (idx!=0 && Child[idx-1]->n>=t)
                borrowFromPrev(idx);

            else if (idx!=n && Child[idx+1]->n>=t)
                borrowFromNext(idx);

            else
            {
                if (idx != n)
                    merge(idx);
                else
                    merge(idx-1);
            }
            return;
        }

        void Node::removeFromLeaf (int idx)
        {
            for (int i=idx+1; i<n; ++i)
                key[i-1] = key[i];
            n--;
        
            return;
        }

        void Node::removeFromNonLeaf(int idx)
        {
        
            int k = key[idx];
        
            if (Child[idx]->n >= t)
            {
                int pred = getPred(idx);
                keys[idx] = pred;
                Child[idx]->remove(pred);
            }
            else if  (Child[idx+1]->n >= t)
            {
                int succ = getSucc(idx);
                keys[idx] = succ;
                Child[idx+1]->remove(succ);
            }
            else
            {
                merge(idx);
                Child[idx]->remove(k);
            }
            return;
        }

        void bTree::deleteFromBTree(int k)
        {
            int idx = findKey(k);
        
            if (idx < n && key[idx] == k)
            {
                if (leaf)
                    removeFromLeaf(idx);
                else
                    removeFromNonLeaf(idx);
            }
            else
            {
                if (leaf)
                {
                    cout << "The key "<< k <<" does not exist in the tree\n";
                    return;
                }
                bool flag = ( (idx==n)? true : false );
    
                if (Child[idx]->n < t)
                    fill(idx);

                if (flag && idx > n)
                    Child[idx-1]->remove(k);
                else
                    Child[idx]->remove(k);
            }
            return;
        }

        void borrowFromPrev(int idx)
        {
        
            Node *child=Child[idx];
            Node *sibling=Child[idx-1];
        
        
            for (int i=child->n-1; i>=0; --i)
                child->keys[i+1] = child->keys[i];
    
            if (!child->leaf)
            {
                for(int i=child->n; i>=0; --i)
                    child->Child[i+1] = child->Child[i];
            }
        
            child->key[0] = key[idx-1];
        
            if(!child->leaf)
                child->Child[0] = sibling->Child[sibling->n];
        
        
            key[idx-1] = sibling->key[sibling->n-1];
        
            child->n += 1;
            sibling->n -= 1;
        
            return;
        }

        void Node::borrowFromNext(int idx)
        {
        
            Node *child=Child[idx];
            Node *sibling=Child[idx+1];
        
            child->key[(child->n)] = key[idx];

            if (!(child->leaf))
                child->Child[(child->n)+1] = sibling->Child[0];

            key[idx] = sibling->key[0];

            for (int i=1; i<sibling->n; ++i)
                sibling->key[i-1] = sibling->key[i];

            if (!sibling->leaf)
            {
                for(int i=1; i<=sibling->n; ++i)
                    sibling->Child[i-1] = sibling->Child[i];
            }
            child->n += 1;
            sibling->n -= 1;
            return;
        }

        // merge Child[idx] with Child[idx+1]
        void Node::merge(int idx)
        {
            Node *child = Child[idx];
            Node *sibling = Child[idx+1];
        

            child->key[t-1] = key[idx];

            for (int i=0; i<sibling->n; ++i)
                child->key[i+t] = sibling->key[i];
        

            if (!child->leaf)
            {
                for(int i=0; i<=sibling->n; ++i)
                    child->Child[i+t] = sibling->Child[i];
            }

            for (int i=idx+1; i<n; ++i)
                key[i-1] = key[i];

            for (int i=idx+2; i<=n; ++i)
                Child[i-1] = Child[i];
                 child->n += sibling->n+1;
            n--;

            delete sibling;
            return;
        }

        void Node::traverse()
        {
            int i;
            for (i = 0; i < n; i++)
            {

                if (leaf == false)
                    Child[i]->traverse();
                cout << " " << keys[i];
            }
            if (leaf == false)
                Child[i]->traverse();
        }

        Node * Node::search1(int k)
        {
            int i = 0;
            while (i < n && k > keys[i])
                i++;
        
            if (keys[i] == k)
                return this;
        
            if (leaf == true)
                return NULL;

            return Child[i]->search1(k);
        }
  
        void Node::remove(int k)
        {
            if (!_root)
            {
                cout << "The tree is empty\n";
                return;
            }
            _root->remove(k);

            if (_root->n==0)
            {
                Node *tmp = _root;
                if (_root->leaf)
                    _root = NULL;
                else
                    _root = _root->Child[0];
        
                delete tmp;
            }
            return;
        }
};
        //------------------------------------------------------------------------
        //
        //
        // //deletion part starts here
        // void deleteFromTree(int val){
             
        //     doDelete(val, getRoot());
            
        //     Node* treeRoot = getRoot();
        //     if (treeRoot->keys.size() == 0){
        //         //tree is empty
        //     }

        // }

        // void doDelete(int val, Node* tree) {
        //     int numOfKeys = tree->getNumKeys();
        //     if (tree != NULL) {
        //         int i = 0;
        //         for (i; i < numOfKeys && tree->keys[i] < val; i++);
        //         if (i == numOfKeys) {
        //             if (!tree->leaf) {
        //                 doDelete(val, (Node *)tree->children[numOfKeys]);
        //             } else {
        //                 //delete the last key

        //             }
        //         }
        //         else if (!tree->leaf && tree->keys[i] == val){
        //             doDelete(val, (Node *)tree->children[i+1]);
        //         }
        //         else if (!tree->leaf) {
        //             doDelete(val, (Node *)tree->children[i]);
        //         }
        //         else if (tree->leaf && tree->keys[i] == val){
        //             int eraseKey(i);
        //             numOfKeys--;
        //             if (tree->nextLeaf != NULL) {
        //                 //
        //             }

        //             Node * parentNode = getParent(tree);
        //             // if we remove the smallest element in a leaf and the leaf is now empty
        //             // go up parent node, and fix index keys
        //             if (i == 0 && parentNode != NULL) {
        //                 int nextSmallest;
        //                 int parentIdx = 0;
        //                 for (parentIdx; parentNode->children[parentIdx] != tree; parentIdx++);
        //                 if (numOfKeys == 0) {
        //                     if (parentIdx == parentNode->getNumKeys()){
        //                         //is this case possible?
        //                     }
        //                     else {
        //                         nextSmallest = (int) parentNode->keys[parentIdx+1];
        //                     }
        //                 }
        //                 else {
        //                     nextSmallest = tree->keys[0];
        //                 }
        //                 while (parentNode != NULL) {
        //                     if (parentNode > 0 && parentNode->keys[parentIdx - 1] == val){
        //                         parentNode->keys[parentIdx - 1] = nextSmallest;
        //                     }
        //                     Node * grandParent = parentNode;
        //                 }
        //             }
        //             repairAfterDeletion(tree);
        //         }
        //         else {
        //             //is there more conditions?
        //         }
        //     } 
        // }

        // Node * mergeRight(Node * tree) {
        //     Node * parentNode = getParent(tree);
        //     int parentIdx = 0;
        //     for (parentIdx = 0; parentNode->children[parentIdx] != tree; parentIdx++);
        //     Node * rightSib = (Node *)parentNode->children[parentIdx+1];

        //     if (!tree->leaf) {
        //         tree->keys[tree->getNumKeys()] = parentNode->keys[parentIdx];
        //     }

        //     int fromParentIdx = tree->getNumKeys();

        //     for (int i = 0; i < rightSib->getNumKeys(); i++){
        //         int insertIdx = tree->getNumKeys() + 1 + i;
        //         if (tree->leaf){
        //             insertIdx -= 1;
        //         }
        //         tree->keys[insertIdx] = rightSib->keys[i];
        //     }
        //     if (!tree->leaf) {
        //         for (int i = 0; i <= rightSib->getNumKeys(); i++){
        //             tree->children[tree->getNumKeys() + 1 + i] = rightSib->children[i];
        //             //do we need to set parent mannually?
        //         }
        //     }
        //     else {
        //         tree->nextLeaf = rightSib->nextLeaf;
        //     }
        //     for (int i = parentIdx + 1; i < parentNode->getNumKeys(); i++) {
        //         parentNode->children[i] = parentNode->children[i+1];
        //         parentNode->keys[i-1] = parentNode->keys[i];
        //         parentNode->eraseKey(i);
        //     }
        //     return tree;
        // }

        // void borrowFromRight(Node * tree, int parentIdx) {
        //     Node * parentNode = getParent(tree);
        //     Node * rightSib = (Node *)parentNode->children[parentIdx + 1];

        //     if (tree->leaf){
        //         tree->keys[tree->getNumKeys()-1] = rightSib->keys[0];
        //         parentNode->keys[parentIdx] = rightSib->keys[1];
        //     }
        //     else {
        //         tree->keys[tree->getNumKeys()-1] = parentNode->keys[parentIdx];
        //         parentNode->keys[parentIdx] = rightSib->keys[0];

        //         tree->children[tree->getNumKeys()] = rightSib->children[0];

        //         for (int i = 1; i < rightSib->getNumKeys(); i++){
        //             rightSib->children[i-1] = rightSib->children[i];
        //         }
        //     }
        //     for (int i = 1; i < rightSib->getNumKeys(); i++){
        //         rightSib->keys[i-1] = rightSib->keys[i];
        //     }
        // }

        // void borrowFromLeft(Node * tree, int parentIdx) {
        //     Node * parentNode = getParent(tree);

        //     for (int i = tree->getNumKeys() - 1; i > 0; i--){
        //         tree->keys[i] = tree->keys[i-1];
        //     }

        //     Node * leftSib = (Node *)parentNode->children[parentIdx - 1];

        //     if (tree->leaf){
        //         tree->keys[0] = leftSib->keys[leftSib->getNumKeys() - 1];
        //         parentNode->keys[parentIdx - 1] = leftSib->keys[leftSib->getNumKeys() - 1];
        //     }
        //     else {
        //         tree->keys[0] = parentNode->keys[parentIdx - 1];
        //         parentNode->keys[parentIdx - 1] = leftSib->keys[leftSib->getNumKeys() - 1];
        //         tree->children[parentIdx - 1] = (Node *)leftSib->keys[leftSib->getNumKeys() - 1];

        //         for (int i = 1; i < tree->getNumKeys(); i--){
        //             tree->children[i] = tree->children[i-1];
        //         }
        //         tree->children[0] = leftSib->children[leftSib->getNumKeys()];
                
        //         leftSib->children[leftSib->getNumKeys()] = NULL;
        //     }
        // }

        // void repairAfterDeletion(Node* tree){
        //     if (tree->getNumKeys() < tree->getMiniNoKeys()){
        //         if (getParent(tree) == NULL) {
        //             if (tree->getNumKeys() == 0){
        //                 //tree is empty.
        //             }
        //         }
        //         else {
        //             Node * parentNode = getParent(tree);

        //             int parentIdx = 0;
        //             for (parentIdx; parentNode->children[parentIdx] != tree; parentIdx++);

        //             if (parentIdx > 0 && ((Node*)parentNode->children[parentIdx-1])->getNumKeys() > tree->getMiniNoKeys()){
        //                 borrowFromLeft(tree, parentIdx);
        //             }
        //             else if (parentIdx < parentNode->getNumKeys() && ((Node*)parentNode->children[parentIdx + 1])->getNumKeys() > tree->getMiniNoKeys()) {
        //                 borrowFromRight(tree, parentIdx);
        //             }
        //             else if (parentIdx == 0){
        //                 Node * nextNode = mergeRight(tree);
        //                 repairAfterDeletion(getParent(nextNode));
        //             }
        //             else {
        //                 Node * nextNode = mergeRight((Node*)parentNode->children[parentIdx - 1]);
        //                 repairAfterDeletion(getParent(nextNode));
        //             }
        //         }
        //     }
        // }   




//}

// int main()
// {
//     BTree t(3); // A B-Tree with minimum degree 3
  
//     t.insert(1);
//     t.insert(3);
//     t.insert(7);
//     t.insert(10);
//     t.insert(11);
//     t.insert(13);
//     t.insert(14);
//     t.insert(15);
//     t.insert(18);
//     t.insert(16);
//     t.insert(19);
//     t.insert(24);
//     t.insert(25);
//     t.insert(26);
//     t.insert(21);
//     t.insert(4);
//     t.insert(5);
//     t.insert(20);
//     t.insert(22);
//     t.insert(2);
//     t.insert(17);
//     t.insert(12);
//     t.insert(6);
  
//     cout << "Traversal of tree constructed is\n";
//     t.traverse();
//     cout << endl;
  
//     t.remove(6);
//         cout << "Traversal of tree after removing 6\n";
//     t.traverse();
//     cout << endl;
  
//     t.remove(13);
//     cout << "Traversal of tree after removing 13\n";
//     t.traverse();
//     cout << endl;
  
//     t.remove(7);
//     cout << "Traversal of tree after removing 7\n";
//     t.traverse();
//     cout << endl;
  
//     t.remove(4);
//     cout << "Traversal of tree after removing 4\n";
//     t.traverse();
//     cout << endl;
  
//     t.remove(2);
//     cout << "Traversal of tree after removing 2\n";
//     t.traverse();
//     cout << endl;
  
//     t.remove(16);
//     cout << "Traversal of tree after removing 16\n";
//     t.traverse();
//     cout << endl;
  
//     return 0;
// }




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
