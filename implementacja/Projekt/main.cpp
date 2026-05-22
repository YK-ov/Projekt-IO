#include "StronaInternetowa.h"
#include <iostream>

using namespace std;

int main()
{
    StronaInternetowa* strona = new StronaInternetowa("https://docent.pl", "Docent - wirtualne repozytorium materialow studenckich");
    Sesja* sesja = new Sesja("192.0.2.1");
    strona->attachSesja(sesja);

    strona->wczytajDaneZPliku("konta.csv");

    string login = "";
    string password = "";

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


    return 0;
}
