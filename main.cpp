#include <iostream>
#include <string>
#include <regex>
#include <vector>

using namespace std;

// ----------REGEXES----------



// ----------DATA & DATA_SET----------

class Data
{
protected:
    string word;
public:
    virtual void set_word(const string& _word) = 0;
    string get_word() const{ return word; }
};

class Vector_Data : public Data
{
private:
    vector<int> asci_codes;
public:
    vector<int> get_Vector() const{ return asci_codes; }
    void set_word(const string &_word) override
    {
        for (const auto& c : word)
        {
            int asci_code = (int)c;
            asci_codes.push_back(asci_code);
        }
    }

};

class String_Data : public Data
{
private:
public:
    void set_word(const string &_word) override { word = _word; }

};

class Data_Set
{
private:
    vector<Data*> data;
    string name;
public:
    vector<Data*> get_all_data() const{ return data; }
    Data* get_data(int i) const { return data[i]; }
    void cin_data(Data* d) { data.push_back(d); }
};

// ----------PI MODEL----------

class PI_Model
{
protected:
    string name;
    int version;
    Data_Set train_data;
public:
    void train(Data_Set ds) { train_data = ds; }
    Data_Set get_data_set() const{ return train_data; }
    
};

class Math_Geek : public PI_Model
{

};

class Grammarly : public PI_Model
{

};

class Parrots : public PI_Model
{

};

// ----------RESPONSE----------

class Response
{
private:
    string response;
    bool has_error;
    string error_text;
public:
    void error_happen(bool e)
    {

    }
    void set_error_text(const string& et) { error_text = et; }
    void set_text(const string& text) { response = text; }
    void print(){}
};

// ----------INT MAIN----------

int main()
{
    string command;
    while (true)
    {
        getline(cin, command);
        // not talking to a specific PI
        if (command.find('!') != string::npos)
        {
            // end of program
            if (command == "!end")
            {
                return 0;
            }
        }
        // talking to an specific PI
        else if (command.find("<_") != string::npos)
        {

        }
        else
        {
            cout << "Invalid Command" << endl;
        }
    }

}