#include <mirage.h>
#include <string>
#include <iostream>

typedef mirage::img::Coding<mirage::colorspace::RGB_24>::Frame ImageRGB; 
typedef mirage::SubFrame<ImageRGB>                             SubImageRGB;
typedef mirage::SubFrame<SubImageRGB>                          SubSubImageRGB;

// Let us define a inversion function that can be applied to any kind of RGB_24 images (i.e for images and sub images here).
template<typename IMAGE>
void Invert(IMAGE& img) {
    typename IMAGE::pixel_type p,end;
    typename IMAGE::value_type white(255,255,255);
    for(p=img.begin(),end=img.end();
        p!=end;
        ++p) // ++p is more efficient that p++ !
      *p = white - *p;
}

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
    mirage::Init();
    
    // We load the image from a jpeg file.
    ImageRGB rgb_image;
    std::string input_name("adriana.jpg");
    std::string output_name;
    mirage::img::JPEG::read(rgb_image,image_dir+"/"+input_name);
    output_name = input_name;
    mirage::img::JPEG::write(rgb_image,output_name,70);
    std::cout << output_name << " generated." << std::endl;

    mirage::Time time;
    time.start();
    
    // Let us inverse the whole image
    Invert<ImageRGB>(rgb_image);
    
    // Now, let us define a subregion in the image. It starts at
    // position (-50,-50), and has a (400,400) size.
    SubImageRGB sub_img(rgb_image,
                        mirage::img::Coordinate(-50,-50),
                        mirage::img::Coordinate(400,400));

    // As the sub image is not contained in the image, one may wonder
    // what happends if pixel at (0,0) in the sub-image is
    // accessed. This is solved by defining a policy for pixels that
    // may be external to the image. Here, we set the behaviour of our
    // image such as the image is considered periodical (in x and y)
    // by mirage.
    rgb_image.setPolicy(new mirage::PeriodicOutFramePolicy<ImageRGB>());
    mirage::img::Coordinate pos(-50,-50);
    std::cout << "Pixel (" << pos[0] << ',' << pos[1] 
              << ") of the image is the color (";
    mirage::colorspace::RGB_24 rgb = rgb_image(pos); // pos is modified here
    std::cout << (unsigned int)(rgb._red) << ','
              << (unsigned int)(rgb._red) << ','
              << (unsigned int)(rgb._red) << ')' << std::endl
              << "    that is indeed pixel (" << pos[0] << ',' << pos[1] 
              << ")." << std::endl;

    // Let us invert the subimage.
    Invert<SubImageRGB>(sub_img);

    // Last, let us build a sub-image within the previous sub-image,
    // and invert it.
    SubSubImageRGB sub_sub_img(sub_img,
                               sub_img.center(),
                               sub_img._dimension*.8);
    Invert<SubSubImageRGB>(sub_sub_img);
    time.stop();

    output_name = std::string("subimages-")+input_name;
    mirage::img::JPEG::write(rgb_image,output_name,70);
    std::cout << output_name << " generated." << std::endl;

    std::cout << "Duration of the computation: " 
              << time.getTime() << " seconds." << std::endl;
  }
  catch(mirage::Exception::Any& e) {
    std::cerr << "Error : " <<  e.what() << std::endl;
  }
  catch(...) {
    std::cerr << "Unknown error" << std::endl;
  }

  return 1;
}
