#include "numeric_utils.hpp"
#include "rsa_cipher.hpp"
#include "string_utils.hpp"

#include <gtest/gtest.h>
#include <iostream>

namespace cr = petliukh::cryptography;

TEST(rsa_cipher, sets_key_correctly)
{
    cr::Rsa_cipher rsa;
    rsa.generate_rand_key();

    std::string key = rsa.get_key().to_string();
    rsa.set_key(cr::utf8_to_utf16(key));

    std::string key2 = rsa.get_key().to_string();
    EXPECT_EQ(key, key2);
}

TEST(rsa_cipher, modulo_exponentiates_correctly)
{
    BigInt a = 3;
    BigInt d = 200;
    BigInt m = 50;

    BigInt mod = cr::mod_exp(a, d, m);
    EXPECT_EQ(BigInt(1), mod);
}

TEST(rsa_cipher, ecnrypts_correctl)
{
    cr::Rsa_cipher rsa;
    rsa.generate_rand_key();

    std::u16string plaintext = u"Hello, World!";
    std::u16string enctext = rsa.encrypt(plaintext);
    std::u16string decrypted = rsa.decrypt(enctext);

    ASSERT_EQ(plaintext, decrypted);
}
