#include "base_buffer.h"
#include <iostream>
#include <cstring>

Buffer::Buffer(int init_size)
{
	_ReAlloc(init_size);
}

unsigned Buffer::GetEmptySize()
{
	return _bufferSize - _endIndex + _beginIndex;
}

void Buffer::ReArrangeBuffer(int len)
{
	if (GetEmptySize() >= len)
	{
		::memmove(_buffer, _buffer + _beginIndex, _endIndex - _beginIndex);
	}
	else
	{
		_ReAlloc(_bufferSize + len * 2);
	}
}

unsigned int Buffer::GetReadSize() const
{
	return _endIndex - _beginIndex;
}

unsigned int Buffer::GetWriteSize() const
{
	return _bufferSize - _endIndex;
}

void Buffer::FillData(unsigned int  size)
{
	_endIndex += size;
	if (_endIndex > _bufferSize)
	{
		_endIndex = _bufferSize;
	}
}

void Buffer::RemoveData(unsigned int size)
{
	_beginIndex += size;
	if (_beginIndex > _endIndex)
	{
		_beginIndex = _endIndex;
	}
}

void Buffer::_ReAlloc(int new_cap)
{
	if (new_cap <= _bufferSize)
	{
		return;
	}

	char* tempBuffer = new char[new_cap];
	unsigned int data_len = _endIndex - _beginIndex;
	::memcpy(tempBuffer, _buffer + _beginIndex, data_len);

	delete[] _buffer;
	_buffer = tempBuffer;

	_beginIndex = 0;
	_endIndex = data_len;
	_bufferSize = new_cap;
}

//int Buffer::GetBuffer(char*& pBuffer) const
//{
//	pBuffer = _buffer + _endIndex;
//	return GetWriteSize();
//}

void Buffer::ReAllocBuffer(const unsigned int dataLength)
{
	// 如果缓冲区超过最大缓冲值，可能有异常，直接关闭socket
	if (_bufferSize >= MAX_SIZE) {
		std::cout << "Buffer::Realloc except!! " << std::endl;
	}

	_ReAlloc(dataLength);
}

void Buffer::write(const char* data, int len)
{
	if (GetWriteSize() < len)
	{
		_ReAlloc(_bufferSize + len);
	}
	::memcpy(_buffer + _endIndex, data, len);
	_endIndex += len;
}

int Buffer::read(char* data, int len)
{
	int s = GetReadSize();
	int min = s > len ? len : s;
	memcpy(data, _buffer + _beginIndex, min);
	_beginIndex += min;
	return min;
}
