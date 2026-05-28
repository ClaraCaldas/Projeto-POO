#ifndef STREAMING_HPP
#define STREAMING_HPP

#include <iostream>
#include <string>
#include <vector>
#include <memory>

class song {
private:
    std::string title_;
    std::string artist_;
    int duration_seconds_;

public:
    song(std::string title, std::string artist, int duration_seconds)
        : title_(title), artist_(artist), duration_seconds_(duration_seconds) {
        std::cout << "[song] Criada: " << title_ << "\n";
    }

    ~song() {
        std::cout << "[song] ~Destruida: " << title_ << "\n";
    }

    std::string title() const { return title_; }
    int duration_seconds() const { return duration_seconds_; }

    void play() const {
        int minutes = duration_seconds_ / 60;
        int seconds = duration_seconds_ % 60;
        std::cout << "▶ Tocando agora: '" << title_ << "' por " << artist_ 
                  << " (" << minutes << ":" << (seconds < 10 ? "0" : "") << seconds << ")\n";
    }
};

class playlist {
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

#endif