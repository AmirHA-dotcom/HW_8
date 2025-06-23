#include <iostream>
#include <string>
#include <vector>
#include <regex>

using namespace std;


//----------REGEXES----------

regex create_data_set_pattern("^!create (\\S+) : word count : (\\d+)(\\s*)$");

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
    Data_Set(const string& n) : name(n) {}
    string get_name() const { return name; }
    const vector<Data>& get_all_data() { return data; }
    const Data& get_data_at(int index) { return data[index]; }
    void cin_data() // im gonna call this function in a loop in order to get all the data
    {
        string line;
        getline(cin, line);
        string word = word_extractor(line);
        // back logic needed!
        cout << "pushed word: \"" << word << "\"" << endl;
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
    vector<Data_Set> data_sets;
    string command;
    smatch match;
    while (true)
    {
        getline(cin, command);
        if (command[0] == '!')
        {
            if (command.find("!end") != string::npos)
            {
                return 0;
            }
            else if (regex_match(command, match, create_data_set_pattern))
            {
                Data_Set ds(match[1]);
                cout << "lets push 4 words to " << ds.get_name() << " !" << endl;
                for (int i = 0; i < stoi(match[2]); i++)
                    ds.cin_data();
            }
        }

        else if (command.find("<-") != string::npos)
        {
            cout << command << " called" << endl;
        }

        else
        {
            cout << "Invalid Command" << endl;
        }
    }
}