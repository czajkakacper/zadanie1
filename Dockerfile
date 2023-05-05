# Autor: Kacper Czajka

# Etap 1: Budowanie programu
FROM alpine:latest as builder

WORKDIR /app

COPY server.cpp .

RUN apk add --no-cache g++ \
    && g++ -o server server.cpp \
    && apk del g++

# Etap 2: Tworzenie obrazu
FROM alpine:latest

WORKDIR /app

# Dodajemy informację o autorze
LABEL author="Kacper Czajka"

# Ustawiamy zmienne środowiskowe
ENV PORT=8080 \
    AUTHOR="Kacper Czajka"

# Instalujemy potrzebne biblioteki
RUN apk add --no-cache libstdc++

# Kopiujemy plik wykonywalny z etapu 1
COPY --from=builder /app/server .

# Otwieramy port
EXPOSE ${PORT}

# Uruchamiamy program serwera
CMD ["./server"]
