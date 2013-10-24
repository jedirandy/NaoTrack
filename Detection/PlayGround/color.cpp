#include <mirage.h>

typedef mirage::img::Coding<mirage::colorspace::RGB_24>::Frame ImageRGB; 

int main(int argc, char* argv[]) {
    ImageRGB input_image;
    ImageRGB output_image;

    std::string input_name("input.jpg");
    std::string output_name("output.jpg");

    try {
        mirage::img::JPEG::read(input_image, input_name);

        output_image.resize(input_image._dimension);

        ImageRGB::pixel_type p1,p2,p_end;
        ImageRGB::value_type black(0,0,0);
        ImageRGB::value_type green(0,255,0);
        int threshold = 50;
        for(p1=input_image.begin(),p2=output_image.begin(),p_end=input_image.end();
            p1 != p_end;
            ++p1,++p2) {
            ImageRGB::value_type& v1 = *p1;
            
            if (v1._green > v1._blue && v1._green > v1._red) {
                int temp = v1._blue < v1._red ? v1._blue : v1._red;
                if (v1._green - temp > threshold)
                    *p2 = green;
                else
                    *p2 = black;
            }
            else
                *p2 = black;
        }

        mirage::img::JPEG::write(output_image, output_name, 70);
    }
    catch(mirage::Exception::Any& e) {
        std::cerr << "Error : " <<  e.what() << std::endl;
    }
    catch(...) {
        std::cerr << "Unknown error" << std::endl;
    }
    return 0;
}

