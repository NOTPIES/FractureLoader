#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace Snowdrop::VFS
{
	std::vector<uint8_t> ExtractVFSFile(const std::string& vfsPath);
}
