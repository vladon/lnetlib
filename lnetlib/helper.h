#pragma once

#include "config.h"

#include <memory>
#include <string>
#include <fstream>

namespace lnetlib
{
	class helper
	{
	public:
		using file = std::pair<std::unique_ptr<char>, uint64_t>;

		helper() = delete;
		~helper() = delete;

		static file read_binary_file(const std::string& filename);

	};
}
