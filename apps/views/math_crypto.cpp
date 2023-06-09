#include "math_crypto.h"
#include "string_utils.hpp"

namespace cr = petliukh::cryptography;

Math_crypto::Math_crypto(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::Math_crypto)
{
    ui->setupUi(this);
    ui->savefile_btn_group->setId(ui->initial_rbtn, 0);
    ui->savefile_btn_group->setId(ui->encrypted_rbtn, 1);
    ui->savefile_btn_group->setId(ui->decrypted_rbtn, 2);
}

Math_crypto::~Math_crypto()
{
    delete ui;
}

// ===========================================================================
//                             Meny Bar Slots
// ===========================================================================

void Math_crypto::on_open_action_triggered()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    QString filename = dialog.getOpenFileName(this, "Open File");

    if (filename.isEmpty()) {
        QMessageBox::warning(this, "Warning", "No filename to open.");
        return;
    }

    m_controller.set_filename(filename.toStdString());
    m_controller.set_curr_state(ui->savefile_btn_group->checkedId());
    QPlainTextEdit* tedit = get_curr_text_edit();
    tedit->setPlainText(
            QString::fromStdString(m_controller.read_content_from_file()));
    this->setWindowTitle("Math Crypto - " + filename);
}

void Math_crypto::on_save_action_triggered()
{
    if (m_controller.get_filename().empty()) {
        QMessageBox::warning(this, "Warning", "No filename to save.");
        return;
    }

    if (ui->bytes_cbox->isChecked()) {
        m_controller.save_file(ui->savefile_btn_group->checkedId());
    } else {
        QPlainTextEdit* txt_edit = get_curr_text_edit();
        QString text = txt_edit->toPlainText();
        m_controller.save_file(text.toStdString());
    }
    QMessageBox::information(this, "Information", "File saved.");
}

void Math_crypto::on_saveas_action_triggered()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    QString filename = dialog.getSaveFileName(this, "Save File");

    if (filename.isEmpty()) {
        QMessageBox::warning(this, "Warning", "No filename to save.");
        return;
    }

    m_controller.set_filename(filename.toStdString());
    if (ui->bytes_cbox->isChecked()) {
        m_controller.save_file(ui->savefile_btn_group->checkedId());
    } else {
        QPlainTextEdit* txt_edit = get_curr_text_edit();
        QString text = txt_edit->toPlainText();
        m_controller.save_file(text.toStdString());
    }
    QMessageBox::information(this, "Information", "File saved.");
}

void Math_crypto::on_create_new_action_triggered()
{
    ui->initial_txt_edit->clear();
    ui->encrypted_txt_edit->clear();
    ui->decrypted_txt_edit->clear();
    ui->key_ln_edit->clear();

    ui->lang_cbox->setCurrentIndex(0);
    ui->cipher_cbox->setCurrentIndex(0);
    ui->bytes_cbox->setChecked(false);
    ui->cipher_specific_ops_stacked_widget->setCurrentIndex(0);
    ui->freq_table_widget->clear();
    this->setWindowTitle("Math Crypto");
    m_controller.reset();
}

void Math_crypto::on_print_action_triggered()
{
    QMessageBox::warning(this, "Warning", "Not implemented yet.");
}

void Math_crypto::on_about_action_triggered()
{
    QMessageBox::about(
            this, "About Math Cryptography",
            "This program is designed to encrypt and "
            "decrypt messages using different ciphers. ");
}

void Math_crypto::on_exit_action_triggered()
{
    QApplication::quit();
}

// ===========================================================================
//                             Getters
// ===========================================================================

QPlainTextEdit* Math_crypto::get_curr_text_edit() const
{
    int idx = ui->savefile_btn_group->checkedId();
    switch (idx) {
    case 0:
        return ui->initial_txt_edit;
    case 1:
        return ui->encrypted_txt_edit;
    case 2:
        return ui->decrypted_txt_edit;
    default:
        return nullptr;
    }
}

// ===========================================================================
//                             Cipher Buttons Slots
// ===========================================================================

void Math_crypto::on_encrypt_btn_clicked()
{
    std::string key = ui->key_ln_edit->text().toStdString();
    try {
        m_controller.set_key(key);
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Warning", "Invalid key.");
        return;
    }

    if (ui->bytes_cbox->isChecked()) {
        std::string to_enc = m_controller.get_content(0);
        std::string enc_bytes = m_controller.encrypt_raw_bytes(to_enc);
        m_controller.set_content(1, enc_bytes);
        ui->encrypted_txt_edit->setPlainText(QString::fromStdString(enc_bytes));
        return;
    }

    QString cont = ui->initial_txt_edit->toPlainText();
    std::string enc_cont = m_controller.encrypt(cont.toStdString());
    ui->encrypted_txt_edit->setPlainText(QString::fromStdString(enc_cont));
}

void Math_crypto::on_decrypt_btn_clicked()
{
    std::string key = ui->key_ln_edit->text().toStdString();
    try {
        m_controller.set_key(key);
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Warning", "Invalid key.");
        return;
    }

    if (ui->bytes_cbox->isChecked()) {
        std::string to_dec = m_controller.get_content(1);
        std::string dec_bytes = m_controller.decrypt_raw_bytes(to_dec);
        m_controller.set_content(2, dec_bytes);
        ui->decrypted_txt_edit->setPlainText(QString::fromStdString(dec_bytes));
        return;
    }

    QString cont = ui->encrypted_txt_edit->toPlainText();
    std::string dec_cont = m_controller.decrypt(cont.toStdString());
    ui->decrypted_txt_edit->setPlainText(QString::fromStdString(dec_cont));
}

void Math_crypto::on_brute_force_btn_clicked()
{
    if (ui->bytes_cbox->isChecked()) {
        QMessageBox::warning(this, "Warning", "Cannot brute force bytes.");
        return;
    }

    QString text = ui->encrypted_txt_edit->toPlainText();
    auto brute_res = m_controller.brute_force(text.toStdString());
    ui->brute_force_table->clear();
    ui->brute_force_table->setRowCount(brute_res.size());
    ui->brute_force_table->setColumnCount(2);
    int row = 0;

    for (auto& [k, v] : brute_res) {
        ui->brute_force_table->setItem(
                row, 0, new QTableWidgetItem(QString::number(k)));
        ui->brute_force_table->setItem(
                row, 1, new QTableWidgetItem(QString::fromStdString(v)));
        row++;
    }
}

void Math_crypto::on_print_freq_clicked()
{
    if (ui->bytes_cbox->isChecked()) {
        QMessageBox::warning(
                this, "Warning", "Cannot print frequencies of bytes.");
        return;
    }

    int idx = ui->savefile_btn_group->checkedId();
    QString text = get_curr_text_edit()->toPlainText();
    auto freqs = m_controller.calc_freqs(text.toStdString());

    ui->freq_table_widget->clear();
    ui->freq_table_widget->setRowCount(freqs.size());
    ui->freq_table_widget->setColumnCount(2);
    int row = 0;
    for (const auto& [k, v] : freqs) {
        ui->freq_table_widget->setItem(
                row, 0, new QTableWidgetItem(QString(k)));
        ui->freq_table_widget->setItem(
                row, 1, new QTableWidgetItem(QString::number(v)));
        row++;
    }
}

void Math_crypto::on_cipher_cbox_currentIndexChanged(int index)
{
    ui->cipher_specific_ops_stacked_widget->setCurrentIndex(index);
    m_controller.set_cipher_index(index);
}

void Math_crypto::on_bytes_cbox_stateChanged(int arg1)
{
    ui->lang_cbox->setEnabled(arg1 == Qt::Unchecked);
    ui->initial_txt_edit->setEnabled(arg1 == Qt::Unchecked);
    ui->encrypted_txt_edit->setEnabled(arg1 == Qt::Unchecked);
    ui->decrypted_txt_edit->setEnabled(arg1 == Qt::Unchecked);
}

void Math_crypto::on_lang_cbox_currentIndexChanged(const QString& arg1)
{
    m_controller.set_lang(arg1.toStdString());
}

void Math_crypto::on_trit_attack_combobox_currentIndexChanged(int index)
{
    ui->trithemius_attacks_stacked_widget->setCurrentIndex(index);
}

void Math_crypto::on_attack_trit_cipher_btn_clicked()
{
    int idx = ui->trit_attack_combobox->currentIndex();
    try {
        switch (idx) {
        case 0: {
            int vec_size = ui->key_vec_size_spinbox->value();
            std::string msg = ui->dec_msg_trit_attack_tedit->toPlainText()
                                      .toStdString();
            std::string enc
                    = ui->enc_msg_tit_attack_tedit->toPlainText().toStdString();
            std::string broken_key = m_controller.break_trithemius_cipher_key(
                    msg, enc, vec_size);
            ui->trit_broken_key_ledit->setText(
                    QString::fromStdString(broken_key));
            break;
        }
        case 1: {
            QMessageBox::information(
                    this, "Not implemented",
                    "Frequency table attack has not been implemented yet.");
            break;
        }
        }
    } catch (std::exception& ex) {
        QMessageBox::warning(this, "Error", ex.what());
    }
}

void Math_crypto::on_generate_rnd_key_btn_clicked()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    QString filename = dialog.getSaveFileName(this, "Save File");

    if (filename.isEmpty()) {
        QMessageBox::warning(this, "Warning", "No filename to save.");
        return;
    }
    m_controller.generate_rand_keyword(filename.toStdString());
}

void Math_crypto::on_load_key_btn_clicked()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    QString filename = dialog.getOpenFileName(this, "Open File");

    if (filename.isEmpty()) {
        QMessageBox::warning(this, "Warning", "No filename to open.");
        return;
    }
    m_controller.read_key_from_file(filename.toStdString());
    ui->key_ln_edit->setText(QString::fromStdString(m_controller.get_key()));
}

void Math_crypto::on_knapsack_genkey_btn_clicked()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    QString filename = dialog.getSaveFileName(this, "Save File");

    if (filename.isEmpty()) {
        QMessageBox::warning(this, "Warning", "No filename to save.");
        return;
    }

    m_controller.generate_rand_knapsack_key(filename.toStdString(), 30);
    ui->knapsack_keyview_tedit->setPlainText(
            QString::fromStdString(m_controller.get_knapsack_key()));
}

void Math_crypto::on_rsa_keygen_btn_clicked()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    QString filename = dialog.getSaveFileName(this, "Save File");

    if (filename.isEmpty()) {
        QMessageBox::warning(this, "Warning", "No filename to save.");
        return;
    }

    m_controller.generate_rsa_key(filename.toStdString(), 10);
    ui->rsa_key_tedit->setPlainText(
            QString::fromStdString(m_controller.get_rsa_key()));
}

void Math_crypto::on_gen_dh_pair_btn_clicked()
{
    auto pair = m_controller.diffie_hellman_gen_common_pair();
    ui->common_dh_pair_text->setPlainText(QString::fromStdString(pair));
}

void Math_crypto::on_generate_a_secret_btn_clicked()
{
    auto a = m_controller.diffie_hellman_gen_a_secret();
    ui->sec_a_text->setPlainText(QString::fromStdString(a));
}

void Math_crypto::on_generate_b_secret_btn_clicked()
{
    auto b = m_controller.diffie_hellman_gen_b_secret();
    ui->sec_b_tex->setPlainText(QString::fromStdString(b));
}

void Math_crypto::on_share_a_clicked()
{
    std::string a = ui->sec_a_text->toPlainText().toStdString();
    std::string dhp = ui->common_dh_pair_text->toPlainText().toStdString();
    auto dhp_vec = cr::str_split(dhp, '\n');
    auto shared_a = m_controller.diffie_hellman_share_a_side(dhp_vec[0], dhp_vec[1], a);
    ui->share_a_text->setPlainText(QString::fromStdString(shared_a));
}

void Math_crypto::on_share_b_clicked()
{
    std::string b = ui->sec_b_tex->toPlainText().toStdString();
    std::string dhp = ui->common_dh_pair_text->toPlainText().toStdString();
    auto dhp_vec = cr::str_split(dhp, '\n');
    auto shared_b = m_controller.diffie_hellman_share_b_side(dhp_vec[0], dhp_vec[1], b);
    ui->share_b_text->setPlainText(QString::fromStdString(shared_b));
}

void Math_crypto::on_get_common_key_a_btn_clicked()
{
    std::string b_shared = ui->share_b_text->toPlainText().toStdString();
    std::string a = ui->sec_a_text->toPlainText().toStdString();
    std::string dhp = ui->common_dh_pair_text->toPlainText().toStdString();
    auto dhp_vec = cr::str_split(dhp, '\n');
    auto K = m_controller.calc_common_key_from_b_shared(b_shared, a, dhp_vec[1]);
    ui->common_key_a_text->setPlainText(QString::fromStdString(K));
}

void Math_crypto::on_get_common_key_b_btn_clicked()
{
    std::string a_shared = ui->share_a_text->toPlainText().toStdString();
    std::string b = ui->sec_b_tex->toPlainText().toStdString();
    std::string dhp = ui->common_dh_pair_text->toPlainText().toStdString();
    auto dhp_vec = cr::str_split(dhp, '\n');
    auto K = m_controller.calc_common_key_from_b_shared(a_shared, b, dhp_vec[1]);
    ui->common_key_b_text->setPlainText(QString::fromStdString(K));
}
