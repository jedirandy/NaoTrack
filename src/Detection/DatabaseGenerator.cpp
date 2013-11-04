#include <string>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <glog/logging.h>
#include "FrameCapturer.h"
#include "FrameProcessor.h"

using namespace std;
using namespace boost::filesystem;

void loggerInit(char* argv0) {
    google::InitGoogleLogging(argv0);
    FLAGS_logtostderr = 1;
    FLAGS_minloglevel = 1;
}

int main(int argc, char* argv[]) {
    loggerInit(argv[0]);

    string filename("data");
    path dataFilePath(filename);
    if (exists(dataFilePath))
        remove(filename);

    ofstream dataFile(filename);
    path dir(".");
    directory_iterator it(dir), eod;
    if(is_directory(dir)) {
        BOOST_FOREACH(path const &p, std::make_pair(it, eod)) {
            std::cout << p.filename().string() << std::endl;
            dataFile << p.filename().string() << endl;
        }
    }

    dataFile.close();

    return 0;
}
