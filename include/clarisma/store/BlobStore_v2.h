// Copyright (c) 2024 Clarisma / GeoDesk contributors
// SPDX-License-Identifier: LGPL-3.0-only

#pragma once

#include <clarisma/store/Store_v2.h>
#include <unordered_map>
#include <clarisma/data/Span.h>
#include <clarisma/util/DataPtr.h>

using std::byte;

namespace clarisma::v2 {

// Important: Offset of free-tables (trunk & leaf) must be
// evenly divisible by 64

class BlobStore : public Store
{
public:
	/*
	void prefetchBlob(void* pBlob)
	{
		uint32_t size = pointer(pBlob).getUnsignedInt() & 0x3fff'ffff;
		prefetch(pBlob, size);
	}
	*/

	using PageNum = uint32_t;

	struct Header		// NOLINT: No constructor needed
	{
		uint32_t magic;
		uint16_t versionLow;
		uint16_t versionHigh;
		DateTime creationTimestamp;
		// uint8_t  guid[16];
		uint8_t  pageSize;
		uint8_t  reserved[3];
		uint32_t metadataSize;
		uint32_t propertiesPointer;
		// uint32_t indexPointer;

		/**
		 * The total number of pages in use (includes free blobs and metadata pages).
		 */
		uint32_t totalPageCount;

		/**
		 * A bitfield indicating which spans of 16 slots in the Trunk Free-Table
		 * have at least one slot that is non-zero.
		 */
		uint32_t trunkFreeTableRanges;
		uint8_t reserved2[28];
	};

	struct HeaderBlock : Header
	{
		uint8_t  subtypeData[1984];
		PageNum trunkFreeTable[512];
	};

	static constexpr size_t HEADER_BLOCK_SIZE = 4096;
	static_assert(sizeof(Header) == 64, "Expected 64-byte Header");
	static_assert(sizeof(HeaderBlock) == HEADER_BLOCK_SIZE, "Expected 4KB HeaderBlock");

	struct Blob
	{
		uint32_t precedingFreeBlobPages;
		uint32_t payloadSize : 30;
		unsigned unused : 1;			// TODO: bool causes alignment issues on MSVC?
		unsigned isFree : 1;
		union
		{
			struct
			{
				PageNum  prevFreeBlob;
				PageNum  nextFreeBlob;
				uint32_t leafFreeTableRanges;
				uint8_t  unused2[44];
				PageNum  leafFreeTable[512];
			};
			byte payload[1];			// variable length
		};
	};

	class Transaction : protected Store::Transaction
	{
	public:
		explicit Transaction(BlobStore* store) :
			Store::Transaction(store)
		{
		}

		void begin(LockLevel lockLevel = LockLevel::LOCK_APPEND)
		{
			Store::Transaction::begin(lockLevel);
		}
		[[nodiscard]] BlobStore* store() const { return reinterpret_cast<BlobStore*>(store_); }
		PageNum alloc(uint32_t payloadSize);
		void free(PageNum firstPage);
		PageNum addBlob(ByteSpan data);
		void commit();
		void end() { Store::Transaction::end(); }

	protected:
		HeaderBlock* getRootBlock()
		{
			return reinterpret_cast<HeaderBlock*>(getBlock(0));
		}

	private:
		Blob* getBlobBlock(PageNum page)
		{
			return reinterpret_cast<Blob*>(
				getBlock(static_cast<uint64_t>(page)
					<< store()->pageSizeShift_));
		}

		void addFreeBlob(PageNum firstPage, uint32_t pages, uint32_t precedingFreePages);
		void removeFreeBlob(Blob* freeBlock);
		PageNum relocateFreeTable(PageNum page, int sizeInPages);
		[[nodiscard]] bool isFirstPageOfSegment(PageNum page) const
		{
			return (page & ((0x3fff'ffff) >> store()->pageSizeShift_)) == 0;
		}

		std::unordered_map<PageNum, uint32_t> freedBlobs_;
	};

	template<typename T>
	class CreateTransaction : public T::Transaction
	{
	public:
		CreateTransaction() : T::Transaction(&store_)
		{
		}

		void begin(const char* filename)
		{
			store_.create(filename);
			Transaction::begin(LOCK_EXCLUSIVE);
			initHeader(T::Transaction::getRootBlock());
		}

		void end()
		{
			Transaction::end();
			store_.close();
		}

	protected:
		T store_;
	};

	uint64_t offsetOf(PageNum page) const
	{
		return static_cast<uint64_t>(page) << pageSizeShift_;
	}

	byte* translatePage(PageNum page)
	{
		return translate(offsetOf(page));
	}

protected:
	static const uint32_t MAGIC = 0x7ADA0BB1;

	static const int BLOB_HEADER_SIZE = 8;

	// TODO: Return BlobPtr
	DataPtr pagePointer(PageNum page)
	{
		return DataPtr(data(static_cast<uint64_t>(page) << pageSizeShift_));
	}

	void verifyHeader() const override;
	void initialize() override;

	Header* getRoot() const
	{
		return reinterpret_cast<Header*>(mainMapping());
	}

	DateTime getLocalCreationTimestamp() const override;
	uint64_t getTrueSize() const override;
	uint32_t pagesForPayloadSize(uint32_t payloadSize) const;
	static void setMetadataSize(Header* header, size_t size);

private:
	static void initHeader(HeaderBlock* header);

	uint32_t pageSizeShift_ = 12;	// TODO: default 4KB page

	template<typename T>
	friend class CreateTransaction;
};

} // namespace clarisma
