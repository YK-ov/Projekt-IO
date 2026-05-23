#include "StronaInternetowa.h"
#include <iostream>

using namespace std;

int main()
{
    StronaInternetowa* strona = new StronaInternetowa("https://docent.pl", "Docent - wirtualne repozytorium materialow studenckich");
    Sesja* sesja = new Sesja("192.0.2.1");
    strona->attachSesja(sesja);

    strona->wczytajDaneZPliku("konta.csv", "przedmioty.csv");

    string login = "";
    string password = "";
    string registerInput = "";

    //cout << "users right after running\n";
    //strona->debugPrintAllUsers();



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

    //strona->debugPrintAllUsers();

    while (true){
        strona->wykonajAkcjeUzytkownika(login);
        //strona->wykonajAkcjeUzytkownika(login);
    }



    //strona->zapiszDaneDoPliku("konta.csv", "przedmioty.csv");

    return 0;
}
