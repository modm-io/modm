/*
 * Copyright (c) 2024, Henrik Hose
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>
#include <modm/debug/logger.hpp>
#include <nlohmann-json/json.hpp>

#undef MODM_LOG_LEVEL
#define MODM_LOG_LEVEL modm::log::INFO

// ----------------------------------------------------------------------------

extern "C" const uint32_t __rom_end[];
using json = nlohmann::json;

constexpr auto max_flash_pages{64};

namespace data
{
struct person
{
	std::string name;
	float height;
	int age;
	auto
	operator<=>(const person&) const = default;
};

void
to_json(json& j, const person& p)
{
	j = json{{"name", p.name}, {"height", p.height}, {"age", p.age}};
}

void
from_json(const json& j, person& p)
{
	j.at("name").get_to(p.name);
	j.at("height").get_to(p.height);
	j.at("age").get_to(p.age);
}
}  // namespace data

int
main()
{
	Board::initialize();

	MODM_LOG_INFO << "\n\n+++++++++++++++++++++++++++" << modm::endl;
	MODM_LOG_INFO << "++ NLOHMANN JSON example ++" << modm::endl;
	MODM_LOG_INFO << "+++++++++++++++++++++++++++\n" << modm::endl;

	// data::person alice_struct = {"Alice", 50};
	data::person alice_struct = {"Alice", 1.85, 50};
	json alice_json = alice_struct;

	// basic JSON character string usage
	{
		auto alice_json_string = alice_json.dump();

		// imagine that we transmit this string over some interface
		MODM_LOG_INFO << "JSON from struct is: " << alice_json_string.c_str() << modm::endl;

		// and now we can retrieve the struct from the json object
		json retrieved_alice_json = json::parse(alice_json_string);

		// JSON is fairly self-annotated, so we might not even need the original struct
		MODM_LOG_INFO << "\nRetrieved from string JSON has fields ... ";
		for (auto& [key, value] : retrieved_alice_json.items())
		{
			MODM_LOG_INFO << key.c_str() << ", ";
		}
		MODM_LOG_INFO << modm::endl;
		MODM_LOG_INFO.flush();

		// check if retrieved struct is same
		auto retrieved_alice_struct = retrieved_alice_json.template get<data::person>();
		if (retrieved_alice_struct == alice_struct)
		{
			MODM_LOG_INFO << "\nRetrieved from string and original struct are identical"
						  << modm::endl;
		}
		MODM_LOG_INFO.flush();
	}

	// basic binary JSON usage
	{
		// convert to BSON
		auto alice_binary = json::to_bson(alice_json);

		MODM_LOG_INFO << "\nBinary JSON (BSON): ";
		for (const auto& byte : alice_binary) { MODM_LOG_INFO.printf(" 0x%02x", byte); }
		MODM_LOG_INFO << modm::endl;

		// put BSON in first flash page after program
		uint32_t err{0};
		const size_t page_start = Flash::getPage(reinterpret_cast<uint32_t>(&__rom_end)) + 1;
		const size_t num_bytes = alice_binary.size();
		const size_t flash_page_size = Flash::getSize(page_start);
		const size_t end_page = page_start + (num_bytes + flash_page_size - 1) / flash_page_size;
		if (end_page >= max_flash_pages)
		{
			MODM_LOG_ERROR << "\nRequested flash end page exceeds flash [" << page_start << ", "
						   << end_page << ")" << modm::endl;
			MODM_LOG_INFO.flush();
			while (1);
		}

		// erase the pages before programming
		MODM_LOG_INFO << "\nErasing flash sectors [" << page_start << ", " << end_page << ")"
					  << modm::endl;
		MODM_LOG_INFO.flush();
		if (not Flash::unlock())
		{
			MODM_LOG_INFO << "Flash unlock failed!" << modm::endl;
			while (1);
		}
		for (size_t page{page_start}; page < end_page; page++) err |= Flash::erase(page);
		if (err != 0)
		{
			MODM_LOG_ERROR << "\nThere was an error while erasing flash!" << modm::endl;
			MODM_LOG_INFO.flush();
			while (1);
		}

		// pad the data with zeros to fit flash words
		size_t unpadded_size{alice_binary.size()};
		size_t word_size{sizeof(Flash::MaxWordType)};
		if (auto padding = (word_size - unpadded_size % word_size) % word_size; padding > 0)
		{
			alice_binary.resize(alice_binary.size() + padding, 0);
		}

		// now, write the padded data
		MODM_LOG_INFO << "\nWriting, word size: " << word_size
					  << ", num of bytes (payload): " << unpadded_size
					  << ", num of bytes (padded): " << alice_binary.size() << "... ";

		for (uint32_t src_addr = reinterpret_cast<uint32_t>(&alice_binary[0]),
					  dst_addr{reinterpret_cast<uint32_t>(Flash::getAddr(page_start))};
			 src_addr < (reinterpret_cast<uint32_t>(&alice_binary[0]) + alice_binary.size());
			 src_addr += sizeof(Flash::MaxWordType), dst_addr += sizeof(Flash::MaxWordType))
		{
			err |= Flash::program(dst_addr, *(Flash::MaxWordType*)src_addr);
		}
		if (err != 0)
		{
			MODM_LOG_ERROR << "\nThere was an error while programming flash!" << modm::endl;
			MODM_LOG_INFO.flush();
			while (1);
		}
		MODM_LOG_INFO << "Writing complete! " << modm::endl;
		MODM_LOG_INFO.flush();

		// we can now read BSON back from flash, first 4 bytes entry is the size
		uint32_t read_len = *(reinterpret_cast<uint32_t*>(Flash::getAddr(page_start)));

		// read the actual data
		MODM_LOG_INFO << "\nReading BSON of size " << read_len << " from flash: ";
		auto raw_bytes =
			std::vector<uint8_t>(Flash::getAddr(page_start), Flash::getAddr(page_start) + read_len);
		for (const auto& byte : raw_bytes) { MODM_LOG_INFO.printf(" 0x%02x", byte); }
		MODM_LOG_INFO << modm::endl;
		MODM_LOG_INFO.flush();

		// reconstruct the data
		auto retrieved_alice_json = json::from_bson(raw_bytes);
		auto retrieved_alice_struct = retrieved_alice_json.template get<data::person>();
		if (retrieved_alice_struct == alice_struct)
		{
			MODM_LOG_INFO << "\nRetrieved from flash and original struct are identical"
						  << modm::endl;
		}
		MODM_LOG_INFO.flush();
	}

	while (1);
	return 0;
}