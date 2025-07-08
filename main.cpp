#include <iostream>
#include <string>
#include <vector>
#include <regex>

using namespace std;

//----------REGEXES----------

regex create_data_set_pattern(R"(^!create (\S+) : word count : (\d+)(\s*)$)");
regex create_PI_model_pattern("^!create PI : (\\S+)(\\s*)$");
regex show_name_version_pattern(R"(^(\S+)_v(\d+) <- \$INTRO(\s*)$)");
regex say_name_version_pattern(R"(^(\S+)_v(\d+) <- \$INTRO(\s*)$)");

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
    vector<int>& get_ascii_vector()
    {
        return ascii_vector;
    }
};

class Data_Set
{
private:
    string name;
    vector<Data*> data;
public:
    Data_Set(string n) : name(n) {}
    string get_name() const { return name; }
    const vector<Data*>& get_all_data() { return data; }
    const Data* get_data_at(int index) { return data[index]; }
    void cin_data() // im gonna call this function in a loop in order to get all the data
    {
        string line;
        getline(cin, line);
        string word = word_extractor(line);
        // back logic needed!
        cout << "pushed word: \"" << word << "\"" << endl;
    }
    void add_data(Data* new_data)
    {
        data.push_back(new_data);
    }
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

//----------PI MODEL----------

class PI_Model
{
protected:
    string name;
    int version;
    Data_Set train_data;
public:
    PI_Model(string n, int v, Data_Set ds) : name(n), version(v), train_data(ds) {}
    string get_name() const { return name; }
    int get_version() { return version; }
    virtual Response response(string input) = 0;
    virtual void train(Data_Set& ds) = 0;


};

class Parrots : public PI_Model
{
public:
    Parrots(string n, int v, Data_Set ds) : PI_Model(n, v, ds) {}
    Response response(string input) override
    {

    }
    void train(Data_Set& ds) override
    {
        vector<Data*> source_data = ds.get_all_data();
        for (Data* d : source_data)
        {
            this->train_data.add_data(d);
        }
    }
};

class Grammarly : public PI_Model
{
public:
    Grammarly(string n, int v, Data_Set ds) : PI_Model(n, v, ds) {}
    Response response(string input) override
    {

    }
    void train(Data_Set& ds) override
    {
        vector<Data*> source_data = ds.get_all_data();
        for (Data* d : source_data)
        {
            this->train_data.add_data(d);
        }
    }
};

class Math_Geek : public PI_Model
{
private:
    int data_vector_size;
public:
    Math_Geek(string n, int v, Data_Set ds) : PI_Model(n, v, ds) { data_vector_size = 5; }
    Response response(string input) override
    {

    }
    void train(Data_Set& ds) override
    {
        vector<Data*> source_data = ds.get_all_data();
        for (Data* d : source_data) {
            Vector_Data* vd = dynamic_cast<Vector_Data*>(d);
            if (vd)
            {
                vector<int>& vec = vd->get_ascii_vector();
                // too long
                while (vec.size() > this->data_vector_size) { vec.pop_back(); }
                // too short
                while (vec.size() < this->data_vector_size) { vec.push_back(' '); }
            }
            this->train_data.add_data(d);
        }
    }
};

//----------INT MAIN----------

int main()
{
    vector<Data_Set> data_sets;
    vector<PI_Model*> PI_models;
    Data_Set dummy_ds("dummy");
    int parrots_count = 1;
    int grammarly_count = 1;
    int math_geek_count = 1;
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
                data_sets.push_back(Data_Set(match[1]));
                cout << "lets push 4 words to " << data_sets.back().get_name() << " !" << endl;
                for (int i = 0; i < stoi(match[2]); i++)
                    data_sets.back().cin_data();
            }
            else if (regex_match(command, match, create_PI_model_pattern))
            {
                if (match[1] == "Parrots")
                {
                    PI_models.push_back(new Parrots(match[1], parrots_count, dummy_ds));
                    cout << "Parrots_v" << parrots_count << " created" << endl;
                    parrots_count++;
                }
                else if (match[1] == "Grammarly")
                {
                    PI_models.push_back(new Grammarly(match[1], grammarly_count, dummy_ds));
                    cout << "Grammarly_v" << grammarly_count << " created" << endl;
                    grammarly_count++;
                }
                else if (match[1] == "MathGeek")
                {
                    PI_models.push_back(new Math_Geek(match[1], math_geek_count, dummy_ds));
                    cout << "MathGeek_v" << math_geek_count << " created" << endl;
                    math_geek_count++;
                }
                else
                {
                    cout << "!!! Error : Invalid name for PI !!!" << endl;
                }
            }
        }

        else if (command.find("<-") != string::npos)
        {
//            if (regex_match(command, match, show_name_version_pattern))
//            {
//                PI_Model* PI = nullptr;
//                for (const auto& pi : PI_models)
//                {
//                    if (pi->get_name() == match[1] && match[2] == pi->get_version())
//                    {
//                        PI = pi;
//                        break;
//                    }
//                }
//                if (PI == nullptr)
//                {
//                    cout << "Invalid Command" << endl;
//                }
//                else
//                {
//                    cout << PI->get_name() << "_v" << PI->get_version() << " -> Hi! I'm " << PI->get_name() << ". You are using version " << PI->get_version() << "!" << endl;
//                }
//            }

            if (regex_match(command, match, say_name_version_pattern))
            {
                PI_Model* PI = nullptr;
                for (const auto& pi : PI_models)
                {
                    if (pi->get_name() == match[1] && pi->get_version() == stoi(match[2]))
                    {
                        PI = pi;
                        break;
                    }
                }
                if (PI != nullptr)
                    cout << PI->get_name() << "_v" << PI->get_version() << " -> Hi! I'm " << PI->get_name() << ". You are using version " << PI->get_version() << "!" << endl;
                else
                    cout << "Invalid Command" << endl;
            }
        }

        else
        {
            cout << "Invalid Command" << endl;
        }
    }
}