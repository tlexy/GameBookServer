#pragma once
#include "disposable.h"

// ׷�Ӵ�С
#define ADDITIONAL_SIZE 1024 //1024 * 128

// ��󻺳�
#define MAX_SIZE		1024 * 1024 // 1M

class Buffer :public IDisposable
{
public:
	Buffer(int init_size = 1024);
	virtual unsigned int GetEmptySize();
	// ��ǰ��д����
	virtual unsigned int GetWriteSize() const;
	// ��ǰ�ɶ�����
	virtual unsigned int GetReadSize() const;

	void write(const char* data, int len);
	int read(char* data, int len);

	void FillData(unsigned int  size);
	void RemoveData(unsigned int size);

	//int GetBuffer(char*& pBuffer) const;

	void ReAllocBuffer(unsigned int dataLength);

	//�����len���ȵ������ռ�
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
	unsigned int _beginIndex{ 0 }; // buffer���� ��ʼλ�����λ
	unsigned int _endIndex{ 0 };//�����һ�����õ�λ��

	unsigned int _bufferSize{ 0 }; // �ܳ���
};

