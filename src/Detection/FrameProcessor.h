#ifndef FRAMEPROCESSOR_H
#define FRAMEPROCESSOR_H

#include <string>
#include <vector>

class FrameCapturer;

typedef std::pair<int, int> Center;
typedef std::pair<double, double> PanTiltCentered;

class FrameProcessor
{
    struct Test {
      bool operator()(const ImageRGB::value_type& rgb) {
        return rgb._green > 127;
      }
    };

    typedef mirage::algo::Labelizer<ImageRGB,Test,
                                    mirage::algo::labelNone
                                    | mirage::algo::labelPixelSet
                                    | mirage::algo::labelBoundingBox> Labelizer;

    public:
        FrameProcessor(FrameCapturer& fc);
        ~FrameProcessor();
        void filterColor(int threshold);
        std::vector<PanTiltCentered> findPositions();
        //void nextFrame();
        void nextFakeFrame(std::string filename);
        void writeFrame(std::string filename);
    protected:
    private:
        FrameCapturer* frameCapturer;
        double pan, tilt, zoom;
        ImageRGB frame_in;
        Labelizer labelizer;
        std::vector<PanTiltCentered> pantiltsCentered;

        void pantiltzoom(double* ppan,double* ptilt,double u,double v,double u0,double v0,double pan0,double tilt0,double zoom);
};

#endif // FRAMEPROCESSOR_H
