#include <bits/stdc++.h>
using namespace std;
 
int global_index = 0;
int final_state = global_index;
bool left_or_completed = 0;
int found = 0;
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
        string auxilary = re.substr(1, n - 3);
        vector<string>temp = refine_regex(auxilary); // temp = <b, b>
        int start_state = global_index;
        for(auto &x : temp)RE2Machine(x);

    
        
        //now check possible transititions of start state
        int curr_state = final_state;
        for(int i = 0; i < 2; i++)
        {
            for(int j = 0; j < q[start_state][i].size(); j++)
            {
                if(q[start_state][i][j] == -1)break;    
                //some transition exists from the start state
                //now just redirect whatever is going from current state to start state + 1
                
                if(q[curr_state][i][0] == -1)q[curr_state][i].pop_back();

                q[curr_state][i].push_back(start_state + 1);
            }
        }
        //no change in final state -- stays as before
    }
    else if(c == '*')
    {
        
        string auxilary = re.substr(1, n - 3);
        global_index++;
        if(q[final_state][2][0]==-1)q[final_state][2].pop_back();
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

        //left done
        // left_or_completed = 1;

        int aux_final_state = final_state;//storing left nfa final state // 1

        //doing for right NFA
        temp = refine_regex(right);
        final_state = start;
        for(auto& x : temp)RE2Machine(x);

        //now final state has changed again

        // int final_final_state = final_state;

        //all transitions going to the final state should now go to afs

        // for(auto &x : q)
        // {
        //     for(auto &y : x)
        //     {
        //         for(auto &z : y)
        //         {
        //             if(z == final_final_state)z = aux_final_state;
        //         }
        //     }
        // }
        
        // final_state = aux_final_state;


        //from final and aux final, make a transition to ultimate final state on seeing c(epsilon)
        global_index++;
        if(q[final_state]['c'-'a'][0] == -1)q[final_state][2].pop_back();
        q[final_state]['c'-'a'].push_back(global_index);
        if(q[aux_final_state]['c'-'a'][0] == -1)q[aux_final_state][2].pop_back();
        q[aux_final_state]['c'-'a'].push_back(global_index);

        final_state = global_index;



        left_or_completed = 0;

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
                // cout << "----------\n";
                break;
            }
            int new_state = x;
            // cout << x << '\n';
            if(is_accepted(to_check, index + 1, new_state))return true;
        }

    }
    for(auto &x : q[curr_state][2])
    {
        if(x == -1){
            // cout << "-------------\n";
            break;
        }
        int new_state = x;
        // cout << x << '\n';
        if(is_accepted(to_check, index, new_state))return true;
    }



    return false;

}

void simulate(string given, int l, int r)
{
    int n = given.length();
    // if(r == n || l == n)return;

    if(l == n)return;

    string to_check = given.substr(l, r - l + 1);
    cout << "to_check stirng : "<<to_check << '\n';
    //now I have a substring and I have to see if this is accepted

    if(is_accepted(to_check, 0, 0))
    {
        output+="$";
        output += to_check;
        simulate(given, r + 1 , n - 1);
    }
    else
    {
        //not accepted -- decrease the string
        if(l < r){
            simulate(given, l, r - 1);
        }
        else if(l == r)
        {
            output+="@";
            output+=given[l];
            simulate(given,l + 1, n - 1);
        }
    }
}



int main()
{

    //reading the input file
    fstream ifile;
    ifile.open("input.txt", ios::in);
    string reg_ex, input_string;
    if (ifile.is_open()){ 
        getline(ifile, reg_ex);
        getline(ifile, input_string);
        ifile.close();
    }


    // string checker = "(((a)(((((a)((b)*))(a))|(((b)((a)*))(b)))*))(a))";
    // cout << bracket_reducer(checker);

    // string enhanced_checker = bracket_reducer(checker);
    // string enhanced_checker = "a((ab|ba))*b" ;
    // vector<string>refined = refine_regex(enhanced_checker);
    // // found=false;
    // // cout<<found;
    // for(auto&x : refined)RE2Machine(x);
    // // is_accepted(to_check, 0, 0, found);
    // // cout<<found;

    // cout<<"\n\n";

    // simulate(to_check, 0, to_check.length() - 1, output);

    // output+='#';

    // cout << output;

    output = "";

    string no_bracket = bracket_reducer(reg_ex);
    // cout << no_bracket << '\n';

    vector<string>refined_answer = refine_regex(no_bracket);

    //build the NFA table here
    for(auto &x : refined_answer)RE2Machine(x);

    for(auto &x : refined_answer)cout << x << " ";
    cout << "\n\n";

    // // cout << "jello\n";
    for(int i = 0; i < 100; i++)
    {

        
        //row no -- state no
        for(int x : q[i][0])
        {
            if(x==-1)break;
            cout<< i << "\t"<<'a'<<"\t"<<x<<"\n";
        }
        for(int x : q[i][1])
        {
            if(x==-1)break;
            cout<< i << "\t"<<'b'<<"\t"<<x<<"\n";
        }
        for(int x : q[i][2])
        {
            if(x == -1)break;
            cout<<i <<"\t"<<'c'<<"\t"<<x<<'\n';
        }
        
    }

    // cout << is_accepted(input_string, 0, 0)<<'\n';
    simulate(input_string,0, input_string.length() - 1);

    cout << "\n\nfinalstate : "<<final_state;

    // cout << output << '\n';

    fstream ofile;
    ofile.open("output.txt", ios::out);

    ofile << output;
    ofile << "#" <<'\n';
    ofile.close();

 
    return 0;
}