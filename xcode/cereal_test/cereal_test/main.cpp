//
//  main.cpp
//  cereal_test
//
//  Created by Joseph Laurino on 6/3/16.
//
//  cereal docs
//      http://uscilab.github.io/cereal/
//
//  StackOverflow:
//      how to properly add include paths in xcode?
//      use $(SRCROOT) like so $(SRCROOT)/../../include in Header Search Paths under Build Settings
//
//      how to get fstream to work in xcode environment?
//      http://stackoverflow.com/questions/16779149/c-program-in-xcode-not-outputting-simple-text-file-using-outfile
//
#include <cereal/archives/binary.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/binary.hpp>

// type support -- needed for serializing std types
#include <cereal/types/map.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/complex.hpp>

#include <sstream>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <memory>
#include <vector>
#include <string>

using namespace std;
using namespace cereal;

// ------------------------------------------------------------------
struct MyData
{
    string name;
    int x, y, z;
    vector<shared_ptr<MyData>> children;
    
    MyData()
    {
        name = "temp_name";
        x = rand() % 100 + 1;
        y = rand() % 100 + 100;
        z = rand() % 100 + 200;
    }
    
    // This method lets cereal know which data members to serialize
    template<class Archive>
    void serialize(Archive & archive)
    {
        archive( CEREAL_NVP(name), CEREAL_NVP(x), CEREAL_NVP(y), CEREAL_NVP(z), CEREAL_NVP(children) ); // serialize things by passing them to the archive
    }
    
    void print(string info, int level)
    {
        cout << info << name << " " <<  x << ", " << y << ", " << z << std::endl;
        for (auto &child : children)
        {
            for(int i = 0; i < level; i++)
                cout << "\t";
            child->print("child: ", level+1);
        }
    }
};

// ------------------------------------------------------------------
void test_binary()
{
    cout << "test_binary" << endl;
    {
        ofstream os("data.bin");
        BinaryOutputArchive oarchive(os); // Create an output archive
        
        MyData m1, m2, m3;
        oarchive(m1, m2, m3); // Write the data to the archive
        
        cout << "data serialized" << endl;
        m1.print("m1: ", 0);
        m2.print("m2: ", 0);
        m3.print("m3: ", 0);
    }
    
    {
        ifstream is("data.bin");
        BinaryInputArchive iarchive(is); // Create an input archive
        
        MyData m1, m2, m3;
        iarchive(m1, m2, m3); // Read the data from the archive
        
        cout << "data deserialized" << endl;
        m1.print("m1: ", 0);
        m2.print("m2: ", 0);
        m3.print("m3: ", 0);
    }
}
// ------------------------------------------------------------------
void printJson(string filename)
{
    std::ifstream is(filename);
    
    std::string temp;
    while(std::getline(is, temp)) {
        cout << temp << endl;
    }
}
// ------------------------------------------------------------------
void test_json()
{
    cout << "test_json" << endl;
    {
        ofstream os("data.json");
        JSONOutputArchive archive(os);
        
        MyData m1;
        
        m1.name = "hello";
        
        shared_ptr<MyData> m1Child = make_shared<MyData>();
        m1Child->name = "world";
        
        m1Child->children.push_back(make_shared<MyData>());
        
        m1.children.push_back(m1Child);
        m1.children.push_back(make_shared<MyData>());
        
        int someInt;
        double d;
        
        m1.x = 100;
        someInt = 200;
        d = M_PI;
        
        archive(CEREAL_NVP(m1),         // Names the output the same as the variable name
                someInt,                // No NVP - cereal will automatically generate an enumerated name
                make_nvp("PI", d) );    // specify a name of your choosing
    }
    
    cout << "json data serialized" << endl;
    printJson("data.json");
    
    {
        ifstream is("data.json");
        JSONInputArchive archive(is);
        
        MyData m1;
        int someInt;
        double d;
        
        archive( m1, someInt, d ); // NVPs not strictly necessary when loading
        
        cout << "json data deserialized" << endl;
        m1.print("m1: ", 0);
        cout << "someInt: " << someInt << endl << "d: " << d << std::endl;
    }
}
// ------------------------------------------------------------------
int main()
{
    srand (time(NULL));
    
    test_binary();
    test_json();
    
    return 0;
}



