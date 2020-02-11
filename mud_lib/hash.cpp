#include "hash.h"

Hash::Hash(std::string str)
{
	CryptoPP::SHA3_256 sha3{};
	sha3.Update(
		reinterpret_cast<const CryptoPP::byte*>(str.data()),
		str.size() * sizeof(char));
	sha3.TruncatedFinal(
		reinterpret_cast<CryptoPP::byte*>(hash_.data()),
		hash_.size() * sizeof(std::int64_t));
}

std::string Hash::GetStringHash()
{
	std::string str;
	for (const auto& field : hash_) {
		str += std::to_string(field);
		if (field != hash_.back())
		{
			str += "/";
		}
	}
	return str;
}
