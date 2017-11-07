#include <bits/stdc++.h>
#include<ctype.h>
using namespace std;
bool is_stopword(string word)
{
    return false;
}
void load_count(string filename,map<string,int> &c_map )
{
    fstream fil;
    string path;
    if(filename=="pos.txt")
        path="C:\\Users\\mudit\\Desktop\\Data\\Study\\ML\\Assignment-3\\aclImdb_v1\\aclImdb\\test\\pos";
    else
        path="C:\\Users\\mudit\\Desktop\\Data\\Study\\ML\\Assignment-3\\aclImdb_v1\\aclImdb\\test\\neg";
    fil.open(filename,ios::in);
    string line;
    while(getline(fil,line))
    {
        cout<<line<<endl;
        fstream fil2;
        string file=path+"\\"+line;
        cout<<"File:"<<file<<endl;
        fil2.open(file,ios::in);
        string str;
        map<string,int> add;
        while(getline(fil2,str))
        {
            int i=0;
            while(i<str.length())
            {
                int start=i;
                while((isalnum(str[i]) || str[i]=='-') && str[i]!=0)
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
void naive_baiyes(string filename,map<string,int> &p_map,map<string,int> &n_map)
{

}
int main()
{
    map<string,int> pos;
    map<string,int> neg;
    load_count("pos.txt",pos); //filename =list of all positive
    load_count("neg.txt",neg)
    //load_count("filename_neg_list.txt",neg); //filename =list of all negative
   for(map<string,int>::iterator it=pos.begin();it!=pos.end();it++)
    {
        cout<<it->first<<" "<<it->second<<endl;
    }
    string testfile="test.txt";
    naive_baiyes(testfile,pos,neg);
    return 0;
}


