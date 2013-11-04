#ifndef FRAMECAPTURER_H
#define FRAMECAPTURER_H

#include <string>
#include <axisPTZ.h>
#include <mirage.h>

using namespace std;

typedef mirage::img::Coding<mirage::colorspace::RGB_24>::Frame ImageRGB;

class FrameCapturer
{
    public:
        FrameCapturer(string host, int port, string user, string password);
        //FrameCapturer(FrameCapturer& fc);
        ~FrameCapturer();

        void getPanTiltZoom(double &pan, double &tilt, double &zoom);
        void setPanTilt(double &pan, double &tilt);
        void setZoom(double &zoom) const;
        string getHost(){return host;}
        int getPort(){return port;}
        string getUsername(){return username;}
        string getPassword(){return password;}
        ImageRGB getFrame();
        ImageRGB getFakeFrame(string filename);

    protected:
    private:
        string host;
        int port;
        string username;
        string password;

        axis::PTZ axis;
        ImageRGB frame;
        ImageRGB fakeFrame;

        void init();
        void rgb2bgr(ImageRGB& img);
        void rgb2bgr(ImageRGB::value_type& rgb);
};

#endif // FRAMECAPTURER_H
