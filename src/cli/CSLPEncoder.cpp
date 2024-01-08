#include "../../libs/cxxopts.hpp"
#include "../codec/encoders/lossless/LosslessHybrid.hpp"
#include "../codec/encoders/lossless/LosslessIntra.hpp"
#include "../codec/encoders/lossy/LossyHybrid.hpp"
#include "../codec/encoders/lossy/LossyIntra.hpp"

#include <iostream>

using namespace std;

int main(const int argc, char **argv) {
    cxxopts::Options options("CSLP", "A video codecs suite");
    options.add_options()("c, codec", "The codec to use", cxxopts::value<std::string>()->default_value("hybrid"))(
            "mode", "Whether to encode or decode", cxxopts::value<std::string>()->default_value("encode"))(
            "i,input", "Path to video file to read from", cxxopts::value<std::string>())(
            "o,output", "Path where to output video file", cxxopts::value<std::string>())(
            "m,golomb_m", "Golomb m parameter, if not provided will pick a good one",
            cxxopts::value<uint8_t>()->default_value("0"))("b,block_size", "Block size",
                                                           cxxopts::value<uint8_t>()->default_value("16"))(
            "p,period", "Period", cxxopts::value<uint8_t>()->default_value("10"))(
            "search,search_radius", "Search radius, if not provided will use faster search", cxxopts::value<uint8_t>())(
            "y,y_quantizer", "Y quantizer", cxxopts::value<uint8_t>())("u,u_quantizer", "U quantizer",
                                                                       cxxopts::value<uint8_t>())(
            "v,v_quantizer", "V quantizer", cxxopts::value<uint8_t>())("h,help", "Print usage");
    cout << endl;
    const auto result = options.parse(argc, argv);
    if (result.count("help")) {
        cout << options.help() << endl;
        return 0;
    }
    if (!result.count("input") || !result.count("output")) {
        cout << "Input and output paths are required" << endl;
        return 1;
    }
    const auto mode = result["mode"].as<string>();
    const auto codec = result["codec"].as<string>();
    const auto input = result["input"].as<string>();
    const auto output = result["output"].as<string>();
    const auto m = result["m"].as<uint8_t>();
    if (mode != "encode" && mode != "decode") {
        cout << "[E] Invalid mode requested" << endl;
        cout << "    Valid modes are 'encode' and 'decode'" << endl;
        return 1;
    }
    if (mode == "encode") {
        const bool lossless = codec.substr(0, 8) == "lossless";
        Encoder *encoder = nullptr;
        if (lossless) {
            if (codec == "lossless_intra") { encoder = new LosslessIntraEncoder(input.c_str(), output.c_str(), m); }
            if (codec == "lossless_hybrid") {
                const auto b = result["block_size"].as<uint8_t>();
                const auto p = result["period"].as<uint8_t>();
                encoder = new LosslessHybridEncoder(input.c_str(), output.c_str(), m, b, p);
            }
        }
        if (!result.count("y") || !result.count("u") || !result.count("v")) {
            cout << "[E] Y, U and V quantizing steps are required for lossy encoding" << endl;
            cout << "    Valid values are between 1 and 255, but prefer values such as (32, 64, 128)" << endl;
            return 1;
        }
        if (codec == "hybrid") {
            const auto b = result["block_size"].as<uint8_t>();
            const auto p = result["period"].as<uint8_t>();
            const auto y = result["y"].as<uint8_t>();
            const auto u = result["u"].as<uint8_t>();
            const auto v = result["v"].as<uint8_t>();
            encoder = new LossyHybridEncoder(input.c_str(), output.c_str(), m, b, p, y, u, v);
        }
        if (codec == "intra") {
            const auto y = result["y"].as<uint8_t>();
            const auto u = result["u"].as<uint8_t>();
            const auto v = result["v"].as<uint8_t>();
            encoder = new LossyIntraEncoder(input.c_str(), output.c_str(), m, y, u, v);
        }
        if (encoder == nullptr) {
            cout << "[E] Invalid codec requested" << endl;
            cout << "    Valid codecs are 'lossless_intra', 'lossless_hybrid', 'intra' and 'hybrid'" << endl;
            return 1;
        }
        cout << "[I] Starting encoding with " << codec << " codec" << endl;
        const auto start = clock();
        encoder->encode();
        const auto end = clock();
        cout << "[I] Encoding took " << (end - start) / static_cast<double>(CLOCKS_PER_SEC) << " seconds" << endl;
        delete encoder;
        return 0;
    }
    if (mode == "decode") {

        Encoder *encoder;
        if (codec == "lossless_intra") {
            encoder = new LosslessIntraEncoder(input.c_str(), output.c_str());
        } else if (codec == "lossless_hybrid") {
            encoder = new LosslessHybridEncoder(input.c_str(), output.c_str());
        } else if (codec == "hybrid") {
            encoder = new LossyHybridEncoder(input.c_str(), output.c_str());
        } else if (codec == "intra") {
            encoder = new LossyIntraEncoder(input.c_str(), output.c_str());
        } else {
            cout << "[E] Invalid codec requested" << endl;
            cout << "    Valid codecs are 'lossless_intra', 'lossless_hybrid', 'intra' and 'hybrid'" << endl;
            return 1;
        }
        cout << "[I] Starting decoding with " << codec << " codec" << endl;
        const auto start = clock();
        encoder->decode();
        const auto end = clock();
        cout << "[I] Decoding took " << (end - start) / static_cast<double>(CLOCKS_PER_SEC) << " seconds" << endl;
        delete encoder;
        return 0;
    }
}