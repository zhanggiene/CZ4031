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
    vector<int > keys;
    int level;
    bool leaf;
    int currentSlot;
    vector< void*> children;
    int fullSize;
    // if it is leave, point to iterator in double linkedlist 
    // if it is inner node, point  to the next node. 
    /*
    |*|*|*|      3 keys
    /  \ \ \     4 children
      */
    Node(int n)
    {
        keys=vector<int>(n,0);
        children=vector<void*> (n+1,NULL);
        currentSlot=0;
        fullSize=n;
        leaf=true;    // when it is created it is true;



    }
    void printThisNode()
    {
        if (leaf==true) cout<<"i am leaf";
        else  cout<<" i am not leaf";
        for(int i=0;i<currentSlot;i++)
        {
            cout<<keys[i]<<"|";
        }
        /*for(int i=0;i<=currentSlot;i++)
        {
          if (!leaf) for(int i=0;i<=currentSlot;i++)
          {
              Node * temp=(Node *) children[i];
              temp->print();

          }
        }
        */
    }
    void printAllNodes()
    {
        for(int i=0;i<currentSlot;i++)
        {
            cout<<keys[i]<<"|";
        }

          if (!leaf) for(int i=0;i<=currentSlot;i++)
          {
              Node * temp=(Node *) children[i];
              temp->printAllNodes();

          }

    }

    void height(Node* currentNode,int& result, int h)
    {
        if (currentNode->leaf)
        {
            result=h;
            return;
        }
        // B+ tree is a balanced tree.
        height((Node*)currentNode->children[0],result,h+1);   //  recursively
    }

    /*void printByLevel(Node* temp,int level)
    {
        if (level>1 && temp->leaf)
        cout<<"Empty";
        else if(level==1)
        {
            // printing the stuff on the leave
            for(int i=0;i<temp->fullSize;i++) {cout<<temp->keys[i];}

        }
        else{
            for(int i=0;i<=temp->fullSize;i++) printByLevel((Node *)children[i],level-1);
        }
        
    }*/

    

    Node * insertNode(int key, void* value,Node* currentNode,Node* root)
    {
        //cout<<"inserting"<<key;
        if (leaf)
        {
            //cout<<"i am leave";
            //cout<<"node is not full";
            // found the leave node and it is not full 
        int i=currentSlot;
        while(keys[i-1] >key && i!=0)
        {
            keys[i]=keys[i-1];   // shift the key to the left one by one
            i-=1;
        }
        keys[i]=key;
        children[i]=value;
        currentSlot+=1;
        }

        else if (!leaf)            // it is not a leave, recursively find the leave
        {
            
            //    binary search 
            // 1 2 ***     insert 2 return 2
            int childrenIndex=upper_bound(keys.begin(),keys.begin()+currentSlot,key)-keys.begin();
            
            /*int childrenIndex=0;
            while(childrenIndex<currentSlot && key>keys[childrenIndex])
            {
                childrenIndex+=1;
            }
            */
            ((Node* )children[childrenIndex])->insertNode(key,value,currentNode,root);   // cast to Node pointer

        }
        // after the insertion,check if it is full
        if (leaf && currentSlot==fullSize)
        {
            // root is full
            if (this==root)
            {
                //cout<<"root is full";
                Node* newNode =new Node(fullSize);
                newNode->leaf=false;
                newNode->children[0]=this;
                newNode->splitChild(this);   // this means the full Node;
                return newNode;
            }
            else{
                currentNode->splitChild(this);      // parent split children
            }
        }
        return root;

    }

    //  the parent is splitting the children node
    void splitChild(Node* toSplit)
    {
        Node* right=new Node(toSplit->fullSize);

        int middle=(toSplit->fullSize)/2; // this key will be moved up
        //cout<<"middle is"<<middle;
        int middleKey=toSplit->keys[middle];
        int rightPointer=0;
        for(int i=middle;i<toSplit->fullSize;i++)
        {
            right->keys[rightPointer]=toSplit->keys[i];
            rightPointer+=1;
            toSplit->currentSlot=toSplit->currentSlot-1;
            right->currentSlot=right->currentSlot+1;
        }
            // has children
        int i_child=0;

        // split the children as well
        if (toSplit->leaf==false)
        {
            for(int i=toSplit->fullSize/2;i<=toSplit->fullSize;i++)
            {
                right->children[i_child]=toSplit->children[i];
                i_child+=1;
            }
            right->leaf=toSplit->leaf;

        }

        // on the parents, find the left children, then the right children is plus 1
        int rightChildrenPos=toSplit->fullSize-1;  // the parent's children
        while(children[rightChildrenPos]!=toSplit)
        {
            children[rightChildrenPos+1]=children[rightChildrenPos];  // shift the children to the right 
            rightChildrenPos-=1;
        }
        children[rightChildrenPos+1]=right;


        // put the middle key to the right possition in the parent node
        int i=currentSlot;
        while(keys[i-1] >middleKey && i!=0)
        {
            keys[i]=keys[i-1];   // shifting od key
            i-=1;
        }
        keys[i]=middleKey;
        currentSlot+=1;   // increase parent number of key by one


        // toSplit->currentSlot=(toSplit->currentSlot-1);



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

    /*void printBylevel(int level)
    {
        if (_root!=NULL)
        {
         _root->printByLevel(_root,level);
        }
    }
    
   void printFromRoot()
   {
       if (_root!=NULL)
       {
           _root->print();
       }
   }
   */

    int getHeight(){
        int result;
        _root->height(_root,result,1);
        return result;
    }

    Node * getRoot()
    {
        return _root;
    }
    void insert(int key, void* value)
    {
         if (_root==NULL) 
         {
             _root=new Node(degree);
             _root->leaf=true;
             _root->keys[0]=key;
             _root->children[0]=value;
             _root->currentSlot=1;

         }
         else {
             _root=_root->insertNode(key,value,_root,_root);
         }
    }

    /*vector<void*> getbyIndex(int i){
        // to some search and return vector of pointer
    }
    */
};



int a=1;
int b=2;
int c=3;
int main()
{
    bTree tree=bTree(3);
    tree.insert(1,&a);
    tree.insert(2,&b);
    
    tree.insert(3,&c);
    tree.insert(4,&c);
    tree.insert(5,&c);
    tree.getRoot()->printThisNode();
    //Node* temp=(Node* )tree.getRoot()->children[2];
    //temp->printThisNode();
    //tree.insert(4,&c);
    //tree.insert(5,&c);
    //tree.insert(6,&c);
    //tree.getRoot()->printAllNodes();
    return 0;
}
