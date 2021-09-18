#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <utility>
#include "Record.cpp"

using namespace std;

class Block {
    public:
    int numberSlot;
    int lastPosition;
    char m[98];



   Block (){
       numberSlot=0;
       lastPosition=98;

   }

   int add(Record a)
   {    int temp1=lastPosition-(int)sizeof(a);
       int temp2=numberSlot*sizeof(int);
       
       //cout << std::boolalpha;  
       //cout<< ((lastPosition-(int)sizeof(a))< numberSlot*sizeof(int));
       //if ((lastPosition-(int)sizeof(a)) < numberSlot*sizeof(int)) { cout<<"overrrrrrrrrrflow";return -1;   }              // overflow, not possible to add more
       if (temp1<temp2) {
        //    cout<<"block overflow \n";
           return -1;}
       int newslotId=0;
        while (newslotId<numberSlot && *((int *)(m+4*newslotId))!=0)      // find the empty one 
           {newslotId+=1;
           }
        //cout<<newslotId<<",,,,";
           
       memcpy(m+lastPosition-sizeof(a),&a,sizeof(a));
       lastPosition-=sizeof(a);
       //cout<<"last position is "<< lastPosition;
       memcpy(m+4*newslotId,&lastPosition,sizeof(lastPosition));
       //*((int *)(m+4*numberSlot)) = lastPosition;
       //
       //*((int *)(m+4*numberSlot)) = lastPosition;
       //m[numberSlot-1]=lastPosition;
       if (!(newslotId<numberSlot)) numberSlot+=1;
       return newslotId;
   }

   Record getRecord(int slotId)
   {
       int lastPosition=*((int *)(m+4*slotId));
       return  *(Record *)(m+lastPosition);
   }

   void deleteSlot(int slotId)
   {    int oldLastposition=*((int *)(m+4*slotId));

   memmove( m+lastPosition+sizeof(Record), m+lastPosition, oldLastposition-lastPosition);
   lastPosition+=sizeof(Record);
        for(int i=slotId+1;i<numberSlot;i++)
        {
            int tempPosition=*((int *)(m+4*i));
            //cout<<"old"<<tempPosition;
            if (tempPosition!=0) *((int *)(m+4*i))=tempPosition+sizeof(Record); // shift the slot reference by Record id.
            // new 
        }
       *((int *)(m+4*slotId))=0;
       //numberSlot-=1;

   }


   void print()
   {
       for(int i=0;i<numberSlot;i++)
       {
           int temp=*((int *)(m+4*i));
           cout<<temp<<"|";
       }

       for(int i=numberSlot;i<lastPosition;i++)
       {

           cout<<".";
       }

        cout<<"\n";

       for(int i=lastPosition;i+sizeof(Record)<=98;i+=(sizeof(Record)))
       {
           Record temp= *(Record *) (m+i);
           cout<<temp.toString();
       }
   }

    

};

