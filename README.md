# Projeto POO - Plataforma de streaming de música

**Nome:** Maria Clara Caldas Fernandes
**Matrícula:** 20250018870

## Descrição do Domínio:
O sistema consiste em uma plataforma de streaming de música. Ele gerencia assinantes (`user`), que possuem um plano de assinatura exclusivo (`subscription_plan`). Além disso, o sistema permite a criação de playlists (`playlist`) que agrupam diversas músicas (`song`) disponíveis no catálogo da plataforma.

## Diagrama UML de Classes :

```mermaid
classDiagram
    class media_content {
        <<abstract>>
        #string title_
        #int duration_seconds_
        +title() string
        +duration_seconds() int
        +calcular()* float
        +exibir() void
    }

    class sharable {
        <<interface>>
        +share(ostream& os)* void
    }

    class song {
        -string artist_
        +calcular() float
        +exibir() void
        +share(ostream& os) void
        +play() void
    }

    class podcast {
        -string host_
        -int episodes_count_
        +calcular() float
        +exibir() void
    }

    class playlist {
        -string name_
        -vector~shared_ptr~song~~ songs_
        +add_song(shared_ptr~song~ s) void
        +calculate_total_duration() int
        +share(ostream& os) void
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

    media_content <|-- song : herança
    media_content <|-- podcast : herança
    sharable <|.. song : implementação
    sharable <|.. playlist : implementação
    user *-- subscription_plan : composição 
    playlist o-- song : agregação 
```

## Herança Avançada

Neste projeto, a classe `podcast` foi marcada com a palavra-chave `final`:
```cpp
class podcast final : public media_content { ... };
```
**Justificativa de Design:**
1. **Impedir Especialização Adicional:** A classe `podcast` representa um tipo de conteúdo final no nosso modelo de streaming de mídia. Não há justificativa de negócio ou arquitetura para estender um `podcast` em novas subclasses.
2. **Otimização do Compilador (Devirtualização):** Ao marcar a classe como `final`, informamos ao compilador que nenhuma outra classe poderá herdar dela. Isso permite que o compilador otimize chamadas de métodos virtuais (como `calcular()` e `exibir()`) transformando-as em chamadas diretas não-virtuais quando o tipo estático é conhecido, eliminando o overhead de consulta na tabela de métodos virtuais (vtable).

