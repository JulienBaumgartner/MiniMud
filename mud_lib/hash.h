#pragma once
#include <string>
#include <array>
#include <cryptopp/sha3.h>

class Hash {
public:
	Hash(std::string str);
	std::string GetStringHash();
private:
	std::array<std::int64_t, 4> hash_;
};