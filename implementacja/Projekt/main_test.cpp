#define CATCH_CONFIG_RUNNER
#include "catch.hpp"
#include "SystemEdukacyjny.h"
#include "Student.h"
#include <ctime>
#include <fstream>
#include <sstream>

using namespace std;

string generateRandomString(){
    int size = 5 + (rand() % 11);
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
    string indexFile = "indeksy_test.csv";

    ofstream kOut(accountFile, ios::out | ios::trunc);
    kOut << "Student,Adriana,Adriana123,Grupa_1A,123456\n";
    kOut.close();

    clear_file_contents(subjectFile);

    SystemEdukacyjny st("SystemTestowy");
    st.wczytajPrzedmiotyIKontaZPliku(accountFile, subjectFile);

    SECTION("Logowanie z poprawnym haslem - musi zalogowac sie") {
        REQUIRE(st.logowanie("Adriana", "Adriana123", "indeksy_test.csv", "konta_test.csv", "przedmioty_test.csv") == true);
    }

    SECTION("Logowanie z blednym haslem - musi nie zalogowac sie") {
        REQUIRE(st.logowanie("Adriana", "jhgjhg", "indeksy_test.csv", "konta_test.csv", "przedmioty_test.csv") == false);
    }

    SECTION("Rejestracja nowego studenta z poprawnym i wolnym indeksem") {
        ofstream iOut(indexFile, ios::out | ios::trunc);
        iOut << "999999,STUDENT,brak\n";
        iOut.close();

        string losowyLogin = generateRandomString();

        string simulatedInput = "student\nHaslo123\n999999\nGrupa_1A\n";
        istringstream iss(simulatedInput);
        streambuf* oldCin = cin.rdbuf(iss.rdbuf());

        bool result = st.rejestracja(losowyLogin, "Haslo123", "indeksy_test.csv", "konta_test.csv", "przedmioty_test.csv");

        cin.rdbuf(oldCin);

        REQUIRE(result == true);
        REQUIRE(st.logowanie(losowyLogin, "Haslo123", "indeksy_test.csv", "konta_test.csv", "przedmioty_test.csv") == true);
    }

    SECTION("Proba rejestracji na login, ktory juz istnieje w systemie") {
        bool result = st.rejestracja("Adriana", "InneHaslo123", "indeksy_test.csv", "konta_test.csv", "przedmioty_test.csv");

        REQUIRE(result == false);
    }

    SECTION("Proba rejestracji z indeksem, ktory jest juz zajety") {
        ofstream iOut(indexFile, ios::out | ios::trunc);
        iOut << "123456,Student,Adriana\n";
        iOut.close();

        string losowyLogin = generateRandomString();

        string simulatedInput = "student\nHaslo123\n123456\n";
        istringstream iss(simulatedInput);
        streambuf* oldCin = cin.rdbuf(iss.rdbuf());

        bool result = st.rejestracja(losowyLogin, "Haslo123", "indeksy_test.csv", "konta_test.csv", "przedmioty_test.csv");

        cin.rdbuf(oldCin);

        REQUIRE(result == false);
    }
SECTION("Wykladowca dodaje zwalidowany material, a przypisany student moze go pozyskac") {
    ofstream kOut(accountFile, ios::out | ios::trunc);
    kOut << "Wykladowca,Jan,Haslo123,0,999888\n";
    kOut << "Student,Adam,Haslo321,Grupa_1A,111222\n";
    kOut.close();

    ofstream pOut(subjectFile, ios::out | ios::trunc);
    pOut << "Matematyka,BrakOpisu,BrakKontaktu,Grupa_1A,WYKLADOWCY,Jan,STUDENCI,Adam,MATERIALY,brak\n";
    pOut.close();

    SystemEdukacyjny stTest("SystemTestowy");
    stTest.wczytajPrzedmiotyIKontaZPliku(accountFile, subjectFile);

    string teacherInput = "Material\nMatematyka\nWazne Notatki z Cial\nnotatki.pdf\nZakoncz dzialanie\n";
    istringstream issTeacher(teacherInput);
    streambuf* oldCin = cin.rdbuf(issTeacher.rdbuf());

    bool wykladowcaResult = stTest.wykonajAkcjeUzytkownika("Jan", accountFile, subjectFile);
    REQUIRE(wykladowcaResult == true);

    string studentInput = "Pozyskac\nMatematyka\nWykladowcy\nWazne Notatki z Cial\n";
    istringstream issStudent(studentInput);
    cin.rdbuf(issStudent.rdbuf());

    ostringstream capturedCout;
    streambuf* oldCout = cout.rdbuf(capturedCout.rdbuf());

    stTest.wykonajAkcjeUzytkownika("Adam", accountFile, subjectFile);

    cin.rdbuf(oldCin);
    cout.rdbuf(oldCout);

    string programScreenOutput = capturedCout.str();

    REQUIRE(programScreenOutput.find("Wazne Notatki z Cial") != string::npos);
    REQUIRE(programScreenOutput.find("notatki.pdf") != string::npos);

    ifstream pIn(subjectFile);
    string fileContent((istreambuf_iterator<char>(pIn)), istreambuf_iterator<char>());
    pIn.close();

    REQUIRE(fileContent.find("Wazne Notatki z Cial") != string::npos);
    REQUIRE(fileContent.find("notatki.pdf") != string::npos);
}

SECTION("Student dodaje sugerowany material do weryfikacji dla wykladowcy") {
    ofstream kOut(accountFile, ios::out | ios::trunc);
    kOut << "Wykladowca,Jan,Haslo123,0,999888\n";
    kOut << "Student,Adam,Haslo321,Grupa_1A,111222\n";
    kOut.close();

    ofstream pOut(subjectFile, ios::out | ios::trunc);
    pOut << "Matematyka,BrakOpisu,BrakKontaktu,Grupa_1A,WYKLADOWCY,Jan,STUDENCI,Adam,MATERIALY,brak\n";
    pOut.close();

    SystemEdukacyjny stTest("SystemTestowy");
    stTest.wczytajPrzedmiotyIKontaZPliku(accountFile, subjectFile);

    string studentInput = "Dodac\nMatematyka\nMoja Propozycja\nsciaga.pdf\n";
    istringstream issStudent(studentInput);
    streambuf* oldCin = cin.rdbuf(issStudent.rdbuf());

    ostringstream capturedCout;
    streambuf* oldCout = cout.rdbuf(capturedCout.rdbuf());

    bool studentResult = stTest.wykonajAkcjeUzytkownika("Adam", accountFile, subjectFile);

    cin.rdbuf(oldCin);
    cout.rdbuf(oldCout);

    REQUIRE(studentResult == true);

    string programScreenOutput = capturedCout.str();
    REQUIRE(programScreenOutput.find("Sugestia materialu zostala wyslana") != string::npos);

    Przedmiot* matPrzedmiot = stTest.getPrzedmiot("Matematyka");
    REQUIRE(matPrzedmiot != nullptr);

    Material* sprawdzanyMaterial = matPrzedmiot->getMaterialPoTytule("Moja Propozycja");
    REQUIRE(sprawdzanyMaterial != nullptr);
    REQUIRE(sprawdzanyMaterial->getMaterialOdWykladowcy() == false);
    REQUIRE(sprawdzanyMaterial->getCzyJestZweryfikowany() == false);
    REQUIRE(sprawdzanyMaterial->getCzyJestDodanyPrzezAdmina() == false);

    ifstream pIn(subjectFile);
    string fileContent((istreambuf_iterator<char>(pIn)), istreambuf_iterator<char>());
    pIn.close();

    REQUIRE(fileContent.find("Moja Propozycja") != string::npos);
    REQUIRE(fileContent.find("sciaga.pdf") != string::npos);
}

SECTION("Wykladowca dodaje nowy przedmiot i przypisuje dodatkowego wykladowce oraz studentow") {
    ofstream kOut(accountFile, ios::out | ios::trunc);
    kOut << "Wykladowca,Jan,Haslo123,0,999888\n";
    kOut << "Wykladowca,Tomasz,Haslo456,0,999777\n";
    kOut << "Student,Adam,Haslo321,Grupa_1A,111222\n";
    kOut.close();

    clear_file_contents(subjectFile);

    SystemEdukacyjny stTest("SystemTestowy");
    stTest.wczytajPrzedmiotyIKontaZPliku(accountFile, subjectFile);

    string teacherInput = "Przedmiot\nFizyka\njan@uczelnia.pl\nPodstawy fizyki klasycznej\nGrupa_1A\nTak\nTomasz\nNie\nZatwierdz\n";
    istringstream issTeacher(teacherInput);
    streambuf* oldCin = cin.rdbuf(issTeacher.rdbuf());

    ostringstream capturedCout;
    streambuf* oldCout = cout.rdbuf(capturedCout.rdbuf());

    bool teacherResult = stTest.wykonajAkcjeUzytkownika("Jan", accountFile, subjectFile);

    cin.rdbuf(oldCin);
    cout.rdbuf(oldCout);

    REQUIRE(teacherResult == true);

    string programScreenOutput = capturedCout.str();
    REQUIRE(programScreenOutput.find("Przedmiot Fizyka zostal pomyslnie dodany do systemu") != string::npos);

    Przedmiot* nowyPrzedmiot = stTest.getPrzedmiot("Fizyka");
    REQUIRE(nowyPrzedmiot != nullptr);
    REQUIRE(nowyPrzedmiot->getOpis() == "Podstawy fizyki klasycznej");
    REQUIRE(nowyPrzedmiot->getKontakt() == "jan@uczelnia.pl");
    REQUIRE(nowyPrzedmiot->getGrupa() == "Grupa_1A");

    REQUIRE(nowyPrzedmiot->getWykladowca("Jan") != nullptr);
    REQUIRE(nowyPrzedmiot->getWykladowca("Tomasz") != nullptr);

    REQUIRE(nowyPrzedmiot->getStudent("Adam") != nullptr);

    ifstream pIn(subjectFile);
    string fileContent((istreambuf_iterator<char>(pIn)), istreambuf_iterator<char>());
    pIn.close();

    REQUIRE(fileContent.find("Fizyka") != string::npos);
    REQUIRE(fileContent.find("jan@uczelnia.pl") != string::npos);
    REQUIRE(fileContent.find("Grupa_1A") != string::npos);
    REQUIRE(fileContent.find("Jan") != string::npos);
    REQUIRE(fileContent.find("Tomasz") != string::npos);
    REQUIRE(fileContent.find("Adam") != string::npos);
}

SECTION("Wykladowca pozytywnie weryfikuje sugestie studenta (akceptacja)") {
        cin.clear();
        ofstream kOut(accountFile, ios::out | ios::trunc);
        kOut << "Wykladowca,Jan,Haslo123,0,999888\n";
        kOut << "Student,Adam,Haslo321,Grupa_1A,111222\n";
        kOut << "Admin,TomaszAdmin,Admin123,0\n";
        kOut.close();

        ofstream pOut(subjectFile, ios::out | ios::trunc);
        pOut << "Matematyka,BrakOpisu,BrakKontaktu,Grupa_1A,WYKLADOWCY,Jan,STUDENCI,Adam,MATERIALY,brak\n";
        pOut.close();

        SystemEdukacyjny stTest("SystemTestowy");
        stTest.wczytajPrzedmiotyIKontaZPliku(accountFile, subjectFile);

        string studentInput = "Dodac\nMatematyka\nSugerowany Skrypt\nanaliza.pdf\n";
        istringstream issStudent(studentInput);
        streambuf* oldCin = cin.rdbuf(issStudent.rdbuf());
        stTest.wykonajAkcjeUzytkownika("Adam", accountFile, subjectFile);

        string teacherInput = "Sprawdz\nTak\nTak\nTak\nZakoncz dzialanie\n";
        istringstream issTeacher(teacherInput);
        cin.rdbuf(issTeacher.rdbuf());

        ostringstream capturedCout;
        streambuf* oldCout = cout.rdbuf(capturedCout.rdbuf());

        bool teacherResult = stTest.wykonajAkcjeUzytkownika("Jan", accountFile, subjectFile);

        cin.rdbuf(oldCin);
        cout.rdbuf(oldCout);

        REQUIRE(teacherResult == false);
        string programScreenOutput = capturedCout.str();
        REQUIRE(programScreenOutput.find("Material o tytule Sugerowany Skrypt zostal zweryfikowany") != string::npos);

        Przedmiot* matPrzedmiot = stTest.getPrzedmiot("Matematyka");
        REQUIRE(matPrzedmiot != nullptr);

        Material* sprawdzanyMaterial = matPrzedmiot->getMaterialPoTytule("Sugerowany Skrypt");
        REQUIRE(sprawdzanyMaterial != nullptr);
        REQUIRE(sprawdzanyMaterial->getCzyJestZweryfikowany() == true);
    }

    SECTION("Wykladowca negatywnie weryfikuje sugestie studenta (odrzucenie i usuniecie)") {
        cin.clear();
        ofstream kOut(accountFile, ios::out | ios::trunc);
        kOut << "Wykladowca,Jan,Haslo123,0,999888\n";
        kOut << "Student,Adam,Haslo321,Grupa_1A,111222\n";
        kOut << "Admin,TomaszAdmin,Admin123,0\n";
        kOut.close();

        ofstream pOut(subjectFile, ios::out | ios::trunc);
        pOut << "Matematyka,BrakOpisu,BrakKontaktu,Grupa_1A,WYKLADOWCY,Jan,STUDENCI,Adam,MATERIALY,brak\n";
        pOut.close();

        SystemEdukacyjny stTest("SystemTestowy");
        stTest.wczytajPrzedmiotyIKontaZPliku(accountFile, subjectFile);

        string studentInput = "Dodac\nMatematyka\nSlaba Sciaga\nsciaga.pdf\n";
        istringstream issStudent(studentInput);
        streambuf* oldCin = cin.rdbuf(issStudent.rdbuf());
        stTest.wykonajAkcjeUzytkownika("Adam", accountFile, subjectFile);

        string teacherInput = "Sprawdz\nNie\nTak\nTak\nZakoncz dzialanie\n";
        istringstream issTeacher(teacherInput);
        cin.rdbuf(issTeacher.rdbuf());

        ostringstream capturedCout;
        streambuf* oldCout = cout.rdbuf(capturedCout.rdbuf());

        bool teacherResult = stTest.wykonajAkcjeUzytkownika("Jan", accountFile, subjectFile);

        cin.rdbuf(oldCin);
        cout.rdbuf(oldCout);

        REQUIRE(teacherResult == false);

        string programScreenOutput = capturedCout.str();
        REQUIRE(programScreenOutput.find("Sugestia zostala odrzucona, nie wszystkie kryteria sa spelnione") != string::npos);

        Przedmiot* matPrzedmiot = stTest.getPrzedmiot("Matematyka");
        REQUIRE(matPrzedmiot != nullptr);

        Material* sprawdzanyMaterial = matPrzedmiot->getMaterialPoTytule("Slaba Sciaga");
        REQUIRE(sprawdzanyMaterial == nullptr);
    }

    SECTION("Admin automatycznie przechodzi do dodawania materialu, gdy licznik > 0 i zatwierdza go") {
        cin.clear();
        ofstream kOut(accountFile, ios::out | ios::trunc);
        kOut << "Wykladowca,Jan,Haslo123,0,999888\n";
        kOut << "Student,Adam,Haslo321,Grupa_1A,111222\n";
        kOut << "Admin,TomaszAdmin,Admin123,0\n";
        kOut.close();

        ofstream pOut(subjectFile, ios::out | ios::trunc);
        pOut << "Matematyka,BrakOpisu,BrakKontaktu,Grupa_1A,WYKLADOWCY,Jan,STUDENCI,Adam,MATERIALY,brak\n";
        pOut.close();

        SystemEdukacyjny stTest("SystemTestowy");
        stTest.wczytajPrzedmiotyIKontaZPliku(accountFile, subjectFile);

        string studentInput = "Dodac\nMatematyka\nSkrypt do zatwierdzenia\nanaliza.pdf\n";
        istringstream issStudent(studentInput);
        streambuf* oldCin = cin.rdbuf(issStudent.rdbuf());
        stTest.wykonajAkcjeUzytkownika("Adam", accountFile, subjectFile);

        string teacherInput = "Sprawdz\nTak\nTak\nTak\nZakoncz dzialanie\n";
        istringstream issTeacher(teacherInput);
        cin.rdbuf(issTeacher.rdbuf());
        stTest.wykonajAkcjeUzytkownika("Jan", accountFile, subjectFile);

        string adminInput = "tak\n";
        istringstream issAdmin(adminInput);
        cin.rdbuf(issAdmin.rdbuf());

        ostringstream capturedCout;
        streambuf* oldCout = cout.rdbuf(capturedCout.rdbuf());

        bool adminResult = stTest.wykonajAkcjeUzytkownika("TomaszAdmin", accountFile, subjectFile);

        cin.rdbuf(oldCin);
        cout.rdbuf(oldCout);

        REQUIRE(adminResult == true);

        Przedmiot* matPrzedmiot = stTest.getPrzedmiot("Matematyka");
        REQUIRE(matPrzedmiot != nullptr);
        Material* sprawdzanyMaterial = matPrzedmiot->getMaterialPoTytule("Skrypt do zatwierdzenia");
        REQUIRE(sprawdzanyMaterial != nullptr);
        REQUIRE(sprawdzanyMaterial->getCzyJestZweryfikowany() == true);
        REQUIRE(sprawdzanyMaterial->getCzyJestDodanyPrzezAdmina() == true);

        string programScreenOutput = capturedCout.str();
        REQUIRE(programScreenOutput.find("zostal dodany do systemu") != string::npos);
    }

    SECTION("Admin nie ma materialow (licznik == 0) i decyduje sie zakonczyc dzialanie") {
        cin.clear();
        ofstream kOut(accountFile, ios::out | ios::trunc);
        kOut << "Admin,TomaszAdmin,Admin123,0\n";
        kOut.close();

        ofstream pOut(subjectFile, ios::out | ios::trunc);
        pOut << "Matematyka,BrakOpisu,BrakKontaktu,Grupa_1A,WYKLADOWCY,brak,STUDENCI,brak,MATERIALY,brak\n";
        pOut.close();

        SystemEdukacyjny stTest("SystemTestowy");
        stTest.wczytajPrzedmiotyIKontaZPliku(accountFile, subjectFile);

        string adminInput = "Zakoncz dzialanie\n";
        istringstream issAdmin(adminInput);
        streambuf* oldCin = cin.rdbuf(issAdmin.rdbuf());

        ostringstream capturedCout;
        streambuf* oldCout = cout.rdbuf(capturedCout.rdbuf());

        bool adminResult = stTest.wykonajAkcjeUzytkownika("TomaszAdmin", accountFile, subjectFile);

        cin.rdbuf(oldCin);
        cout.rdbuf(oldCout);

        REQUIRE(adminResult == false);
        string programScreenOutput = capturedCout.str();
        REQUIRE(programScreenOutput.find("Trwa zamykanie programu...") != string::npos);
    }

    SECTION("Admin nie ma materialow (licznik == 0), podaje bledny input, a potem wybiera 'Dodac'") {
        cin.clear();
        ofstream kOut(accountFile, ios::out | ios::trunc);
        kOut << "Admin,TomaszAdmin,Admin123,0\n";
        kOut.close();

        ofstream pOut(subjectFile, ios::out | ios::trunc);
        pOut << "Matematyka,BrakOpisu,BrakKontaktu,Grupa_1A,WYKLADOWCY,brak,STUDENCI,brak,MATERIALY,brak\n";
        pOut.close();

        SystemEdukacyjny stTest("SystemTestowy");
        stTest.wczytajPrzedmiotyIKontaZPliku(accountFile, subjectFile);

        string adminInput = "ZlaKomenda\nDodac\n";
        istringstream issAdmin(adminInput);
        streambuf* oldCin = cin.rdbuf(issAdmin.rdbuf());

        ostringstream capturedCout;
        streambuf* oldCout = cout.rdbuf(capturedCout.rdbuf());

        bool adminResult = stTest.wykonajAkcjeUzytkownika("TomaszAdmin", accountFile, subjectFile);

        cin.rdbuf(oldCin);
        cout.rdbuf(oldCout);

        REQUIRE(adminResult == true);
        string programScreenOutput = capturedCout.str();

        REQUIRE(programScreenOutput.find("Nieprawidlowy input, prosze sprobowac ponownie") != string::npos);
        REQUIRE(programScreenOutput.find("Nie masz w tej chwili zweryfikowanych materialow do dodania") != string::npos);
    }

    SECTION("INTEGRACJA: Pelny przeplyw materialu (Student -> Wykladowca -> Admin) z persystencja pliku") {
    cin.clear();
    string accountFile = "konta_integracja.csv";
    string subjectFile = "przedmioty_integracja.csv";

    ofstream kOut(accountFile, ios::out | ios::trunc);
    kOut << "Student,Kamil,Haslo123,Grupa_1A,111222\n";
    kOut << "Wykladowca,DrJan,Haslo123,0,999888\n";
    kOut << "Admin,GlownyAdmin,Admin123,0\n";
    kOut.close();

    ofstream pOut(subjectFile, ios::out | ios::trunc);
    pOut << "Programowanie,BrakOpisu,Kontakt,Grupa_1A,WYKLADOWCY,DrJan,STUDENCI,Kamil,MATERIALY,brak\n";
    pOut.close();

    SystemEdukacyjny sys1("Instancja_1");
    sys1.wczytajPrzedmiotyIKontaZPliku(accountFile, subjectFile);

    string studentIn = "Dodac\nProgramowanie\nProjekt C++\nmain.cpp\n";
    istringstream issStudent(studentIn);
    streambuf* oldCin = cin.rdbuf(issStudent.rdbuf());
    sys1.wykonajAkcjeUzytkownika("Kamil", accountFile, subjectFile);

    string teacherIn = "Sprawdz\nTak\nTak\nTak\nZakoncz dzialanie\n";
    istringstream issTeacher(teacherIn);
    cin.rdbuf(issTeacher.rdbuf());
    sys1.wykonajAkcjeUzytkownika("DrJan", accountFile, subjectFile);

    string adminIn = "tak\n";
    istringstream issAdmin(adminIn);
    cin.rdbuf(issAdmin.rdbuf());
    sys1.wykonajAkcjeUzytkownika("GlownyAdmin", accountFile, subjectFile);

    cin.rdbuf(oldCin);

    SystemEdukacyjny sys2("Instancja_2");
    sys2.wczytajPrzedmiotyIKontaZPliku(accountFile, subjectFile);

    Przedmiot* p2 = sys2.getPrzedmiot("Programowanie");
    REQUIRE(p2 != nullptr);

    Material* m2 = p2->getMaterialPoTytule("Projekt C++");
    REQUIRE(m2 != nullptr);

    INFO("Material powinien byc w pelni zweryfikowany i dodany przez admina");
    REQUIRE(m2->getCzyJestZweryfikowany() == true);
    REQUIRE(m2->getCzyJestDodanyPrzezAdmina() == true);
    }

    SECTION("INTEGRACJA: Pelny cykl zycia konta (Modyfikacja indeksow -> Rejestracja -> Logowanie w nowej instancji)") {
    cin.clear();
    string indexFile = "indeksy_integracja.csv";
    string accountFile = "konta_integracja.csv";
    string subjectFile = "przedmioty_integracja.csv";

    ofstream iOut(indexFile, ios::out | ios::trunc);
    iOut << "555888,STUDENT,brak\n";
    iOut.close();

    clear_file_contents(accountFile);
    clear_file_contents(subjectFile);

    SystemEdukacyjny sys1("Instancja_Rejestracji");
    sys1.wczytajPrzedmiotyIKontaZPliku(accountFile, subjectFile);

    string registerInput = "student\nHasloBezpieczne1\n555888\nGrupa_1A\n";
    istringstream issRegister(registerInput);
    streambuf* oldCin = cin.rdbuf(issRegister.rdbuf());

    bool czyRejestracjaUdana = sys1.rejestracja("NowyStudent", "HasloBezpieczne1", indexFile, accountFile, subjectFile);
    REQUIRE(czyRejestracjaUdana == true);

    cin.rdbuf(oldCin);

    ifstream iIn(indexFile);
    string indexContent((istreambuf_iterator<char>(iIn)), istreambuf_iterator<char>());
    iIn.close();
    REQUIRE(indexContent.find("555888") != string::npos);
    REQUIRE(indexContent.find("NowyStudent") != string::npos);

    SystemEdukacyjny sys2("Instancja_Logowania");
    sys2.wczytajPrzedmiotyIKontaZPliku(accountFile, subjectFile);

    bool czyLogowanieUdana = sys2.logowanie("NowyStudent", "HasloBezpieczne1", indexFile, accountFile, subjectFile);

    INFO("Nowa instancja systemu powinna bez problemu zalogowac zarejestrowanego wczesniej studenta z pliku");
    REQUIRE(czyLogowanieUdana == true);
    }
}

SCENARIO("Funkcjonalnosc: Blokowanie rejestracji dla zajetego loginu") {

    GIVEN("System z jednym zarejestrowanym uzytkownikiem") {
        string accountFile = "konta_funkcjonalne.csv";
        string subjectFile = "przedmioty_funkcjonalne.csv";

        ofstream kOut(accountFile, ios::out | ios::trunc);
        kOut << "Student,Adriana,Adriana123,Grupa_1A,123456\n";
        kOut.close();
        clear_file_contents(subjectFile);

        SystemEdukacyjny st("SystemFunkcjonalny");
        st.wczytajPrzedmiotyIKontaZPliku(accountFile, subjectFile);

        WHEN("Inna osoba probuje zarejestrowac sie na ten sam login: 'Adriana'") {
            bool wynikRejestracji = st.rejestracja("Adriana", "InneHaslo123", "indeksy_test.csv", accountFile, subjectFile);

            THEN("System odrzuca rejestracje i nie pozwala stworzyc duplikatu") {
                REQUIRE(wynikRejestracji == false);
            }
        }
    }
}

SCENARIO("Funkcjonalnosc: Blokowanie logowania przy blednym hasle") {

    GIVEN("System z jednym zarejestrowanym studentem 'Adriana'") {
        string accountFile = "konta_funkcjonalne_logowanie.csv";
        string subjectFile = "przedmioty_funkcjonalne_logowanie.csv";

        ofstream kOut(accountFile, ios::out | ios::trunc);
        kOut << "Student,Adriana,Adriana123,Grupa_1A,123456\n";
        kOut.close();
        clear_file_contents(subjectFile);

        SystemEdukacyjny st("SystemFunkcjonalnyLogowanie");
        st.wczytajPrzedmiotyIKontaZPliku(accountFile, subjectFile);

        WHEN("Uzytkownik probuje zalogowac sie na login 'Adriana', ale podaje zle haslo 'ZleHaslo123'") {
            bool wynikLogowania = st.logowanie("Adriana", "ZleHaslo123", "indeksy_test.csv", accountFile, subjectFile);

            THEN("System odrzuca probe uwierzytelnienia") {
                REQUIRE(wynikLogowania == false);
            }
        }
    }
}

SCENARIO("Funkcjonalnosc: Blokowanie rejestracji dla indeksu spoza bazy uczelni", "[funkcjonalne]") {

    GIVEN("Pusta baza indeksow uczelni (brak wygenerowanych numerow)") {
        string indexFile = "indeksy_funkcjonalne_brak.csv";
        string accountFile = "konta_funkcjonalne_brak.csv";
        string subjectFile = "przedmioty_funkcjonalne_brak.csv";

        clear_file_contents(indexFile);
        clear_file_contents(accountFile);
        clear_file_contents(subjectFile);

        SystemEdukacyjny st("SystemFunkcjonalnyIndeks");
        st.wczytajPrzedmiotyIKontaZPliku(accountFile, subjectFile);

        WHEN("Student probuje zarejestrowac konto podajac zmyslony numer indeksu '999111'") {
            string simulatedInput = "student\nHaslo123\n999111\nGrupa_1A\n";
            istringstream iss(simulatedInput);
            streambuf* oldCin = cin.rdbuf(iss.rdbuf());

            bool wynikRejestracji = st.rejestracja("ZmyslonyStudent", "Haslo123", indexFile, accountFile, subjectFile);

            cin.rdbuf(oldCin);

            THEN("System odrzuca rejestracje z powodu braku uprawnionego indeksu") {
                REQUIRE(wynikRejestracji == false);
            }
        }
    }
}

SCENARIO("Funkcjonalnosc: Izolacja materialow pomiedzy grupami studenckimi", "[funkcjonalne]") {
    GIVEN("Przedmiot przypisany do Grupy_1A oraz Student przypisany do Grupy_2B") {
        string accountFile = "konta_f4.csv";
        string subjectFile = "przedmioty_f4.csv";

        ofstream kOut(accountFile, ios::out | ios::trunc);
        kOut << "Student,Krzysztof,Haslo123,Grupa_2B,111222\n";
        kOut.close();

        ofstream pOut(subjectFile, ios::out | ios::trunc);
        pOut << "Matematyka,Opis,Kontakt,Grupa_1A,WYKLADOWCY,Jan,STUDENCI,brak,MATERIALY,brak\n";
        pOut.close();

        SystemEdukacyjny st("SystemFunkcjonalny4");
        st.wczytajPrzedmiotyIKontaZPliku(accountFile, subjectFile);

        WHEN("Student z grupy 2B probuje pozyskac materialy z przedmiotu dla grupy 1A") {
            string studentInput = "Pozyskac\nMatematyka\n";
            istringstream issStudent(studentInput);
            streambuf* oldCin = cin.rdbuf(issStudent.rdbuf());

            ostringstream capturedCout;
            streambuf* oldCout = cout.rdbuf(capturedCout.rdbuf());

            st.wykonajAkcjeUzytkownika("Krzysztof", accountFile, subjectFile);

            cin.rdbuf(oldCin);
            cout.rdbuf(oldCout);

            THEN("System jawnie odmawia dostepu, poniewaz student nie jest przypisany do przedmiotu") {
                string output = capturedCout.str();

                REQUIRE(output.find("nie jestes przypisany do tego przedmiotu") != string::npos);
            }
        }
    }
}

SCENARIO("Funkcjonalnosc: Tworzenie nowego przedmiotu przez Wykladowce", "[funkcjonalne]") {
    GIVEN("Zalogowany wykladowca i pusta baza przedmiotow") {
        string accountFile = "konta_f5.csv";
        string subjectFile = "przedmioty_f5.csv";

        ofstream kOut(accountFile, ios::out | ios::trunc);
        kOut << "Wykladowca,DrInzynier,Haslo123,0,999888\n";
        kOut.close();

        clear_file_contents(subjectFile);

        SystemEdukacyjny st("SystemFunkcjonalny5");
        st.wczytajPrzedmiotyIKontaZPliku(accountFile, subjectFile);

        WHEN("Wykladowca tworzy przedmiot 'Logika' dla grupy 'Grupa_1A'") {
            string teacherInput = "Przedmiot\nLogika\nlogika@uczelnia.pl\nLogika matematyczna\nGrupa_1A\nNie\nNie\nZatwierdz\n";
            istringstream issTeacher(teacherInput);
            streambuf* oldCin = cin.rdbuf(issTeacher.rdbuf());

            bool wynikOk = st.wykonajAkcjeUzytkownika("DrInzynier", accountFile, subjectFile);
            cin.rdbuf(oldCin);

            THEN("System potwierdza sukces, a przedmiot staje sie dostepny w pamieci") {
                REQUIRE(wynikOk == true);

                Przedmiot* nowyPrzedmiot = st.getPrzedmiot("Logika");
                REQUIRE(nowyPrzedmiot != nullptr);
                REQUIRE(nowyPrzedmiot->getGrupa() == "Grupa_1A");
                REQUIRE(nowyPrzedmiot->getKontakt() == "logika@uczelnia.pl");
            }
        }
    }
}


int main() {
    srand(time(nullptr));
    int result = Catch::Session().run();
    system("pause");
    return result;
}
