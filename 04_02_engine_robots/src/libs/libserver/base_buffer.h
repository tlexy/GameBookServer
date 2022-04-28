#pragma once
#include "disposable.h"

// 追加大小
#define ADDITIONAL_SIZE 1024 //1024 * 128

// 最大缓冲
#define MAX_SIZE		1024 * 1024 // 1M

class Buffer :public IDisposable
{
public:
	Buffer(int init_size = 1024);
	virtual unsigned int GetEmptySize();
	// 当前可写长度
	virtual unsigned int GetWriteSize() const;
	// 当前可读长度
	virtual unsigned int GetReadSize() const;

	void write(const char* data, int len);
	int read(char* data, int len);

	void FillData(unsigned int  size);
	void RemoveData(unsigned int size);

	//int GetBuffer(char*& pBuffer) const;

	void ReAllocBuffer(unsigned int dataLength);

	//整理出len长度的连续空间
	void ReArrangeBuffer(int len);
	unsigned int GetEndIndex() const
	{
		return _endIndex;
	}

	unsigned int GetBeginIndex() const
	{
		return _beginIndex;
	}

	unsigned int GetTotalSize() const
	{
		return _bufferSize;
	}

private:
	void _ReAlloc(int new_cap);

protected:
	char* _buffer{ nullptr };
	unsigned int _beginIndex{ 0 }; // buffer数据 开始位与结束位
	unsigned int _endIndex{ 0 };//第向第一个可用的位置

	unsigned int _bufferSize{ 0 }; // 总长度
};

