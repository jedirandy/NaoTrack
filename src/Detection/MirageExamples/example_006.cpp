#include <mirage.h>
#include <math.h>
#include <iostream>
#include <iomanip>

// In this tutorial, we play with image filtering.

typedef mirage::img::Coding<mirage::colorspace::GRAY_8>::Frame       ImageGray8;
typedef mirage::img::Coding<mirage::colorspace::RGB_24>::Frame       ImageRGB24;
typedef mirage::img::Coding<int>::Frame                              ImageInt;
typedef mirage::img::Coding<double>::Frame                           ImageDouble;
typedef mirage::img::Coding<mirage::colorspace::RGB<double> >::Frame ImageRGBDouble;

// For convolution, mirage provides a generic algorithm.
typedef mirage::algo::Convolution<ImageRGB24, // source type
                                  ImageInt,   // filter type
                                  ImageRGB24, // result type
                                  // This is the sum of products that is involved. This one is suitable for RGB and int.
                                  mirage::algo::SumProduct<mirage::colorspace::RGB_24, // Result value type, given directly...
                                                           ImageRGB24::value_type,     // ...Source value type, given in a smarter way
                                                           ImageInt::value_type,       // ImageInt value type
                                                           // External multiplication (value*filter)
                                                           mirage::colorspace::RGBMultiply<ImageRGB24::value_type::value_type,
                                                                                           ImageRGB24::value_type::value_type,
                                                                                           ImageInt::value_type> >,
                                  0, // 0/1 flag for buffering result. Default is 0
                                  1> // 0/1 flag to allow reset of result image before processing. Default is 1.
Convolution;

// When we work with usual frames, one can guess whose values will be
// concerned with border effects, and testing time can be saved. This
// is the role of optimized convolution. Parameters are the same as
// above, and we use default ones.
typedef mirage::algo::OptimizedConvolution<ImageRGB24,ImageInt,ImageRGB24,  
                                           mirage::algo::SumProduct<ImageRGB24::value_type,
                                                                    ImageRGB24::value_type,
                                                                    ImageInt::value_type,
                                                                    mirage::colorspace::RGBMultiply<ImageRGB24::value_type::value_type,
                                                                                                    ImageRGB24::value_type::value_type,
                                                                                                    ImageInt::value_type> > >
OptimizedConvolution;


// The latter can be simplified, since in many cases, RGB values are
// used, and the uggly mirage::algo::SumProduct<...RGBMultiply...> can
// be automatically written by a template. You will see in the
// following such simplifications.


// Let us define here some converters.

class SobelRGBDoubleToRGB24Op {
public:
  // Yes, src is copied (no &)
  inline void operator()(const mirage::colorspace::RGB<double> src, mirage::colorspace::RGB_24& res) {
    // Since values are in [-255*4,255*4] (see main), we rescale src.
    mirage::colorspace::RGB<double> tmp = src / 8;
    tmp += 127;

    res._red   = (unsigned char)(tmp._red+.5);                          // explicite unsigned char is not so clean...
    res._green = (mirage::colorspace::RGB_24::value_type)(tmp._green);  // ... this is much better.
    res._blue  = (mirage::colorspace::RGB_24::value_type)(tmp._blue); 
  }
};




int main(int argc, char* argv[]) {


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

    ImageRGB24 source,result;
    ImageInt   filter;

    mirage::Init();

    // Here, we play with border effects.
    
    // We load an image from filename.
    mirage::img::JPEG::read(source,image_dir+"/spiderman.jpg");
    std::cout << "Generating spiderman.jpg" << std::endl;
    mirage::img::JPEG::write(source,"spiderman.jpg",75);

    // We create a bigger image
    result.resize(source._dimension*2.5);

    // We create a filter
    filter.resize(source._dimension/2);
    filter = 0; // All values are 0.
    *(filter.begin()) = 1; // upper left is 1.

    // Let us try the default out frame policy.
    source.setPolicy(new mirage::ZeroOutFramePolicy<ImageRGB24>());
    // This will generates an error message (and is inefficient),
    // since optimized convolution needs source and result images
    // having the same size.
    OptimizedConvolution(source,filter,result);  
    // This will generates an error message (and is inefficient),
    // since optimized convolution (involved by the template) needs
    // source and result images having the same size.
    mirage::colorspace::Convolution<ImageRGB24,ImageInt,ImageRGB24>::RGB::Process(source,filter,result);
    // And last, the good one for this border-effect example
    Convolution(source,filter,result);  
    std::cout << "Generating spiderman-zero.jpg" << std::endl;
    mirage::img::JPEG::write(result,"spiderman-zero.jpg",75);
    
    // Let us try other out frame policies.

    source.setPolicy(new mirage::ClosePointOutFramePolicy<ImageRGB24>());
    Convolution(source,filter,result);  
    std::cout << "Generating spiderman-close-point.jpg" << std::endl;
    mirage::img::JPEG::write(result,"spiderman-close-point.jpg",75);

    source.setPolicy(new mirage::PeriodicOutFramePolicy<ImageRGB24>());
    Convolution(source,filter,result);  
    std::cout << "Generating spiderman-periodic.jpg" << std::endl;
    mirage::img::JPEG::write(result,"spiderman-periodic.jpg",75);
    

    source.setPolicy(new mirage::SymetricOutFramePolicy<ImageRGB24>());
    Convolution(source,filter,result);  
    std::cout << "Generating spiderman-symetric.jpg" << std::endl;
    mirage::img::JPEG::write(result,"spiderman-symetric.jpg",75);
    

    // Now, we know how to filter images. Let us try to apply
    // ready-to-use mirage filters.

    mirage::img::JPEG::read(source,image_dir+"/shrek.jpg");
    std::cout << "Generating shrek.jpg" << std::endl;
    mirage::img::JPEG::write(source,"shrek.jpg",75);
    

    // We can get the vertical sobel filter in an ImageInt data.

    ImageInt sobel_vert;
    ImageInt::iterator sobel_vert_iter, sobel_vert_iter_end;
    int w,sobel_vert_width;

    mirage::img::Filter<int>::Sobel::Vertical(sobel_vert);
    sobel_vert_width = sobel_vert._dimension[0];
    std::cout << "Sobel vertical filter :" << std::endl;
    for(sobel_vert_iter=sobel_vert.begin(),sobel_vert_iter_end=sobel_vert.end();
        sobel_vert_iter != sobel_vert_iter_end;
        std::cout << std::endl)
      for(w=0;
          w<sobel_vert_width;
          ++sobel_vert_iter,++w)
        std::cout << std::setw(3) << *sobel_vert_iter << ' ';

    // Nevertheless, we can set up the sobel filtering with a set of
    // predifine functions, that allocates the sobel filter obtenied
    // here internally. These are available for RGB and Gray, on 2D
    // frames.
    
    mirage::SubFrame<ImageRGB24> subimage(source,mirage::img::Coordinate(0,0),source._dimension/2);
    ImageRGBDouble sobel_result;


    // Template parameters are static-flag, source type, destination
    // type. No buffering would have has some strange
    // effects since source image would have changed during the
    // filtering process.
    //
    // The first parameter, the static flag, is set to 1 since we want
    // to use static data for intermediate computations (this avoids
    // many reallocations). Be carful with that, is is not thread
    // safe. In a multi-thread context, set this flag to 0.
    sobel_result.resize(subimage._dimension);
    mirage::img::Filtering<1,
      mirage::SubFrame<ImageRGB24>,
      ImageRGBDouble>::RGB::Sobel::Horizontal(subimage,sobel_result);
    
    // sobel_result has values in [-255*4,255*4], so we rescale them
    // and convert in RGB24.
    mirage::algo::UnaryOp<ImageRGBDouble,mirage::SubFrame<ImageRGB24>,SobelRGBDoubleToRGB24Op>(sobel_result,subimage);
    
    // It is to be notices here that filtering the sub-image deals
    // with its border... but the outframe policy is involved only for
    // the border of the sub-image corresponding to actual borders of
    // parent image. So for right and bottom border, out of bound
    // during filtering consist in picking existing pixels in the
    // parent image.

    // Now, let us use the full sobel filter on another subpart of the
    // image.  Note that passing 1 for buffer mode (last template
    // parameter) is required, since source and destination are the
    // same.
    subimage.resize(source._dimension/3,source._dimension/2);
    sobel_result.resize(subimage._dimension);
    mirage::img::Filtering<1,
      mirage::SubFrame<ImageRGB24>,
      mirage::SubFrame<ImageRGB24>,
      1>::RGB::Sobel::Norm(subimage,subimage);

    std::cout << "Generating shrek-sobel.ppm" << std::endl;
    mirage::img::PPM::write(source,"shrek-sobel.ppm");

    // Actually, sobel filtering extracts lines from grayscaled images.
    
    ImageGray8 gray_result;

    // As source is altered, we reload it from file.
    mirage::img::JPEG::read(source,image_dir+"/shrek.jpg");
    
    gray_result.resize(source._dimension);
    // There is an unary operator pre-define for color to gray transforms.
    mirage::algo::UnaryOp<ImageRGB24,ImageGray8,
      mirage::colorspace::RGBToGray<ImageRGB24::value_type,ImageGray8::value_type> >(source,gray_result);

    std::cout << "Generating shrek-gray.jpg" << std::endl;
    mirage::img::JPEG::write(gray_result,"shrek-gray.jpg",10); // Low compression quality for fun...
    
    mirage::img::Filtering<1,
      ImageGray8,
      ImageGray8,
      1>::Gray::Sobel::Norm(gray_result,gray_result);

    std::cout << "Generating shrek-gray-sobel.ppm" << std::endl;
    mirage::img::PPM::write(gray_result,"shrek-gray-sobel.ppm"); 
    
    
  }
  catch(mirage::Exception::Any& e) {
    std::cerr << "Error : " <<  e.what() << std::endl;
  }
  catch(...) {
    std::cerr << "Unknown error" << std::endl;
  }

  return 0;
}
