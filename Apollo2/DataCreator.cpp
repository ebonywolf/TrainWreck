
#include "DataCreator.h"
#include <iostream>
#include <sstream>
using namespace std;

#define _Json Json::Value

namespace pg{




Settings settings;


//_index(X)= "[\"#X\"]"
string _index(string s){
    string alce = "[\"";
    alce+=s;
    alce+="\"]";
    return alce;
}
string _upper(string s)
{
    if(s.size())
        s[0] =toupper(s[0]);
    return s;
}

string _lower(string s)
{
    if(s.size())
        s[0] = tolower(s[0]);
    return s;
}



struct WriteContext {
    WriteContext(ostream& out,Json::Value val,string name, int ident=0)
        :out(out),val(val),name(name), ident(ident)
    {
    }
public :
    void write()
    {
        writeIdent();
        switch (getType()) {
        case ARRAY:
            return writeArray();
        case CLASS:
            return writeClass();
        case STRING:
            return writeString();
        case DOUBLE:
            return writeDouble();
        case BOOLEAN:
        	return writeBoolean();
        default:
            throw std::runtime_error("type unspecified");
        }
    }
    Type getType()
    {
        return getType(val);
    }

    Type getType(Json::Value& val)
    {
        if(val.isArray())
            return ARRAY;
        if(val.isObject())
            return CLASS;
        if(val.isString())
            return STRING;
        if(val.isNumeric())
            return DOUBLE;
        if(val.isBool())
            return BOOLEAN;
        return INVALID;
    }
    std::string fromJsonConversion()
    {
        stringstream ss;
        ss<<"fromJson("<< getAttributeName()<<","<<settings.jsonVariable+_index(name)<<")";
        return ss.str();


        auto type = getType();
        switch ( type) {
            case ARRAY: {

                WriteContext context(out,val[0u],name,ident+1);
                std::string objName = context.getAttributeType();
                ss << "fromJson<"<<objName<<">"<<"("<<settings.jsonVariable<<_index(name)<<")";
                return ss.str();
            }
            case CLASS:
                return settings.jsonVariable+_index(name);

            case STRING:
                return settings.jsonVariable+_index(name)+settings.jsonToString;
            case DOUBLE:
                return  settings.jsonVariable+_index(name)+settings.jsonToDouble;
            case BOOLEAN:
                return  settings.jsonVariable+_index(name)+settings.jsonToBoolean;
            default:
                throw std::runtime_error("type unspecified");
        }
    }

    std::string getAttributeName(){
        std::string alce = _lower(name);
        if(getType()==ARRAY)alce+=settings.listSuffix;
        return alce;
    }
    std::string  getAttributeType(){
        switch (getType()) {
        case ARRAY:{
        	std::string base = settings.listName;
        	base+="<";
        	Json::Value first = val[0u];

			WriteContext context(out, first, getAttributeName(), ident);
			base+= context.getAttributeType() + ">";
			return base;
        }
        case CLASS:
            return _upper(name);
        case STRING:
            return settings.strName;
        case DOUBLE:
            return settings.doubleName;
        case BOOLEAN:
            return settings.boolName;
        default:
            throw std::runtime_error("type unspecified");
        }
    }


private:
    void writeIdent()
    {
        for (int i=0; i<ident; i++) {
            out<<"\t";
        }
    }

    void writeAttribuitions()
    {
        if(!val.isObject())
            throw "Fooo";

        for(auto& field: val.getMemberNames()) {
            WriteContext context(out, val[field], field, ident+1 );

            std::string attribution = context.fromJsonConversion();
            out<<"\t"<<attribution<<";\n";
            writeIdent();
        }
    }

    void writeToJsonCoversion()
    {
        if(!val.isObject())
            throw "Fooo";

        for(auto& field: val.getMemberNames()) {
            WriteContext context(out, val[field], field, ident+1 );

            std::string attributeName = settings.jsonVariable;
            std::string attribution = context.getAttributeName();

            out<<"\t"<<"toJson("<<attribution<<","<<settings.jsonVariable<<_index(field)<<");\n";
          //  attributeName<< _index(field)<<" = "<<attribution<<";\n";
            writeIdent();
        }
    }

    void writeConstructors()
    {
        if(!val.isObject())
            throw "Fooo";

        string objName = _upper(name);
        out<<objName<<"()=default;\n";

        writeIdent();
        out<<objName<<"(const "<<settings.jsonName<<"& val){\n";
        writeIdent();
        writeAttribuitions();
        out<<"\n";
        writeIdent();
        out<<"}\n";
    }

    void writeToJsonOperator()
    {
        if(!val.isObject())
            throw "Fooo";

        out<<settings.writeOperator<<"{\n";

        writeIdent();
        out<<"\t"<<settings.jsonName<<" "<<settings.jsonVariable<<";\n";

        writeIdent();
        writeToJsonCoversion();

        writeIdent();
        out<<"\treturn "<<settings.jsonVariable<<";\n";
        writeIdent();

        out<<"}\n";

    }
    void writeFromJsonOperator()
    {
        if(!val.isObject())
            throw "Fooo";

        out<<settings.readOperator<<"{\n";
        writeIdent();
        writeAttribuitions();
        out<<"\n";
        writeIdent();
        out<<"}\n";
    }

    void writeOperators()
    {
        writeToJsonOperator();
        out<<"\n";
        writeIdent();
        writeFromJsonOperator();
        out<<"\n";
    }
    template<class T>
    WriteContext& operator<<( const T& t)
    {
        out<<t;
        return *this;
    }

    void writeString()
    {
        out<<settings.strName<<" "<< _lower(name)<<";\n";
    }
    void writeDouble()
    {
        out<<settings.doubleName<<" "<< _lower(name)<<";\n";
    }
    void writeBoolean()
	{
		out << settings.boolName << " " << _lower(name) << ";\n";
	}

    void writeArray()
    {
        string objName = _upper(name);
        string attName = _lower(name)+settings.listSuffix;

        Json::Value first = val[0u];
        string listType = objName;

        WriteContext context(out, first, objName, ident);

        if(context.getType()==CLASS){
        	  context.writeClassDeclaration();
        }
        listType = context.getAttributeType();

        out<<"\n";
        writeIdent();
        out<< settings.listName << "<"<<listType<<"> " << attName<<";\n";

    }

    void writeClassDeclaration()
    {

        out<<"\n";
        writeIdent();
        out<< settings.className<<" "<<_upper(name)<<" {\n";

        for(auto& field: val.getMemberNames()) {
            WriteContext context(out, val[field], field, ident+1 );
            context.write();
        }

        WriteContext methods(out, val, name, ident+1 );
        methods.writeIdent();
        methods.writeConstructors();
        methods.writeIdent();
        methods.writeOperators();

        writeIdent();
        out<<"};\n";
    }

    void writeClass()
    {
        string objName = _upper(name);
        string attName = _lower(name);

        writeClassDeclaration();
        writeIdent();
        out<< objName << " "<<attName<<";\n";
    }

    std::ostream& out;
    Json::Value val; //Its json value

    std::string name;//holds its name in someones jsonValue
    int ident=0;
};


void writeStart( ostream& out)
{
    out<<"#pragma once\n\
    #include <vector>\n\
    #include <string>\n\
    #include <json/json.h>\n";
}



void DataCreator::writeAsHeader(ostream& out)
{
    if(!data.isObject())
        throw std::runtime_error("Data isnt a object");

    writeStart(out);
    out<<settings.toJsonMethod<<"\n";
    out<<settings.fromJsonMethod<<"\n";
//    out<<settings.strToJsonMethod<<"\n";

    for(auto& field: data.getMemberNames()) {
        WriteContext context(out,data[field], field);

        if(context.getType() == CLASS || context.getType() == ARRAY  )
            context.write();
        else {
            WriteContext error(cerr,data[field], field);
            cerr<<"Weird object found: ";
            error.write();
        }
    }
}

}
