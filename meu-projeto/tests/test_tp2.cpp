#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "streaming.hpp"
#include <sstream>
#include <memory>
#include <vector>

using Catch::Approx;

TEST_CASE("TEST_CASE 1 (Q1): Construtores e destrutores da hierarquia", "[q1]") {
    test_destruction_log.clear();
    {
        std::unique_ptr<media_content> ptr = std::make_unique<song>("Test Song", "Test Artist", 200);
        // objeto song criado
    }
    // Destrutores devem ser chamados quando ptr sai do escopo.
    // Ordem: derivada (~song) primeiro, depois a base (~media_content).
    REQUIRE(test_destruction_log.size() == 2);
    REQUIRE(test_destruction_log[0] == "song");
    REQUIRE(test_destruction_log[1] == "media_content");
}

TEST_CASE("TEST_CASE 2 (Q2): Polimorfismo dinamico", "[q2]") {
    std::vector<std::unique_ptr<media_content>> itens;
    itens.push_back(std::make_unique<song>("Bohemian Rhapsody", "Queen", 354)); // 354 * 1.5f = 531.0f
    itens.push_back(std::make_unique<podcast>("DevCast", "Presenter", 1200, 10)); // 1200 * 0.8f + 10 * 10 = 960 + 100 = 1060.0f

    // Verificar cálculo
    REQUIRE(itens[0]->calcular() == Approx(531.0f));
    REQUIRE(itens[1]->calcular() == Approx(1060.0f));

    // Verificar a função livre maior_valor
    const media_content* maior = maior_valor(itens);
    REQUIRE(maior != nullptr);
    REQUIRE(maior->title() == "DevCast");
    REQUIRE(maior->calcular() == Approx(1060.0f));
}

TEST_CASE("TEST_CASE 3 (Q3): Interface pura", "[q3]") {
    // Passamos para uma função por referência para verificar se ela pode executar sem conhecer a classe concreta
    auto check_share = [](const sharable& s) -> std::string {
        std::stringstream ss;
        s.share(ss);
        return ss.str();
    };

    song s("Imagine", "John Lennon", 183);
    playlist p("My Favorites");
    
    // song implementa sharable
    REQUIRE(check_share(s) == "Compartilhando música: 'Imagine' por John Lennon");

    // playlist implementa sharable independentemente de media_content
    REQUIRE(check_share(p) == "Compartilhando playlist: 'My Favorites' com 0 músicas");
}
