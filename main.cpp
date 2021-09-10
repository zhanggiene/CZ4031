#include <iostream> 
#include <string> 
#include "Database.cpp"
 
int main(){
    int n = 7;
    Database database("tsv_files/data.tsv", n);
    // Database database("tsv_files/testing3.tsv",3);

    int choice;
    cout << "Which experiment result to view? (Type 1-7)\n";
    cout << "Experiment 1: Store data in disk \n";
    cout << "Experiment 2: Build a B+ tree on the attribute 'numVotes'\n";
    cout << "Choice: ";
    cin >> choice;

    while (cin.fail() || choice < 1 || choice > 6 ) {
        cout << "Please insert a valid integer from 1-6!\n";
        cout << "Which experiment result to view? (Type 1-6)\n";
        cin.clear();
        cin.ignore(256,'\n');
        cin >> choice;
    }
    switch(choice){
        case 1:
            cout << "Experiment 1: Store data in disk\n";
            database.addAllRecordsWithNoIndex();
            // database.printBlocks(); //for debugging
            break;
        case 2: 
            cout << "Experiment 2: Build a B+ tree on the attribute 'numVotes'\n";
            database.addToDiskAndBplus();
            database.experiment2();
            // database.printTree(); //for debugging
            break;
        default:
            cout << "Not coded yet";
            break;
    }
    
    return 0;
}

// int main() 
// { 
//     /*  testing for record 
//     Record a=Record("tt00000a	5.3   	1807");
//     Record b=Record("tt00000b	5.3   	1807");
//     Record c=Record("tt00000c	5.3   	1807");
//     Record d=Record("tt00000d	5.3   	1807");
//     Block block=Block();
//     //cout<<a.toString();
//     block.add(a);
//     //block.add(b);
//     //block.add(c);
//     //block.add(d);
//     //block.deleteSlot(3);
//     //block.deleteSlot(0);
//     //cout<<block.numberSlot;
//     block.add(b);
//     //block.add(c);
//     //block.deleteSlot(0);
//      //block.deleteSlot(1);
//     //block.add(b);
//     //cout<<block.add(b);
    
//     cout<<block.add(c);
//     cout<<block.add(b);
//     cout<<block.add(b);
//     //cout<<block.add(b);
//     //block.deleteSlot(1);
//     block.print();
//     //cout<<block.add(c)<<"_______";
//     //cout<<block.get(2).toString();
//     */


   
//     //testing for disk
//     /*
//     Disk d=Disk();
//     d.insert("tt00000a	5.3   	1807");
//     d.insert("tt00000a	5.3   	1807");
//     d.insert("tt00000a	5.3   	1807");
//     d.insert("tt00000a	5.3   	1807");
//     d.insert("tt00000a	5.3   	1807");
//     d.insert("tt00000a	5.3   	1807");
//     d.insert("tt00000a	5.3   	1807");
//     d.insert("tt00000a	5.3   	1807");
//     d.insert("tt00000a	5.3   	1807");
//     d.insert("tt00000a	5.3   	1807");
//     d.insert("tt00000a	5.3   	1807");
//     d.insert("tt00000a	5.3   	1807");
//     d.insert("tt00000a	5.3   	1807");
//     d.insert("tt00000a	5.3   	1807");
//     d.insert("tt00000a	5.3   	1807");
//     d.insert("tt00000a	5.3   	1807");
//     d.insert("tt00000a	5.3   	1807");
//     d.insert("tt00000a	5.3   	1807");
//     d.insert("tt00000a	5.3   	1807");
//     d.insert("tt00000a	5.3   	1807");
//     d.insert("tt00000a	5.3   	1807");
//     d.insert("tt00000a	5.3   	1807");
//     d.insert("tt00000a	5.3   	1807");
//     d.insert("tt00000a	5.3   	1807");
//     d.insert("tt00000a	5.3   	1807");
//     d.insert("tt00000a	5.3   	1807");
//     d.insert("tt00000a	5.3   	1807");
//     d.insert("tt00000a	5.3   	1807");
//     d.printAllRecord();
//     cout<<"size is "<<sizeof(d.blocks[0])*d.getTotalBlocks();
//     */

    
//     return 0; 
// }