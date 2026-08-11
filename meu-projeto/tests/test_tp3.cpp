#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "streaming.hpp"
#include <sstream>
#include <memory>
#include <vector>

using Catch::Approx;

// 1. Teste de templates e concepts (Q1)
TEST_CASE("TEST_CASE 1: templates e concepts", "[q1]") {
    std::vector<song> songs;
    songs.push_back(song(1, "Imagine", "John Lennon", 180)); // calcular() = 270.0
    songs.push_back(song(2, "Song B", "Artist B", 240));    // calcular() = 360.0

    // Verifica restrição de concept via soma_total
    double total = soma_total(songs);
    REQUIRE(total == Approx(630.0));
}

// 2. Teste de tratamento de erros (Q2)
TEST_CASE("TEST_CASE 2: tratamento de erros e optional", "[q2]") {
    // Lançamento de exceção customizada em argumentos inválidos
    auto validate_song = [](const song& s) {
        if (s.title().empty() || s.duration_seconds() <= 0) {
            throw item_invalido("Campos de música inválidos.");
        }
    };
    
    // Lança derivada, verifica se Catch consegue capturar pelo tipo base erro_dominio
    REQUIRE_THROWS_AS(validate_song(song(3, "", "Artist A", 120)), erro_dominio);
    REQUIRE_THROWS_AS(validate_song(song(4, "Imagine", "John Lennon", -10)), erro_dominio);
    
    // Busca e retorno de optional
    std::vector<std::shared_ptr<media_content>> v;
    v.push_back(std::make_shared<song>(1, "Track A", "Artist A", 120));
    
    auto opt1 = buscar(v, 1);
    REQUIRE(opt1.has_value());
    REQUIRE(opt1.value()->title() == "Track A");
    
    auto opt2 = buscar(v, 999);
    REQUIRE_FALSE(opt2.has_value());
}

// 3. Teste de serialização round-trip (Q4)
TEST_CASE("TEST_CASE 3: serializacao round-trip", "[q4]") {
    std::vector<std::unique_ptr<media_content>> catalog;
    catalog.push_back(std::make_unique<song>(1, "Imagine", "John Lennon", 183));
    catalog.push_back(std::make_unique<podcast>(2, "DevCast", "Marcos", 3600, 10));

    nlohmann::json j = catalog; // Serializa o vetor de unique_ptrs
    
    std::vector<std::unique_ptr<media_content>> loaded_catalog;
    j.get_to(loaded_catalog); // Desserializa
    
    REQUIRE(loaded_catalog.size() == 2);
    REQUIRE(loaded_catalog[0]->title() == "Imagine");
    REQUIRE(loaded_catalog[0]->type_name() == "song");
    
    REQUIRE(loaded_catalog[1]->title() == "DevCast");
    REQUIRE(loaded_catalog[1]->type_name() == "podcast");
}

// 4. Teste de DIP usando memory_repository (Q4-D)
TEST_CASE("TEST_CASE 4: DIP usando memory_repository", "[q4-dip]") {
    memory_repository mem_repo;
    streaming_service app(mem_repo);

    app.add_item(std::make_unique<song>(1, "In-Memory Track", "Memory Artist", 100));
    app.add_item(std::make_unique<podcast>(2, "In-Memory Pod", "Memory Host", 3000, 5));

    // Salva o estado na memória
    app.save_state();

    // Cria outra instância utilizando o mesmo memory repository
    streaming_service app_loader(mem_repo);
    app_loader.load_state();

    // Verifica que os dados foram carregados corretamente sem tocar o disco
    const auto& loaded = app_loader.catalog();
    REQUIRE(loaded.size() == 2);
    REQUIRE(loaded[0]->title() == "In-Memory Track");
    REQUIRE(loaded[1]->title() == "In-Memory Pod");
}
