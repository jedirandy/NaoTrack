#include <glog/logging.h>
#include "FrameCapturer.h"
#include "FrameProcessor.h"

FrameProcessor::FrameProcessor(FrameCapturer& fc)
{
    LOG(INFO) << __PRETTY_FUNCTION__;
}

FrameProcessor::~FrameProcessor()
{
    LOG(INFO) << __PRETTY_FUNCTION__;
}
