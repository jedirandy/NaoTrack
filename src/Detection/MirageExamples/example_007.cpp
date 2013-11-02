#include <mirage.h>

#define FILTER_SIZE 11
// 3-D fft
typedef mirage::fft::Algo<3> FFT3;

// 2-D fft
typedef mirage::fft::Algo<2> FFT2;

typedef mirage::img::Coding<mirage::colorspace::RGB_24>::Frame ImageRGB24;


class DisplayParam {
public:
  static double maxModule(void) {return 1;}
};
typedef mirage::algo::UnaryOp< FFT2::frame_type,ImageRGB24,mirage::fft::ComplexToRGBOp<DisplayParam> > FFT2RGB;


class RGBtoComplexOp {
public:
  void operator()(const mirage::colorspace::RGB_24& src,mirage::fft::Complex& res) {
    mirage::colorspace::RGBToGray<mirage::colorspace::RGB_24,double> rgb_to_gray;

    rgb_to_gray(src,res.real);
    res.real /= 255;
    res.imaginary = 0;
  }
};
typedef mirage::algo::UnaryOp<ImageRGB24,FFT2::frame_type,RGBtoComplexOp> RGBtoComplex;


// A gabor function 
double gabor(int w, int h, int width, int height) {
  double x = (w-width/2)/(double)(width-1);
  double y = (h-height/2)/(double)(height-1);

  return exp(-8*(x*x+y*y))*sin(8*x);
}

int main(int argc, char* argv[]) {

  // This is a workspace for all FFT manipulations.
  mirage::fft::Workspace workspace;

  // Cnft operations are performed on complex images, that have to be
  // defined as follows.
  FFT3::frame_type             fft3;
  FFT3::frame_type::point_type dim3;

  FFT2::frame_type             fft2;
  FFT2::frame_type             filter;
  FFT2::frame_type             fft_filter;
  FFT2::frame_type             fft_source;
  FFT2::frame_type             tmp;

  // Other variables
  ImageRGB24 display;
  ImageRGB24 source;


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

    // Let is set up a 3D image, with 0 everywhere, except in the middle
    // where a 1 square stands.
    dim3((mirage::Args<3,int>(),100,200,50));
    fft3.resize(dim3);
    fft3=0;
    dim3((mirage::Args<3,int>(),10,10,10));
    mirage::SubFrame<FFT3::frame_type> square(fft3,
                                              fft3._dimension/2 - dim3/2,
                                              dim3);
    square=1;
    
    // Now, we need do declare each dimension of the images. For
    // example, if you have a 3x4x5 3D image, declare 3, 4, and 5 to the
    // workspace. Then if you handle a 7*5 2D image, only 7 needs to be
    // declared to the workspace. Declaring a dimension twice is not a
    // problem. For convenience, you can pass the whole _dimension
    // vector of an image so that the workspace declares the sizes in
    // that vector one by one internally.
    FFT3::declare(workspace,fft3);
    FFT3::transform<mirage::fft::direct>(workspace,fft3); // mirage::fft::inverse is also available.
    
    // Let us now visualize the 3D-FFT, by taking a slice of it, and
    // putting it in a 2D fft image.
    fft2.resize(mirage::img::Coordinate(fft3._dimension[0],fft3._dimension[1]));
    for(FFT2::frame_type::pixel_type pixel = fft2.begin();
        pixel != fft2.end();
        ++pixel)
      *pixel = fft3[dim3((mirage::Args<3,int>(),
                          (!pixel)[0],
                          (!pixel)[1],
                          0))];
    
    // Let us display the result.
    display.resize(fft2._dimension);
    FFT2RGB(fft2,display);
    mirage::img::PPM::write(display,"0-SliceFFT-3D.ppm");

    // So now, let us use 2D fft to illustrate filtering. We set up
    // the filter. Its components are complex, but we can set real
    // values for initializing complexes. Let us define a vertical
    // non-symetrical gabor filter.

    filter.resize(mirage::img::Coordinate(FILTER_SIZE,FILTER_SIZE));
    for(FFT2::frame_type::pixel_type pixel = filter.begin();
        pixel != filter.end();
        ++pixel) 
      *pixel = gabor((!pixel)[0],(!pixel)[1],
                     filter._dimension[0],filter._dimension[1]);
    
    // It is crucial to understand out frame policy effects for
    // filters. Filters are to be considered as having an infinite
    // support. This is why we set the out-frame policy as follows.
    filter.setPolicy(new mirage::ZeroOutFramePolicy<FFT2::frame_type>());
    
    // Let us display the result.
    display.resize(filter._dimension);
    FFT2RGB(filter,display);
    mirage::img::PPM::write(display,"GaborFilter.ppm");

    // Let us filter some image. We first load it.
    mirage::img::JPEG::read(source,image_dir+"/shrek.jpg");

    // Now, for a convolution, we need to build the FFT of the filter,
    // so that is has the size of our image. This can be re-used for
    // any filtering of images having the same size.
    FFT2::makeFilter(workspace,source._dimension,
                     filter,fft_filter);
    display.resize(fft_filter._dimension);
    FFT2RGB(fft_filter,display);
    mirage::img::PPM::write(display,"FFT-GaborFilter.ppm");

    // Let us build a complex image from the source.
    fft_source.resize(source._dimension);
    RGBtoComplex(source,fft_source);

    // Now, we can filter it.
    FFT2::convolution(workspace,fft_source,fft_filter);
    display.resize(fft_source._dimension);
    FFT2RGB(fft_source,display);
    mirage::img::PPM::write(display,"ShrekConvolution.ppm");
    


  }
  catch(mirage::Exception::Any& e) {
    std::cerr << "Error : " <<  e.what() << std::endl;
  }
  catch(...) {
    std::cerr << "Unknown error" << std::endl;
  }

  return 0;
}
