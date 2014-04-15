#include "stdafx.h"
#include "DDCircularBuffer.h"


DDCircularBuffer::DDCircularBuffer( size_t capacity )
: mBeginIndex( 0 ), mEndIndex( 0 ), mCurrentSize( 0 ), mCapacity( capacity )
{
	mData = new char[capacity];
}

DDCircularBuffer::~DDCircularBuffer( )
{
	delete[] mData;
}

bool DDCircularBuffer::Write( const char* data, size_t bytes )
{
	if ( bytes == 0 )
		return false;

	/// �뷮 ����
	if ( bytes > mCapacity - mCurrentSize )
		return false;

	// �ٷ� ���� ������ ���
	if ( bytes <= mCapacity - mEndIndex )
	{
		memcpy( mData + mEndIndex, data, bytes );
		mEndIndex += bytes;

		if ( mEndIndex == mCapacity )
			mEndIndex = 0;
	}
	// �ɰ��� ��� �� ���
	else
	{
		size_t size1 = mCapacity - mEndIndex;
		memcpy( mData + mEndIndex, data, size1 );

		size_t size2 = bytes - size1;
		memcpy( mData, data + size1, size2 );
		mEndIndex = size2;
	}

	mCurrentSize += bytes;

	return true;
}

bool DDCircularBuffer::Read( char* data, size_t bytes )
{
	if ( bytes == 0 )
		return false;

	if ( mCurrentSize < bytes )
		return false;

	/// �ٷ� �ѹ��� �о� �� �� �ִ� ���
	if ( bytes <= mCapacity - mBeginIndex )
	{
		memcpy( data, mData + mBeginIndex, bytes );
		mBeginIndex += bytes;

		if ( mBeginIndex == mCapacity )
			mBeginIndex = 0;
	}
	/// �о�� �����Ͱ� �ɰ��� �ִ� ���
	else
	{
		size_t size1 = mCapacity - mBeginIndex;
		memcpy( data, mData + mBeginIndex, size1 );

		size_t size2 = bytes - size1;
		memcpy( data + size1, mData, size2 );
		mBeginIndex = size2;
	}

	mCurrentSize -= bytes;

	return true;
}

void DDCircularBuffer::Peek( char* data )
{
	/// �ٷ� �ѹ��� �о� �� �� �ִ� ���
	if ( mCurrentSize <= mCapacity - mBeginIndex )
	{
		memcpy( data, mData + mBeginIndex, mCurrentSize );
	}
	/// �о�� �����Ͱ� �ɰ��� �ִ� ���
	else
	{
		size_t size1 = mCapacity - mBeginIndex;
		memcpy( data, mData + mBeginIndex, size1 );

		size_t size2 = mCurrentSize - size1;
		memcpy( data + size1, mData, size2 );
	}
}

bool DDCircularBuffer::Peek( char* data, size_t bytes )
{
	if ( bytes == 0 )
		return false;

	if ( mCurrentSize < bytes )
		return false;

	/// �ٷ� �ѹ��� �о� �� �� �ִ� ���
	if ( bytes <= mCapacity - mBeginIndex )
	{
		memcpy( data, mData + mBeginIndex, bytes );
	}
	/// �о�� �����Ͱ� �ɰ��� �ִ� ���
	else
	{
		size_t size1 = mCapacity - mBeginIndex;
		memcpy( data, mData + mBeginIndex, size1 );

		size_t size2 = bytes - size1;
		memcpy( data + size1, mData, size2 );
	}

	return true;
}


bool DDCircularBuffer::Consume( size_t bytes )
{
	if ( bytes == 0 )
		return false;

	if ( mCurrentSize < bytes )
		return false;

	/// �ٷ� �ѹ��� ������ �� �ִ� ���
	if ( bytes <= mCapacity - mBeginIndex )
	{
		mBeginIndex += bytes;

		if ( mBeginIndex == mCapacity )
			mBeginIndex = 0;
	}
	/// ������ �����Ͱ� �ɰ��� �ִ� ���
	else
	{
		size_t size2 = bytes + mBeginIndex - mCapacity;
		mBeginIndex = size2;
	}

	mCurrentSize -= bytes;

	return true;

}