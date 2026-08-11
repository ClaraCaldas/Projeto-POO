#ifndef JANELA_HPP
#define JANELA_HPP

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QListWidget>
#include <QLineEdit>
#include <QLabel>
#include "streaming.hpp"

// Janela principal que expõe as operações do domínio de maneira desacoplada
class MainWindow : public QWidget {
    Q_OBJECT
private:
    streaming_service& app_;
    
    QListWidget* list_widget_;
    QLineEdit* title_input_;
    QLineEdit* artist_host_input_;
    QLineEdit* duration_input_;
    QLineEdit* episodes_input_;
    QPushButton* add_song_btn_;
    QPushButton* add_podcast_btn_;
    QPushButton* remove_btn_;
    QPushButton* save_btn_;
    QPushButton* load_btn_;
    QPushButton* calc_btn_;
    QLabel* status_label_;

public:
    explicit MainWindow(streaming_service& app, QWidget* parent = nullptr);
    void update_list();

private slots:
    void on_add_song();
    void on_add_podcast();
    void on_remove();
    void on_save();
    void on_load();
    void on_calculate();
};

#endif
