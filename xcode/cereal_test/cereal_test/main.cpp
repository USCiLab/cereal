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

using namespace std;


struct MyData
{
    int x, y, z;
    
    // This method lets cereal know which data members to serialize
    template<class Archive>
    void serialize(Archive & archive)
    {
        archive( x, y, z ); // serialize things by passing them to the archive
    }
};


void test_binary()
{
    {
        std::ofstream os("data.bin");
        cereal::BinaryOutputArchive oarchive(os); // Create an output archive
        
        MyData m1, m2, m3;
        m1.x = 101;
        m2.x = 202;
        m3.x = 303;
        oarchive(m1, m2, m3); // Write the data to the archive
    }
    
    {
        std::ifstream is("data.bin");

        cereal::BinaryInputArchive iarchive(is); // Create an input archive
        
        MyData m1, m2, m3;
        iarchive(m1, m2, m3); // Read the data from the archive
        
        cout << m1.x << " " << m2.x << " " << m3.x << std::endl;
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
        
        cout << m1.x << " " << someInt << " " << d << std::endl;
        // but could be used (even out of order)
    }
}


int main()
{
    test_binary();
    test_json();
    
    return 0;
}


