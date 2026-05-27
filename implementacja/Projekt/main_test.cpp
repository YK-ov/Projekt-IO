#define CATCH_CONFIG_RUNNER
#include "catch.hpp"
#include "StronaInternetowa.h"
#include "Student.h"

TEST_CASE("Logowanie z poprawnym haslem, musi zalogowac sie") {
    StronaInternetowa st("adres", "nazwa");
    Sesja* sesja = new Sesja("151.51.249.120");
    st.attachSesja(sesja);
    st.wczytajDaneZPliku("konta.csv", "przedmioty.csv");
    REQUIRE(st.logowanie("Adriana", "Adriana123", "151.51.249.120") == true);

    delete sesja;
}

TEST_CASE("Logowanie z blednym haslem, musi nie zalogowac sie") {
    StronaInternetowa st("adres", "nazwa");
    Sesja* sesja = new Sesja("151.51.249.120");
    st.attachSesja(sesja);
    st.attachSesja(sesja);
    st.wczytajDaneZPliku("konta.csv", "przedmioty.csv");
    REQUIRE(st.logowanie("Adriana", "jhgjhg", "151.51.249.120") == false);

    delete sesja;
}

int main() {
    int result = Catch::Session().run();
    system("pause");
    return result;
}
