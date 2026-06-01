#define CATCH_CONFIG_RUNNER
#include "catch.hpp"
#include "SystemEdukacyjny.h"
#include "Student.h"
#include <ctime>
#include <fstream>
#include <sstream>

using namespace std;

string generateRandomString(){
    int size = rand() % 15;
    string randomString = "";

    for (int i = 0; i < size; i++){
        char randomChar = 'a' + rand() % 26;
        randomString += randomChar;
    }

    return randomString;
}

void clear_file_contents(string fileName){
    ofstream f(fileName, ios::out | ios::trunc);
    f.close();
}

TEST_CASE("Testy jednostkowe rejestracji i logowania") {
    // 1. Resetujemy plik sesji na starcie kazdej sekcji
    clear_file_contents("sesje.csv");

    // 2. Definiujemy wylacznie pliki testowe
    string accountFile = "konta_test.csv";
    string subjectFile = "przedmioty_test.csv";
    string indexFile = "indeksy_test.csv";

    // 3. Przygotowujemy staly, czysty stan poczatkowy kont dla kazdej sekcji
    ofstream kOut(accountFile, ios::out | ios::trunc);
    kOut << "Student,Adriana,Adriana123,Grupa_1A,123456\n";
    kOut.close();

    // 4. Czyscimy plik przedmiotow
    clear_file_contents(subjectFile);

    // 5. Inicjalizacja systemu na plikach testowych
    SystemEdukacyjny st("SystemTestowy");
    st.wczytajPrzedmiotyIKontaZPliku(accountFile, subjectFile);

    SECTION("Logowanie z poprawnym haslem - musi zalogowac sie") {
        REQUIRE(st.logowanie("Adriana", "Adriana123", "indeksy_test.csv") == true);
    }

    SECTION("Logowanie z blednym haslem - musi nie zalogowac sie") {
        REQUIRE(st.logowanie("Adriana", "jhgjhg", "indeksy_test.csv") == false);
    }

    SECTION("Rejestracja nowego studenta z poprawnym i wolnym indeksem") {
        // Przygotowujemy czysty, staly plik indeksow testowych
        ofstream iOut(indexFile, ios::out | ios::trunc);
        iOut << "999999,STUDENT,brak\n";
        iOut.close();

        string losowyLogin = generateRandomString();

        // Symulacja cin: typ -> powtorzenie hasla -> indeks -> grupa
        string simulatedInput = "student\nHaslo123\n999999\nGrupa_1A\n";
        istringstream iss(simulatedInput);
        streambuf* oldCin = cin.rdbuf(iss.rdbuf());

        bool result = st.rejestracja(losowyLogin, "Haslo123", "indeksy_test.csv");

        cin.rdbuf(oldCin);

        REQUIRE(result == true);
        REQUIRE(st.logowanie(losowyLogin, "Haslo123", "indeksy_test.csv") == true);
    }

    SECTION("Proba rejestracji na login, ktory juz istnieje w systemie") {
        // Adriana juz istnieje w konta_test.csv (zapisalismy to na gorze TEST_CASE)
        bool result = st.rejestracja("Adriana", "InneHaslo123", "indeksy_test.csv");

        REQUIRE(result == false);
    }

    SECTION("Proba rejestracji z indeksem, ktory jest juz zajety") {
        // Przygotowujemy plik indeksow testowych z zajetym indeksem
        ofstream iOut(indexFile, ios::out | ios::trunc);
        iOut << "123456,Student,Adriana\n";
        iOut.close();

        string losowyLogin = generateRandomString();

        // Symulacja cin: typ -> powtorzenie hasla -> zajety indeks
        string simulatedInput = "student\nHaslo123\n123456\n";
        istringstream iss(simulatedInput);
        streambuf* oldCin = cin.rdbuf(iss.rdbuf());

        bool result = st.rejestracja(losowyLogin, "Haslo123", "indeksy_test.csv");

        cin.rdbuf(oldCin);

        // Naprawiony blad ze zmienna wynikRejestracji -> teraz jest result
        REQUIRE(result == false);
    }
}

int main() {
    srand(time(nullptr));
    int result = Catch::Session().run();
    system("pause");
    return result;
}
