#include <bits/stdc++.h>
using namespace std;
 
int global_index = 0;
int final_state = global_index;
bool left_or_completed = 0;
string output = "";
const int MAXN = 1000;
vector<vector<vector<int>>>q(MAXN, vector<vector<int>>(3, vector<int>(1, -1)));
void corresponding_brackets(map<int ,int>& marked_indices, string original_re)
{
    int n = original_re.length();
    stack<pair<char, int>>st;
    for(int i = 0; i < n; i++)
    {
        char c = original_re[i];
        if(c == '(')
        {
            st.push({'(', i});
        }
        else if(c == ')')
        {
            if(marked_indices.find(i)!=marked_indices.end())
            {
                //have to mark the corresponding index as well
                marked_indices[(st.top().second)] = marked_indices[i];
            }
            st.pop();
        }
    }

    return;

}
string bracket_reducer(string original_re)
{
    map<int, int>marked_indices; //index, frequency 
    int n = original_re.length();
    for(int i = 0; i < n ; i++)
    {
        char c = original_re[i];
        if(c == '*')
        {
            marked_indices[i - 1]++;
        }
        else if(c == '+')
        {
            marked_indices[i - 1]++;
        }
        else if(c == '|')
        {
            stack<char>st;
            for(int j = i + 1 ; j < n; j++)
            {

                if(st.empty())
                {
                    if(original_re[j] == ')'){
                        marked_indices[j]++;
                        break;
                    }
                }

                if(original_re[j] == '(')
                {
                    st.push('(');
                }
                else if(original_re[j] == ')')
                {
                    st.pop();
                }
            }
        }
    }

    corresponding_brackets(marked_indices, original_re);
    

    string answer = "";

    for(int i = 0; i < n; i++)
    {
        if(marked_indices.find(i) != marked_indices.end()){

            while(marked_indices[i]!=0)
            {
                answer+=original_re[i];
                marked_indices[i]--;
            }
           
        }
        else if(original_re[i] != '(' && original_re[i]!=')')answer+=original_re[i];
    }


    return answer;
}
int or_handler(string re)
{
    int n = re.length();
    stack<pair<char, int>>st;
    for(int i = 0; i < n; i++)
    {
        char c = re[i];
        if(c == '(')st.push({'(', i});
        else if(c == ')')st.pop();
        else if(c == '|')
        {
            if(st.size() == 1)
            {
                return i;
            }
        }
    }
    return -1;
}
vector<string> refine_regex(string re)
{
    int n = re.length();
    vector<string>refined_answer;
    string curr_ans = "";
    stack<pair<char, int>>st;
    bool inside_bracket = 0;

    for(int i = 0; i < n; i++)
    {
        if(re[i] == 'a' || re[i] == 'b')
        {
            if(!inside_bracket){
                curr_ans += re[i];
                refined_answer.push_back(curr_ans);
                curr_ans = "";
            }
        }
        else if(re[i] == '(')
        {
            inside_bracket = 1;
            st.push({'(', i});
        }
        else if(re[i] == ')')
        {

            if(st.size() == 1)
            {
                int start_index = st.top().second;
                int end_index = i;
                st.pop();
                curr_ans = re.substr(start_index, end_index - start_index + 1);
                if(i!=n-1)
                {
                    //not the last letter
                    if(re[i + 1] == '*' || re[i + 1] == '+')curr_ans+=re[i + 1];
                }
                refined_answer.push_back(curr_ans);
                curr_ans = "";
                inside_bracket = 0;
            }
            else st.pop();
        }
    }
    return refined_answer;
}

void RE2Machine(string re)
{
    int n = re.length();
    char c = re[n - 1];
    if(n == 1)
    {
        if(q[final_state][c-'a'][0] == -1)q[final_state][c-'a'].pop_back();
        q[final_state][c-'a'].push_back(global_index + 1);
        global_index++;
        final_state = global_index;
    }
    else if(c == '+')
    {
        int lexxer = 0;
        string auxilary = re.substr(1, n - 3); //ab
        vector<string>temp = refine_regex(auxilary); // temp = <b, b>
        int start_state = global_index;
        lexxer = 2;
        for(auto &x : temp)RE2Machine(x);
        //now check possible transititions of start state
        int curr_state = final_state;

        lexxer -= 1;
        string aux2 = "(" + auxilary + ")*";
        RE2Machine(aux2); 
        lexxer++;

        lexxer = 0;
        //no change in final state -- stays as before
    }
    else if(c == '*')
    {
        string auxilary = re.substr(1, n - 3);
        global_index++;
        if(q[final_state][2][0]==-1)q[final_state][2].pop_back();
        int lex = 4;
        q[final_state][2].push_back(global_index);
        int start_state = global_index; // start state -- > 2
        final_state = start_state; // necessary because next machine will be made from final state
        vector<string>temp = refine_regex(auxilary);
        for(auto &x : temp)RE2Machine(x);
        
        //now final state has changed -- > 4
        int curr_state = final_state;

        //transitions going to the last state should now go to the start state

        for(auto &x : q)
        {
            for(auto &y : x)
            {
                for(auto &z : y)
                {
                    if(z == curr_state)z = start_state;
                }
            }
        }
        final_state = start_state;
        global_index = global_index - 1;
    }
    else if(c == ')')
    {
        //it's definitely an "or" bracket
        int n = re.length();
        int main_or_index = or_handler(re);
        string left = re.substr(1, main_or_index - 1);
        string right = re.substr(main_or_index + 1, n - main_or_index - 2);

        stack<int> st;
        int start = final_state; // 0

        //perform nfa on left

        vector<string>temp = refine_regex(left);
        for(auto&x : temp)RE2Machine(x);

        //now my final state has changed

        int aux_final_state = final_state;//storing left nfa final state // 1

        //doing for right NFA
        temp = refine_regex(right);
        final_state = start;
        for(auto& x : temp)RE2Machine(x);

        //from final and aux final, make a transition to ultimate final state on seeing c(epsilon)
        global_index++;
        if(q[final_state]['c'-'a'][0] == -1)q[final_state][2].pop_back();
        q[final_state]['c'-'a'].push_back(global_index);
        if(q[aux_final_state]['c'-'a'][0] == -1)q[aux_final_state][2].pop_back();
        q[aux_final_state]['c'-'a'].push_back(global_index);

        final_state = global_index;
    }
}

bool is_accepted(string to_check, int index, int curr_state)
{
    int n = to_check.length();
    if(index == n && curr_state == final_state)return true;
    char c ;
    if(index < n)
    {
        c = to_check[index];
        for(auto &x : q[curr_state][c-'a'])
        {
            if(x == -1){
                break;
            }
            int new_state = x;
            if(is_accepted(to_check, index + 1, new_state))return true;
        }

    }
    for(auto &x : q[curr_state][2])
    {
        if(x == -1){
            break;
        }
        int new_state = x;
        if(is_accepted(to_check, index, new_state))return true;
    }
    return false;
}

int main()
{

    //reading the input file
    fstream ifile;
    ifile.open("input.txt", ios::in);
    int n;
    string aux;
    string input_string;

    if(ifile.is_open())
    {
        getline(ifile, aux);
        n = stoi(aux);
    }
    vector<string>all_regex(n);

    if(ifile.is_open())
    {
        // all_regex.resize(n);
        int i = 0;
        while(i < n)
        {
            getline(ifile, all_regex[i]);
            i++;
        }
        getline(ifile, input_string);
        ifile.close();
    }

    vector<vector<int>>cache(input_string.length(), vector<int>(input_string.length(), -1));
    int x = n;

    while(n--)
    {
        // cout << x - n << '\n';
        string reg_ex = all_regex[x - n - 1];
        string no_bracket = bracket_reducer(reg_ex);
        vector<string>refined_answer = refine_regex(no_bracket);

        //build the NFA table here
        for(auto &x : refined_answer)RE2Machine(x);

        // cout << "hello\n";

        for(int i = 0; i < input_string.length(); i++)
        {
            for(int j = i; j < input_string.length(); j++)
            {
                string curr_string = input_string.substr(i, j - i + 1);
                // cout <<i<<j<<":"<<curr_string << " ";
                if(cache[i][j] != -1)continue;
                // cout << "enter\n";

                if(is_accepted(curr_string, 0, 0))
                {
                    // cout << "| ";
                    cache[i][j] = x - n ;
                }
            }
            // cout << "\n";
        }
        // cout << "'\n'\n''\n'";
        q.clear();
        q.resize(MAXN,vector<vector<int>>(3, vector<int>(1, -1)));
        global_index = 0;
        final_state = global_index;
    }

    // for(int i = 0; i < input_string.length(); i++)
    // {
    //     for(int j = 0; j < input_string.length(); j++)
    //     {
    //         cout << cache[i][j] << " ";
    //     }
    //     cout << '\n';
    // }

    string output = "";
    for(int row = 0; row < input_string.length(); row++)
    {
        bool all_absent = 1;
        for(int col = input_string.length() - 1; col >= row; col--)
        {
            if(cache[row][col] != -1)
            {
                // cout << row << col << '\n';
                output += '$';
                output += to_string(cache[row][col]);
                row = col;
                all_absent = 0;
                break;
            }
        }
        if(all_absent)
        {
            output += '@';
            output += input_string[row];
        }
    }

    fstream ofile;
    ofile.open("output.txt", ios::out);

    ofile << output;
    ofile << "#" <<'\n';
    ofile.close();

 
    return 0;
}