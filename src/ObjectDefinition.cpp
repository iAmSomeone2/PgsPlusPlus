//
// Created by bdavidson on 10/24/20.
//

#include "ObjectDefinition.hpp"
#include <endian.h>

using namespace Pgs;


uint32_t ObjectDefinition::read3Bytes(const uint8_t *data, uint16_t &readPos)
{
    auto val = static_cast<uint32_t>((data[readPos] << 16u) | (data[readPos + 1] << 8u) |
                                     (data[readPos + 2]) & 0x00FFFFFF);
    readPos += 3;

    return le32toh(val);
}

ObjectDefinition::ObjectDefinition()
{
    this->id = 0u;
    this->version = 0u;
    this->sequenceFlag = SequenceFlag::Only;
    this->dataLength = 0u;
    this->width = 0u;
    this->height = 0u;
}

void ObjectDefinition::import(const char *data, const uint16_t &size)
{
    if(!data)
    {
        throw ImportException("ObjectDefinition: no data provided to import.");
    }

    if(size < ObjectDefinition::MIN_BYTE_SIZE)
    {
        throw ImportException("ObjectDefinition: Insufficient data provided to import.");
    }

    const auto byteData = reinterpret_cast<const uint8_t *>(data);
    uint16_t readPos = 0u;
    this->id = read2Bytes(byteData, readPos);
    this->version = byteData[readPos];
    ++readPos;
    this->sequenceFlag = SequenceFlag(byteData[readPos]);
    ++readPos;

    /*
     * At some point, I should check what exactly this value is counting. An individual segment can't be
     * larger than 64kb, but a 24-bit size value allows for indexing up to ~16mb. This value is likely the
     * total number of bytes across the entire sequence.
     */
    this->dataLength = read3Bytes(byteData, readPos);

    this->width = read2Bytes(byteData, readPos);
    this->height = read2Bytes(byteData, readPos);

    const uint16_t remainingSize = size - readPos;
    this->objectData.resize(remainingSize);
    for(uint16_t i = 0u; i < remainingSize; ++i)
    {
        this->objectData[i] = byteData[readPos];
        ++readPos;
    }
}

void ObjectDefinition::import(const vector<char> &data)
{
    this->import(data.data(), data.size());
}