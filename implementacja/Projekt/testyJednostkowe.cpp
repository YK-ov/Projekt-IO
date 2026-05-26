    #include <iostream>
    #include <string>
    #include <vector>
    #include "StronaInternetowa.h"
    #include "Konto.h"
    #include "Przedmiot.h"
    #include "Material.h"
    #include "testyJednostkowe.h"


void testyJednostkowe::testujLogowanie(){
    StronaInternetowa* st = new StronaInternetowa("adres", "nazwa");
    bool result = st->logowanie("user", "user123", "192.105.202");
    bool expectedResult = true;

    assertEqualsBool(result, true);


    delete st;
}

bool testyJednostkowe::assertEqualsBool(bool a, bool b){
    if (a == b){
        passedTestCounter++;
        return true;
    }
    else {
        failedTestCounter++;
        return false;
    }
}



















