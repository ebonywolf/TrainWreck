#pragma once


#include <json/json.h>
#include <fstream>
#include <unordered_map>

namespace pg{

enum Type {
    INVALID,STRING,DOUBLE,BOOLEAN, CLASS,ARRAY
};


struct Settings {
    std::string strName = "std::string";
    std::string boolName = "bool";
    std::string doubleName = "double";
    std::string className = "struct";
    std::string listName = "std::vector";
    std::string listSuffix = "_list";
    std::string readOperator = "void operator=(const Json::Value& val)";
    std::string writeOperator = "operator Json::Value() const ";

    std::string jsonName = "Json::Value";
    std::string jsonToVector = "";
    std::string jsonVariable = "val";

    std::string jsonToString = ".asString()";

    std::string jsonToDouble = ".asDouble()";
    std::string jsonToBoolean = ".asBool()";


    std::string fromJsonMethod = "template <class T>\n\
        void fromJson(T& t , const Json::Value& other){\n\
            t = other;\n\
        }\n\
        template <>\n\
        void fromJson(double& t, const Json::Value& other){\n\
            t = other.asDouble();\n\
        }\n\
        template <>\n\
        void fromJson(std::string& t, const Json::Value& other){\n\
            t = other.asString();\n\
        }\n\
        template <>\n\
        void fromJson(bool& t, const Json::Value& other){\n\
            t = other.asBool();\n\
        }\n\
        template <class T>\n\
        void fromJson(std::vector<T>& t, const Json::Value& other){\n\
            if(!other.isArray())\n\
                throw std::runtime_error(\"Not array object\");\n\
            std::vector<T> vetor;\n\
            for( auto& jso:other ) {\n\
                T val;\n\
                fromJson(val, jso);\n\
                vetor.push_back(val);\n\
            }\n\
            t.swap(vetor);\n\
        }\n"
        ;

    std::string toJsonMethod = " template <class T>\n\
        void toJson(const T& t,  Json::Value& other ){\n\
            other = t;\n\
        }\n\
        template <class T>\n\
        void toJson(const std::vector<T>& lista,  Json::Value& other ){\n\
            other = Json::Value(Json::arrayValue);\n\
            for(auto& x: lista)\n\
            {\n\
                Json::Value alce;\n\
                toJson(x, alce);\n\
                other.append(alce);\n\
            }\n\
        }\n"
        ;



};



struct DataCreator {
    DataCreator(Json::Value file){
        data = file;
    }
    DataCreator(std::string file){
        std::fstream input(file);
        if(!input.is_open()){
            throw std::runtime_error(std::string("unable to open file: ")+file);
        }
        input>>data;
    }

    void writeAsHeader(std::ostream& out);

private:
    std::unordered_map<std::string,Json::Value> allObjects;

    Json::Value data;
};

}
