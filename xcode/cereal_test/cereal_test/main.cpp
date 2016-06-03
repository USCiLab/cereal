//
//  main.cpp
//  cereal_test
//
//  Created by Joseph Laurino on 6/3/16.
//  Copyright © 2016 Joseph Laurino. All rights reserved.
//

// StackOverflow:
//  how to properly add include paths in xcode
//  http://stackoverflow.com/questions/14134064/how-to-set-include-path-in-xcode-project
//
//  how to get fstream to work in xcode environment
//  http://stackoverflow.com/questions/16779149/c-program-in-xcode-not-outputting-simple-text-file-using-outfile

#include <cereal/archives/binary.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/binary.hpp>
#include <sstream>
#include <iostream>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <time.h>

using namespace std;

struct MyData
{
    int x, y, z;
    
    MyData()
    {
        x = rand() % 100 + 1;
        y = rand() % 100 + 100;
        z = rand() % 100 + 200;
    }
    
    // This method lets cereal know which data members to serialize
    template<class Archive>
    void serialize(Archive & archive)
    {
        archive( x, y, z ); // serialize things by passing them to the archive
    }
    
    void print(string info)
    {
        cout << info << x << ", " << y << ", " << z << std::endl;
    }
};


void test_binary()
{
    {
        std::ofstream os("data.bin");
        cereal::BinaryOutputArchive oarchive(os); // Create an output archive
        
        MyData m1, m2, m3;
        oarchive(m1, m2, m3); // Write the data to the archive
        
        cout << "data serialized" << endl;
        m1.print("m1: ");
        m2.print("m2: ");
        m3.print("m3: ");
    }
    
    {
        std::ifstream is("data.bin");

        cereal::BinaryInputArchive iarchive(is); // Create an input archive
        
        MyData m1, m2, m3;
        iarchive(m1, m2, m3); // Read the data from the archive
        
        cout << "data deserialized" << endl;
        m1.print("m1: ");
        m2.print("m2: ");
        m3.print("m3: ");
    }
}


void test_json()
{
    {
        std::ofstream os("data.json");
        cereal::JSONOutputArchive archive(os);
        
        MyData m1;
        int someInt;
        double d;
        
        m1.x = 100;
        someInt = 200;
        d = 3.14;
        
        archive( CEREAL_NVP(m1), // Names the output the same as the variable name
                someInt,        // No NVP - cereal will automatically generate an enumerated name
                cereal::make_nvp("this name is way better", d) ); // specify a name of your choosing
    }
    
    {
        std::ifstream is("data.json");
        cereal::JSONInputArchive archive(is);
        
        MyData m1;
        int someInt;
        double d;
        
        archive( m1, someInt, d ); // NVPs not strictly necessary when loading
        
        cout << "json data read" << endl;
        m1.print("m1: ");
        cout << someInt << " " << d << std::endl;
        // but could be used (even out of order)
    }
}


int main()
{
    srand (time(NULL));
    
    test_binary();
    test_json();
    
    return 0;
}


