#include <mirage.h>
#include <string>
#include <iostream>


// In this example, we will need images made of double and images made
// of rgb colors. Images can be 1D, 2D, 3D (cube), 4D, ... but in
// order to simplify the notations, every template specialization for
// 2D images are gathered in the namespace 'img'.

// Let us first define the image types.
typedef mirage::img::Coding< mirage::colorspace::RGB_24      >::Frame ImageRGB; 
typedef mirage::img::Coding< mirage::colorspace::RGB<double> >::Frame ImageRGBd; 

// The following definitions are used further in the main
// function. Skip those lines for the moment and jump directly to the
// main.

// Operator of inverting a RGB image
class InvertOp {
public:
  void operator()(const mirage::colorspace::RGB_24& src,
                  mirage::colorspace::RGB_24&       res) {
    res = mirage::colorspace::RGB_24(255,255,255) - src;
  }
};
typedef mirage::algo::UnaryOp<ImageRGB,ImageRGB,InvertOp> Invert;


// Operator of converting RGB_24 to RGB<double>
class UCharToDoubleOp {
public:
  void operator()(const mirage::colorspace::RGB_24& src,
                  mirage::colorspace::RGB<double>&  res) {
    res = mirage::colorspace::RGB<double>(src._red,src._green,src._blue)/255.0;
  }
};
typedef mirage::algo::UnaryOp<ImageRGB,ImageRGBd,UCharToDoubleOp> UCharToDouble;


// Operator of converting RGB<double> to RGB_24 
class DoubleToUCharOp {
public:
  void operator()(const mirage::colorspace::RGB<double>& src,
                  mirage::colorspace::RGB_24& res) {
    res._red   = (unsigned char)(src._red*255   +.5);
    res._green = (unsigned char)(src._green*255 +.5);
    res._blue  = (unsigned char)(src._blue*255  +.5);
  }
};
typedef mirage::algo::UnaryOp<ImageRGBd,ImageRGB,DoubleToUCharOp> DoubleToUChar;



int main(int argc,char* argv[]) {
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
    ImageRGB output_image;
    std::string output_name;

    mirage::Init();
    
    ImageRGB rgb_image;
    std::string input_name("adriana.jpg");
    mirage::img::JPEG::read(rgb_image,image_dir+"/"+input_name);
    output_name = input_name;
    mirage::img::JPEG::write(rgb_image,output_name,70);
    std::cout << output_name << " generated." << std::endl;

    // Let is build a second image, that is the negative version of
    // rgb_image. We use image iterors, i.e. pixels, for that, since
    // it is much more efficient that accessing pixels from
    // coordinates.
    // We resize the image to the size of rgb_image.
    output_image.resize(rgb_image._dimension);
    // We iterate on both images.
    ImageRGB::pixel_type p1,p2,p_end;
    ImageRGB::value_type white_rgb_value(255,255,255);
    for(p1=rgb_image.begin(),p2=output_image.begin(),p_end=rgb_image.end();
        p1 != p_end;
        ++p1,++p2) 
      *p2 = white_rgb_value - *p1;
    output_name = std::string("inv-1-")+input_name;
    mirage::img::JPEG::write(output_image,output_name,30 /* jpeg low quality */);
    std::cout << output_name << " generated." << std::endl;

    // In order to avoid the writing of for loops using iterators,
    // operators can be defined. Let us use one of them for inverting
    // the image. The following inversion is equivalent to the one we
    // have just performed. See the definition of Invert in the piece
    // of code placed before the main function.
    Invert(rgb_image,output_image);
    output_name = std::string("inv-2-")+input_name;
    mirage::img::JPEG::write(output_image,output_name,30);
    std::cout << output_name << " generated." << std::endl;
    
    // Let us create another image, containing colors whose rgb
    // components are floating values. 
    ImageRGBd rgbd_image;
    ImageRGBd::pixel_type p3,p3_end;
    double norm;
    rgbd_image.resize(rgb_image._dimension);
    // We convert char-based rgb to double-based rgb, with values in
    // [0,1]. We have defined a specific operator for that as for
    // Invert.
    UCharToDouble(rgb_image,rgbd_image);
    // We normalize the image.
    for(p3=rgbd_image.begin(),p3_end=rgbd_image.end();
        p3 != p3_end;
        ++p3) {
      norm = 0
        + (*p3)._red   * (*p3)._red
        + (*p3)._green * (*p3)._green
        + (*p3)._blue  * (*p3)._blue;
      *p3 /= sqrt(norm);
    } 
    DoubleToUChar(rgbd_image,output_image);
    output_name = std::string("normalized-")+input_name;
    mirage::img::JPEG::write(output_image,output_name,30);
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
