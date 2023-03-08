#include <iostream>
#include <string.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <cstdlib>
#include <chrono>
#include "extractor.h"


int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Specify path to PGM file" << std::endl;
        return 1;
    }
    std::vector<int> points_array;
    std::vector<PLZERI001::OutputSpec> outputs;
    std::string dir = "";
    int framew = 0; int frameh = 0;
    
    for (int i = 1; i < argc; i++)
	{
		if (strcmp("-t", argv[i]) == 0)
        {
            points_array.push_back(std::abs(std::stoi(argv[++i])));
            points_array.push_back(std::abs(std::stoi(argv[++i])));
            points_array.push_back(std::abs(std::stoi(argv[++i])));
            points_array.push_back(std::abs(std::stoi(argv[++i])));
        }
        if (strcmp("-p", argv[i]) == 0)
        {
            int N = std::stoi(argv[++i]);
            for (int n=0; n<N; ++n)
            {
                points_array.push_back(std::abs(std::stoi(argv[++i])));
                points_array.push_back(std::abs(std::stoi(argv[++i])));
            }
        }
		else if (strcmp("-s", argv[i]) == 0)
        {
            framew = std::abs(std::stoi(argv[++i]));
            frameh = std::abs(std::stoi(argv[++i]));
        }
        else if (strcmp("-w", argv[i]) == 0)
        {
            std::string operation = argv[++i];
            unsigned short int op;
            if (operation == "none") op = NOOP;
            else if (operation == "invert") op = INVOP;
            else if (operation == "reverse") op = REVOP;
            else if (operation == "revinvert") op = (INVOP | REVOP);
            else
            {
                std::cout << "Unknown frame operation " << operation;
                continue;
            }
            PLZERI001::OutputSpec spec;
            spec.name = argv[++i];
            spec.operation = op;
            outputs.push_back(spec);
        }
        // Useful output directory so that the source directory doesn't get
        // cluttered with .pgm files while testing.
        else if (strcmp("-o", argv[i]) == 0) dir = argv[++i];
	}
    if (framew <= 0 || frameh <= 0)
    {
        std::cout << "Invalid frame width/height for frame\n";
        return 1;
    }
    PLZERI001::FrameSequence frameSequence;
    auto t1 = std::chrono::high_resolution_clock::now();
    int nframes = frameSequence.generate_frames(argv[1], frameh, framew, points_array, outputs, dir);
    auto t2 = std::chrono::high_resolution_clock::now();
    auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    std::cout << "Wrote " << nframes << " frames in " << ms_int.count() << "ms\n";
    return 0;
}