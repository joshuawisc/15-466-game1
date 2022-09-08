#include <iostream>
#include <fstream>

//for get path to data
#include "data_path.hpp"


struct TileAsset {
    PPU466::Tile tile;
    TileAsset(std::string filename) {
        std::ifstream file(filename, std::ios::binary);
        if (file.fail())
            throw std::runtime_error("Failed to open player file");
        if (!file.read(reinterpret_cast< char * >(&tile.bit0[0]), sizeof(tile.bit0))) {
            throw std::runtime_error("Failed to read chunk data.");
        }
        if (!file.read(reinterpret_cast< char * >(&tile.bit1[0]), sizeof(tile.bit1))) {
            throw std::runtime_error("Failed to read chunk data.");
        }
    }
};
