#include "streaming.hpp"
#include <iostream>

int main() {
    std::cout << "--- INICIANDO SISTEMA DE TESTES ---\n\n";

    auto song1 = std::make_shared<song>("Bohemian Rhapsody", "Queen", 354);
    auto song2 = std::make_shared<song>("Stairway to Heaven", "Led Zeppelin", 482);
    std::cout << "\n";

    song1->play();
    song2->play();
    std::cout << "\n";

    {
        std::cout << ">> Entrando no escopo interno (Sessão do Usuário) <<\n\n";

        user active_user("carlos_rock", "Premium", 21.90f);
        active_user.print_info();

        playlist my_playlist("Rock Clássico");
        my_playlist.add_song(song1);
        my_playlist.add_song(song2);

        std::cout << "Duração total da playlist: " 
                  << my_playlist.calculate_total_duration() << " segundos.\n\n";

        std::cout << ">> Saindo do escopo interno (Objetos locais serão destruídos) <<\n";
    } 

    std::cout << "\n--- FORA DO ESCOPO INTERNO ---\n";
    std::cout << "Provando a independência da agregação:\n";
    song1->play(); 

    std::cout << "\n--- FIM DO PROGRAMA ---\n";
    return 0;
}