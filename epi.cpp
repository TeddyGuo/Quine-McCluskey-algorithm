#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <set>
#include <iomanip>

using namespace std;

void to1or0(string, vector<string>&);
void combine(vector<vector<string> >&, set<string>&, int, int);
int check_epi(map<string, vector<bool> >&, vector<string>&, int);

// Turn '-' to 0 or 1
void to1or0(string str, vector<string>& vec)
{
    size_t posi = str.find("-");
    if (str.find("-") != string::npos)
    {
        str[posi] = '0';
        to1or0(str, vec);
        str[posi] = '1';
        to1or0(str, vec);
    }
    else
        vec.push_back(str);
}
void combine(vector<vector<string> >& group, set<string>& prime_set, int count, int varcount)
{
    int i = 0, j = 0, k = 0, posi = 0;
    string str = "";
    int same = 0; // count the same digit for two minterm
    int bigsame = 0;
    vector<string> vec; // store new group

    if (count == 0) return;

    for (j = 0; j < group[0].size(); j++)
    {
        bigsame = 0;
        for (k = 0; k < group[1].size(); k++)
        {
            posi = 0;
            while (posi < varcount)
            {
                if (group[0][j][posi] == group[1][k][posi])
                {
                    same += 1;
                    str += group[0][j][posi];
                }
                else
                {
                    str += '-';
                }
                posi++;
            }
            if (same == varcount - 1)
            {
                if (str != "")
                    vec.push_back(str);
                bigsame = 1;
            }

            same = 0; str = ""; // reset
        }

        if (bigsame == 0) prime_set.insert(group[0][j]);
    }

    // erase first element and push back new vector
    group.erase(group.begin());
    group.push_back(vec);

    // test output
    // for (int a = 0; a < vec.size(); a++)
    // {
    //     cout << vec[a] << " ";
    // }
    // cout << endl;

    vec.clear(); // reset

    combine(group, prime_set, count-1, varcount);
}
int check_epi(map<string, vector<bool> >& succ, vector<string>& prime, int posi) // position
{
    int i = 0;
    int index = -1, count = 0; // store index of epi and the count of 1 occurrance

    for (i = 0; i < succ.size(); i++)
    {
        if (succ[prime[i]][posi] == true)
        {
            count += 1;
            index = i;
        }
        if (count >= 2) return -1;
    }
    if (count == 1) return index;

    return -1;
}

int main(int argc, char** argv)
{
    // recording info like .i, .o, .p, etc
    map<string, int> parInt;
    map<string, string> parStr;
    // minterm
    set<string> m_set;
    // func output 1
    vector<string> m_success;
    // function outcome
    map<string, string> f;
    map<string, string> dontcare;
    // different group
    vector<vector<string> > group;
    // dont-care vector
    vector<string> dont;
    // epi possible set
    set<string> prime_set;
    vector<string> prime;
    // map for a boolean vector storing whether a term containing how many successful minterm
    map<string, vector<bool> > succ;
    // epi
    vector<string> epi;

    fstream in(argv[1], ios::in);
    fstream out(argv[2], ios::out);

    // temp variable
    string line2 = "";
    int i = 0, j = 0, k = 0;

    // wrangling the input file
    string line = "";
    while (getline(in, line, '\n'))
    {
        // check whether the line has something
        if (line[0] == '.')
        {
            if (line == ".e")
            {
                // do nothing
            }
            else if (line.find(".i") != string::npos && line.find(".ilb") == string::npos)
            {
                parInt[".i"] = atoi(line.substr(line.find(".i") + 3).c_str() );
            }
            else if (line.find(".o") != string::npos && line.find(".ob") == string::npos)
            {
                parInt[".o"] = atoi(line.substr(line.find(".o") + 3).c_str() );

                // cout << parInt[".o"] << endl;
            }
            else if (line.find(".ilb") != string::npos)
            {
                parStr[".ilb"] = line.substr(line.find(".ilb") + 5);

                // cout << parStr[".ilb"] << endl;
            }
            else if (line.find(".ob") != string::npos)
            {
                parStr[".ob"] = line.substr(line.find(".ob") + 4);

                // cout << parStr[".ob"] << endl;
            }
            else if (line.find(".p") != string::npos)
            {
                parInt[".p"] = atoi(line.substr(line.find(".p") + 3).c_str() );

                // cout << parInt[".p"] << endl;
            }
            else
                cout << "Something strange. You'd better check your input file." << endl;
        }
        else
        {
            istringstream iss(line);

            // var part; f part
            iss >> line; iss >> line2;
            // store minterm info
            vector<string> vec;
            string str = line;
            to1or0(str, vec);
            for (i = 0; i < vec.size(); i++)
            {
                if (line2 == "-")
                {
                    dontcare[vec[i]] = "-";
                    dont.push_back(vec[i]);
                }
                else if (line2 == "1")
                {
                    f[vec[i]] = "1";
                    m_success.push_back(vec[i]);
                    // test output
                    // cout << m_success[m_success.size() - 1] << endl;
                    // 13 success
                }
                else
                    f[vec[i]] = line2;

                m_set.insert(vec[i] );
            }

            str = ""; // reset
        }
    }

    // test output
    // for (set<string>::iterator it = m_set.begin(); it != m_set.end(); it++)
    // {
    //     cout << *it << " " << f[*it] << endl;
    // }

    // Step 1: finding prime implicants
    for (i = 0; i <= parInt[".i"]; i++)
    {
        vector<string> vec;
        group.push_back(vec);
    }
    for (set<string>::iterator it = m_set.begin(); it != m_set.end(); it++)
    {
        string str = *it;
        group[count(str.begin(), str.end(), '1') ].push_back(*it);
    }

    // test output for step 1 - 1
    // cout << "Number of one's" << endl;
    // for (i = 0; i <= parInt[".i"]; i++)
    // {
    //     cout << i << setw(5);
    //     for (j = 0; j < group[i].size(); j++)
    //     {
    //         cout << group[i][j] << " ";
    //     }
    //     cout << endl;
    // }

    // Start to combine
    i = 0;
    while (i < parInt[".i"])
    {
        combine(group, prime_set, parInt[".i"], parInt[".i"]);
        i++;
    }
    // put prime_set elements into prime vector
    // && test output
    for (set<string>::iterator it = prime_set.begin(); it != prime_set.end(); it++)
    {
        if (*it != "")
            prime.push_back(*it);
    }
    // test output 
    // for (i = 0; i < prime.size(); i++) cout << prime[i] << " ";
    // cout << endl;

    // Step 2: prime implicant chart
    // success
    for (i = 0; i < prime.size(); i++)
    {
        vector<string> vec;
        to1or0(prime[i], vec);
        for (j = 0; j < m_success.size(); j++)
        {
            succ[prime[i]].push_back(false); // initial
            
            for (k = 0; k < vec.size(); k++)
            {
                if (m_success[j] == vec[k]) succ[prime[i]][j] = true;
            }
        }
    }
    // test output
    // for (i = 0; i < prime.size(); i++)
    // {
    //     for (j = 0; j < succ[prime[i]].size(); j++)
    //     {
    //         cout << succ[prime[i]][j] << " ";
    //     }
    //     cout << endl;
    // }
    // do more
    for (i = 0; i < m_success.size(); i++)
    {
        int epinum = check_epi(succ, prime, i);
        if (epinum != -1)
        {
            epi.push_back(prime[epinum]);
        }
    }
    sort(epi.begin(), epi.end());
    // write data into output
    out << ".i " << parInt[".i"] << "\n";
    out << ".o " << parInt[".o"] << "\n";
    out << ".ilb " << parStr[".ilb"] << "\n";
    out << ".ob " << parStr[".ob"] << "\n";
    out << ".p " << epi.size() << "\n";
    // write epi into output
    for (i = 0; i < epi.size(); i++)
    {
        out << epi[i] << " 1\n";
    }

    // end of output
    out << ".e\n";

    return 0;
}