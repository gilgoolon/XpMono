#include "Crypto/Aes.hpp"

#include <stdexcept>
#include <vector>
#include <cryptopp/aes.h>
#include <cryptopp/filters.h>
#include <cryptopp/modes.h>
#include <cryptopp/osrng.h>

void Aes::encrypt(std::vector<uint8_t>& data, const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv)
{
	if (key.size() != CryptoPP::AES::DEFAULT_KEYLENGTH &&
		key.size() != CryptoPP::AES::MAX_KEYLENGTH &&
		key.size() != CryptoPP::AES::MIN_KEYLENGTH)
	{
		throw std::invalid_argument("Invalid AES key length");
	}

	std::vector<uint8_t> actualIV = iv;
	if (actualIV.empty())
	{
		actualIV.resize(CryptoPP::AES::BLOCKSIZE);
		CryptoPP::AutoSeededRandomPool rng;
		rng.GenerateBlock(actualIV.data(), actualIV.size());
	}
	else if (actualIV.size() != CryptoPP::AES::BLOCKSIZE)
	{
		throw std::invalid_argument("IV must be 16 bytes");
	}

	CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption encryption;
	encryption.SetKeyWithIV(key.data(), key.size(), actualIV.data());

	std::vector<uint8_t> ciphertext;
	CryptoPP::ArraySource(
		data.data(),
		data.size(),
		true,
		new CryptoPP::StreamTransformationFilter(
			encryption,
			new CryptoPP::VectorSink(ciphertext),
			CryptoPP::BlockPaddingSchemeDef::PKCS_PADDING
		)
	);

	// Prepend IV to the ciphertext
	ciphertext.insert(ciphertext.begin(), actualIV.begin(), actualIV.end());

	data = std::move(ciphertext);
}

void Aes::decrypt(std::vector<uint8_t>& data, const std::vector<uint8_t>& key)
{
	if (key.size() != CryptoPP::AES::DEFAULT_KEYLENGTH &&
		key.size() != CryptoPP::AES::MAX_KEYLENGTH &&
		key.size() != CryptoPP::AES::MIN_KEYLENGTH)
	{
		throw std::invalid_argument("Invalid AES key length");
	}

	if (data.size() < CryptoPP::AES::BLOCKSIZE)
	{
		throw std::invalid_argument("Ciphertext too short");
	}

	std::vector<uint8_t> iv(data.begin(), data.begin() + CryptoPP::AES::BLOCKSIZE);
	std::vector<uint8_t> ciphertext(data.begin() + CryptoPP::AES::BLOCKSIZE, data.end());

	CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption decryption;
	decryption.SetKeyWithIV(key.data(), key.size(), iv.data());

	std::vector<uint8_t> plaintext;
	CryptoPP::ArraySource(
		ciphertext.data(),
		ciphertext.size(),
		true,
		new CryptoPP::StreamTransformationFilter(
			decryption,
			new CryptoPP::VectorSink(plaintext),
			CryptoPP::BlockPaddingSchemeDef::PKCS_PADDING
		)
	);

	data = std::move(plaintext);
}
