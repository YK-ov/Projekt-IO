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

    vector<Przedmiot*>::iterator it;

    for (it = system->getPrzedmioty().begin(); it != system->getPrzedmioty().end(); it++){
        vector<Material*>::iterator mIt;
        for (mIt = (*it)->getMaterialy().begin(); mIt != (*it)->getMaterialy().end(); mIt++){
            delete (*mIt);
        }
        (*it)->getMaterialy().clear();

        delete (*it);
    }

    vector<Konto*>::iterator kIt;
    for (kIt = system->getKonta().begin(); kIt != system->getKonta().end(); kIt++){
        delete (*kIt);
    }

    system->getKonta().clear();
    system->getPrzedmioty().clear();

    delete system;

    return 0;
}
