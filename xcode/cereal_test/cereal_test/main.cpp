//
//  main.cpp
//  cereal_test
//
//  Created by Joseph Laurino on 6/3/16.
//  Copyright © 2016 Joseph Laurino. All rights reserved.
//

// StackOverflow:
// how to properly add include paths in xcode
// http://stackoverflow.com/questions/14134064/how-to-set-include-path-in-xcode-project

#include <cereal/archives/binary.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/xml.hpp>
#include <cereal/archives/json.hpp>

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


#include <cereal/archives/binary.hpp>
#include <sstream>

// StackOverflow
// how to get fstream to work in xcode environment
// http://stackoverflow.com/questions/16779149/c-program-in-xcode-not-outputting-simple-text-file-using-outfile

#include <fstream>
#include <iostream>

using namespace std;

int main()
{

    
    {
        std::ofstream os("data.bin");
        cereal::BinaryOutputArchive oarchive(os); // Create an output archive
        
        MyData m1, m2, m3;
        m1.x = 100;
        m2.x = 200;
        m3.x = 300;
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


