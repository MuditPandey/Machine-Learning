#include <bits/stdc++.h>
#include <stdlib.h>
#include <ctype.h>
using namespace std;
struct result
{
    int tp;
    int tn;
    int fp;
    int fn;
};
bool is_stopword(string word)
{
    return false;
}
void load_count(string filename,map<string,int> &c_map,int &examples)
{
    fstream fil;
    string path;
    if(filename=="pos.txt")
        path="C:\\Users\\mudit\\Desktop\\Data\\Study\\ML\\Assignment-3\\aclImdb_v1\\aclImdb\\train\\pos";
    else
        path="C:\\Users\\mudit\\Desktop\\Data\\Study\\ML\\Assignment-3\\aclImdb_v1\\aclImdb\\train\\neg";
    fil.open(filename,ios::in);
    string line;
    while(getline(fil,line))
    {
        examples++;
        system("cls");
        cout<<"Loading count\n";
        cout<<"Progress:"<<floor((double)examples/12500*100)<<"%"<<endl;
        //cout<<line<<endl;
        fstream fil2;
        string file=path+"\\"+line;
        //cout<<"File:"<<file<<endl;
        fil2.open(file,ios::in);
        string str;
        map<string,int> add;
        while(getline(fil2,str))
        {
            int i=0;
            while(i<str.length())
            {
                int start=i;
                while(isalnum(str[i])&& str[i]!=0)
                 i++;
                string word=str.substr(start,i-start);
                //cout<<"Word:="<<word<<"."<<endl;
                if(!is_stopword(word))
                    add[word]=1;
                while(!isalnum(str[i])&& str[i]!=0)
                    i++;
            }
        }
        fil2.close();
        for(map<string,int>::iterator it=add.begin();it!=add.end();it++)
        {
            if(c_map.find(it->first)!=c_map.end())
                c_map[it->first]++;
            else
                c_map[it->first]=1;
        }
    }
     fil.close();
}
bool naive_baiyes(string filename,map<string,int> &p_map,map<string,int> &n_map,int &pos_count,int &neg_count,result &ans)
{
    fstream fil2;
    string path;
    bool pos=false;
    if(filename=="test_pos.txt")
    {
        path="C:\\Users\\mudit\\Desktop\\Data\\Study\\ML\\Assignment-3\\aclImdb_v1\\aclImdb\\test\\pos";
        pos=true;
    }
    else
        path="C:\\Users\\mudit\\Desktop\\Data\\Study\\ML\\Assignment-3\\aclImdb_v1\\aclImdb\\test\\neg";
    fil2.open(filename,ios::in);
    string line;
    while(getline(fil2,line))
    {
        //cout<<line<<endl;
        fstream fil2;
        string file=path+"\\"+line;
        cout<<"File:"<<file<<endl;
        fstream fil;
        fil.open(filename,ios::in);
        string str;
        double prob_pos=(double)pos_count/(pos_count+neg_count);
        //cout<<"PROB_POS:"<<prob_pos;
        double prob_neg=(double)neg_count/(pos_count+neg_count);
        //cout<<" PROB_NEG:"<<prob_neg<<endl;
        while(getline(fil,str))
        {
            int i=0;
            while(i<str.length())
            {
                int start=i;
                while(isalnum(str[i]) && str[i]!=0)
                    i++;
                string word=str.substr(start,i-start);
                //cout<<"Word:="<<word<<"."<<endl;
                if(!is_stopword(word))
                {
                       if(p_map.find(word)!=p_map.end())
                        prob_pos*=((double)p_map[word]/pos_count);
                       if(n_map.find(word)!=n_map.end())
                        prob_neg*=((double)n_map[word]/neg_count);
                }
                while(!isalnum(str[i])&& str[i]!=0)
                    i++;
            }
        }
        fil.close();
        cout<<"Prob_pos:"<<prob_pos<<endl;
        if(prob_pos > prob_neg && pos)
        {
            ans.tp++;
        }
        else if(prob_pos <= prob_neg && pos)
        {
            ans.fn++;
        }
        else if(prob_pos < prob_neg && !pos)
        {
            ans.tn++;
        }
        else if(prob_pos > prob_neg && !pos)
        {
            ans.fp++;
        }
    }
    fil2.close();
}

void get_results(result &ans)
{
    //Put correct formulas
    cout<<"Accuracy : "<<(double)(ans.tp+ans.tn)/(ans.tp+ans.tn+ans.fp+ans.fn)<<endl;
    cout<<"Precision : "<<1<<endl;
    cout<<"Recall : "<<1<<endl;
    cout<<"F-Measure : "<<1<<endl;
}
int main()
{
    map<string,int> pos;
    map<string,int> neg;

    int pos_count=0,neg_count=0;
    load_count("pos.txt",pos,pos_count); //filename =list of all positive
    load_count("neg.txt",neg,neg_count);
    //load_count("filename_neg_list.txt",neg); //filename =list of all negative
    cout<<"Printing count:\n";
   /*for(map<string,int>::iterator it=pos.begin();it!=pos.end();it++)
    {
        cout<<it->first<<" "<<it->second<<endl;
    }*/
    result final_ans;
    final_ans.tp=0;
    final_ans.tn=0;
    final_ans.fp=0;
    final_ans. fn=0;
    string testfile="test_pos.txt";
    cout<<"Testing neg\n";
    naive_baiyes(testfile,pos,neg,pos_count,neg_count,final_ans);
    testfile="test_neg.txt";
    cout<<"Testing pos\n";
    naive_baiyes(testfile,pos,neg,pos_count,neg_count,final_ans);
    get_results(final_ans);
    return 0;
}


