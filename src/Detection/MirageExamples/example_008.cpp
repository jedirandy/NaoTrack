#include <mirage.h>
#include <math.h>
#include <iostream>


// In this tutorial, we illustrate some basical image
// manipulation. The use of templates leads to syntaxically compicated
// stuff, so one good method is to set up some #define to simplify the
// code.

// First, let us consider frames of data. Frames are hypercubes of
// data. Each data is indexed by a vector, i.e. its
// coordinate. Vectors allow to express coordinates. Let us define a
// 2D frame, for classical images. We need a vector with 2 integer
// components for indexing image elements.
typedef mirage::Vector<2,int> Vector2D;

// mirage uses parameter namings. See the documentation when you need
// them. For example, here, vector2D::scalar_type is int, and
// vector2D::dim_int is 2.

// Now, we need to build a 2D frame. Let say that content is
// double... so we want to define a 2D grid of double. The Frame
// template does the job.
typedef mirage::Frame<double,Vector2D> Grid;

// Ok, as you may guess, 2D grids of data is of intensive use in image
// processing, so types as Vector2D and Grid are ready-to-use in
// mirage. These ready-to-use types for 2D frames are gathered in img
// namespace. So Vector2D is mirage::img::Coordinate. Let us define
// Grid in a much mirage-like way.
typedef mirage::img::Coding<double>::Frame ImageDouble;


// Now, classically, I want to deal with color images, so data isn't
// double but a structure of rgb components. Template
// mirage::colorspace::RGB<...> provides one. It is also defined for
// unsigned char based coding.
typedef mirage::img::Coding<mirage::colorspace::RGB_24>::Frame ImageRGB24;
typedef mirage::img::Coding<mirage::colorspace::GRAY_8>::Frame ImageGray8;

// Ok, let us suppose now that we want ton consider RGB images, and
// that we want to extract the norm of the RGB vector, and a color
// image with normalized colors.

// Having the norm in [0..1] consists in iterating on all pixels, and
// compute n=sqrt(r*r+g*g+b*b)/(sqrt(3)*255). For such a systematic
// procedure, the use of unary operator is very convenient.
class NormalizeOp {
public: 
  void operator()(const mirage::colorspace::RGB_24& src,double& res) {
    res = sqrt(src._red*src._red + src._green*src._green + src._blue*src._blue)/441.68;
  }
};
typedef mirage::algo::UnaryOp<ImageRGB24,ImageDouble,NormalizeOp> Normalize;

// For writing out norm in a JPEG file, we need to convert it in
// grayscale. Once again, let us define an unary operator to do the
// job.
class NormToGrayOp {
public: 
  void operator()(const double& src,mirage::colorspace::GRAY_8& res) {
    res = (mirage::colorspace::GRAY_8)(255*src+.5);
  }
};
typedef mirage::algo::UnaryOp<ImageDouble,ImageGray8,NormToGrayOp> NormToGray;


// For normalizing colors, we use a binary operator. You can see that
// RGB behaves like a number, since it implements the
// mirage::concept::Scalar concept.
class NormalizeColorOp {
public:
  void operator()(const mirage::colorspace::RGB_24& src,
                  const double& norm,
                  mirage::colorspace::RGB_24& result) {
    if(norm>.0001)
      result = src/(norm*1.732);
  }
};



int main(int argc, char* argv[]) {
  ImageRGB24 source;
  ImageDouble norm;
  ImageGray8 norm_out;

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
    
    // You have to initialize some intenal static stuff.
    mirage::Init();


    // Let us play with vector.
    mirage::Vector<5,double> v5; // v is a vector made of 5 double.
    
    // This expression sets the vector... and returns it for further
    // use in an expression.
    v5( (mirage::Args<5,double>(),
         1.0,
         2.0,
         3.0,
         4.0,
         5.0) ); 

    // Some specific implementation is available for Vector<2,...>
    // types., allowing an initialization without the mirage::Args
    // stuff.
    mirage::Vector<2,double> v2;
    v2(1.0,2.0);
    // This works for mirage::img::Coordinate type, that is a typedef on mirage::Vector<2,double>.
    mirage::img::Coordinate v2bis;
    v2bis(1.0,2.0);

    
    // Let us load source from an image. You have to provide a string
    // for filename.
    mirage::img::JPEG::read(source,image_dir+"/flowers.jpg");
    
    // Now, we have to resize the other images.
    norm.resize(source._dimension);
    norm_out.resize(source._dimension);
    
    // Then, we can use our unary operator to compute the norm ...
    Normalize(source,norm);
    // ... and make a grayscale version of it.
    NormToGray(norm,norm_out);
    
    // Now, we write the resulting image as a jpeg file, but also as a
    // ppm file (for fun only).
    std::cout << "Generating norm.ppm" << std::endl;
    mirage::img::PPM::write(norm_out,"norm.ppm");
    std::cout << "Generating norm.jpg" << std::endl;
    // Last number is compression quality.
    mirage::img::JPEG::write(norm_out,"norm.jpg",75); 



    // Ok, now let us see how to iterate on pics. We will draw a grid
    // in video inverse mode over norm_out image. This could be done
    // with an unary operator, but let'us do it with iterators.
    //  *iter is the content, !iter the coordinate.
    ImageGray8::iterator iter,iter_end; // ImageGray8::pixel_type is the same.
    for(iter=norm_out.begin(),iter_end=norm_out.end();
        iter!=iter_end;
        ++iter)
      if((!iter)[0]%50==0 || (!iter)[1]%50==0)
        *iter = (255-*iter);
    std::cout << "Generating norm-grid.ppm" << std::endl;
    mirage::img::PPM::write(norm_out,"norm-grid.ppm");

    

    // Now, let us see how an image can manage an external buffer. 
    int width=320;
    int height=240;
    int depth=3;
    int w,h,k;
    unsigned char* img=new unsigned char[width*height*depth];
    ImageRGB24 externalized;
    mirage::colorspace::RGB_24 rgb;
    mirage::img::Coordinate coord;

    // externalized uses img as memory buffer. This buffer has to be
    // big enough !
    externalized.resize(mirage::img::Coordinate(width,height),
                        (mirage::colorspace::RGB_24*)img); 

    // Let us fill the buffer with values in source. In mirage, two
    // operators are avilable for random access to pixels. One of them
    // is [], the other is (). The latter manages out of bounds, and
    // modifies its argument (the coordinate) to be kept inside the
    // image. With [], you have to ensure that coordinates are
    // actually inside the image. Let us play with ().
    source.setPolicy(new mirage::ClosePointOutFramePolicy<ImageRGB24>);
    for(k=0,h=0;h<height;h++)
      for(w=0;w<width;w++)
        {
          // The use of negative coordinates actualy works with
          // source(...), but it would have failed with
          // source[...]. Nevertheless, [] is much faster that (),
          // since no checkings are made.
          coord(w-50,h-50);
          rgb = source(coord);
          img[k]=rgb._red;k++;
          img[k]=rgb._green;k++;
          img[k]=rgb._blue;k++;
        }

    // Let us write externalized to see the source out frame policy
    // effect.
    std::cout << "Generating external.ppm" << std::endl;
    mirage::img::PPM::write(externalized,"external.ppm");

    // One can retrieve a pointer to any image internal buffer, by
    // getting the adress of the value of the first pixel.
    // source.begin() --> the first pixel.
    // *(...)         --> its value.
    // &(...)         --> the adress of that value.
    unsigned char* internal_buf = (unsigned char*)( &( *(source.begin()) ) );


    // Let us now see the concept of sub-frames. A sub-frame is frame,
    // that handles a part of another frame. So having a subframe
    // doesn't create memory, since only the memory pf the mother
    // frame is involved. As sub-frames are also frames, you can deal
    // with sub-frames of them, and so on, recursively. Let us
    // normalize colors on a part of the source image, according to
    // the norm we have computed.
    mirage::img::Coordinate origin = source._dimension*.25;
    mirage::img::Coordinate size   = source._dimension*.60;
    mirage::SubFrame<ImageRGB24> subsource(source,origin,size);
    mirage::SubFrame<ImageDouble> subnorm(norm,origin,size);

    // As subframes are not frames, from a type point of view, we have
    // to keep types of images as parameters if we wand to re-use
    // NormalizeColorOp.
    mirage::algo::BinaryOp<mirage::SubFrame<ImageRGB24>,
                           mirage::SubFrame<ImageDouble>,
                           mirage::SubFrame<ImageRGB24>,
                           NormalizeColorOp>  (subsource,subnorm,subsource);
    // We can write the source frame in a file.
    std::cout << "Generating subframe1.ppm" << std::endl;
    mirage::img::PPM::write(source,"subframe1.ppm");
    
    // Let consider this trick for having the subframe in a single
    // frame. This involve a copy.
    ImageRGB24 trick;
    
    trick.resize(subsource._dimension);
    mirage::algo::UnaryOp<mirage::SubFrame<ImageRGB24>,
                          ImageRGB24,
                          mirage::algo::Affectation<mirage::colorspace::RGB_24,mirage::colorspace::RGB_24> >(subsource,trick);
std::cout << "Generating subframe2.ppm" << std::endl;
mirage::img::PPM::write(trick,"subframe2.ppm");
    
delete[]  img;


// Last, let us see image rescaling facilities.
ImageRGB24 eye, big_eye;
mirage::img::Coordinate rescale;
mirage::Time timer;

mirage::img::JPEG::read(eye,image_dir+"/eye.jpg");
    
std::cout << "Generating eye.ppm" << std::endl;
mirage::img::PPM::write(eye,"eye.ppm");
rescale = eye._dimension*10;
// Each change in size will trigger reallocation.
big_eye._buffer.setPolicy(mirage::allocateAlways); 
    
// Let us try different rescalings.

big_eye = eye;
big_eye.setRescalePolicy(new mirage::ClosePolicy<ImageRGB24>());
timer.start();
big_eye.rescale(rescale);
timer.stop();
std::cout << "Image eye-close.ppm has been computed in " << timer.getTime() << " sec." << std::endl;
mirage::img::PPM::write(big_eye,"eye-close.ppm");

big_eye = eye;
big_eye.setRescalePolicy(new mirage::BilinearPolicy<ImageRGB24>());
timer.start();
big_eye.rescale(rescale);
timer.stop();
std::cout << "Image eye-bilinear.ppm has been computed in " << timer.getTime() << " sec." << std::endl;
mirage::img::PPM::write(big_eye,"eye-bilinear.ppm");

big_eye = eye;
big_eye.setRescalePolicy(new mirage::BicubicPolicy<ImageRGB24>());
timer.start();
big_eye.rescale(rescale);
timer.stop();
std::cout << "Image eye-bicubic.ppm has been computed in " << timer.getTime() << " sec." << std::endl;
mirage::img::PPM::write(big_eye,"eye-bicubic.ppm");
    

    
    
}
  catch(mirage::Exception::Any& e) {
    std::cerr << "Error : " <<  e.what() << std::endl;
  }
  catch(...) {
    std::cerr << "Unknown error" << std::endl;
  }

  return 0;
}


// This method shows how to deal with const iterators. This is a silly example.
typedef mirage::img::Coding<double>::Frame ImageDouble;
void the_const_problem(ImageDouble& img, const ImageDouble& cimg) {

  mirage::img::Coordinate coord, origin, size;

  //                           //
  //  The problem with images  //
  //                           //

  ImageDouble::pixel_type pix,pix_end;
  ImageDouble::const_pixel_type cpix,cpix_end;
  ImageDouble::value_type *ptr;
  const ImageDouble::value_type *cptr;


  for(pix = img.begin(), pix_end = img.end(), 
        cpix = cimg.const_begin(), cpix_end = cimg.const_end();
      pix != pix_end && cpix != cpix_end;
      ++pix,++cpix) {
    coord = !pix;
    coord = !cpix;
    *pix  = *cpix;
    // *cpix = *pix;  ...is forbidden by the compiler.
  }

  coord(10,10);
  ptr  = &(img(coord));
  cptr = &(img(coord));
  ptr  = &(img[coord]);
  cptr = &(img[coord]);
  // ptr  = &(cimg(coord));  ...is forbidden by the compiler.
  cptr = &(cimg(coord));
  // ptr  = &(cimg[coord]);  ...is forbidden by the compiler.
  cptr = &(cimg[coord]);



  //                               //
  //  The problem with sub images  //
  //                               //



  origin(10,10);
  size(320,240);
  mirage::SubFrame<ImageDouble> subimg(img,origin,size);
  // mirage::SubFrame<ImageDouble> csubimg(cimg,origin,size); ... forbidden by the compiler.
  mirage::ConstSubFrame<ImageDouble> csubimg(cimg,origin,size);

  
  mirage::SubFrame<ImageDouble>::pixel_type spix,spix_end; // const_pixel_type is also available.
  mirage::ConstSubFrame<ImageDouble>::const_pixel_type cspix,cspix_end;

  
  for(spix = subimg.begin(), spix_end = subimg.end(), 
        cspix = csubimg.const_begin(), cspix_end = csubimg.const_end();
      spix != spix_end && cspix != cspix_end;
      ++spix, ++cspix) {
    coord = !spix;
    coord = !cspix;
    *spix  = *cspix;
    // *cspix = *spix;  ...is forbidden by the compiler.
  }

  coord(10,10);
  ptr  = &(subimg(coord));
  cptr = &(subimg(coord));
  ptr  = &(subimg[coord]);
  cptr = &(subimg[coord]);
  // ptr  = &(csubimg(coord));  ...is forbidden by the compiler.
  cptr = &(csubimg(coord));
  // ptr  = &(csubimg[coord]);  ...is forbidden by the compiler.
  cptr = &(csubimg[coord]);



  //                          //
  //  The problem with lines  //
  //                          //

  mirage::img::Line<ImageDouble> line;
  mirage::img::ConstLine<ImageDouble> cline;
  mirage::img::Line<ImageDouble>::pixel_type lpix,lpix_end; // const_pixel_type is also available.
  mirage::img::ConstLine<ImageDouble>::const_pixel_type clpix,clpix_end;

  line << img;
  // line << cimg;  ...is forbidden by the compiler.
  
  cline << img;
  cline << cimg;
  
  line(1,2,3);
  cline(1,2,3);

  for(lpix = line.begin(), lpix_end = line.end(), 
        clpix = cline.const_begin(), clpix_end = cline.const_end();
      lpix != lpix_end && clpix != clpix_end;
      ++lpix, ++clpix) {
    coord = !lpix;
    coord = !clpix;
    *lpix  = *clpix;
    // *clpix = *lpix;  ...is forbidden by the compiler.
  }


  lpix  = line.begin();
  clpix = cline.const_begin();
  ptr  = &(*lpix);
  cptr = &(*lpix);
  // ptr  = &(*clpix);  ...is forbidden by the compiler.
  cptr = &(*clpix);


}
