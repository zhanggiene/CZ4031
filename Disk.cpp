// double linkedlist(directory)    containing page and record id
// blocks are vector of block
// getBlock(int pageid)
// insertBlock()
// go pageRecord(int pageid,recordid)

//

#include "Block.cpp"
#include <list>
#include <iterator>
// 
using namespace std;
class Disk
{
    public: vector<Block> blocks;
    list<pair<int,int>> directory;

    Disk (){
        blocks.push_back(Block());

    }

    void readBlock(int blockid)
    {
        blocks[blockid].print();
    }
    void readRecord(int blockid,int recordid)
    {
        blocks[blockid].getRecord(recordid).toString();
    }

    void deleteRecord(int blockid, int recordid)
    {
        // assume the blockid and recordid are correct

    }

    int getTotalBlocks()
    {
        return blocks.size();
    }

    void * insert(string s)
    {
        // insert at the end 
        Record temp=Record(s);
        int recordId=blocks.back().add(temp);
         //cout<<"the record Id is "<<recordId<<"-------";
        if (recordId==-1)
        {
            // cout<<"disk overflow \n";
            blocks.push_back(Block());
            recordId=blocks.back().add(temp);
        }
        directory.push_back(make_pair(blocks.size()-1,recordId));
        return &directory.back();
    }

    void printAllRecord()
    {
        for(auto x: blocks) x.print();
    }

};