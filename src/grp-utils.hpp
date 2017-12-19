#pragma once


#include "dsp/ringbuffer.hpp"


template <typename T, int S>
struct RingBufferReader : public rack::RingBuffer<T, S> {
     T readAtOffset(int offset){
        // return RingBuffer<T,S>::data[2];
        return RingBuffer<T,S>::data[RingBuffer<T,S>::mask(RingBuffer<T,S>::start + offset)];
     }
};


