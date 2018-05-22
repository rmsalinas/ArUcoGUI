#ifndef _Generic_Params_
#define _Generic_Params_
#include <string>
#include <iostream>
#include <sstream>
#include <streambuf>
#include <vector>
#include <map>
#include "exports.h"
#include "exceptions.h"
#include "encryptor.h"


namespace gparam
{

//%p full path to the file
//%d dir to the file
//%s file base name QFileInfo(%p).baseName()
//%a applicationDirPath
//Example: open file test_file.eje in dir /home/me/project
//%p: /home/me/project/test_file.eje
//%d: /home/me/project
//%s: test_file
//%a: <path to the installed location of this app>


class GPARAM_API Param
{
 public:
    enum ParamType {UNKNOWN,STRING,STRINGLIST,INTEGER,REAL,BOOLEAN,FILEPATH,EXISTING_FILEPATH,DIRPATH};
    //returns a string identifying the param type
    std::string getParamTypeStr ( ParamType ) const;

public:
    //creates empty param (_type=NONE)
    Param () throw ( gparam::Exception );
    //creates indicated param
    Param ( std::string name ,ParamType pt ) throw ( gparam::Exception );
    Param ( std::string name ) throw ( gparam::Exception );
    explicit Param ( std::string name, std::string val ) throw ( gparam::Exception );
    explicit Param ( std::string name,const std::vector<std::string> &strlist ) throw ( gparam::Exception );
    explicit Param ( std::string name, char* val ) throw ( gparam::Exception );
    explicit Param ( std::string name, int val ,int min=0,int max=0,int stepInc=0 ) throw ( gparam::Exception );
    explicit Param ( std::string name, double val ,double min=0,double max=0,double stepInc=0 ) throw ( gparam::Exception );
    explicit Param ( std::string name, bool val ) throw ( gparam::Exception );


 
    /** Returns the value. UNSAFE!!!! (can't return strings with spaces)
     */
    template<typename T> T  get() const throw ( gparam::Exception ); //unsafe

    /**Returns an integer value
     */
    int asInt() const throw ( gparam::Exception )
    {
        return get<int>();
    }
    /**Returns a double value
     */
    double asDouble() const throw ( gparam::Exception )
    {
        return get<double>();
    }

    /**Returns a double value
     */
    std::string asString() const throw ( gparam::Exception )
    {
        return _value;
    }

    /**Returns the list of possible values if param is of type  STRINGLIST
     */
    std::vector<std::string> &getStringList()
    {
        return _strlist;
    }
    
    /**In stringlists, returns the index in stringlist that  value  has. If value is not in stringlist, returns -1
     */
    int getStrListIndex();
    /**Sets the value only (does not change type)
     */
    template<typename T> void  set ( T &v ) throw ( gparam::Exception ) ;
    /**Sets the value and  type. If
     */
    template<typename T> void  set ( T &v,ParamType type ) throw ( gparam::Exception ) ;
    //sets the value to v and the type to INTEGER
    Param & operator= ( int v ) throw ( gparam::Exception ) ;
    //sets the value to v and the type to REAL
    Param & operator= ( double v ) throw ( gparam::Exception ) ;
    //sets the value to v and the type to BOOLEAN
    Param & operator= ( bool v ) throw ( gparam::Exception ) ;
    //sets the value to v and the type is now changed
    Param & operator= ( std::string v ) throw ( gparam::Exception ) ;

    //compares two elements (comparion only considers name and value)
    bool operator== ( const Param &p )const;
    //compares two elements (comparion only considers name and value)
    bool operator!= ( const Param &p )const;
    /**Indicates if the param has limits specified
     */
    bool hasLimits() const;

    /**Gets lower and upper limits values
     */
    template<typename T> T  getLowerLimit ( ) throw ( gparam::Exception ) ;
    template<typename T> T  getUpperLimit ( ) throw ( gparam::Exception ) ;

    /**
     * Indicates if the parameter has data.
     */
    bool empty() const;
    /**A param is invalid if has no name
     */
    bool isValid() const;
    /**Returns the name
     */
    std::string getName() const;
    /**Sets the name
    */
    void setName ( std::string name );
    /**Sets the description
     */
    void setDescription ( std::string des )
    {
        _description=des;
    }
    /**Returns the description
     */
    std::string getDescription() const;
    /**Returns the type
     */
    ParamType getType() const;
    /**Returns the step increment if indicated. If not it will be 0
     */
    double getStepIncrement() const
    {
        return _stepIncrement;
    }
    /**
     * Short print string for debugging purpouses
     */
    std::string getPrintString() const;
    //Ostream operators
    friend GPARAM_API std::ostream & operator<< ( std::ostream &str,const Param &p );

    //Ostream operators
    friend GPARAM_API std::istream & operator>> ( std::istream &str,  Param &p ) throw ( gparam::Exception );

private:
    std::string _name,_description,_value;
    std::vector<std::string> _strlist;

    void initValues();
    ParamType _type;
    double _min,_max;
    double _stepIncrement;

 
};
/**
 *
 */
class GPARAM_API ParamSet: public std::vector<Param>
{
    std::string _name;//name of the set
    Param _dummy;

public:

    ParamSet(){}
    ParamSet(const std::string&name){_name=name;}
    void setName ( const std::string &name )
    {
        _name=name;
    }
    std::string getName() const
    {
        return _name;
    }
    //finds the required param. If not found, returns an inValid object
    Param &find ( const Param &p );
    const Param &find ( const Param &p ) const;
    Param &find ( std::string name );
    const Param &find ( std::string name )const;

    Param & operator[] ( size_t i );
    const Param & operator[] ( size_t i ) const;
    Param & operator[] ( const Param &p );
    const Param & operator[] ( const Param &p ) const;
    Param & operator[] ( std::string name );
    const Param & operator[] ( std::string name ) const;
    //convenient insert method
    Param &  insert ( const Param &p,bool overwriteOldValue=false );

    // merge two ParamSets
    void merge ( const ParamSet &PS, bool overwriteRepeatedValues=false );

    //
    friend GPARAM_API std::ostream &operator<< ( std::ostream &str,const ParamSet & ps );
    //
    friend GPARAM_API std::istream &operator>> ( std::istream &str, ParamSet & ps ) throw ( gparam::Exception );


    /** Save this to a file.
     * If !merge, removes previous content
     * Else,  the file is parsed looking for another paramset with the same name. Then, the whole file is copied
     * kept except for the paramset that is replaced with the contents of this
     */
    void saveToFile ( std::string filePath, bool merge=false ) throw ( gparam::Exception );
    void saveToFileEncrypted ( std::string filePath, std::string key, bool merge=false ) throw ( gparam::Exception );
    void saveToStreamEncrypted ( std::ostream& str,std::string key ) throw ( gparam::Exception );

    void saveToBinStream ( std::ostream& str) throw ( gparam::Exception );
    //finds the param set of name specified in a file and set its contents in ps_out. If  found return true, else returns false
    /**Reads this from a file.
     * @param filePath where to look for the data
     * @param name of the paramset to be found. If name is empty (by default), the getName() member is invoked for obtaining the name. Therefore,
     * this function is directly empmloyable by inherited classes that setName. If name=="*", then the first instance is read. If name =="" and this->getName()=="", then
     * it is equivalent to name=="*"
     *
     *
     * Example:
     * \code
     * class MyP: public gparam::ParamSet{
     *
     *  public:
     * MyP(){
     * setName("MyP");
     *  insert(  gparam::Param ( "minDecLevel",int ( 0 ),0,std::numeric_limits<int>::max() ));
     *  insert(  gparam::Param ( "ligthThres",int ( 15 ),0,255));
     * }
     * };
     *
     * int main(){
     *  MyP mp;
     *  mp.readFromFile(pathToFile); //equivalent to mp.readFromFile(pathToFile,mp.getName()); and mp.readFromFile(pathToFile,"MyP");
     * }
     * \endcode
     *
     */
    bool readFromFile ( std::string filePath,std::string name="" ) throw ( gparam::Exception );
    bool readFromFileEncrypted ( std::string filePath,std::string key,std::string name="" ) throw ( gparam::Exception );
    bool readFromStreamEncrypted ( std::istream &str,std::string key,std::string name="" ) throw ( gparam::Exception );
    bool readFromBinStream ( std::istream& str) throw ( gparam::Exception );

    /**
     */
    std::string  getPrintString() const;
    
    //indicates if two sets are equal
    bool operator ==(const ParamSet &p);
    //indicates if two sets are equal
    bool operator !=(const ParamSet &p);
    /**Returns a signature depending on the name of the elements and their type, but not on their values.
     * It can be used to know if a paramset has changed its params, added new or changed its typename
     */
    std::string getSignature();
};

class HyperParamSet: public std::vector<gparam::ParamSet>
{
public:
    //reads the set of param sets found in a file
    HyperParamSet readFromFile ( std::string filePath ) throw ( gparam::Exception );

};

/**Returns the value
  */
template<typename T> T  Param::get() const throw ( gparam::Exception )
{
    int size=sizeof ( T );
    if ( _type==INTEGER && size!=4 ) throw gparam::Exception ( gparam::Exceptions::InvalidParam, "Param::get","Parameter has can not be converted to INTEGER. TYPE="+getParamTypeStr ( _type ),__FILE__,__LINE__ );
    if ( _type==REAL && ( size!=8 && size!=4 ) ) throw gparam::Exception ( gparam::Exceptions::InvalidParam, "Param::get","Parameter has can not be converted to REAL. TYPE="+getParamTypeStr ( _type ),__FILE__,__LINE__ );
    if ( _type==STRING || _type==STRINGLIST )
    {
        std::cerr<<"UNSAFE OPERATION get<std::string>() in STRING AND STRINGLIST types since can not return spaced elements "<<__FILE__<<" "<<__LINE__<<std::endl;
    }
    std::stringstream sstr ( _value.c_str() );
    T tval;
    sstr>>tval;
    return tval;
}
/**Sets the value
 */
template<typename T> void Param::set ( T &v ) throw ( gparam::Exception )
{

    int size=sizeof ( v );
    if ( _type==INTEGER && size!=4 ) throw gparam::Exception ( gparam::Exceptions::InvalidParam, "Param::set","Parameter has can not be converted to INTEGER. TYPE="+getParamTypeStr ( _type ),__FILE__,__LINE__ );
    if ( _type==REAL && ( size!=8 && size!=4 ) ) throw gparam::Exception ( gparam::Exceptions::InvalidParam, "Param::set","Parameter has can not be converted to REAL. TYPE="+getParamTypeStr ( _type ),__FILE__,__LINE__ );

    if ( _type==INTEGER  || _type==REAL )
    {
        if ( hasLimits() )
        {
            if ( v<_min || v>_max )
                throw gparam::Exception ( gparam::Exceptions::OutOfBounds, "Param::set","Parameter <"+_name+"> value can not be assigned out of bounds",__FILE__,__LINE__ );
        }
    }

    std::stringstream sstr;
    sstr<<v;
    _value=sstr.str();

}
/**Sets the value
 */
template<typename T> void Param::set ( T &v,ParamType t ) throw ( gparam::Exception )
{
    _type=t;
    set ( v );
}

/**Gets lower and upper limits values
 */
template<typename T> T Param::getLowerLimit() throw ( gparam::Exception )
{
    return _min;
}

template<typename T> T Param::getUpperLimit() throw ( gparam::Exception )
{
    return _max;
}



};
#endif


