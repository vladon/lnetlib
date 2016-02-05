#include "helper.h"

namespace lnetlib
{
helper::file helper::read_binary_file(const std::string& filename)
{
    uint64_t size = 0;
    char* data = nullptr;

    std::ifstream stream(filename, std::ios::binary | std::ios::ate);

    if (stream.is_open())
    {
        size = stream.tellg();
        data = new char[size];

        stream.seekg(0);
        stream.read(data, size);
    }

    return file(std::unique_ptr<char>(data), size);
}
}
