#include <mirage.h>


typedef mirage::img::Coding<mirage::colorspace::RGB_24>::Frame ImageRGB24;

// We labelize pixels for which the green value is over 127.
class Test {
public:
  bool operator()(const ImageRGB24::value_type& rgb) {
    return rgb._green > 127;
  }
};

// The mask used as last template parameter defines which extra computation you want to perform. Use mirage::algo::labelNone at least.
typedef mirage::algo::Labelizer<ImageRGB24,Test,
                                mirage::algo::labelNone
                                | mirage::algo::labelPixelSet
                                | mirage::algo::labelBoundingBox> Labelizer;

#define CHECKER_SIZE 5
#define NB_LABEL_COLORS 21
ImageRGB24::value_type colors[NB_LABEL_COLORS];

ImageRGB24::value_type colorOfLabel(Labelizer::label_type lbl) {
  if(lbl==0)
    return colors[0];
  else
    return colors[(lbl-1)%(NB_LABEL_COLORS-1)+1];
}

int main(int argc, char* argv[]) {

  ImageRGB24 source;
  ImageRGB24 display;
  Labelizer labelizer;

  
  // Let us define a set of colors.
  colors[ 0](255,255,255);
  colors[ 1](255,000,000);
  colors[ 2](255,255,000);
  colors[ 3](000,255,000);
  colors[ 4](000,255,255);
  colors[ 5](000,000,255);
  colors[ 6](255,000,255);
  colors[ 7](255,127,127);
  colors[ 8](255,255,127);
  colors[ 9](127,255,127);
  colors[10](127,255,255);
  colors[11](127,127,255);
  colors[12](255,127,255);
  colors[13](127,000,000);
  colors[14](127,127,000);
  colors[15](000,127,000);
  colors[16](000,127,127);
  colors[17](000,000,127);
  colors[18](127,000,127);
  colors[19](127,127,127);
  colors[20](000,000,000);


  std::string image_dir;
  if(argc!=2)
    {
      std::cout << "Usage :" << std::endl
                << "  " << argv[0] << " <directory>" << std::endl
                << "ex : " << argv[0] << " /usr/share/mirage-images" << std::endl;
      return 1;
    }
  image_dir = argv[1];

  try {
    mirage::img::JPEG::read(source,image_dir+"/labels.jpg");

    // Let us now labelize the image.
    labelizer.neighborhoodSurround(); // 8 neighbors considered
    labelizer(source);

    std::cout << "I have found " << labelizer.nb_labels 
              << " connected components. Each corresponds to a number in [1.." 
              << labelizer.nb_labels << "] for 8-neighbors." << std::endl;
    
    
    // Now, we can use labels for whatever we want. 

    // Let us draw labels on the display image.
    ImageRGB24::pixel_type pix,pix_end;
    display.resize(source._dimension);
    for(pix=display.begin(), pix_end=display.end();
        pix!=pix_end;
        ++pix)
      *pix = colorOfLabel(labelizer(!pix));

    // Since the labelPixelSet flag is raised, we can retrieve the
    // coordinates for any label. Let us do this for label 1. We will draw
    // a checker texture on it. In the following, = isn't an
    // affectation (that would be quite slow) but a reference
    // initialization.
    const Labelizer::coord_list_type& coords = labelizer.coordinates(1);
    Labelizer::coord_list_type::const_iterator iter,iter_end;
    mirage::img::Coordinate coord;
    for(iter = coords.begin(), iter_end = coords.end();
        iter != iter_end;
        ++iter) {
      coord = *iter;
      if( (coord[0]/CHECKER_SIZE)%2 == (coord[1]/CHECKER_SIZE)%2 )
        display[coord] = ImageRGB24::value_type(130,130,130);
      else
        display[coord] = ImageRGB24::value_type(90,90,90);
    }

    // Since the flag labelBoundingBox is raised, we can retrieve the
    // bounding box of any label. Let us do this for label 1. We draw
    // it on the display picture.
    mirage::img::Line<ImageRGB24> line;
    const Labelizer::BoundingBox& box = labelizer.boundingBox(1);
    mirage::img::Coordinate A,B,C,D;

    line << display;
    A = box.min();
    C = box.max();
    B(C[0],A[1]);
    D(A[0],C[1]);
    
    line(A,B,false,false);
    line = ImageRGB24::value_type(0,255,0);
    line(B,C,false,false);
    line = ImageRGB24::value_type(0,255,0);
    line(C,D,false,false);
    line = ImageRGB24::value_type(0,255,0);
    line(D,A,false,false);
    line = ImageRGB24::value_type(0,255,0);
    
    mirage::img::PPM::write(display,"Labels-8.ppm");


    // Let us now re-labelize the image.
    labelizer.neighborhoodAxes(); // 4 neighbors considered
    labelizer(source);
    std::cout << "I have found " << labelizer.nb_labels 
              << " connected components. Each corresponds to a number in [1.." 
              << labelizer.nb_labels << "] for 4-neighbors." << std::endl;
    for(pix=display.begin(), pix_end=display.end();
        pix!=pix_end;
        ++pix)
      *pix = colorOfLabel(labelizer(!pix));
    mirage::img::PPM::write(display,"Labels-4.ppm");
    
  }
  catch(mirage::Exception::Any& e) {
    std::cerr << "Error : " <<  e.what() << std::endl;
  }
  catch(...) {
    std::cerr << "Unknown error" << std::endl;
  }

  return 0;
}
