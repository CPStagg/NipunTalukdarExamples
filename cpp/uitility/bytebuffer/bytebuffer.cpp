#include <stdlib.h>
#include <string.h>
#ifndef __USE_BSD
#define __USE_BSD
#endif
#include <endian.h>
#include <bytebuffer.hpp>

using GeetPutula::ByteBufferException;

GeetPutula::ByteBuffer::ByteBuffer(size_t size, Endian endian)
{
    _size = size;
    _position = 0;
    _endian = endian;
    if (size > 0)
        _data = calloc(1, size);
    else
        _data = 0;
}

GeetPutula::ByteBuffer::ByteBuffer(const ByteBuffer& buffer)
{
    _size = buffer._size;
    _position = buffer._position;
    _endian = buffer._endian;
    if (_size > 0) {
        _data = malloc(_size);
        memcpy(_data, buffer._data, _size);
    } else {
        _data = 0;
    }
}

GeetPutula::ByteBuffer& GeetPutula::ByteBuffer::operator=(const ByteBuffer& buffer)
{
    if (this != &buffer) {
        _size = buffer._size;
        _position = buffer._position;
        _endian = buffer._endian;
        if (_data)
            free(_data);
        if (_size > 0) {
            _data = malloc(_size);
            memcpy(_data, buffer._data, _size);
        } else {
            _data = 0;
        }
    }
    return *this;
}

void GeetPutula::ByteBuffer::resize(size_t size) 
{
    void *temp = calloc(1, size);
    memcpy(temp, _data, _size > size ? size : _size);
    free(_data);
    _data = temp;
    _size = size;
}

bool GeetPutula::ByteBuffer::putInt32(int32_t val, size_t position)
{
    int32_t converted = 0;
    if (_endian == BIG) 
        converted = htobe32(val);
    else
        converted = htole32(val);
    return putBytes((void *)&converted, 4, position);
}

int32_t GeetPutula::ByteBuffer::getInt32(size_t position)
{
    int32_t val = 0;
    readBytes((void *)&val, 4, position);
    if (_endian == BIG) 
        return be32toh(val);
    return le32toh(val);
}

bool GeetPutula::ByteBuffer::putInt16(int16_t val, size_t position)
{
    int16_t converted = 0;
    if (_endian == BIG) 
        converted = htobe16(val);
    else
        converted = htole16(val);
    return putBytes((void *)&converted, 2, position);
}

int16_t GeetPutula::ByteBuffer::getInt16(size_t position)
{
    int16_t val = 0;
    readBytes((void *)&val, 2, position);
    if (_endian == BIG) 
        return be16toh(val);
    return le16toh(val);
}

bool GeetPutula::ByteBuffer::putInt64(int64_t val, size_t position)
{
    int64_t converted = 0;
    if (_endian == BIG) 
        converted = htobe64(val);
    else
        converted = htole64(val);
    return putBytes((void *)&converted, 8, position);
}

int64_t GeetPutula::ByteBuffer::getInt64(size_t position)
{
    int64_t val = 0;
    readBytes((void *)&val, 8, position);
    if (_endian == BIG) 
        return be64toh(val);
    return le64toh(val);
}

bool GeetPutula::ByteBuffer::putUInt32(uint32_t val, size_t position)
{
    return putInt32((int32_t)val, position);
}

uint32_t GeetPutula::ByteBuffer::getUInt32(size_t position)
{
    return (uint32_t) getInt32(position);
}

bool GeetPutula::ByteBuffer::putUInt16(uint16_t val, size_t position)
{
    return putInt16((int16_t)val, position);
}

uint16_t GeetPutula::ByteBuffer::getUInt16(size_t position)
{
    return (uint16_t) getInt16(position);
}

bool GeetPutula::ByteBuffer::putUInt64(uint64_t val, size_t position)
{
    return putInt64((int64_t)val, position);
}

uint64_t GeetPutula::ByteBuffer::getUInt64(size_t position)
{
    return (uint64_t)getInt64(position);
}

bool GeetPutula::ByteBuffer::putDouble(double val, size_t position)
{
    position = adjustPosition(position);
    if ((position + sizeof(double)) > _size)
        return false;
    if (((_endian == BIG) && littleEndianHost()) ||
            ((_endian == LITTLE) && !littleEndianHost())) {
        // swap needed
        int i = 0, j = sizeof(double);
        char *ptr = (char *)&val;
        while (i < j){
            *((char *)_data + position + i) = *(ptr + j - i -1);
            i++;
        }
    } else {
        memcpy((char *)_data + _position, (void *)&val, sizeof(double));
    }
    _position = position + sizeof(double);
    return true;
}

double GeetPutula::ByteBuffer::getDouble(size_t position)
{
    double val = 0.0;
    position = adjustPosition(position);

    if ((position + sizeof(double)) > _size)
        throw ByteBufferException("Cannot get a double from current position");
    if (((_endian == BIG) && littleEndianHost()) ||
            ((_endian == LITTLE) && !littleEndianHost())) {
        int i = 0, j = sizeof(double);
        char *ptr = (char *)&val;
        while (i < j){
            *(ptr + j - i -1) = *((char *)_data + position + i);
            i++;
        }
    } else {
        memcpy((void *)&val, (char *)_data + _position, sizeof(double));
    }
    _position = position + sizeof(double);
    return val;
}

bool GeetPutula::ByteBuffer::putFloat(float val, size_t position)
{
    position = adjustPosition(position);
    if ((position + sizeof(float)) > _size)
        return false;
    if (((_endian == BIG) && littleEndianHost()) ||
            ((_endian == LITTLE) && !littleEndianHost())) {
        // swap needed
        int i = 0, j = sizeof(float);
        char *ptr = (char *)&val;
        while (i < j){
            *((char *)_data + position + i) = *(ptr + j - i -1);
            i++;
        }
    } else {
        memcpy((char *)_data + _position, (char *)&val, sizeof(float));
    }
    _position = position + sizeof(float);
    return true;
}

float GeetPutula::ByteBuffer::getFloat(size_t position)
{
    float val = 0.0;
    position = adjustPosition(position);

    if ((position + sizeof(float)) > _size)
        throw ByteBufferException("Cannot get a float from current position");
    if (((_endian == BIG) && littleEndianHost()) ||
            ((_endian == LITTLE) && !littleEndianHost())) {
        // swap needed
        int i = 0, j = sizeof(float);
        char *ptr = (char *)&val;
        while (i < j){
            *(ptr + j - i -1) = *((char *)_data + position + i);
            i++;
        }
    } else {
        memcpy((void *)&val, (char *)_data + _position, sizeof(float));
    }
    _position = position + sizeof(float);
    return val;
}

bool GeetPutula::ByteBuffer::putChar(char c, size_t position)
{
    return putBytes(&c, sizeof(char), position);
}

char GeetPutula::ByteBuffer::getChar(size_t position)
{
    char val = '0';
    readBytes((void *)&val, sizeof(char), position);
    return val;
}

bool GeetPutula::ByteBuffer::putWChar(wchar_t c, size_t position)
{
    return putBytes(&c, sizeof(wchar_t), position);
}

wchar_t GeetPutula::ByteBuffer::getWChar(size_t position)
{
    wchar_t val = '0';
    readBytes((void *)&val, sizeof(wchar_t), position);
    return val;
}

GeetPutula::ByteBuffer::~ByteBuffer()
{
    if (_data)
        free(_data);
}

void GeetPutula::ByteBuffer::rewind()
{
    _position = 0;
}

size_t GeetPutula::ByteBuffer::position(size_t newposition)
{
    size_t oldposition = _position;
    if (newposition > _size)
        _position = _size;
    else
        _position = newposition;
    return oldposition;
}

bool GeetPutula::ByteBuffer::putBytes(void *bytes, size_t size, size_t position)
{
    position = adjustPosition(position); 
    if ((position + size ) > _size)
        return false;
    memcpy((char *)_data + _position, bytes, size);
    _position = position + size;
    return true;
}

void GeetPutula::ByteBuffer::readBytes(void *dest, size_t size, size_t position)
{
    position = adjustPosition(position);
    if ((_position + size ) > _size)
        throw ByteBufferException("Not enough data to read");
    memcpy(dest , (char *)_data + _position, size);
    _position = position + size;
}

void GeetPutula::ByteBuffer::proceed(size_t steps)
{
    if ((_position + steps) > _size )
        throw ByteBufferException("Overflow error");
    _position += steps;
}

void GeetPutula::ByteBuffer::back(size_t steps)
{
    if (_position < steps)
        throw ByteBufferException("Underflow error");
    _position -= steps;
}

size_t GeetPutula::ByteBuffer::copyRaw(void *output, size_t start, size_t maxbytes) const
{
    if (!output || (start >= _size) || (maxbytes == 0))
        return 0;
    maxbytes = ((start + maxbytes) > _size) ? (_size - start + 1) :
                        maxbytes;
    memcpy(output, (char *)_data + start , maxbytes);
    
    return maxbytes;
}

bool GeetPutula::ByteBuffer::littleEndianHost()
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
    return true;
#else
    return false;
#endif
}
