#ifndef FRAMEPROCESSOR_H
#define FRAMEPROCESSOR_H

class FrameCapturer;

class FrameProcessor
{
    public:
        FrameProcessor(FrameCapturer& fc);
        ~FrameProcessor();
    protected:
    private:
};

#endif // FRAMEPROCESSOR_H
