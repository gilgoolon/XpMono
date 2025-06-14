#include "Crypto/Aes.hpp"

#include "cryptopp/gcm.h"
#include "Utils/Buffer.hpp"

#include <stdexcept>
#include <vector>
#include <cryptopp/aes.h>
#include <cryptopp/filters.h>
#include <cryptopp/modes.h>
#include <cryptopp/osrng.h>

static constexpr size_t GCM_IV_SIZE = 12;

void Aes::encrypt(std::vector<uint8_t>& data,
                  const std::vector<uint8_t>& key,
                  const std::vector<uint8_t>& iv,
                  const Mode mode)
{
	if (key.size() != CryptoPP::AES::DEFAULT_KEYLENGTH &&
		key.size() != CryptoPP::AES::MAX_KEYLENGTH &&
		key.size() != CryptoPP::AES::MIN_KEYLENGTH)
	{
		throw std::invalid_argument("Invalid AES key length");
	}

	std::vector<uint8_t> actual_iv = iv;
	std::vector<uint8_t> ciphertext;

	switch (mode)
	{
	case Mode::CBC:
	{
		if (actual_iv.empty())
		{
			actual_iv.resize(CryptoPP::AES::BLOCKSIZE);
			CryptoPP::AutoSeededRandomPool rng;
			rng.GenerateBlock(actual_iv.data(), actual_iv.size());
		}
		else if (actual_iv.size() != CryptoPP::AES::BLOCKSIZE)
		{
			throw std::invalid_argument("IV must be 16 bytes");
		}

		CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption encryption;
		encryption.SetKeyWithIV(key.data(), key.size(), actual_iv.data());

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

		ciphertext.insert(ciphertext.begin(), actual_iv.begin(), actual_iv.end());
	}

	case Mode::GCM:
	{
		if (actual_iv.size() != GCM_IV_SIZE)
		{
			throw std::invalid_argument("IV must be 12 bytes");
		}

		CryptoPP::GCM<CryptoPP::AES>::Encryption encryption;
		encryption.SetKeyWithIV(key.data(), key.size(), actual_iv.data());

		CryptoPP::ArraySource(
			data.data(),
			data.size(),
			true,
			new CryptoPP::AuthenticatedEncryptionFilter(
				encryption,
				new CryptoPP::VectorSink(ciphertext)
			)
		);
	}
	}

	data = std::move(ciphertext);
}

void Aes::decrypt(std::vector<uint8_t>& data,
                  const std::vector<uint8_t>& key,
                  const std::vector<uint8_t>& iv,
                  const Mode mode)
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

	std::vector<uint8_t> plaintext;

	switch (mode)
	{
	case Mode::CBC:
	{
		Buffer actual_iv = iv;
		if (actual_iv.empty())
		{
			actual_iv.resize(CryptoPP::AES::BLOCKSIZE);
		}

		if (actual_iv.size() != CryptoPP::AES::BLOCKSIZE)
		{
			throw std::invalid_argument("iv expected size is 16");
		}

		std::vector<uint8_t> ciphertext(data.begin() + CryptoPP::AES::BLOCKSIZE, data.end());

		CryptoPP::GCM<CryptoPP::AES>::Decryption decryption;
		decryption.SetKeyWithIV(key.data(), key.size(), actual_iv.data());

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
	}

	case Mode::GCM:
	{
		if (iv.size() != GCM_IV_SIZE)
		{
			throw std::invalid_argument("iv is too long");
		}

		CryptoPP::GCM<CryptoPP::AES>::Decryption decryption;
		decryption.SetKeyWithIV(key.data(), key.size(), iv.data());

		CryptoPP::AuthenticatedDecryptionFilter df(
			decryption,
			new CryptoPP::VectorSink(plaintext)
		);

		CryptoPP::ArraySource(
			data.data(),
			data.size(),
			true,
			new CryptoPP::Redirector(df)
		);
	}
	}

	data = std::move(plaintext);
}
