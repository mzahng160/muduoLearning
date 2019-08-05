#ifndef MUDUO_NET_BUFFER_H
#define MUDUO_NET_BUFFER_H

#include "datetime/copyable.h"

#include <string>
#include <vector>

namespace muduo
{
	class Buffer : public muduo::copyable
	{
	public:
		static const size_t kCheapPrepend = 8;
		static const size_t kInitialSize = 1024;

		Buffer()
			: buffer_(kCheapPrepend + kInitialSize),
			  readerIndex_(kCheapPrepend),
			  writerIndex_(kCheapPrepend)
	  	{
	  		assert(readableBytes() == 0);
	  		assert(writableBytes() == kInitialSize);
	  		assert(prependableBytes() == kCheapPrepend);
	  	}

		//~Buffer();
		void swap(Buffer& rhs)
		{
			buffer_.swap(rhs.buffer_);
			std::swap(readerIndex_, rhs.readerIndex_);
			std::swap(writerIndex_, rhs.writerIndex_);
		}
		
		size_t readableBytes() const
		{ return writerIndex_ - readerIndex_; }

		size_t writableBytes() const
		{ return buffer_.size() - writerIndex_; }

		size_t prependableBytes() const
		{ return readerIndex_; }

		const char* peek() const
		{ return begin() + readerIndex_; }

		void retrieve(size_t len)
		{
			assert(le <= readableBytes());
			readerIndex_ += len;
		}

		void retrieveUntil(const char* end)
		{
			assert(peek() <= end);
			assert(end <= beginWrite());
			retrieve(end-peek());
		}

		void retrieveAll()
		{
			readerIndex_ = kCheapPrepend;
			writerIndex_ = kCheapPrepend;
		}

		void retrieveAsString()
		{
			std::string str(peek(), readableBytes());
			retrieveAll();
			return str;
		}

		void append(const std::string str)
		{
			append(str.data(), str.length());
		}

		void append(const char* data, size_t len)
		{
			ensureWritableBytes(len);
			std::copy(data, data+len, beginWrite());
			hasWritten(len);
		}

		void append(const void* data, size_t len)
		{
			append(static_cast<char*>(data), len);
		}

		void ensureWritableBytes(size_t len)
		{
			if(writableBytes() < len)
				makeSpace(len);
			assert(writableBytes() >= len);
		}

		char* beginWrite()
		{ return begin() + writerIndex_; }

		const char* beginWrite() const
		{ return begin() + writerIndex_; }

		void hasWritten(size_t len)
		{ writerIndex_ += len; }

		void prepend()
		{}

		void shrink(size_t reserve)
		{
			
		}
	private:
		char* begin()
		{ return &*buffer_.begin(); }

		const char* begin() const
		{ return &*buffer_.begin(); }

		void makeSpace(size_t len)
		{

		}


		std::vector<char> buffer_;
		size_t readerIndex_;
		size_t writerIndex_;
		
	};
}

#endif