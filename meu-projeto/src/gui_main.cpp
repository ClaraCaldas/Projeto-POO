#include <QApplication>
#include "janela.hpp"
#include "streaming.hpp"

// Ponto de entrada para a interface gráfica com Qt
int main(int argc, char* argv[]) {
    QApplication qt_app(argc, argv);

    // Inicializa o repositório de produção (arquivo estado.json)
    json_repository repo("estado.json");
    streaming_service app(repo);

    // Tenta carregar o estado inicial se o arquivo existir
    try {
        app.load_state();
    } catch (...) {
        // Ignora falha de carregamento inicial
    }

    // Instancia e exibe a janela principal
    MainWindow window(app);
    window.show();

    return qt_app.exec();
}
