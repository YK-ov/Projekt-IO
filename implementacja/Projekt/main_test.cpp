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
    clear_file_contents("sesje.csv");

    string accountFile = "konta_test.csv";
    string subjectFile = "przedmioty_test.csv";

    ofstream kOut(accountFile, ios::out | ios::trunc);
    kOut << "Student,Adriana,Adriana123,Grupa_1A,123456\n";
    kOut.close();

    clear_file_contents(subjectFile);

    SystemEdukacyjny st("SystemTestowy");
    st.wczytajPrzedmiotyIKontaZPliku(accountFile, subjectFile);

    SECTION("Logowanie z poprawnym haslem - musi zalogowac sie") {
        // Adriana istnieje i podajemy dobre haslo - brak interakcji z cin
        REQUIRE(st.logowanie("Adriana", "Adriana123") == true);
    }

    SECTION("Logowanie z blednym haslem - musi nie zalogowac sie") {
        // Adriana istnieje, ale podajemy zle haslo - zwroci false bez cin
        REQUIRE(st.logowanie("Adriana", "jhgjhg") == false);
    }

    SECTION("Rejestracja nowego studenta z poprawnym i wolnym indeksem") {
        // Zmieniono na indeksy.csv, bo tak masz w kodzie SystemEdukacyjny::rejestracja
        string indexFile = "indeksy.csv";
        ofstream iOut(indexFile, ios::out | ios::trunc);
        iOut << "999999,Student,brak\n";
        iOut.close();

        string losowyLogin = generateRandomString();

        // KROK 1: Przygotowanie danych dla cin (typ -> powtorzenie hasla -> indeks -> grupa)
        string simulatedInput = "student\nHaslo123\n999999\nGrupa_1A\n";
        istringstream iss(simulatedInput);
        streambuf* oldCin = cin.rdbuf(iss.rdbuf()); // Przekierowanie cin

        // KROK 2: Wywolanie funkcji z 2 argumentami (zgodnie z Twoja implementacja)
        bool result = st.rejestracja(losowyLogin, "Haslo123");

        // KROK 3: Przywrocenie oryginalnego cin
        cin.rdbuf(oldCin);

        REQUIRE(result == true);
        REQUIRE(st.logowanie(losowyLogin, "Haslo123") == true);
    }

    SECTION("Proba rejestracji na login, ktory juz istnieje w systemie") {
        // Adriana juz istnieje, wiec metoda rejestracja przerywa dzialanie
        // na samym poczatku (if sprawdzCzyUzytkownikIstnieje) i NIE wywoluje zadnego cin.
        bool result = st.rejestracja("Adriana", "InneHaslo123");

        REQUIRE(result == false);
    }

    SECTION("Proba rejestracji z indeksem, ktory jest juz zajety") {
        string indexFile = "indeksy.csv";
        ofstream iOut(indexFile, ios::out | ios::trunc);
        iOut << "123456,Student,Adriana\n";
        iOut.close();

        string losowyLogin = generateRandomString();

        // Symulujemy wpisanie: typ -> potwierdzenie hasla -> zajety indeks.
        // Metoda zweryfikujIndeks zwroci false i funkcja sie zakonczy,
        // wiec cin o grupe nigdy sie nie wykona.
        string simulatedInput = "student\nHaslo123\n123456\n";
        istringstream iss(simulatedInput);
        streambuf* oldCin = cin.rdbuf(iss.rdbuf());

        bool result = st.rejestracja(losowyLogin, "Haslo123");

        cin.rdbuf(oldCin);

        // Poprawiono wynikRejestracji na result
        REQUIRE(result == false);
    }
}

int main() {
    srand(time(nullptr)); // Inicjalizacja losowosci dla generateRandomString
    int result = Catch::Session().run();
    system("pause");
    return result;
}
