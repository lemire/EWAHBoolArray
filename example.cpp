#include <stdlib.h>
#include "ewah.h"


int main(void) {
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
    for(EWAHBoolArray<uword32>::const_iterator i = bitset1.begin(); i!=bitset1.end(); ++i)
        cout<<*i<<endl;
    cout<<endl;
    EWAHBoolArray<uword32> bitset2;
    bitset2.set(1);
    bitset2.set(3);
    bitset2.set(1000);
    bitset2.set(1007);
    bitset2.set(100000);
    cout<<"second bitset : "<<endl;
    for(EWAHBoolArray<uword32>::const_iterator i = bitset2.begin(); i!=bitset2.end(); ++i)
        cout<<*i<<endl;
    cout<<endl;
    EWAHBoolArray<uword32> orbitset;
    EWAHBoolArray<uword32> andbitset;
    bitset1.logicalor(bitset2,orbitset);
    bitset1.logicaland(bitset2,andbitset);
    // we will display the or
    cout<<"logical and: "<<endl;
    for(EWAHBoolArray<uword32>::const_iterator i = andbitset.begin(); i!=andbitset.end(); ++i)
        cout<<*i<<endl;
    cout<<endl;
    cout<<"memory usage of compressed bitset = "<<andbitset.sizeInBytes()<<" bytes"<<endl;
    cout<<endl;
    // we will display the and
    cout<<"logical or: "<<endl;
    for(EWAHBoolArray<uword32>::const_iterator i = orbitset.begin(); i!=orbitset.end(); ++i)
        cout<<*i<<endl;
    cout<<endl;
    cout<<endl;
    cout<<"memory usage of compressed bitset = "<<orbitset.sizeInBytes()<<" bytes"<<endl;
    cout<<endl;
    return EXIT_SUCCESS;
}
