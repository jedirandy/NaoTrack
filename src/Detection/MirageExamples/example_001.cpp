/*
  Compilation : Use the pkg-config utility !

  g++ -c -O3 `pkg-config --cflags mirage` toto.cc
  g++ -c ...
  g++ -c ...
  g++ -c ...
  g++ -o prog.exe *.o `pkg-config --libs mirage`

*/

#include <mirage.h>
#include <string>
#include <iostream>
#include <cmath>

// These are typedefs for the definition of two 2D image types. The
// first is an image made of RGB values, the second an image made of
// double.
typedef mirage::img::Coding<mirage::colorspace::RGB_24>::Frame ImageRGB; 
typedef mirage::img::Coding<double>::Frame                     ImageDouble;  

#define WIDTH  640
#define HEIGHT 480

// This is a function that associate a value at each pixel
// position. Positions in mirage are vectors. Actually, it is more
// easy to use here a specification of nD vectors to 2D vectors. This
// is what Coordinates are. Everything that belongs to namespace
// mirage::img is a specification of general mirage types to 2D
// images, allowing to simplify the notations.
double valueAt(const mirage::img::Coordinate& pos) {
  return exp(-.00005*(pos*pos))*sin(.02*pos[0]);
}

// This is a convertor that transforms a value in [-1,1] into a color
// (blue for -1, white for 0, and red for 1).
mirage::colorspace::RGB_24 doubleToColor(double value) {
  value *= 255;
  if(value >= 0)
    return mirage::colorspace::RGB_24(255,
                                      (unsigned char)(255-value),
                                      (unsigned char)(255-value));
  else
    return mirage::colorspace::RGB_24((unsigned char)(255+value),
                                      (unsigned char)(255+value),
                                      255);
    
}

int main(int argc, char* argv[]) {

  try {
    // We have to initialize some internal static stuff.
    mirage::Init();

    ImageDouble             img(mirage::img::Coordinate(WIDTH,HEIGHT));
    mirage::img::Coordinate origin(img.center());
    ImageRGB                output_image;
    std::string             output_name("gabor.jpg");

    // Initializing an image with a constant pixel value is easy in
    // mirage.
    img = 0; 

    // Let us iterate on the pixels in the image.
    ImageDouble::pixel_type p1,p1_end;
    for(p1=img.begin(),p1_end=img.end();
        p1 != p1_end;
        ++p1) // ++p1 is much more efficient than p1++
      *p1 = valueAt(!p1 - origin); // *p1 is the value, !p1 is its position.
    
    output_image.resize(img._dimension);

    ImageRGB::pixel_type p2;
    for(p1=img.begin(),p1_end=img.end(),p2=output_image.begin();
        p1 != p1_end;
        ++p1,++p2)
      *p2 = doubleToColor(*p1);

    mirage::img::JPEG::write(output_image,output_name,70);
    std::cout << output_name << " generated." << std::endl;
  }
  catch(mirage::Exception::Any& e) {
    std::cerr << "Error : " <<  e.what() << std::endl;
  }
  catch(...) {
    std::cerr << "Unknown error" << std::endl;
  }

  return 1;
}
