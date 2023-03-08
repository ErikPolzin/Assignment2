#include <iostream>
#include <string.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <cmath>
#include "extractor.h"

namespace PLZERI001 {

Frame::Frame(int w, int h, PGMMetadata * imd):
    inputMdata(imd), width(w), height(h), x(-1), y(-1), inverted(false)
{
    // Dynamically allocate memory from the heap
    data = new unsigned char*[height];
    for (int i = 0; i < height; ++i) data[i] = new unsigned char[width];
};

// Copying frames isn't necessary when simply updating a single frame.
// Included to conform to assignment spec.
Frame::Frame(const Frame &frame)
{
    height = frame.height;
    width = frame.width;
    x=frame.x; y=frame.y;
    inputMdata = frame.inputMdata;
    // Deep copy frame data
    data = new unsigned char*[height];
    for (int i = 0; i < height; ++i){
        data[i] = new unsigned char[width];
        std::copy(frame.data[i], frame.data[i]+frame.width, data[i]);
    }
}

Frame::~Frame()
{
    // Clean up memory allocated to this frame
    for (int i = 0; i < this->height; ++i) delete [] data[i];
    delete [] data;
};

/// Unused, purely to conform to assignment spec using FrameSequence[i][row][col]
unsigned char * Frame::operator[](int idx) {return data[idx];};

void Frame::setOrigin(int _x, int _y) {x = _x; y = _y;};

void Frame::setInverted(bool inv) {inverted=inv;};

std::ostream &operator<<(std::ostream& stream, const Frame& f)
{
    for (int r = 0; r < f.height; ++r)
    {
        for (int c = 0; c < f.width; ++c)
        {
            if (f.inverted) stream << char(255 - f.data[r][c]);
            else stream << f.data[r][c];
        }
    }
    return stream;
}

std::ifstream &operator>>(std::ifstream& stream, Frame& f)
{
    const int maxpos = f.inputMdata->file_height*f.inputMdata->file_width;
    int pos = f.inputMdata->data_offset+f.y*f.inputMdata->file_width+f.x;
    for (int i = 0; i < f.height; ++i)
    {
        // Wrap position so that out-of-bounds coordinates don't crash
        pos = pos % maxpos;
        stream.seekg(pos);
        stream.read((char*) f.data[i], f.width);
        pos += f.inputMdata->file_width;
    }
    return stream;
}

std::ostream &operator<<(std::ostream& stream, const PGMMetadata& md)
{
    stream << "P5" << std::endl << md.file_width << ' ' << md.file_height 
        << std::endl << "255" << std::endl;
    return stream;
}

std::ifstream &operator>>(std::ifstream& stream, PGMMetadata& md)
{
    std::string line;
    std::getline(stream, line);
    while (stream.peek() == '#') std::getline(stream, line);
    stream >> md.file_width;
    stream >> md.file_height >> std::ws;
    std::getline(stream, line);
    md.data_offset = stream.tellg();
    return stream;
}

std::string OutputSpec::file_name (int frame_index)
{
    if (operation & REVOP) frame_index = MAX_FRAMES - frame_index;
    std::stringstream fn;
    fn << name << "-" << std::setfill('0') << std::setw(5) << frame_index << ".pgm";
    return fn.str();
}

int FrameSequence::generate_frames(
    std::string inputFileName, int frameh, int framew, std::vector<int> points_array,
    std::vector<OutputSpec> outputs, std::string dir)
{
    std::ifstream inputFile(inputFileName, std::ios::binary);
    if (!inputFile)
    {
        std::cout << "Cannot open file at " << inputFileName << std::endl;
        return 0;
    }
    std::string fp;
    PLZERI001::OutputSpec output;
    PLZERI001::PGMMetadata input_meta;
    PLZERI001::PGMMetadata output_meta(framew, frameh);
    inputFile >> input_meta;  // Parse input metadata from file
    // Create a single frame (with fixed width/height/input metadata).
    PLZERI001::Frame frame = PLZERI001::Frame(framew, frameh, &input_meta);
    int x1, x2, y1, y2;
    double x, y, dx, dy, spd, tot_dist, dist, prog;
    int fileidx = 0; int nframes = 0;
    // Iterate through legs, i.e. (x1,y1,x2,y2), (x2,y2,x3,y3), (x3,y3,x4,y4), ...
    int npoints = points_array.size()/2-1;
    for (int i=0; i<npoints; ++i)
    {
        x1 = points_array[i*2]; y1 = points_array[i*2+1];
        x2 = points_array[i*2+2]; y2 = points_array[i*2+3];
        tot_dist = std::sqrt(std::pow(x2-x1, 2)+std::pow(y2-y1, 2));
        x = x1; y = y1;
        dx = double(x2-x1)/tot_dist; dy = double(y2-y1)/tot_dist;
        do {
            dist = std::sqrt(std::pow(x-x1, 2)+std::pow(y-y1, 2));
            prog = dist / tot_dist;
            spd = MAX_PAN_SPEED*(prog*(1-prog))+1;  // Make sure speed is non-zero
            frame.setOrigin(x, y);
            try {
                inputFile >> frame;  // Populate frame's pixel data from input file
                // Unnecessary line of code that hogs a lot of memory.
                // Keeping track of each frame in a vector uses 16 TIMES the memory when running the
                // smallest example (and will use much more with examples with larger/more frames).
                // It's not necessary to store frames when a single global frame can be updated and then
                // written to disk on-the-fly at every iteration of the loop.
                // Functionality is the same when commenting out this line (but it's included to
                // conform to assignment spec)
                imageSequence.push_back(PLZERI001::Frame(frame));
            } catch (std::bad_alloc& e) {
                // Maybe not very efficient to catch exceptions, but easier and
                // cleaner than checking each new array sub-item for std::nothrow nullptrs.
                std::cout << "Unable to alloc. memory for frame " << i << std::endl;
                continue;
            }
            // Write frame to each of the specified output profiles.
            for (int k = 0; k < outputs.size(); k++)
            {
                output = outputs[k];
                fp = output.file_name(fileidx++);
                if (!dir.empty()) fp = dir + "/" + fp;
                std::ofstream outfile(fp, std::ios::binary);
                if (!outfile)
                {
                    std::cout << "Unable to open output file at " << fp << std::endl;
                    continue;
                }
                outfile << output_meta;
                frame.setInverted(output.operation & INVOP);
                outfile << frame;  // Write frame to output file.
                outfile.close();
                nframes++;
            }
            std::cout << "Wrote file " << nframes << "\r";
            x+=dx*spd; y+=dy*spd;
        } while (dist<tot_dist && nframes <= MAX_FRAMES);
        // Constrain total number of frames so that the filesystem isn't completely
        // fried if the acceleration algorithm goes into an infinite loop.
    }
    return nframes;
}
}
