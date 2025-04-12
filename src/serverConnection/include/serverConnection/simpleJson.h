// simpleJson.h

#ifndef SIMPLEJSON_H
#define SIMPLEJSON_H

#include <string>
#include <tablut/state.h>
#include <tablut/common.h>

class SimpleJson {
public:

    static std::string toJson(const std::string& name);

    /*
    Mossa inviata come: (con turno)
    //{"from":"g3","to":"g6","turn":"WHITE"}
    */

    static std::string toJson(const Move&, const Turn&);
    
    /*
    stato inviato come:

    //{"board":[["EMPTY","EMPTY","EMPTY","BLACK","BLACK","BLACK","EMPTY","EMPTY","EMPTY"],["EMPTY","EMPTY","EMPTY","EMPTY","BLACK","EMPTY","EMPTY","EMPTY","EMPTY"],["EMPTY","EMPTY","EMPTY","EMPTY","WHITE","EMPTY","EMPTY","EMPTY","EMPTY"],["BLACK","EMPTY","EMPTY","EMPTY","WHITE","EMPTY","EMPTY","EMPTY","BLACK"],["BLACK","BLACK","WHITE","WHITE","KING","WHITE","WHITE","BLACK","BLACK"],["BLACK","EMPTY","EMPTY","EMPTY","WHITE","EMPTY","EMPTY","EMPTY","BLACK"],["EMPTY","EMPTY","EMPTY","EMPTY","WHITE","EMPTY","EMPTY","EMPTY","EMPTY"],["EMPTY","EMPTY","EMPTY","EMPTY","BLACK","EMPTY","EMPTY","EMPTY","EMPTY"],["EMPTY","EMPTY","EMPTY","BLACK","BLACK","BLACK","EMPTY","EMPTY","EMPTY"]],"turn":"WHITE"}

    per qualche motivo la casella centrale, se non è presente il king, è indicata come trono:
    //{"board":[["EMPTY","BLACK","EMPTY","EMPTY","EMPTY","EMPTY","EMPTY","BLACK","EMPTY"],["EMPTY","BLACK","EMPTY","EMPTY","EMPTY","EMPTY","EMPTY","EMPTY","EMPTY"],["BLACK","EMPTY","EMPTY","EMPTY","EMPTY","EMPTY","EMPTY","EMPTY","EMPTY"],["EMPTY","EMPTY","EMPTY","EMPTY","EMPTY","EMPTY","EMPTY","WHITE","EMPTY"],["EMPTY","EMPTY","EMPTY","EMPTY","THRONE","WHITE","EMPTY","EMPTY","EMPTY"],["EMPTY","EMPTY","BLACK","EMPTY","BLACK","EMPTY","EMPTY","BLACK","EMPTY"],["EMPTY","EMPTY","EMPTY","EMPTY","EMPTY","BLACK","WHITE","EMPTY","WHITE"],["EMPTY","BLACK","EMPTY","EMPTY","EMPTY","WHITE","EMPTY","KING","BLACK"],["BLACK","EMPTY","EMPTY","EMPTY","EMPTY","BLACK","WHITE","EMPTY","EMPTY"]],"turn":"BLACK"}

    quindi dovrei, prima di trasformala, sostituire la stringa "THRONE" con "EMPTY"
    */
    
    
    static State fromJson(std::string json);

};


#endif // SIMPLEJSON_H