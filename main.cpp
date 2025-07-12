#include <iostream>
#include <string>
#include <vector>
#include <regex>

using namespace std;

//----------REGEXES----------

regex create_data_set_pattern(R"(^!create (\S+) : word count : (\d+)(\s*)$)");
regex create_PI_model_pattern("^!create PI : (\\S+)(\\s*)$");
//regex show_name_version_pattern(R"(^(\S+)_v(\d+) <- \$INTRO(\s*)$)");
regex say_name_version_pattern(R"(^(\S+)_v(\d+) <- \$INTRO(\s*)$)");
regex train_pattern(R"(^!train (\S+)_v(\d+) with (\S+)(\s*)$)");
regex command_pattern(R"(^(\S+)_v(\d+)\s*<-\s*(.*)$)");
regex change_data_size_vec_pattern(R"(^MathGeek_v(\d+) <- $DVS_(\d+)(\S)(\d+)(\s*)$)");
//----------HELPER FUNCTIONS----------

string word_extractor(string line)
{
    while (line[0] == ' ')
        line.erase(line.begin());
    while (line[line.size() - 1] == ' ')
        line.erase(line.end() - 1);
    return line;
}

string trim(string line)
{
    while (line[0] == ' ')
        line.erase(line.begin());
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
        Data* new_data_point = new String_Data();
        new_data_point->set_word(word);
        data.push_back(new_data_point);
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
        Response response;
        input = trim(input);
        response.set_text(input);
        return response;
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
private:
    bool autocorrect;
    int compare(const string& s1, const string& s2) {
        int m = s1.size(), n = s2.size();
        vector<vector<int>> dp(m + 1, vector<int>(n + 1));

        for (int i = 0; i <= m; i++) dp[i][0] = i;
        for (int j = 0; j <= n; j++) dp[0][j] = j;

        for (int i = 1; i <= m; i++) {
            for (int j = 1; j <= n; j++) {
                if (s1[i - 1] == s2[j - 1])
                    dp[i][j] = dp[i - 1][j - 1];
                else
                    dp[i][j] = 1 + min({dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]});
            }
        }

        return dp[m][n];
    }
public:
    Grammarly(string n, int v, Data_Set ds) : PI_Model(n, v, ds) { autocorrect = false; }
    Response response(string input) override
    {
        Response response;
        input = trim(input);
        if (input == "$ATON")
        {
            autocorrect = true;
            response.set_text("autocorrect <= true");
            return response;
        }
        else if (input == "$ATOFF")
        {
            autocorrect = false;
            response.set_text("autocorrect <= false");
            return response;
        }
        else if (!autocorrect)
        {
            //cout << "called NO autocorrect" << endl;
            response.set_text(input);
        }
        else if (autocorrect)
        {
            //cout << "called autocorrect" << endl;
            const vector<Data*>& trained_data = this->train_data.get_all_data();
            // no data
            if (trained_data.empty())
            {
                cout << "No data" << endl;
                response.set_text(input);
                return response;
            }

            // to find the closest word
            string best_match_word = "";
            int min_len_diff = -1;
            int min_compare_dist = -1;

            for (Data* data_item : trained_data)
            {
                string selected_word = data_item->get_word();
                int current_len_diff = abs(static_cast<int>(input.length()) - static_cast<int>(selected_word.length()));

                if (min_len_diff == -1 || current_len_diff < min_len_diff)
                {
                    // found a word with a closer length
                    min_len_diff = current_len_diff;
                    min_compare_dist = compare(input, selected_word);
                    best_match_word = selected_word;
                }
                else if (current_len_diff == min_len_diff)
                {
                    // same length difference
                    int current_compare_dist = compare(input, selected_word);
                    // a better word!
                    if (current_compare_dist < min_compare_dist)
                    {
                        min_compare_dist = current_compare_dist;
                        best_match_word = selected_word;
                    }
                }
            }
            response.set_text(best_match_word);
        }
        return response;
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
        for (Data* d : source_data)
        {
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
    void change_data_vector_size(int size) { data_vector_size = size; }
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
                cout << "lets push " << match[2] << " words to " << data_sets.back().get_name() << " !" << endl;
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
            else if (regex_match(command, match, train_pattern))
            {
                PI_Model* PI = nullptr;
                Data_Set* DS = nullptr;
                for (const auto& pi : PI_models)
                {
                    if (match[1] == pi->get_name() && stoi(match[2]) == pi->get_version())
                    {
                        PI = pi;
                    }
                }
                for (auto& ds : data_sets)
                {
                    if (match[3] == ds.get_name())
                    {
                        DS = &ds;
                    }
                }
                if (PI != nullptr && DS != nullptr)
                {
                    PI->train(*DS);
                    cout << "Trained " << PI->get_name() << "_v" << PI->get_version() << " with dataset " << DS->get_name() << endl;
                }
                else
                {
                    cout << "Invalid Command" << endl;
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
            else if (regex_match(command, match, change_data_size_vec_pattern))
            {
                PI_Model* PI = nullptr;
                for (const auto& pi : PI_models)
                {
                    if (pi->get_name() == "MathGeek" && pi->get_version() == stoi(match[2]))
                    {
                        PI = pi;
                        break;
                    }
                }
                if (PI != nullptr)
                {
                    dynamic_cast<Math_Geek*>(PI)->change_data_vector_size(0);
                }
                else
                {
                    cout << "Invalid Command" << endl;
                }
            }
            else if (regex_match(command, match, command_pattern))
            {
                //cout << match[3];
                PI_Model* PI = nullptr;
                Data_Set* DS = nullptr;
                for (const auto& pi : PI_models)
                {
                    if (match[1] == pi->get_name() && stoi(match[2]) == pi->get_version())
                    {
                        PI = pi;
                    }
                }
                if (PI != nullptr)
                {
                    PI->response(trim(match[3])).print();
                }
                else
                {
                    cout << "Invalid Command" << endl;
                }
            }
        }

        else
        {
            cout << "Invalid Command" << endl;
        }
    }
}