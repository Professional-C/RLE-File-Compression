#include "RleFile.h"
#include "RleData.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>

void RleFile::CreateArchive(const std::string& source)
{
    // Requires <fstream>
    std::ifstream::pos_type size;
    char* memblock;
    // Open the file for input, in binary mode, and start ATE (at the end)
    std::ifstream file (source, std::ios::in|std::ios::binary|std::ios::ate);
    if (file.is_open()){
        
        size = file.tellg(); // Save the size of the file
        memblock = new char[static_cast<unsigned int>(size)];
        file.seekg(0, std::ios::beg); // Seek back to start of file
        file.read(memblock, size); // File data is now in memblock array
        mRleData.Compress(memblock, size);
        delete[] memblock;
        file.close();
        
        // Write to file
        std::string outFile = source + ".rl1";
        mHeader.mFileNameLength = static_cast<unsigned char>(source.size());
        mHeader.mFileName = source;
        mHeader.mFileSize = size;
        std::ofstream arc(outFile, std::ios::out|std::ios::binary|std::ios::trunc);
        if (arc.is_open())
        {
            arc.write(mHeader.mSig, 4);
            arc.write(reinterpret_cast<char*>(&(mHeader.mFileSize)),4);
            arc.write(reinterpret_cast<char*>(&(mHeader.mFileNameLength)),1);
            arc.write(mHeader.mFileName.c_str(),mHeader.mFileNameLength);
            arc.write(mRleData.mData,mRleData.mSize);
        }
    }

}

void RleFile::ExtractArchive(const std::string& source)
{
    // Requires <fstream>
    std::ifstream::pos_type size;
    char* memblock;
    // Open the file for input, in binary mode, and start ATE (at the end)
    std::ifstream file (source, std::ios::in|std::ios::binary|std::ios::ate);
    if (file.is_open()){
        
        size = file.tellg(); // Save the size of the file
        memblock = new char[static_cast<unsigned int>(size)];
        file.seekg(0, std::ios::beg); // Seek back to start of file
        file.read(memblock, size);
        
        
        //Write to file
        mHeader.mFileSize = *(reinterpret_cast<int*>(&memblock[4]));
        mHeader.mFileNameLength = *(reinterpret_cast<int*>(&memblock[8]));
        std::string outFile;
        for(int i = 9; i < 9+mHeader.mFileNameLength; i++){
            outFile += memblock[i];
        }
        mRleData.Decompress(&memblock[9+mHeader.mFileNameLength], static_cast<size_t>(size) - (9+mHeader.mFileNameLength), mHeader.mFileSize);
        file.close();
        std::ofstream arc(outFile, std::ios::out|std::ios::binary|std::ios::trunc);
        if (arc.is_open())
        {
            arc.write(mRleData.mData,mRleData.mSize);
        }
        delete[] memblock;
        
    }
}
