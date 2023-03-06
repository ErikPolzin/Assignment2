#ifndef EXTRACTOR_H
#define EXTRACTOR_H

#include <iostream>
#include <string.h>
#include <fstream>
#include <vector>

#define MAX_FRAMES 2000
#define NOOP 0
#define INVOP 1
#define REVOP 2

namespace PLZERI001 {

struct OutputSpec {

    std::string name;
    unsigned short int operation;

    std::string file_name (int frame_index);
};

class PGMMetadata {

    public:
        int file_width, file_height, data_offset;
        PGMMetadata() = default;
        PGMMetadata(int fw, int fh): file_width(fw), file_height(fh) {};
    
        friend std::ostream& operator<<(std::ostream& stream, const PGMMetadata& md);
        
        friend std::ifstream& operator>>(std::ifstream& stream, PGMMetadata& md);

};

class Frame {

    private:
        int width, height, x, y, inverted;
        PGMMetadata *inputMdata;
        char ** data;

    public:
        Frame(int w, int h, PGMMetadata * imd);

        ~Frame();

        void setOrigin(int _x, int _y);

        void setInverted(bool inv);

        friend std::ostream& operator<<(std::ostream& stream, const Frame& frame);

        friend std::ifstream& operator>>(std::ifstream& stream, Frame& frame);
};
}
#endif
