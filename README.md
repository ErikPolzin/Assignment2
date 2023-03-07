# PGM frame extractor

Extracts smaller subsections ('frames') from a larger PGM image, and outputs them to new files.
The frames are loaded in small increments, effectively panning through the large PGM image along a polyline.
They can be stitched together with FFMPEG to generate a video.

## Build instructions

Run `make extractor` (or just `make`) inside the extracted folder. An executable should be built, to be run as `./extractor`.

Run `make clean` to remove the generated object files.

Run `ffmpeg -y -framerate 10 -i "<output_dir>/<output_name>-%05d.pgm" -pix_fmt rgb24 -vcodec png <video_name>.mp4` to generate the video.


## Command-line interface

The CLI has the format `extractor <input_file>.pgm <FLAGS>` where FLAGS are:

- s: frame_width, frame_height
- t: X1, Y1, X2, Y2
- p: N, X1, Y1, ..., XN, YN
- w: output_name, output_modifier ['none', 'invert', 'reverse', 'revinvert']
- o: output_directory


## Files

- `extractor.h`: Definitions, including Frame, FrameSequence, PGMMetadata and OutputSpec classes.
- `extractor.cpp`: Implements definitions and command line parser.
- `Makefile`: Build instructions for `make`
- `README.md`: Build instructions for you :)