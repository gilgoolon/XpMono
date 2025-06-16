#include "Crypto/Aes.hpp"

#include "Exception.hpp"
#include "cryptopp/gcm.h"
#include "Utils/Buffer.hpp"

#include <vector>
#include <cryptopp/aes.h>
#include <cryptopp/filters.h>
#include <cryptopp/modes.h>
#include <cryptopp/osrng.h>

static void validate_key(const Buffer& key, [[maybe_unused]] const Aes::Mode mode)
{
	if (key.size() != CryptoPP::AES::DEFAULT_KEYLENGTH &&
		key.size() != CryptoPP::AES::MAX_KEYLENGTH &&
		key.size() != CryptoPP::AES::MIN_KEYLENGTH)
	{
		throw Exception(ErrorCode::INVALID_CRYPTO_KEY);
	}
}

static void validate_data(const Buffer& data, [[maybe_unused]] const Aes::Mode mode)
{
	if (data.size() < CryptoPP::AES::BLOCKSIZE)
	{
		throw Exception(ErrorCode::INVALID_CRYPTO_DATA);
	}
}

static Buffer validate_iv(const Buffer& iv, const Aes::Mode mode)
{
	static constexpr size_t GCM_IV_SIZE = 12;
	Buffer resulting_iv;

	switch (mode)
	{
	case Aes::Mode::GCM:
	{
		if (iv.size() != GCM_IV_SIZE)
		{
			throw Exception(ErrorCode::INVALID_CRYPTO_IV);
		}
		break;
	}

	case Aes::Mode::CBC:
	{
		if (iv.empty())
		{
			resulting_iv.resize(CryptoPP::AES::BLOCKSIZE);
			CryptoPP::AutoSeededRandomPool rng;
			rng.GenerateBlock(resulting_iv.data(), resulting_iv.size());
		}
		else if (iv.size() != CryptoPP::AES::BLOCKSIZE)
		{
			throw Exception(ErrorCode::INVALID_CRYPTO_IV);
		}
		break;
	}

	default:
	{
		throw Exception(ErrorCode::UNCOVERED_ENUM_VALUE);
	}
	}

	return resulting_iv;
}

Buffer Aes::encrypt(const Buffer& data, const Buffer& key, const Buffer& iv, const Mode mode)
{
	validate_key(key, mode);
	validate_iv(iv, mode);
	const Buffer actual_iv = validate_iv(iv, mode);

	std::vector<uint8_t> ciphertext;

	switch (mode)
	{
	case Mode::CBC:
	{
		CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption encryption;
		encryption.SetKeyWithIV(key.data(), key.size(), actual_iv.data());

		std::ignore = CryptoPP::ArraySource(
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
		CryptoPP::GCM<CryptoPP::AES>::Encryption encryption;
		encryption.SetKeyWithIV(key.data(), key.size(), actual_iv.data());

		std::ignore = CryptoPP::ArraySource(
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

	return ciphertext;
}

Buffer Aes::decrypt(const Buffer& data, const Buffer& key, const Buffer& iv, Mode mode)
{
	validate_key(key, mode);
	validate_data(data, mode);
	const Buffer actual_iv = validate_iv(iv, mode);

	std::vector<uint8_t> plaintext;

	switch (mode)
	{
	case Mode::CBC:
	{
		Buffer ciphertext(data.begin() + CryptoPP::AES::BLOCKSIZE, data.end());

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
		CryptoPP::GCM<CryptoPP::AES>::Decryption decryption;
		decryption.SetKeyWithIV(key.data(), key.size(), iv.data());

		CryptoPP::AuthenticatedDecryptionFilter gcm_decryption_filter(
			decryption,
			new CryptoPP::VectorSink(plaintext)
		);

		std::ignore = CryptoPP::ArraySource(
			data.data(),
			data.size(),
			true,
			new CryptoPP::Redirector(gcm_decryption_filter)
		);
	}
	}

	return plaintext;
}
