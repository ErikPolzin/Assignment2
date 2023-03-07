#ifndef EXTRACTOR_H
#define EXTRACTOR_H

#include <iostream>
#include <string.h>
#include <fstream>
#include <vector>

#define MAX_FRAMES 2000
#define MAX_PAN_SPEED 5
// Output spec modifiers, as a bit field
#define NOOP 0
#define INVOP 1
#define REVOP 2

namespace PLZERI001 {

/// @brief Output specification for a sequence of frames
/// Describes output name and frame operations (like inverting, reversing etc)
struct OutputSpec {

    std::string name;
    unsigned short int operation;

    /// @brief Generate a file name for a frame at a given index
    /// @param frame_index Frame index
    /// @return File name
    std::string file_name (int frame_index);
};

/// @brief Metadata for a PGM file
class PGMMetadata {

    public:
        int file_width, file_height, data_offset;
        PGMMetadata() = default;
        PGMMetadata(int fw, int fh): file_width(fw), file_height(fh) {};
    
        /// @brief Write PGM file metadata to a stream
        /// @param stream Output stream
        /// @param md PGM metadata
        /// @return Same stream
        friend std::ostream& operator<<(std::ostream& stream, const PGMMetadata& md);
        
        /// @brief Parse PGM metadata from the first lines of a file
        /// @param stream Input file
        /// @param md PGM metadata
        /// @return Same stream
        friend std::ifstream& operator>>(std::ifstream& stream, PGMMetadata& md);

};

/// @brief Holds data loaded from a subsection of a pgm file
class Frame {

    private:
        int width, height, x, y, inverted;
        PGMMetadata *inputMdata;
        char ** data;

    public:
        /// @brief Initialize a frame with a set width, height and file metadata
        /// @param w Frame width
        /// @param h Frame height
        /// @param imd Input file metadata
        Frame(int w, int h, PGMMetadata * imd);

        /// @brief Deep copy a frame's data
        /// @param frame Source frame
        Frame(const Frame &frame);

        /// @brief Destroy frame, freeing pixel data
        ~Frame();

        /// @brief Set the coordinate origin in the input file
        /// @param _x x coordinate
        /// @param _y y coordinate
        void setOrigin(int _x, int _y);

        /// @brief Force this frame to invert/uninvert its colours
        /// @param inv Invert colours
        void setInverted(bool inv);

        /// @brief Return char data at a given index
        char * operator[](int idx);

        /// @brief Write data from a frame to an output stream (like a file)
        /// @param stream Output stream
        /// @param f Frame object
        /// @return The same output stream
        friend std::ostream& operator<<(std::ostream& stream, const Frame& frame);

        /// @brief Read data from an input stream into a frame
        /// @param stream Input stream
        /// @param frame Frame object
        /// @return The same input stream
        friend std::ifstream& operator>>(std::ifstream& stream, Frame& frame);
};

// Not really necessary. Included to conform to assignment spec
typedef std::vector<Frame> FrameSequence;
}
#endif
