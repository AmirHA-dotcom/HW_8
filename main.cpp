#include <iostream>
#include <string>
#include <vector>
#include <regex>

using namespace std;


//----------REGEXES----------



//----------HELPER FUNCTIONS----------

string word_extractor(string line)
{
    while (line[0] == ' ')
        line.erase(line.begin());
    while (line[line.size() - 1] == ' ')
        line.erase(line.end() - 1);
    return line;
}

//----------DATA & DATA_SET----------

class Data
{
protected:
    string word;
public:
    virtual void set_word(string _word) = 0;
    string get_word() { return word; }
};

class String_Data : public Data
{
public:
    void set_word(string _word) override { word = _word; }
};

class Vector_Data : public Data
{
private:
    vector<int> ascii_vector;
public:
    void set_word(string _word) override
    {
        word = _word;
        for (const char& c : word)
            ascii_vector.push_back((int)c);
    }
};

class Data_Set
{
private:
    string name;
    vector<Data> data;
public:
    const vector<Data>& get_all_data() { return data; }
    const Data& get_data_at(int index) { return data[index]; }
    void cin_data() // im gonna call this function in a loop in order to get all the data
    {
        string line;
        getline(cin, line);

    }
};

//----------PI MODEL----------

class PI_Model
{

};

class Parrots : public PI_Model
{

};

class Grammarly : public PI_Model
{

};

class Math_Geek : public PI_Model
{

};

//----------RESPONSE----------

class Response
{
private:
    string text;
    bool has_error;
    string error_text;
public:
    void error_happen() { has_error = true; }
    void set_error_text(const string& et) { error_text = et; }
    void set_text(const string& t) { text = t; }
    void print()
    {
        if (has_error)
            cout << error_text << endl;
        else
            cout << text << endl;
    }
};

//----------INT MAIN----------

int main()
{
    string command;
    while (true)
    {
        getline(cin, command);
        if (command.find('!') != string::npos)
        {
            if (command.find("!end") != string::npos)
            {
                return 0;
            }
        }

        else if (command.find("<-") != string::npos)
        {

        }

        else
        {
            cout << "Invalid Command" << endl;
        }
    }
}