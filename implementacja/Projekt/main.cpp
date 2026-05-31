#include "SystemEdukacyjny.h"
#include <iostream>

using namespace std;

int main()
{
    SystemEdukacyjny* system = new SystemEdukacyjny("Docent - wirtualna platforma materialow studenckich");

    system->wczytajPrzedmiotyIKontaZPliku("konta.csv", "przedmioty.csv");

    string login = "";
    string password = "";
    string registerInput = "";

    while (true){
        cout << "Wprowadz login:\n";
        cin >> login;
        cout << "Wprowadz haslo:\n";
        cin >> password;

        bool isLogged = system->logowanie(login, password);

        if (isLogged){
            break;
        }
    }

    while (true){
        bool userIsActive = system->wykonajAkcjeUzytkownika(login);

        if (!userIsActive){
            break;
        }
    }

    delete system;

    return 0;
}
