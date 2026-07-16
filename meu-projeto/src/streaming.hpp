#ifndef STREAMING_HPP
#define STREAMING_HPP

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <ostream>

// Log global para rastrear a ordem de destruição programaticamente nos testes
inline std::vector<std::string> test_destruction_log;

// Q3(A) Interface Pura
class sharable {
public:
    virtual void share(std::ostream& os = std::cout) const = 0;
    virtual ~sharable() = default;
};

// Q1(A) Classe Base Abstrata
class media_content {
protected:
    std::string title_;
    int duration_seconds_;

public:
    media_content(std::string title, int duration_seconds)
        : title_(title), duration_seconds_(duration_seconds) {
        std::cout << "[media_content] Criado: " << title_ << "\n";
    }

    // Q1(A) Destrutor Virtual
    virtual ~media_content() {
        std::cout << "[media_content] ~Destruido: " << title_ << "\n";
        test_destruction_log.push_back("media_content");
    }

    std::string title() const { return title_; }
    int duration_seconds() const { return duration_seconds_; }

    // Q1(A) Método Virtual Puro
    virtual float calcular() const = 0;

    // Q1(A) Método Virtual não-puro com implementação padrão
    virtual void exibir() const {
        std::cout << "Conteúdo: " << title_ << " (" << duration_seconds_ << "s)";
    }
};

// Q1(B) Classe Derivada Concreta song herdando de media_content e Q3(B) da interface sharable
class song : public media_content, public sharable {
private:
    std::string artist_;

public:
    song(std::string title, std::string artist, int duration_seconds)
        : media_content(title, duration_seconds), artist_(artist) {
        std::cout << "[song] Criada: " << title_ << "\n";
    }

    // Q1(C) Registro de destrutor para verificação de encadeamento
    ~song() override {
        std::cout << "[song] ~Destruida: " << title_ << "\n";
        test_destruction_log.push_back("song");
    }

    // Q1(B) Sobrescrita do método virtual puro
    float calcular() const override {
        return duration_seconds_ * 1.5f;
    }

    // Q1(B) Sobrescrita de método virtual não-puro chamando Base::metodo()
    void exibir() const override {
        media_content::exibir(); // Chama a versão da base
        std::cout << " por " << artist_ << "\n";
    }

    // Q3(B) Implementação da interface
    void share(std::ostream& os = std::cout) const override {
        os << "Compartilhando música: '" << title_ << "' por " << artist_;
    }

    void play() const {
        int minutes = duration_seconds_ / 60;
        int seconds = duration_seconds_ % 60;
        std::cout << "▶ Tocando agora: '" << title_ << "' por " << artist_ 
                  << " (" << minutes << ":" << (seconds < 10 ? "0" : "") << seconds << ")\n";
    }
};

// Q1(B) Segunda Classe Derivada Concreta (Q3(C) marcada como final)
class podcast final : public media_content {
private:
    std::string host_;
    int episodes_count_;

public:
    podcast(std::string title, std::string host, int duration_seconds, int episodes_count)
        : media_content(title, duration_seconds), host_(host), episodes_count_(episodes_count) {
        std::cout << "[podcast] Criado: " << title_ << "\n";
    }

    ~podcast() override {
        std::cout << "[podcast] ~Destruido: " << title_ << "\n";
        test_destruction_log.push_back("podcast");
    }

    // Q1(B) Sobrescrita de método virtual puro
    float calcular() const override {
        return duration_seconds_ * 0.8f + episodes_count_ * 10.0f;
    }

    // Q1(B) Sobrescrita de método virtual
    void exibir() const override {
        std::cout << "🎙 Podcast: " << title_ << " apresentado por " << host_ 
                  << " (" << duration_seconds_ << "s, " << episodes_count_ << " episódios)\n";
    }
};

// Q3(B) Classe concreta herdando da interface
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

    // Q3(B) Implementação da interface
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

#endif