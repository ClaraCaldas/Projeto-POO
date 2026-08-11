#include "janela.hpp"
#include <QMessageBox>

MainWindow::MainWindow(streaming_service& app, QWidget* parent)
    : QWidget(parent), app_(app) {
    setWindowTitle("Streaming Platform Manager (POO TP3)");
    resize(650, 450);

    auto* main_layout = new QVBoxLayout(this);

    // List of Items
    list_widget_ = new QListWidget(this);
    main_layout->addWidget(new QLabel("Catálogo de Mídias Cadastradas:", this));
    main_layout->addWidget(list_widget_);

    // Input fields layout
    auto* input_label = new QLabel("Novo Item:", this);
    main_layout->addWidget(input_label);

    auto* form_layout = new QHBoxLayout();
    title_input_ = new QLineEdit(this);
    title_input_->setPlaceholderText("Título");
    artist_host_input_ = new QLineEdit(this);
    artist_host_input_->setPlaceholderText("Artista / Host");
    duration_input_ = new QLineEdit(this);
    duration_input_->setPlaceholderText("Duração (segundos)");
    episodes_input_ = new QLineEdit(this);
    episodes_input_->setPlaceholderText("Nº Episódios (Podcast)");

    form_layout->addWidget(title_input_);
    form_layout->addWidget(artist_host_input_);
    form_layout->addWidget(duration_input_);
    form_layout->addWidget(episodes_input_);
    main_layout->addLayout(form_layout);

    // Buttons Layout 1 (Modify list)
    auto* btn_layout1 = new QHBoxLayout();
    add_song_btn_ = new QPushButton("Adicionar Música", this);
    add_podcast_btn_ = new QPushButton("Adicionar Podcast", this);
    remove_btn_ = new QPushButton("Remover Selecionado", this);
    btn_layout1->addWidget(add_song_btn_);
    btn_layout1->addWidget(add_podcast_btn_);
    btn_layout1->addWidget(remove_btn_);
    main_layout->addLayout(btn_layout1);

    // Buttons Layout 2 (Persistence & Operations)
    auto* btn_layout2 = new QHBoxLayout();
    save_btn_ = new QPushButton("Salvar Estado (JSON)", this);
    load_btn_ = new QPushButton("Carregar Estado (JSON)", this);
    calc_btn_ = new QPushButton("Calcular Score Paralelo", this);
    btn_layout2->addWidget(save_btn_);
    btn_layout2->addWidget(load_btn_);
    btn_layout2->addWidget(calc_btn_);
    main_layout->addLayout(btn_layout2);

    status_label_ = new QLabel("Status: Pronto", this);
    main_layout->addWidget(status_label_);

    // Connect signals to slots
    connect(add_song_btn_, &QPushButton::clicked, this, &MainWindow::on_add_song);
    connect(add_podcast_btn_, &QPushButton::clicked, this, &MainWindow::on_add_podcast);
    connect(remove_btn_, &QPushButton::clicked, this, &MainWindow::on_remove);
    connect(save_btn_, &QPushButton::clicked, this, &MainWindow::on_save);
    connect(load_btn_, &QPushButton::clicked, this, &MainWindow::on_load);
    connect(calc_btn_, &QPushButton::clicked, this, &MainWindow::on_calculate);

    update_list();
}

void MainWindow::update_list() {
    list_widget_->clear();
    for (const auto& item : app_.catalog()) {
        QString text = QString("[%1] ID: %2 - %3 (%4s)")
            .arg(QString::fromStdString(item->type_name()).toUpper())
            .arg(item->id())
            .arg(QString::fromStdString(item->title()))
            .arg(item->duration_seconds());
        
        if (item->type_name() == "song") {
            const auto* s = dynamic_cast<const song*>(item.get());
            text += QString(" por %1").arg(QString::fromStdString(s->artist()));
        } else {
            const auto* p = dynamic_cast<const podcast*>(item.get());
            text += QString(" (Host: %1, %2 eps)")
                .arg(QString::fromStdString(p->host()))
                .arg(p->episodes_count());
        }
        list_widget_->addItem(text);
    }
}

void MainWindow::on_add_song() {
    try {
        std::string title = title_input_->text().toStdString();
        std::string artist = artist_host_input_->text().toStdString();
        int duration = duration_input_->text().toInt();

        // Validação usando exceções do domínio
        if (title.empty() || artist.empty()) {
            throw item_invalido("Título ou Artista não podem estar vazios.");
        }
        if (duration <= 0) {
            throw item_invalido("Duração deve ser maior que zero.");
        }

        // Gera um ID simples e adiciona
        int new_id = app_.catalog().size() + 1;
        app_.add_item(std::make_unique<song>(new_id, title, artist, duration));
        update_list();
        status_label_->setText("Status: Música adicionada.");
        
        title_input_->clear();
        artist_host_input_->clear();
        duration_input_->clear();
    } catch (const erro_dominio& e) {
        QMessageBox::critical(this, "Erro de Validação", e.what());
    }
}

void MainWindow::on_add_podcast() {
    try {
        std::string title = title_input_->text().toStdString();
        std::string host = artist_host_input_->text().toStdString();
        int duration = duration_input_->text().toInt();
        int episodes = episodes_input_->text().toInt();

        // Validação usando exceções do domínio
        if (title.empty() || host.empty()) {
            throw item_invalido("Título ou Host não podem estar vazios.");
        }
        if (duration <= 0 || episodes <= 0) {
            throw item_invalido("Duração e número de episódios devem ser maiores que zero.");
        }

        // Gera um ID simples e adiciona
        int new_id = app_.catalog().size() + 1;
        app_.add_item(std::make_unique<podcast>(new_id, title, host, duration, episodes));
        update_list();
        status_label_->setText("Status: Podcast adicionado.");

        title_input_->clear();
        artist_host_input_->clear();
        duration_input_->clear();
        episodes_input_->clear();
    } catch (const erro_dominio& e) {
        QMessageBox::critical(this, "Erro de Validação", e.what());
    }
}

void MainWindow::on_remove() {
    int row = list_widget_->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Remoção", "Selecione um item no catálogo para remover.");
        return;
    }
    try {
        app_.remove_item(row);
        update_list();
        status_label_->setText("Status: Item removido.");
    } catch (const erro_dominio& e) {
        QMessageBox::critical(this, "Erro ao remover", e.what());
    }
}

void MainWindow::on_save() {
    try {
        app_.save_state();
        status_label_->setText("Status: Estado do catálogo salvo em JSON.");
        QMessageBox::information(this, "Salvar", "Dados salvos com sucesso!");
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Erro ao salvar", e.what());
    }
}

void MainWindow::on_load() {
    try {
        app_.load_state();
        update_list();
        status_label_->setText("Status: Estado do catálogo carregado do JSON.");
        QMessageBox::information(this, "Carregar", "Dados carregados com sucesso!");
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Erro ao carregar", e.what());
    }
}

void MainWindow::on_calculate() {
    // Chama cálculo paralelo e exibe
    float total = app_.calculate_total_score_parallel();
    QMessageBox::information(this, "Cálculo Popularidade", 
        QString("Popularidade acumulada (calculada em paralelo): %1").arg(total));
}
