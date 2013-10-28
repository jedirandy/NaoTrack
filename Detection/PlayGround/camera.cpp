#include <mirage.h>
#include <axisPTZ.h>
#include <string>
#include <cstdlib>
#include <cc++/thread.h>


typedef mirage::img::Coding<mirage::colorspace::RGB_24>::Frame ImageRGB;

void rgb2bgr(ImageRGB& img);

int main(int argc, char* argv[]) {
  std::string hostname,user,password;
  int port;
  double pan,tilt,zoom;

  if(argc!=10) {
     std::cout << "Usage :" << std::endl
	       << "  " << argv[0] 
	       << " <hostname> <port:80> <username> <password> <pan> <tilt> <zoom> <x> <y>" 
	       << std::endl;
      return 0;
  }

  hostname = argv[1];
  port     = atoi(argv[2]);
  user     = argv[3];
  password = argv[4];

  axis::PTZ axis(hostname,port);
  if(!axis.connect(user,password)){
    std::cout << "Connot connect " << user 
	      << " (" << password << ") on " 
	      << hostname << ':' << port << ". Aborting."
	      << std::endl;
    return 1;
  }

  axis.setAutoiris("off");
  axis.setIris(1000);

  axis.getPosition(pan,tilt,zoom);
  std::cout << "Current position is " << std::endl
	    << "  pan  = " << pan << std::endl
	    << "  tilt = " << tilt << std::endl
	    << "  zoom = " << zoom << std::endl;

  pan  = atof(argv[5]);;
  tilt = atof(argv[6]);
  zoom = atof(argv[7]);
  
  std::cout << "Reaching now... " << std::endl
	    << "  pan  = " << pan << std::endl
	    << "  tilt = " << tilt << std::endl
	    << "  zoom = " << zoom << std::endl;
  axis.setPanTilt(pan,tilt);
  axis.setZoom(zoom);
  axis.wait();

  std::cout << "... reached." << std::endl;
  axis.getPosition(pan,tilt,zoom);
  std::cout << "  pan  = " << pan << std::endl
	    << "  tilt = " << tilt << std::endl
	    << "  zoom = " << zoom << std::endl;

  // Let us now grab an image.
  // We wait 2 seconds for autofocus to stabilize, since we mah have zoomed.
  ost::Thread::sleep(2000);
  axis.getDefaultBMPImage();
  
  // Let us now handle the image with mirage 
  ImageRGB img;
  int dummy;
  mirage::img::Coordinate img_size(axis.getWidth(),axis.getHeight());
  img.resize(img_size,
	     (ImageRGB::value_type*)axis.getImageBytes(dummy,dummy,dummy));

  // Let is save the mirage image in a file.
  std::ostringstream outputnamestream;
  outputnamestream << "X_" << argv[8] << "Y_" << argv[9] << "pan_" << pan << "tilt_" << tilt << "zoom_" << zoom << ".jpg";
  std::string outputname = outputnamestream.str();
  rgb2bgr(img);
  mirage::img::JPEG::write(img,outputname,80);
  std::cout << "Image has been captured in ptz.jpg file." << std::endl;
	     
  return 0;
}

void rgb2bgr(ImageRGB::value_type& rgb) {
  ImageRGB::value_type::value_type tmp;

  tmp       = rgb._red;
  rgb._red  = rgb._blue;
  rgb._blue = tmp;
}

void rgb2bgr(ImageRGB& img) {
  ImageRGB::pixel_type pix,pix_end;

  for(pix=img.begin(),pix_end=img.end();
      pix!=pix_end;
      ++pix) 
    rgb2bgr(*pix);
}

