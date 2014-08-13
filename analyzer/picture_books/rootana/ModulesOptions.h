#ifndef MODULESOPTIONS__HH_
#define MODULESOPTIONS__HH_
#include <map>
#include <vector>
#include <string>
#include <utility>

namespace modules{
    class options;
}

/// @brief Class to handle options given through the MODULES file
class modules::options{
    public:
        /// Helper typedef, intended for the list of key-value pairs for each option
        typedef std::map<std::string,std::string> OptionsList_t;
        /// Helper typedef, intended to keep the order of option declaration
        typedef std::vector<OptionsList_t::iterator> OptionsOrder_t;

    public:
        /// @brief Constructor of an option.  The name of the module that will
        //receive these options should be passed in
        options(const std::string& name):fModuleName(name){
            fIdNumber=MakeIdNumber();
        };
        virtual ~options(){};

        /// @brief Flatten all options into a string
        /// @details Used to provide unique identifiers of a module or
        /// generator, based on it's configuration
        std::string StringDescription()const;

        /*-------------  Used by modules::reader -------------*/

        /// @brief Set the value of an option
        /// @details If a previous value was set, it is replaced by this one
        /// @param key The name of the option to set
        /// @param value The new value 
        void SetOption(const std::string& key, const std::string& value);

        /// @brief Append a new value to a list
        /// @details The new value is separated from the previous one with: " : "
        /// @param key The name of the option to add to
        /// @param value The value to append
        /// @return true if a value existed already and false if not
        bool AppendToOption(const std::string& key, const std::string& value);

        /// @brief Set the value of an argument.
        /// @details If the module has not specified a name for this argument, the key
        /// for this option is set to number
        /// @seealso ALCAP_REGISTER_MODULE
        /// @param number The number of the argument to set
        /// @param value The new value of the argument
        void AddArgument(const int& number, const std::string& value);

        /*-------------  Used to retrieve values -------------*/

        /// @group \retrieve_values
        /// @{
        /// @brief Get the named option as an int
        /// @param name Name of the option to get
        /// @param defVal Value to use if not found
        int GetInt(const std::string& name,int defVal=-1)const;

        /// @brief Get the named option as a double
        /// @param name Name of the option to get
        /// @param defVal Value to use if not found
        double GetDouble(const std::string& name,double defVal=1.)const;

        /// @brief Get the named option as a string.
        /// @details Trims whitespace from the front and back of the option
        /// @param name Name of the option to get
        /// @param defVal Value to use if not found
        std::string GetString(const std::string& name,const std::string& defVal="")const;

        /// @brief Get the named option as a bool
        /// @param name Name of the option to get
        /// @param defVal Value to use if not found
        bool GetBool(const std::string& name,bool defVal=false)const;

        /// @brief Fill a vector of strings with whitespace delimited tokens found
        /// in the named option
        /// @param name Name of option to use
        /// @param vect vector to fill with tokens
        /// @return Number of tokens pushed in to vect
        int GetVectorStringsByWhiteSpace(const std::string& name, std::vector<std::string>& vect)const;

        /// @brief Fill a vector of strings with tokens found
        /// in the named option
        /// @ Uses strtok so not suitable for multi-threading (if we ever do something
        /// like that)
        /// @param name Name of option to use
        /// @param vect vector to fill with tokens
        /// @param delim c-string of chars to use as delimiter
        /// @return Number of tokens pushed in to vect
        int GetVectorStringsByDelimiter(const std::string& name, std::vector<std::string>& vect,const char* delim=":")const;

        /// @brief Check if a named option has been set
        /// @param name Name of option to use
        /// @return true if option was set, false if not
        bool HasOption(const std::string& name)const;

        /// @brief Check if a flag option has been set
        /// @details If the option is present with no value return true, else
        /// return true if the value gives boolean true
        /// @param name Name of option to use
        /// @return true if option was set, false if not
        bool GetFlag(const std::string& name)const;

        /// Get the number of options managed by this options list
        bool GetNumOptions()const{return fOptions.size();};

        /// @brief Returns the unprocessed value assigned to name
        /// @param name Name of the option to get
        std::string GetOption(const std::string& name)const;

        /// Get a const iterator to the first key-value pair (sorted alphabetically by
        OptionsList_t::const_iterator begin()const{return fOptions.begin();};
        /// Get a const iterator to the last key-value pair (sorted alphabetically by key)
        OptionsList_t::const_iterator end()const{return fOptions.end();};

        //OptionsList_t::iterator begin(){return fOptions.begin();};
        //OptionsList_t::iterator end(){return fOptions.end();};

        /// @brief Print all options managed by this list to cout.  
        /// @param prefix Appended before each line is printed
        void DumpOptions(const std::string& prefix="     ")const;

        /// Set the alias option for this module
        void SetAlias(const std::string& alias){SetOption("alias",alias);};
        /// Get the alias option for this module
        std::string GetAlias()const{return GetString("alias");};

        /// @}

    private:
        /// @brief Get an option using std::stringstream
        /// @tparam T return type
        /// @param name Name of option to get
        /// @param defVal value to use if no option exists
        template <typename T>
            T GetOption(const std::string& name,const T& defVal)const;

        /// Create a unique ID number used for each option
        static int MakeIdNumber();

    private:
        OptionsList_t fOptions;
        OptionsOrder_t fOrder;
        int fIdNumber;

        std::string fModuleName;

};

inline void modules::options::SetOption(const std::string& name, const std::string& option){
    // if a new key was added, store an iterator in the order list
    OptionsList_t::iterator it = fOptions.find(name);
    fOptions[name]=option;
    if(it==fOptions.end()) fOrder.push_back(fOptions.find(name));
}

inline bool modules::options::HasOption(const std::string& name)const{
    return fOptions.find(name)!=fOptions.end();
}

inline bool modules::options::GetFlag(const std::string& name)const{
    return (HasOption(name) && (GetOption(name).empty() || GetBool(name)));
}

#endif  //MODULESOPTIONS__HH_
