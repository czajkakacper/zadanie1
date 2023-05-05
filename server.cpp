#include <iostream>
#include <sstream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

const int PORT = 8080;
const std::string AUTHOR = "Kacper Czajka";

void log_server_info(int port) {
    // Pobieramy aktualny czas
    std::time_t now = std::time(nullptr);
    std::tm *local_time = std::localtime(&now);

    // Tworzymy komunikat logu
    std::stringstream log;
    log << "Server pracuje na porcie: " << port << std::endl << " Czas lokalny: " << std::asctime(local_time)
        << std::endl << "Autor: " << AUTHOR << std::endl;

    // Wypisujemy komunikat logu na standardowe wyjście
    std::cout << log.str();
}

std::string get_client_info(int client_socket) {
    // Pobieramy adres IP klienta
    struct sockaddr_in client_address;
    socklen_t addr_len = sizeof(client_address);
    getpeername(client_socket, (struct sockaddr *)&client_address, &addr_len);
    std::string client_ip = inet_ntoa(client_address.sin_addr);

    // Pobieramy czas w strefie czasowej klienta
    std::time_t now = std::time(nullptr);
    std::tm *local_time = std::localtime(&now);

    // Tworzymy treść strony informacyjnej
    std::stringstream page;
    page << "<html><head><title>Client information</title></head><body>"
         << "<p>Client IP: " << client_ip << "</p>"
         << "<p>Lokalny czas " << std::asctime(local_time) << "</p>"
         << "</body></html>";

    // Tworzymy nagłówek HTTP z treścią strony informacyjnej
    std::stringstream response;
    response << "HTTP/1.1 200 OK\r\n"
             << "Content-Type: text/html\r\n"
             << "Content-Length: " << page.str().length() << "\r\n"
             << "\r\n"
             << page.str();

    return response.str();
}

int main() {
    // Ustawiamy seed dla funkcji rand()
    std::srand(std::time(nullptr));

    // Tworzymy gniazdo serwera
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Ustawiamy opcję SO_REUSEADDR, aby umożliwić ponowne użycie adresu
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // Konfigurujemy adres serwera
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    // Wiążemy gniazdo serwera z adresem
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // Nasłuchujemy na gnieździe serwera
    if (listen(server_socket, 5) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    // Wypisujemy informacje o uruchomieniu serwera w logach
    log_server_info(PORT);

    // Obsługujemy połączenia klientów
    while (true) {
    // Akceptujemy połączenie klienta
    int client_socket = accept(server_socket, nullptr, nullptr);
    if (client_socket < 0) {
        perror("accept");
        continue;
    }

    // Generujemy losowy czas oczekiwania przed wysłaniem odpowiedzi
    int delay = std::rand() % 5 + 1;
    sleep(delay);

    // Wysyłamy odpowiedź do klienta
    std::string response = get_client_info(client_socket);
    if (send(client_socket, response.c_str(), response.length(), 0) < 0) {
        perror("send");
    }

    // Zamykamy gniazdo klienta
    close(client_socket);
    }

    // Zamykamy gniazdo serwera
    close(server_socket);

return 0;
}