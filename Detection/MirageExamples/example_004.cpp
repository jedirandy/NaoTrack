#include <mirage.h>
#include <math.h>
#include <iostream>

// Jump directly to main, and go back to the stuff here when you need
// to understand some definitions used in the main.

typedef mirage::img::Coding<mirage::colorspace::RGB_24>::Frame  ImageRGB24;
typedef mirage::img::Coding<mirage::colorspace::GRAY_8>::Frame  ImageGray8;
typedef mirage::img::Coding<bool>::Frame                        ImageBool;
typedef mirage::img::Coding<double>::Frame                      ImageDouble;
typedef mirage::img::Coding<mirage::morph::MaskValue>::Frame    Element;

// This is the color comparator. We use booleans, but double allows
// fuzzy-logic decision.
class ColorCompareParam {
public:
  // This defines the maximal value the norm of an RGB vector can
  // reach.
  static inline double maxn(void) {return 255*1.7321;}

  // This is the norm test. Is the norm high enough for color to be
  // extracted. Norm is given in [0(for min) 1(for max)].
  static inline bool g(double n) {return n>.05;}

  // Parameter c is the cosine of two rgb vectors. If it is close to
  // 1, colors are similar. Function h is thus similarity test.
  static inline double h(double c) {return c>.985;}

  // This combines norm test and similarity test 
  static inline bool f(bool norm, bool same) {return norm && same;}
};

// To compute cosine, we need a dot product for color values. Here it
// is for RGB. We have to set rgb component type as parameter. We
// could put unsigned char, but we do the following, which is much
// more smart.
typedef mirage::colorspace::RGBDotProduct<mirage::colorspace::RGB_24::value_type> ColorDotProduct;

typedef mirage::colorspace::ColorCompare<ImageRGB24, // Source type
                                         ImageBool,  // ResultType
                                         mirage::colorspace::RGB<double>, // This is a floting point version of values in source type.
                                         ColorDotProduct, // The dot product.
                                         ColorCompareParam> ColorCompare;
// Some converters

class BoolToGrayOp {
public:
  inline void operator()(const bool& src, mirage::colorspace::GRAY_8& res) {
    if(src)
      res=(mirage::colorspace::GRAY_8)255;
    else
      res=(mirage::colorspace::GRAY_8)0;
  }
};
typedef mirage::algo::UnaryOp<ImageBool,ImageGray8,BoolToGrayOp> BoolToGray;

class DoubleToGrayOp {
public:
  inline void operator()(const double& src, mirage::colorspace::GRAY_8& res) {
    res=(mirage::colorspace::GRAY_8)(src+.5);
  }
};
typedef mirage::algo::UnaryOp<ImageDouble,ImageGray8,DoubleToGrayOp> DoubleToGray;



int main(int argc, char* argv[]) {

  ImageRGB24 source;
  ImageBool  result;
  ImageBool  tmp;
  ImageGray8 out;
  mirage::colorspace::RGB_24 shirt_color;

  std::string image_dir;
  if(argc!=2)
    {
      std::cout << "Usage :" << std::endl
                << "  " << argv[0] << " <directory>" << std::endl
                << "ex : " << argv[0] << " /usr/share/mirage-images" << std::endl;
      return 3;
    }
  image_dir = argv[1];


  try {

    mirage::Init();

    // Let us load an image of Adriana Lima.

    mirage::img::JPEG::read(source,image_dir+"/adriana.jpg");
    std::cout << "Generating adriana.jpg" << std::endl;
    mirage::img::JPEG::write(source,"adriana.jpg",75);

    result.resize(source._dimension);
    out.resize(source._dimension);

    // She wears a purple shirt, let us pick its color. 
    shirt_color = source[mirage::img::Coordinate(340,600)];

    // Let us extract shirt from color.
    ColorCompare(source,result,shirt_color);

    // We display the mask
    BoolToGray(result,out);
    std::cout << "Generating colormask.ppm" << std::endl;
    mirage::img::PPM::write(out,"colormask.ppm");

    // As you can see, the mask is very noisy. Let us use morphomath
    // to work on it.
    Element structural_element;
    mirage::morph::Mask::Disk(structural_element,7 /* radius */);
    // Parameters are source type, result type, buffered flag and init
    // flag (default is used here).
    mirage::morph::Format<ImageBool,ImageBool,1>::Closing(result,structural_element,result);
    // Opening, Dilatation and Erosion are also available.
    
    BoolToGray(result,out);
    std::cout << "Generating colormask-cleaned.ppm" << std::endl;
    mirage::img::PPM::write(out,"colormask-cleaned.ppm");


    // Just for fun, let us skeletize this.
    tmp.resize(source._dimension);
    std::cout << "Waiting for skeletization to be done..." << std::endl;
    mirage::morph::Format<ImageBool,ImageBool,0>::Skeleton(result,tmp);
    std::cout << "Waiting for pruning to be done..." << std::endl;
    mirage::morph::Format<ImageBool,ImageBool,1>::Pruning(tmp,tmp,-1 /* positive value n means n iteration, -1 means "until is is invariant"*/);
    
    BoolToGray(tmp,out);
    std::cout << "Generating colormask-skeleton.ppm" << std::endl;
    mirage::img::PPM::write(out,"colormask-skeleton.ppm");
    

    // Now, let us extract contours, and we draw them on source image.
    ImageBool::point_type pos;
    ImageBool::pixel_type pix1,pix2,pix_end;
    ImageRGB24::value_type contour_color(255,255,0);

    tmp=result;
    for(pix1=tmp.begin(),pix_end=tmp.end(),pix2=result.begin();
        pix1!=pix_end;
        ++pix1,++pix2) {
      if(*pix1)
        {
          pos = !pix2 + mirage::img::Coordinate(1,0);
          *pix2 = !(tmp(pos));
          
          if(!(*pix2))
            {
              pos = !pix2 + mirage::img::Coordinate(0,1);
              *pix2 = !(tmp(pos));
              
              if(!(*pix2))
                {
                  
                  pos = !pix2 + mirage::img::Coordinate(0,-1);
                  *pix2 = !(tmp(pos));
                  
                  if(!(*pix2))
                    {
                      pos = !pix2 + mirage::img::Coordinate(-1,0);
                      *pix2 = !(tmp(pos));
                    }
                }
            }
        }
      if(*pix2)
        source[!pix2]=contour_color;
    }
    
    std::cout << "Generating colormask-edge.ppm" << std::endl;
    mirage::img::PPM::write(source,"colormask-edge.ppm");


    // Now, let us play with Hough transform. Mirage provides an
    // mirage::algo::HoughSpace class, that allows to hit values
    // cumulated to build an hough space. In case of 2D images and
    // line detection, mirage profides a specialization of that class.
    // We use this one here.
    mirage::img::hough::Line houghspace;
    
    // Let set resolution of hough space.
    houghspace.setResolution(source._dimension,
                             3,1*M_PI/180);
    
    // We declare points belonging to the contour.
    houghspace.startHits();
    for(pix1=result.begin(),pix_end=result.end();
        pix1!=pix_end;
        ++pix1) 
      if(*pix1)
        houghspace.declarePoint(!pix1,1 /* The default value : hits can be weighted. */);
    houghspace.normalize(255); // put values in [0,255].
    
    out.resize(houghspace._dimension);
    DoubleToGray(houghspace,out);
    std::cout << "Generating houghspace1.ppm" << std::endl;
    mirage::img::PPM::write(out,"houghspace1.ppm");

    // Pikes of activity in the hough space correspond to different
    // lines. The problem is to determine where the top of the
    // different pikes are.... Some people do morphomath for that
    // purpose. We will do something else here. Let us extract the
    // maximum value of the hough space (it is already available),
    // draw this line on source, and delete this line on the edge
    // image. Then, we can restart a full houghspace construction to
    // get next line.
    double a,b,c;
    mirage::img::Line<ImageRGB24> line_in_source;
    mirage::img::Line<ImageBool>  line_in_result;
    mirage::img::Line<ImageRGB24>::pixel_type lpix,lpix_end;
    ImageRGB24::value_type line_color(0,255,0);

    houghspace.maxLine(a, b, c); // ax+by+c=0
    line_in_source << source; // The line operate in source.
    line_in_source(a, b, c); // We set equation.
    // We draw the line in the source image.
    for(lpix=line_in_source.begin(),lpix_end=line_in_source.end();
        lpix!=lpix_end;
        ++lpix)
      *lpix = line_color; // lpix is a full pixel of the source image, ! and * operate usually.
    
    std::cout << "Generating line1.ppm" << std::endl;
    mirage::img::PPM::write(source,"line1.ppm");

    // We have to erase this line in the result boolean image, where
    // contour stands. We can do it easily with sub images.
    mirage::SubFrame<ImageBool> erasor(result,pos(0,0),pos(1,1)); // dummy values
    // We could have done the following steps inside the previous loop.
    for(lpix=line_in_source.begin(),lpix_end=line_in_source.end();
        lpix!=lpix_end;
        ++lpix) {
      // Be careful, you may want to write the following, using pos to
      // simplify the writing. The problem is that you do not have
      // control of the order of evaluations of the to pos(..) calls,
      // and only the last one is effective, and given to both
      // argument. So use such pos tricks once in each expression, or
      // use two pos1 and pos2 variables.
      //
      // erasor.resize(!lpix - pos(2,2),pos(5,5));
      erasor.resize(!lpix - mirage::img::Coordinate(2,2),mirage::img::Coordinate(5,5));
      erasor = false; // Erases a square around !lpix
    }
    
    // We can rebuild hough space with remaining points.
    houghspace.startHits();
    for(pix1=result.begin(),pix_end=result.end();
        pix1!=pix_end;
        ++pix1) 
      if(*pix1)
        houghspace.declarePoint(!pix1,1);
    houghspace.normalize(255);
    out.resize(houghspace._dimension);
    DoubleToGray(houghspace,out);
    std::cout << "Generating houghspace2.ppm" << std::endl;
    mirage::img::PPM::write(out,"houghspace2.ppm");

    houghspace.maxLine(a, b, c); // ax+by+c=0
    line_in_source << source; // The line operate in source.
    line_in_source(a, b, c); // We set equation.
    // We draw the line in the source image.
    for(lpix=line_in_source.begin(),lpix_end=line_in_source.end();
        lpix!=lpix_end;
        ++lpix)
      *lpix = line_color; // lpix is a full pixel of the source image, ! and * operate usually.
    
    std::cout << "Generating line2.ppm" << std::endl;
    mirage::img::PPM::write(source,"line2.ppm");
    

  }
  catch(mirage::Exception::Any& e) {
    std::cerr << "Error : " <<  e.what() << std::endl;
  }
  catch(...) {
    std::cerr << "Unknown error" << std::endl;
  }


  return 0;
}
