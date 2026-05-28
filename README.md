# Projeto POO - Plataforma de streaming de música

**Nome:** Maria Clara Caldas Fernandes
**Matrícula:** 20250018870

## Descrição do Domínio:
O sistema consiste em uma plataforma de streaming de música. Ele gerencia assinantes (`user`), que possuem um plano de assinatura exclusivo (`subscription_plan`). Além disso, o sistema permite a criação de playlists (`playlist`) que agrupam diversas músicas (`song`) disponíveis no catálogo da plataforma.

## Diagrama UML de Classes :

```mermaid
classDiagram
    class song {
        -string title_
        -string artist_
        -int duration_seconds_
        +title() string
        +duration_seconds() int
        +play() void
    }

    class playlist {
        -string name_
        -vector~shared_ptr~song~~ songs_
        +add_song(shared_ptr~song~ s) void
        +calculate_total_duration() int
    }

    class subscription_plan {
        -string name_
        -float monthly_price_
        +name() string
        +calculate_annual_cost() float
    }

    class user {
        -string username_
        -unique_ptr~subscription_plan~ plan_
        +print_info() void
    }

    user *-- subscription_plan : composição 
    playlist o-- song : agregação 
