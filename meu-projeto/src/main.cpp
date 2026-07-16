#include "streaming.hpp"
#include <iostream>
#include <vector>
#include <memory>

// Q3(D) Função livre recebendo const sharable& (sem conhecer o tipo concreto)
void share_content(const sharable& s) {
    std::cout << "  [share_content] ";
    s.share(std::cout);
    std::cout << "\n";
}

int main() {
    std::cout << "--- INICIANDO SISTEMA DE TESTES (POO TP2) ---\n\n";

    // ==========================================
    // QUESTÃO 1: Hierarquia de Herança
    // ==========================================
    std::cout << "=== QUESTÃO 1: Hierarquia de Herança ===\n";
    std::cout << "Demonstrando a cadeia de destrutores virtuais (Q1-C):\n";
    {
        media_content* test_ptr = new song("Bohemian Rhapsody (Temp)", "Queen", 354);
        delete test_ptr; // Deve imprimir ~song() e depois ~media_content()
    }
    std::cout << "==========================================\n\n";

    // ==========================================
    // QUESTÃO 2: Polimorfismo Dinâmico
    // ==========================================
    std::cout << "=== QUESTÃO 2: Polimorfismo Dinâmico ===\n";
    {
        // Q2(A) vector<unique_ptr<Base>> usando make_unique
        std::vector<std::unique_ptr<media_content>> itens;
        itens.push_back(std::make_unique<song>("Stairway to Heaven", "Led Zeppelin", 482));
        itens.push_back(std::make_unique<podcast>("DevCast #42", "Marcos e Ana", 3600, 15));

        // Q2(B) Iterar com const auto& chamando pelo menos 2 métodos via ponteiro da base
        std::cout << "Iterando itens no vetor polimórfico:\n";
        for (const auto& item : itens) {
            item->exibir(); // Primeiro método polimórfico
            std::cout << "  -> Calcular() = " << item->calcular() << "\n"; // Segundo método polimórfico
        }
        std::cout << "\n";

        // Q2(D) Função livre maior_valor
        std::cout << "Buscando o elemento com maior valor de calcular():\n";
        const media_content* maior = maior_valor(itens);
        if (maior) {
            std::cout << "  Maior encontrado: ";
            maior->exibir();
            std::cout << "  Valor: " << maior->calcular() << "\n";
        }
        std::cout << "\n";

        // Q2(C) Demonstrando a destruição correta ao sair do escopo do vetor
        std::cout << "Destruindo o vetor de itens polimórficos:\n";
    }
    std::cout << "==========================================\n\n";

    // ==========================================
    // QUESTÃO 3: Interfaces Puras e Herança Avançada
    // ==========================================
    std::cout << "=== QUESTÃO 3: Interfaces e Herança Avançada ===\n";
    {
        // Q3(D) Demonstrar o uso de interface pura passando por referência
        song song_item("Imagine", "John Lennon", 183);
        playlist playlist_item("Melhores do Pop");
        
        std::cout << "Chamando share_content por referência para song:\n";
        share_content(song_item);

        std::cout << "Chamando share_content por referência para playlist (interface independente):\n";
        share_content(playlist_item);
    }
    std::cout << "==========================================\n\n";

    // ==========================================
    // COMPORTAMENTO ORIGINAL DO TP1 (Preservado)
    // ==========================================
    std::cout << "=== FUNCIONALIDADES PRESERVADAS DO TP1 ===\n";
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