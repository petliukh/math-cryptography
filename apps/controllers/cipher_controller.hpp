#include "crypto_ciphers.hpp"

#include <array>
#include <memory>

namespace petliukh::controllers {

namespace cr = petliukh::cryptography;

class cipher_controller {
public:
    cipher_controller();

    // Misc

    void set_cipher(int idx);

    void set_key(const std::string& key);

    std::string get_key() const;

    void set_lang(const std::string& lang);

    std::string get_lang() const;

    void set_filename(const std::string& filename);

    void read_file(
            std::ios_base::openmode mode
            = std::ios_base::in | std::ios::binary);

    std::string get_filename() const;

    std::string get_filecontent() const;

    std::string get_filecontent_enc() const;

    std::string get_filecontent_dec() const;

    std::string get_filecontent_save() const;

    void set_filecontent_save(const std::string& content);

    void set_filecontent(const std::string& content);

    void set_filecontent_enc(const std::string& content);

    void set_filecontent_dec(const std::string& content);

    void save_file();

    // Cipher methods

    std::string encrypt(const std::string& message);

    std::string decrypt(const std::string& message);

    std::string encrypt_raw_bytes(const std::string& bytes);

    std::string decrypt_raw_bytes(const std::string& bytes);

private:
    std::array<std::unique_ptr<cr::cipher>, 2> m_ciphers;
    int m_curr_idx{};
    std::string m_key;
    std::string m_filename;
    std::string m_filecontent;
    std::string m_filecontent_enc;
    std::string m_filecontent_dec;
    std::string m_save_filecontent;
    std::string m_lang;
};

}  // namespace petliukh::controllers
