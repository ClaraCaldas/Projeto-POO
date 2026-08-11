#include "streaming.hpp"
#include <iostream>
#include <vector>
#include <memory>
#include <ranges>

void share_content(const sharable& s) {
    std::cout << "  [share_content] ";
    s.share(std::cout);
    std::cout << "\n";
}

int main() {
    std::cout << "--- INICIANDO SISTEMA DE TESTES (POO TP3) ---\n\n";

    // ========================================================================
    // Questão 1 — Programação Genérica: Templates, CRTP, Concepts e Ranges
    // ========================================================================
    std::cout << "=== QUESTÃO 1: Programação Genérica ===\n";
    
    // Q1(A) Instanciação do template registry com pelo menos dois tipos diferentes
    registry<song> song_registry;
    registry<podcast> podcast_registry;
    
    song_registry.add(song(1, "Song A", "Artist A", 180));
    song_registry.add(song(2, "Song B", "Artist B", 240));
    podcast_registry.add(podcast(3, "Pod A", "Host A", 3600, 10));
    
    std::cout << "Registry de Músicas possui: " << song_registry.size() << " itens.\n";
    std::cout << "Registry de Podcasts possui: " << podcast_registry.size() << " itens.\n";

    // Q1(B) CRTP - Contagem estática de instâncias
    std::cout << "Instâncias ativas de música (song): " << song::alive() << "\n";
    std::cout << "Instâncias ativas de podcast: " << podcast::alive() << "\n";

    // Q1(C)(D) Conceito e função restrita
    std::vector<song> calculaveis_song;
    calculaveis_song.push_back(song(4, "Song C", "Artist C", 200)); // calcular() = 300
    calculaveis_song.push_back(song(5, "Song D", "Artist D", 300)); // calcular() = 450
    double soma = soma_total(calculaveis_song);
    std::cout << "Soma total dos calculáveis (via Concept): " << soma << "\n";

    // Q1(E) Pipeline de Ranges (C++20) com pelo menos dois adaptadores encadeados
    namespace rv = std::ranges::views;
    std::vector<song> playlist_songs = {
        song(6, "Bohemian Rhapsody", "Queen", 354),
        song(7, "Under Pressure", "Queen", 242),
        song(8, "Stairway to Heaven", "Led Zeppelin", 482),
        song(9, "Yesterday", "The Beatles", 125)
    };

    std::cout << "Músicas longas (duracao > 240s) filtradas e transformadas em título:\n";
    auto titulos_longos = playlist_songs
        | rv::filter([](const song& s) { return s.duration_seconds() > 240; })
        | rv::transform([](const song& s) { return s.title(); });

    for (const auto& titulo : titulos_longos) {
        std::cout << "  - " << titulo << "\n";
    }
    std::cout << "==========================================\n\n";

    // ========================================================================
    // Questão 2 — Tratamento de Erros: exceções, optional e variant
    // ========================================================================
    std::cout << "=== QUESTÃO 2: Tratamento de Erros ===\n";

    // Q2(A)(D) Validação e lançamento/captura de exceções próprias
    try {
        std::cout << "Tentando adicionar música inválida com duração zero:\n";
        song s_invalid(10, "", "Artist E", 0);
        // Regra de validação:
        if (s_invalid.title().empty() || s_invalid.duration_seconds() <= 0) {
            throw item_invalido("Título vazio ou duração <= 0.");
        }
    } catch (const erro_dominio& e) {
        // Q2(D) Capturando pela base erro_dominio
        std::cout << "Capturado erro_dominio pela Base: " << e.what() << "\n";
    }

    // Q2(B)(D) optional de busca nos dois casos
    std::vector<std::shared_ptr<media_content>> catalog;
    catalog.push_back(std::make_shared<song>(11, "Imagine", "John Lennon", 183));
    catalog.push_back(std::make_shared<podcast>(12, "Flow #1", "Igor", 7200, 1));

    std::cout << "Buscando com optional (caso existe - ID 11):\n";
    auto opt1 = buscar(catalog, 11);
    if (opt1.has_value()) {
        std::cout << "  Achou! " << opt1.value()->title() << "\n";
    } else {
        std::cout << "  Não achou.\n";
    }

    std::cout << "Buscando com optional (caso não existe - ID 999):\n";
    auto opt2 = buscar(catalog, 999);
    if (opt2.has_value()) {
        std::cout << "  Achou! " << opt2.value()->title() << "\n";
    } else {
        std::cout << "  Não achou (nullopt retornado corretamente).\n";
    }

    // Q2(C)(D) Tratamento de std::variant com std::visit
    std::cout << "Buscando e tratando resultado com std::variant:\n";
    std::vector<int> ids_to_search = {12, 999};
    for (int id : ids_to_search) {
        resultado_busca res = buscar_com_status(catalog, id);
        std::visit([](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::shared_ptr<media_content>>) {
                std::cout << "  [Sucesso] Encontrado: " << arg->title() 
                          << " (" << arg->type_name() << ")\n";
            } else if constexpr (std::is_same_v<T, std::string>) {
                std::cout << "  [Erro] Mensagem: " << arg << "\n";
            }
        }, res);
    }
    std::cout << "==========================================\n\n";

    // ========================================================================
    // Questão 3 — STL e Concorrência
    // ========================================================================
    std::cout << "=== QUESTÃO 3: STL e Concorrência ===\n";

    // Q3(A) Map (índice ordenado por ID) e Unordered Set (unicidade de tags)
    std::map<int, std::shared_ptr<media_content>> catalog_map;
    std::unordered_set<std::string> unique_genres;

    catalog_map[1] = std::make_shared<song>(1, "Imagine", "John Lennon", 183);
    catalog_map[2] = std::make_shared<podcast>(2, "Flow #1", "Igor", 7200, 1);

    unique_genres.insert("Rock");
    unique_genres.insert("Pop");
    unique_genres.insert("Rock"); // Duplicado, não será inserido

    std::cout << "Tamanho do map (catálogo ordenado por ID): " << catalog_map.size() << "\n";
    std::cout << "Tamanho do unordered_set de gêneros (unicidade): " << unique_genres.size() << "\n";

    // Q3(B) Três algoritmos da STL com lambda com captura
    std::vector<song> algorithm_songs = {
        song(21, "Song X", "Artist X", 120),
        song(22, "Song Y", "Artist Y", 350),
        song(23, "Song Z", "Artist Z", 290)
    };

    // 1. std::sort
    std::sort(algorithm_songs.begin(), algorithm_songs.end(), [](const song& a, const song& b) {
        return a.duration_seconds() < b.duration_seconds();
    });

    // 2. std::count_if com captura
    int limite_duracao = 200;
    int long_songs = std::count_if(algorithm_songs.begin(), algorithm_songs.end(), [limite_duracao](const song& s) {
        return s.duration_seconds() > limite_duracao;
    });

    // 3. std::accumulate
    int duracao_total = std::accumulate(algorithm_songs.begin(), algorithm_songs.end(), 0, [](int acc, const song& s) {
        return acc + s.duration_seconds();
    });

    std::cout << "Resultados dos algoritmos STL:\n";
    std::cout << "  - Músicas ordenadas por duração.\n";
    std::cout << "  - Músicas mais longas que " << limite_duracao << "s: " << long_songs << "\n";
    std::cout << "  - Duração total acumulada: " << duracao_total << "s\n";

    // Q3(C)(D) Operação paralela com std::async, mutex e lock_guard
    json_repository file_repo("estado.json");
    streaming_service service(file_repo);
    service.add_item(std::make_unique<song>(1, "Imagine", "John Lennon", 183));
    service.add_item(std::make_unique<podcast>(2, "Podcast Tech", "Developer", 1200, 5));
    service.add_item(std::make_unique<song>(3, "Song Pop", "Artist Pop", 200));

    std::cout << "Disparando cálculo polimórfico paralelo de total score...\n";
    float total_score = service.calculate_total_score_parallel();
    std::cout << "Total score calculado em paralelo: " << total_score << "\n";
    std::cout << "==========================================\n\n";

    // ========================================================================
    // Questão 4 — Serialização e SOLID
    // ========================================================================
    std::cout << "=== QUESTÃO 4: Serialização e SOLID ===\n";

    // Q4(A)(B) Persistência JSON e teste do json_repository (produção)
    std::cout << "Salvando estado no arquivo estado.json...\n";
    service.save_state();

    // Cria outro serviço para carregar
    streaming_service loader_service(file_repo);
    std::cout << "Carregando estado a partir do arquivo estado.json...\n";
    loader_service.load_state();
    std::cout << "Itens carregados:\n";
    for (const auto& item : loader_service.catalog()) {
        std::cout << "  - [" << item->type_name() << "] " << item->title() << "\n";
    }

    // Q4(C)(D) Injeção de dependência e memory_repository (teste sem tocar o disco)
    std::cout << "\nDemonstrando DIP com memory_repository (sem arquivo):\n";
    memory_repository mem_repo;
    streaming_service test_service(mem_repo);
    test_service.add_item(std::make_unique<song>(1, "Memory Track", "Memory Artist", 100));
    test_service.save_state();

    streaming_service test_loader_service(mem_repo);
    test_loader_service.load_state();
    std::cout << "Itens carregados do memory_repository:\n";
    for (const auto& item : test_loader_service.catalog()) {
        std::cout << "  - " << item->title() << "\n";
    }
    std::cout << "==========================================\n\n";

    std::cout << "--- FIM DOS TESTES (TUDO COMPILADO E EXECUTADO COM SUCESSO) ---\n";
    return 0;
}