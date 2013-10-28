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

int main(int argc, char* argv[]) {
    
    ImageRGB24 input_image;
    //ImageRGB24 output_image;
    Labelizer labelizer;

    std::string input_name("output.jpg");
    //std::string output_name("labels_output.jpg");

    try {
        mirage::img::JPEG::read(input_image, input_name);
        
        labelizer.neighborhoodSurround(); // 8 neighbors considered
        labelizer(input_image);

        std::cout << "I have found " << labelizer.nb_labels 
                  << " connected components. Each corresponds to a number in [1.." 
                  << labelizer.nb_labels << "] for 8-neighbors." << std::endl;

        for (int i = 1; i<= labelizer.nb_labels; ++i) {
            const Labelizer::BoundingBox& box = labelizer.boundingBox(i);
            mirage::img::Coordinate A,C,size;

            A = box.min();
            C = box.max();
            size = input_image._dimension;

            std::cout << "Label: " << i << std::endl;
            std::cout << "Width: " << size[0] << std::endl;
            std::cout << "Height: " << size[1] << std::endl;
            std::cout << "Center_X: " << (A[0] + C[0]) / 2 << std::endl;
            std::cout << "Center_Y: " << (A[1] + C[1]) / 2 << std::endl;
        }
    }
    catch(mirage::Exception::Any& e) {
        std::cerr << "Error : " <<  e.what() << std::endl;
    }
    catch(...) {
        std::cerr << "Unknown error" << std::endl;
    }

    return 0;
}

