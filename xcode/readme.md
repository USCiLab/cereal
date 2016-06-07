```cpp
//  Getting cereal samples to work under XCode required some StackOverflow searches...  
//
//  StackOverflow:
//
//      How to properly add include paths in xcode? (to solve can't find <cereal/*.hpp> errors
//
//          use $(SRCROOT) 
//          example: $(SRCROOT)/../../include in Header Search Paths under Build Settings
//          set for Debug and Release
//
//      How to get fstream to work in xcode environment? (to be able to read/write files)
//
//          set working directory by editing scheme, instructions on the link below 
//          http://stackoverflow.com/questions/16779149/c-program-in-xcode-not-outputting-simple-text-file-using-outfile
```