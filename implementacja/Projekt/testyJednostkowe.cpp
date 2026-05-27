#include "catch.hpp"
#include "StronaInternetowa.h"

TEST_CASE("Logowanie"){
    StronaInternetowa st("adres", "nazwa");

    REQUIRE(st.logowanie("Adriana", "Adriana123", "192.220.105.100") == true);
}
