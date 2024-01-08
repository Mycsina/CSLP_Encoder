#include "RLEEncoder.hpp"

using namespace std;

RLEEncoder::RLEEncoder(Golomb *golomb) {
    g = golomb;
    last_value = 0;
    cnt = 0;
}

RLEEncoder::~RLEEncoder() {
    flush_buffer();
}

void RLEEncoder::flush_buffer() {
    g->encode(cnt);
    g->encode(last_value);
    cnt = 0;
}

void RLEEncoder::fetch_buffer() {
    cnt = g->decode();
    last_value = g->decode();
}

void RLEEncoder::push(const int value) {
    if (value != last_value && cnt != 0) {
        flush_buffer();
    }
    cnt += 1;
    last_value = value;
}

int RLEEncoder::pop() {
    if (cnt == 0) {
        fetch_buffer();
    }
    cnt -= 1;
    return last_value;
}