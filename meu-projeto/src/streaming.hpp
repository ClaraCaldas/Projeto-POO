#ifndef STREAMING_HPP
#define STREAMING_HPP

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <ostream>
#include <fstream>
#include <sstream>
#include <concepts>
#include <optional>
#include <variant>
#include <future>
#include <mutex>
#include <unordered_set>
#include <map>
#include <utility>
#include <algorithm>
#include <numeric>
#include <ranges>
#include <nlohmann/json.hpp>

// Log global para rastrear a ordem de destruição programaticamente nos testes
inline std::vector<std::string> test_destruction_log;

// ============================================================================
// Q2(A) Hierarquia de Exceções Própria
// ============================================================================
class erro_dominio : public std::runtime_error {
public:
    using std::runtime_error::runtime_error; // Herda construtores
};

class item_invalido : public erro_dominio {
public:
    explicit item_invalido(const std::string& msg)
        : erro_dominio("Item Inválido: " + msg) {}
};

class recurso_ausente : public erro_dominio {
public:
    explicit recurso_ausente(const std::string& msg)
        : erro_dominio("Recurso Ausente: " + msg) {}
};

// ============================================================================
// Q1(C) Concept restritivo (C++20)
// ============================================================================
template <typename T>
concept calculavel = requires (const T& t) {
    { t.calcular() } -> std::convertible_to<double>;
};

// Função genérica restrita pelo concept
template <calculavel T>
double soma_total(const std::vector<T>& v) {
    double s = 0.0;
    for (const auto& x : v) {
        s += x.calcular();
    }
    return s;
}

// ============================================================================
// Q1(A) Template de Classe Reutilizável
// ============================================================================
template <typename T>
class registry {
private:
    std::vector<T> items_;
public:
    void add(T item) { items_.push_back(std::move(item)); }
    const T& at(std::size_t i) const { return items_.at(i); }
    std::size_t size() const { return items_.size(); }
    auto begin() const { return items_.begin(); }
    auto end() const { return items_.end(); }
    auto begin() { return items_.begin(); }
    auto end() { return items_.end(); }
};

// ============================================================================
// Q1(B) CRTP Mixin de Contagem de Instâncias (sem vtable)
// ============================================================================
template <typename Derived>
class counted {
private:
    static inline int count_ = 0;
public:
    counted() { ++count_; }
    counted(const counted&) { ++count_; }
    counted(counted&&) noexcept { ++count_; }
    counted& operator=(const counted&) { return *this; }
    counted& operator=(counted&&) noexcept { return *this; }
    virtual ~counted() { --count_; }
    static int alive() { return count_; }
};

// ============================================================================
// Classes do Domínio
// ============================================================================

// Q3(A) Interface Pura
class sharable {
public:
    virtual void share(std::ostream& os = std::cout) const = 0;
    virtual ~sharable() = default;
    sharable() = default;
    sharable(const sharable&) = default;
    sharable(sharable&&) noexcept = default;
    sharable& operator=(const sharable&) = default;
    sharable& operator=(sharable&&) noexcept = default;
};

// Q1(A) Classe Base Abstrata
class media_content {
protected:
    int id_;
    std::string title_;
    int duration_seconds_;

public:
    media_content(int id, std::string title, int duration_seconds)
        : id_(id), title_(title), duration_seconds_(duration_seconds) {
        std::cout << "[media_content] Criado: " << title_ << "\n";
    }

    media_content(std::string title, int duration_seconds)
        : media_content(0, title, duration_seconds) {}

    // Q1(A) Destrutor Virtual
    virtual ~media_content() {
        std::cout << "[media_content] ~Destruido: " << title_ << "\n";
        test_destruction_log.push_back("media_content");
    }

    media_content(const media_content&) = default;
    media_content(media_content&&) noexcept = default;
    media_content& operator=(const media_content&) = default;
    media_content& operator=(media_content&&) noexcept = default;

    int id() const { return id_; }
    std::string title() const { return title_; }
    int duration_seconds() const { return duration_seconds_; }

    // Método virtual puro
    virtual float calcular() const = 0;

    // Método virtual com implementação padrão
    virtual void exibir() const {
        std::cout << "Conteúdo: " << title_ << " (" << duration_seconds_ << "s)";
    }

    // Retorna o tipo de classe para serialização
    virtual std::string type_name() const = 0;
};

// Q1(B) Classe song herdando de media_content, sharable e counted (CRTP)
class song : public media_content, public sharable, public counted<song> {
private:
    std::string artist_;

public:
    song(int id, std::string title, std::string artist, int duration_seconds)
        : media_content(id, title, duration_seconds), artist_(artist) {
        std::cout << "[song] Criada: " << title_ << "\n";
    }

    song(std::string title, std::string artist, int duration_seconds)
        : song(0, title, artist, duration_seconds) {}

    ~song() override {
        std::cout << "[song] ~Destruida: " << title_ << "\n";
        test_destruction_log.push_back("song");
    }

    song(const song&) = default;
    song(song&&) noexcept = default;
    song& operator=(const song&) = default;
    song& operator=(song&&) noexcept = default;

    float calcular() const override {
        return duration_seconds_ * 1.5f;
    }

    void exibir() const override {
        media_content::exibir(); // Chama a versão da base
        std::cout << " por " << artist_ << "\n";
    }

    void share(std::ostream& os = std::cout) const override {
        os << "Compartilhando música: '" << title_ << "' por " << artist_;
    }

    std::string artist() const { return artist_; }
    std::string type_name() const override { return "song"; }

    void play() const {
        int minutes = duration_seconds_ / 60;
        int seconds = duration_seconds_ % 60;
        std::cout << "▶ Tocando agora: '" << title_ << "' por " << artist_ 
                  << " (" << minutes << ":" << (seconds < 10 ? "0" : "") << seconds << ")\n";
    }
};

// Q1(B) Segunda classe derivada final, usando counted (CRTP)
class podcast final : public media_content, public counted<podcast> {
private:
    std::string host_;
    int episodes_count_;

public:
    podcast(int id, std::string title, std::string host, int duration_seconds, int episodes_count)
        : media_content(id, title, duration_seconds), host_(host), episodes_count_(episodes_count) {
        std::cout << "[podcast] Criado: " << title_ << "\n";
    }

    podcast(std::string title, std::string host, int duration_seconds, int episodes_count)
        : podcast(0, title, host, duration_seconds, episodes_count) {}

    ~podcast() override {
        std::cout << "[podcast] ~Destruido: " << title_ << "\n";
        test_destruction_log.push_back("podcast");
    }

    podcast(const podcast&) = default;
    podcast(podcast&&) noexcept = default;
    podcast& operator=(const podcast&) = default;
    podcast& operator=(podcast&&) noexcept = default;

    float calcular() const override {
        return duration_seconds_ * 0.8f + episodes_count_ * 10.0f;
    }

    void exibir() const override {
        std::cout << "🎙 Podcast: " << title_ << " apresentado por " << host_ 
                  << " (" << duration_seconds_ << "s, " << episodes_count_ << " episódios)\n";
    }

    std::string host() const { return host_; }
    int episodes_count() const { return episodes_count_; }
    std::string type_name() const override { return "podcast"; }
};

// playlist
class playlist : public sharable {
private:
    std::string name_;
    std::vector<std::shared_ptr<song>> songs_; // Agregação

public:
    playlist(std::string name) : name_(name) {
        std::cout << "[playlist] Criada: " << name_ << "\n";
    }

    ~playlist() {
        std::cout << "[playlist] ~Destruida: " << name_ << "\n";
    }

    void add_song(std::shared_ptr<song> s) {
        if (s) {
            songs_.push_back(s);
            std::cout << "  -> Música '" << s->title() << "' adicionada à playlist '" << name_ << "'\n";
        }
    }

    int calculate_total_duration() const {
        int total = 0;
        for (const auto& s : songs_) {
            total += s->duration_seconds();
        }
        return total;
    }

    void share(std::ostream& os = std::cout) const override {
        os << "Compartilhando playlist: '" << name_ << "' com " << songs_.size() << " músicas";
    }
};

class subscription_plan {
private:
    std::string name_;
    float monthly_price_;

public:
    subscription_plan(std::string name, float monthly_price)
        : name_(name), monthly_price_(monthly_price) {
        std::cout << "[subscription_plan] Criado: " << name_ << "\n";
    }

    ~subscription_plan() {
        std::cout << "[subscription_plan] ~Destruido: " << name_ << "\n";
    }

    std::string name() const { return name_; }
    float calculate_annual_cost() const {
        return monthly_price_ * 12.0f;
    }
};

class user {
private:
    std::string username_;
    std::unique_ptr<subscription_plan> plan_; // Composição

public:
    user(std::string username, std::string plan_name, float plan_price)
        : username_(username), 
          plan_(std::make_unique<subscription_plan>(plan_name, plan_price)) {
        std::cout << "[user] Criado: " << username_ << "\n";
    }

    ~user() {
        std::cout << "[user] ~Destruido: " << username_ << "\n";
    }

    void print_info() const {
        std::cout << "\n=== Perfil de " << username_ << " ===\n";
        std::cout << "Plano Atual: " << plan_->name() << "\n";
        std::cout << "Custo Anual Projetado: R$ " << plan_->calculate_annual_cost() << "\n";
        std::cout << "===========================\n";
    }
};

// ============================================================================
// Q2(B) Busca com std::optional
// ============================================================================
template <typename T>
inline std::optional<T> buscar(const std::vector<T>& v, int id) {
    for (const auto& p : v) {
        if (p.id() == id) return p;
    }
    return std::nullopt;
}

inline std::optional<const media_content*> buscar(const std::vector<std::unique_ptr<media_content>>& v, int id) {
    for (const auto& p : v) {
        if (p->id() == id) return p.get();
    }
    return std::nullopt;
}

inline std::optional<std::shared_ptr<media_content>> buscar(const std::vector<std::shared_ptr<media_content>>& v, int id) {
    for (const auto& p : v) {
        if (p->id() == id) return p;
    }
    return std::nullopt;
}

// Q2(C) std::variant para multi-tipo ou resultado/erro
using resultado_busca = std::variant<std::shared_ptr<media_content>, std::string>;

inline resultado_busca buscar_com_status(const std::vector<std::shared_ptr<media_content>>& v, int id) {
    auto res = buscar(v, id);
    if (res.has_value()) {
        return res.value();
    }
    return std::string("Erro: Recurso com ID " + std::to_string(id) + " não encontrado.");
}

// maior_valor
inline const media_content* maior_valor(const std::vector<std::unique_ptr<media_content>>& itens) {
    if (itens.empty()) return nullptr;
    const media_content* maior = itens[0].get();
    for (const auto& item : itens) {
        if (item->calcular() > maior->calcular()) {
            maior = item.get();
        }
    }
    return maior;
}

// ============================================================================
// Q4(A)(B) Serialização não-intrusiva JSON
// ============================================================================
inline void to_json(nlohmann::json& j, const std::unique_ptr<media_content>& p) {
    if (!p) {
        j = nullptr;
        return;
    }
    if (p->type_name() == "song") {
        const auto* s = dynamic_cast<const song*>(p.get());
        j = nlohmann::json{
            {"type", "song"},
            {"id", s->id()},
            {"title", s->title()},
            {"duration_seconds", s->duration_seconds()},
            {"artist", s->artist()}
        };
    } else if (p->type_name() == "podcast") {
        const auto* pod = dynamic_cast<const podcast*>(p.get());
        j = nlohmann::json{
            {"type", "podcast"},
            {"id", pod->id()},
            {"title", pod->title()},
            {"duration_seconds", pod->duration_seconds()},
            {"host", pod->host()},
            {"episodes_count", pod->episodes_count()}
        };
    }
}

inline void from_json(const nlohmann::json& j, std::unique_ptr<media_content>& p) {
    std::string type = j.at("type").get<std::string>();
    int id = j.at("id").get<int>();
    std::string title = j.at("title").get<std::string>();
    int duration = j.at("duration_seconds").get<int>();
    
    if (type == "song") {
        std::string artist = j.at("artist").get<std::string>();
        p = std::make_unique<song>(id, title, artist, duration);
    } else if (type == "podcast") {
        std::string host = j.at("host").get<std::string>();
        int episodes = j.at("episodes_count").get<int>();
        p = std::make_unique<podcast>(id, title, host, duration, episodes);
    }
}

// ============================================================================
// Q4(C)(D) DIP (Dependency Inversion Principle)
// ============================================================================
class repository {
public:
    virtual void save(const std::vector<std::unique_ptr<media_content>>& items) = 0;
    virtual std::vector<std::unique_ptr<media_content>> load() = 0;
    virtual ~repository() = default;
};

// Implementação 1: Produção (salva em arquivo JSON)
class json_repository : public repository {
private:
    std::string filename_;
public:
    explicit json_repository(std::string filename) : filename_(std::move(filename)) {}

    void save(const std::vector<std::unique_ptr<media_content>>& items) override {
        nlohmann::json doc;
        doc["version"] = 1; // versionando o formato
        doc["itens"] = items;
        std::ofstream file(filename_);
        if (!file.is_open()) {
            throw recurso_ausente("Não foi possível abrir o arquivo para gravação: " + filename_);
        }
        file << doc.dump(4);
    }

    std::vector<std::unique_ptr<media_content>> load() override {
        std::ifstream file(filename_);
        if (!file.is_open()) {
            return {}; // Retorna vazio se não existir
        }
        nlohmann::json doc;
        try {
            file >> doc;
        } catch (...) {
            throw item_invalido("Arquivo JSON de dados corrompido.");
        }
        int version = doc.at("version").get<int>();
        if (version != 1) {
            throw erro_dominio("Versão do arquivo JSON incompatível.");
        }
        std::vector<std::unique_ptr<media_content>> items;
        doc.at("itens").get_to(items);
        return items;
    }
};

// Implementação 2: Testes (salva apenas na memória)
class memory_repository : public repository {
private:
    nlohmann::json data_;
public:
    void save(const std::vector<std::unique_ptr<media_content>>& items) override {
        nlohmann::json doc;
        doc["version"] = 1;
        doc["itens"] = items;
        data_ = doc;
    }

    std::vector<std::unique_ptr<media_content>> load() override {
        if (data_.empty()) {
            return {};
        }
        std::vector<std::unique_ptr<media_content>> items;
        data_.at("itens").get_to(items);
        return items;
    }
};

// Classe de alto nível que depende da abstração repository (DIP)
class streaming_service {
private:
    repository& repo_;
    std::vector<std::unique_ptr<media_content>> catalog_;

public:
    explicit streaming_service(repository& repo) : repo_(repo) {}

    void add_item(std::unique_ptr<media_content> item) {
        if (!item) {
            throw item_invalido("Ponteiro nulo de item.");
        }
        catalog_.push_back(std::move(item));
    }

    void remove_item(int index) {
        if (index < 0 || index >= static_cast<int>(catalog_.size())) {
            throw recurso_ausente("Índice fora dos limites.");
        }
        catalog_.erase(catalog_.begin() + index);
    }

    void save_state() {
        repo_.save(catalog_);
    }

    void load_state() {
        catalog_ = repo_.load();
    }

    const std::vector<std::unique_ptr<media_content>>& catalog() const {
        return catalog_;
    }

    // Q3(C)(D) Concorrência: paraleliza cálculo independente com mutex protegendo acumulador
    float calculate_total_score_parallel() const {
        std::mutex mtx;
        float total_score = 0.0f;
        std::vector<std::future<float>> futures;

        for (const auto& item : catalog_) {
            futures.push_back(std::async(std::launch::async, [&item]() {
                return item->calcular(); // Operação puramente de leitura, independente
            }));
        }

        for (auto& f : futures) {
            float val = f.get(); // Coleta
            std::lock_guard<std::mutex> lock(mtx); // Região crítica
            total_score += val;
        }

        return total_score;
    }
};

#endif