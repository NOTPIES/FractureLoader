#include "../../Public/Snowdrop/VFS.h"
#include "../Memory/functions.h"
#include "../Logging/logger.h"
#include "../Loader/loader_manager.h"
#include <algorithm>
#include <cstdio>

namespace Snowdrop::VFS
{
	std::vector<uint8_t> ExtractVFSFile(const std::string& vfsPath)
	{
		if (vfsPath.find(':') != std::string::npos)
		{
			FRACTURE_ERROR("ExtractVFSFile: System paths are not allowed: {}", vfsPath);
			return {};
		}

		void* readerMemory = Alloc(88);
		if (!readerMemory)
		{
			FRACTURE_ERROR("ExtractVFSFile: Failed to allocate reader memory");
			return {};
		}

		void* readerContext = SnowdropInitFileStruct(readerMemory);
		if (!readerContext)
		{
			FRACTURE_ERROR("ExtractVFSFile: Failed to initialize reader struct");
			return {};
		}

		if (!SnowdropOpenFile((__int64)readerContext, (unsigned char*)vfsPath.c_str(), 1))
		{
			FRACTURE_ERROR("ExtractVFSFile: Failed to open file: {}", vfsPath);
			return {};
		}

		printf("readerContext: 0x%p\n", readerContext);

		// size is at offset 4 of readerContext (int*)
		auto ctxInt = static_cast<int*>(readerContext);
		int fileSize = ctxInt[1];

		if (fileSize <= 0)
		{
			FRACTURE_ERROR("ExtractVFSFile: File size is invalid: {}", fileSize);
			return {};
		}

		std::vector<uint8_t> buffer(fileSize);
		if (!ReadData(readerContext, buffer.data(), fileSize))
		{
			FRACTURE_ERROR("ExtractVFSFile: Failed to read data from: {}", vfsPath);
			return {};
		}

		if (g_loaderManager)
		{
			g_loaderManager->dumpFile(vfsPath, buffer);
		}

		return buffer;
	}
}
