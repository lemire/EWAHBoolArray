#include "ewah.h"


int main(void) {
    vector<ulong> ids;
    EWAHBoolArray<uword32> bitset1;
    bitset1.set(1);
    bitset1.set(2);
    bitset1.set(1000);
    bitset1.set(1001);
    bitset1.set(1002);
    bitset1.set(1003);
    bitset1.set(1007);
    bitset1.set(1009);
    bitset1.set(100000);
    cout<<"first bitset : "<<endl;
    ids.clear();
    bitset1.appendRowIDs(ids);
    for(vector<ulong>::const_iterator i = ids.begin(); i!=ids.end(); ++i)
        cout<<*i<<endl;
    cout<<endl;
    EWAHBoolArray<uword32> bitset2;
    bitset2.set(1);
    bitset2.set(3);
    bitset2.set(1000);
    bitset2.set(1007);
    bitset2.set(100000);
    cout<<"second bitset : "<<endl;
    ids.clear();
    bitset2.appendRowIDs(ids);
    for(vector<ulong>::const_iterator i = ids.begin(); i!=ids.end(); ++i)
        cout<<*i<<endl;
    cout<<endl;
    EWAHBoolArray<uword32> orbitset;
    EWAHBoolArray<uword32> andbitset;
    bitset1.rawlogicalor(bitset2,orbitset);
    bitset1.rawlogicaland(bitset2,andbitset);
    // we will display the or
    cout<<"logical and: "<<endl;
    ids.clear();
    orbitset.appendRowIDs(ids);
    for(vector<ulong>::const_iterator i = ids.begin(); i!=ids.end(); ++i)
        cout<<*i<<endl;
    cout<<endl;
    cout<<"memory usage of compressed bitset = "<<orbitset.sizeInBytes()<<" bytes"<<endl;
    cout<<endl;
    // we will display the and
    cout<<"logical or: "<<endl;
    ids.clear();
    andbitset.appendRowIDs(ids);
    for(vector<ulong>::const_iterator i = ids.begin(); i!=ids.end(); ++i)
        cout<<*i<<endl;
    cout<<endl;
    cout<<"memory usage of compressed bitset = "<<andbitset.sizeInBytes()<<" bytes"<<endl;
    cout<<endl;
    return EXIT_SUCCESS;
}