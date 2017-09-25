#include<bits/stdc++.h>
#include<ctype.h>
#include<string>
#include<stdlib.h>

using namespace std;
vector<vector<int> > data;
map<int,string> attribute_name;
map<int,set<int> > attribute_val;

bool is_gen(vector<int>,vector<int> );
void init();
void load_data();
bool consistent(vector<int> , vector<int> ,int);
void candidate_elimination(vector<vector<int> > ,int );
vector<vector<int> > minimal_generlization(vector<int>,vector<int>);
vector<vector<int> > minimal_specialization(vector<int> ,vector<int> );


int main()
{
    cout<<"Initializing...\n";
    init();
    cout<<"Done.\nLoading training data.\n";
    load_data();
    cout<<"Done\n";
    for(int i=1;i<=7;i++)
    {
        cout<<"For:"<<i<<endl;
        candidate_elimination(data,i);
    }
    return 0;
}


void candidate_elimination(vector<vector<int> > training,int class_val)
{
    vector< vector<int> > Gen;
    vector< vector<int> > Spec;
    vector<int> G,S;
    for(int i=0;i<training[0].size()-1;i++)
    {
        G.push_back(-1);
        S.push_back(100);
    }
    Gen.push_back(G);
    Spec.push_back(S);
    for(int i=0;i<training.size();i++)
    {
        if(training[i][training[i].size()-1]==class_val)
        {
            //Positive example
            for(int i=0;i<Gen.size();i++)
            {
                if(!consistent(Gen[i],training[i],class_val))
                {
                    Gen.erase(Gen.begin()+i);
                    i--;
                }
            }
            for(int i=0;i<Spec.size();i++)
            {
                if(!consistent(Spec[i],training[i],class_val))
                {
                    vector<vector<int> > add=minimal_generlization(Spec[i],training[i]);
                    Spec.erase(Spec.begin()+i);
                    i--;
                    for(int j=0;j<add.size();j++)
                    {
                        bool add_flag=false;
                        for(int k=0;k<Gen.size();k++)
                        {
                            if(is_gen(Gen[k],add[j]))
                            {
                                add_flag=true;
                                break;
                            }
                        }
                        if(add_flag)
                          {
                              bool adding=true;
                              for(int k=0;k<Spec.size();k++)
                              {
                                  if(is_gen(add[j],Spec[k]))
                                  {
                                      adding=false;
                                      break;
                                  }
                                  if(is_gen(Spec[k],add[j]))
                                  {
                                      Spec.erase(Spec.begin()+k);
                                      k--;
                                  }
                              }
                                if(adding)
                                    Spec.push_back(add[j]);
                          }
                     }
                }

            }


        }
        else
        {
            //Negative example
            for(int i=0;i<Spec.size();i++)
            {
                if(!consistent(Spec[i],training[i],class_val))
                {
                    Spec.erase(Spec.begin()+i);
                    i--;
                }
            }
            for(int i=0;i<Gen.size();i++)
            {
                if(!consistent(Gen[i],training[i],class_val))
                {
                    vector<vector<int> > add=minimal_specialization(Gen[i],training[i]);
                    Gen.erase(Gen.begin()+i);
                    i--;
                    for(int j=0;j<add.size();j++)
                    {
                        bool add_flag=false;
                        for(int k=0;k<Spec.size();k++)
                        {
                            if(is_gen(add[j],Spec[k]))
                            {
                                add_flag=true;
                                break;
                            }
                        }
                        if(add_flag)
                          {
                              bool adding=true;
                              for(int k=0;k<Gen.size();k++)
                              {
                                  if(is_gen(add[j],Gen[k]))
                                  {
                                      Gen.erase(Gen.begin()+k);
                                      k--;
                                  }
                                  if(is_gen(Gen[k],add[j]))
                                  {
                                      adding=false;
                                      break;
                                  }
                              }
                                if(adding)
                                    Gen.push_back(add[j]);
                          }
                    }
                }
            }
        }
    }
    cout<<"G="<<Gen.size()<<" S="<<Spec.size()<<endl;

}
bool is_gen(vector<int> hypot1,vector<int> hypot2)
{
    //Checks if hypot1 is more general than hypot2
    if(hypot1.size()!=hypot2.size())
    {
        cout<<"Hypothesis of different sizes!"<<endl;
        return false;
    }
    bool gen=true;
    for(int i=0;i<hypot1.size();i++)
    {
        if(hypot1[i]==-1)
            continue;
        if(hypot1[i]!=hypot2[i])
        {
            gen=false;
            break;
        }
    }
    return gen;
}
vector<vector<int> > minimal_generlization(vector<int> hypothesis,vector<int> example)
{
    //return all minimal generalizations of hypothesis consistent with example
    for(int i=0;i<hypothesis.size();i++)
    {
        if(hypothesis[i]==-1)
            continue;
        if(hypothesis[i]!=example[i])
        {
            hypothesis[i]=-1;
        }
    }
    vector<vector<int> > ret;
    ret.push_back(hypothesis);
    return ret;
}
vector<vector<int> > minimal_specialization(vector<int> hypothesis,vector<int> example)
{
    //return all minimal specializations of hypothesis consistent with example
    vector<vector<int> > ret;
    for(int i=0;i<hypothesis.size();i++)
    {
        if(hypothesis[i]==example[i])
            continue;
        if(hypothesis[i]==-1)
        {

            for(set<int>::iterator it=attribute_val[i].begin();it!=attribute_val[i].end();i++)
            {
                vector<int> add=hypothesis;
                if(example[i]!=*it)
                {
                    add[i]=*it;
                    ret.push_back(add);
                    continue;
                }

            }
        }
    }
    return ret;
}
bool consistent(vector<int> hypothesis, vector<int> example,int target_class)
{
    if(example.size()!=hypothesis.size()+1)
    {
        cout<<"Error in size of hypothesis and example!"<<endl;
        return false;
    }
    int result_class=example[example.size()-1];
    bool cons=true;
    for(int i=0;i<hypothesis.size() && i<example.size()-1;i++)
    {
        if(hypothesis[i]==-1)
            continue;
        if(hypothesis[i]!=example[i])
        {
            cons=false;
            break;
        }
    }
    if((result_class==target_class && cons) || (result_class!=target_class && !cons))
        return true;
    else
        return false;
}
void init()
{
    attribute_name[0]="hair";
    attribute_name[1]="feathers";
    attribute_name[2]="eggs";
    attribute_name[3]="milk";
    attribute_name[4]="airborne";
    attribute_name[5]="aquatic";
    attribute_name[6]="predator";
    attribute_name[7]="toothed";
    attribute_name[8]="backbone";
    attribute_name[9]="breathes";
    attribute_name[10]="venomous";
    attribute_name[11]="fins";
    attribute_name[12]="legs";
    attribute_name[13]="tail";
    attribute_name[14]="domestic";
    attribute_name[15]="catsize";
    attribute_name[16]="type";

    set<int> add;
    add.insert(0);
    add.insert(1);

    attribute_val[0]=add;
    attribute_val[1]=add;
    attribute_val[2]=add;
    attribute_val[3]=add;
    attribute_val[4]=add;
    attribute_val[5]=add;
    attribute_val[6]=add;
    attribute_val[7]=add;
    attribute_val[8]=add;
    attribute_val[9]=add;
    attribute_val[10]=add;
    attribute_val[11]=add;
    attribute_val[13]=add;
    attribute_val[14]=add;
    attribute_val[15]=add;
    add.clear();
    add.insert(0);
    add.insert(2);
    add.insert(4);
    add.insert(5);
    add.insert(6);
    add.insert(8);
    attribute_val[12]=add;
    add.clear();
    add.insert(1);
    add.insert(2);
    add.insert(3);
    add.insert(4);
    add.insert(5);
    add.insert(6);
    add.insert(7);
    attribute_val[16]=add;
}

void load_data()
{
    fstream fil;
    fil.open("zoo.txt",ios::in);
    string line;
    while(getline(fil,line))
    {
       // cout<<line;
        vector<int> temp;
        int i=0;
        while(!isdigit(line[i]) && i<line.length())
            i++;
        while(i<line.length())
        {
            if(isdigit(line[i]))
            {
                char c[2];
                c[0]=line[i];
                c[1]=0;
                temp.push_back(atoi(c));
            }
            i++;

        }
       // for(int i=0;i<temp.size();i++)
           // cout<<temp[i]<<",";
        //cout<<endl;
        data.push_back(temp);

    }
    fil.close();
}
