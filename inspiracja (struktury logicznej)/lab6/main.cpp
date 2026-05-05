#include <iostream>
#include "Konto.h"
#include "Website.h"

using namespace std;

int main()
{
    Sesja* sesja = new Sesja("199.12.130.213");

    Website umcs("https://umcs.pl");

    umcs.attachSesja(sesja);

    Konto* firstKonto = new Konto("user", "user123");
    Konto* secondKonto = new Konto("admin", "admin123");
    Konto* thirdKonto = new Konto("login", "login123");

    umcs.attachKonto(firstKonto);
    umcs.attachKonto(secondKonto);
    umcs.attachKonto(thirdKonto);

    while(true){
        int currentProba = sesja->getProby();

            cout << "Witamy na stronie " << umcs.getAdresHtml() << ", prosze sie zalogowac do systemu\n";

            string password = "";
            string login = "";

            cout << "Prosze podac login:\n";
            cin >> login;
            cout << "Porsze podac haslo:\n";
            cin >> password;

            bool log = umcs.logowanie(login, password, sesja->getIp());

            if (log){
                cout << "Zalogowales sie\n";
                break;
            }
            else {
                if (currentProba == sesja->getProby() && !sesja->getCzyJestZablokowana()){
                    string curreintInput = "";
                    cout << "Wpisz tak lub nie dla rejestcji:\n";
                    cin >> curreintInput;

                    if (curreintInput == "tak"){
                        while (true){
                            cout << "Wpisz login dla rejestracji:\n";
                            string loginForRegistration = "";

                            cin >> loginForRegistration;

                            string passwordForRegistration = "";
                            cout << "Wpisz haslo dla rejestracji:\n";

                            cin >> passwordForRegistration;

                            bool reg = umcs.rejestracja(loginForRegistration, passwordForRegistration, sesja->getIp());

                            if (reg){
                                break;
                            }
                        }
                    }
                    if (curreintInput == "nie"){
                        cout << "Dobrze, kontynujemu proces logowania...\n";
                    }
                    else {
                        cout << "Nieprawidlowy input\n";
                    }
                }
                else {
                        if (!sesja->getCzyJestZablokowana()){
                            cout << "Bledne haslo, pozostalo prob: " << 5 - sesja->getProby() << "\n";
                        }
                }
            }
    }


    umcs.detachSesja(sesja->getIp());
    delete sesja;

    umcs.detachKonto(firstKonto->getLogin());
    umcs.detachKonto(secondKonto->getLogin());
    umcs.detachKonto(thirdKonto->getLogin());

    delete firstKonto;
    delete secondKonto;
    delete thirdKonto;

    return 0;
}
