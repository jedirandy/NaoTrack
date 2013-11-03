#include <string>
#include <glog/logging.h>
#include "FrameCapturer.h"
#include "FrameProcessor.h"

void loggerInit(char* argv0) {
    google::InitGoogleLogging(argv0);
    FLAGS_logtostderr = 1;
    FLAGS_minloglevel = 0;
}

int main(int argc, char* argv[]) {
    loggerInit(argv[0]);

    //std::string host("ptz1.grid.metz.supelec.fr");
    //int port = 80;
    //std::string user("frezza");
    //std::string password("gloubi");

    FrameCapturer fc("ptz1.grid.metz.supelec.fr",80,"frezza","gloubi");

    //double pan, tilt, zoom;
    //fc.getPanTiltZoom(pan, tilt, zoom);
    //std::cout << "pan:" << pan << std::endl;
    //std::cout << "tilt:" << tilt << std::endl;
    //std::cout << "zoom:" << zoom << std::endl;
    //FrameProcessor * fp = new FrameProcessor();
    //delete fp;
    return 0;
}

