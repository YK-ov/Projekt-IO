#include "StronaInternetowa.h"
#include "testyJednostkowe.h"
#include <iostream>

using namespace std;

int main()
{
    testyJednostkowe testy();
    testy.testujLogowanie();
    cout << testy.getPassedTestCounter();

    StronaInternetowa* strona = new StronaInternetowa("https://docent.pl", "Docent - wirtualne repozytorium materialow studenckich");
    Sesja* sesja = new Sesja("192.0.2.1");
    strona->attachSesja(sesja);

    strona->wczytajDaneZPliku("konta.csv", "przedmioty.csv");

    string login = "";
    string password = "";
    string registerInput = "";

    while (true){
        cout << "Wprowadz login:\n";
        cin >> login;
        cout << "Wprowadz haslo:\n";
        cin >> password;

        bool isLogged = strona->logowanie(login, password, sesja->getIpAdres());

        if (isLogged){
            break;
        }
    }

    while (true){
        bool userIsActive = strona->wykonajAkcjeUzytkownika(login);

        if (!userIsActive){
            break;
        }
    }

    delete sesja;
    delete strona;

    return 0;
}
