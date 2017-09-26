#include<bits/stdc++.h>
#include<ctype.h>
#include<string>
#include<stdlib.h>

using namespace std;
vector<vector<int> > data;
map<int,string> attribute_name;
map<int,set<int> > attribute_val;

bool vector_equal(vector<int> ,vector<int> );
void print_vv(vector<vector<int> > );
void print_v(vector<int>);
bool is_gen(vector<int>,vector<int> );
void init();
void load_data();
bool consistent(vector<int> , vector<int> ,int);
void candidate_elimination(vector<vector<int> > ,int );
vector<vector<int> > minimal_generlization(vector<int>,vector<int>,vector<vector<int> >);
vector<vector<int> > minimal_specialization(vector<int> ,vector<int> ,vector<vector<int> >);


int main()
{
    cout<<"Initializing...\n";
    init();
    cout<<"Done.\nLoading training data.\n";
    load_data();
    cout<<"Done\n";
    for(int i=1;i<=1;i++)
    {
        cout<<"For:"<<i<<endl;
        candidate_elimination(data,i);
    }
    return 0;
}


void candidate_elimination(vector<vector<int> > training,int class_val)
{
    cout<<"***************************************** CLASS "<<class_val<<" ************************************************\n";
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
    //print_vv(training);
    for(int i=0;i<training.size();i++)
    {
        cout<<"----------Training example:"<<i<<"---------------"<<endl;
        cout<<"Training ex:";
        print_v(training[i]);
        if(training[i][training[i].size()-1]==class_val)
        {
            //Positive example
            cout<<"Positive!"<<endl;
            for(int j=0;j<Gen.size();j++)
            {
                if(!consistent(Gen[j],training[i],class_val))
                {
                    cout<<"Gen ";
                    print_v(Gen[j]);
                    cout<<" inconsistent!"<<endl;
                    Gen.erase(Gen.begin()+j);
                    j--;
                }
            }
            //cout<<"Gen: ";
            //print_vv(Gen);
            for(int p=0;p<Spec.size();p++)
            {
                if(!consistent(Spec[p],training[i],class_val))
                {
                    //Remove inconsistent Spec
                    cout<<"Spec inconsistent ..!"<<endl;
                    vector<int> x=Spec[p];
                    cout<<"Removing:";
                    print_v(Spec[p]);
                    cout<<"\n";
                    Spec.erase(Spec.begin()+p);
                    p--;
                    //get minimal generalization
                    vector<vector<int> > add=minimal_generlization(x,training[i],Gen);
                    print_vv(add);
                    for(int j=0;j<add.size();j++)
                        Spec.push_back(add[j]);
                    //Remove from S any hypothesis that is more general than any other
                    for(int j=0;j<Spec.size();j++)
                    {
                        for(int k=0;k<Spec.size();k++)
                        {
                            if(!vector_equal(Spec[j],Spec[k]))
                            {
                                if(is_gen(Spec[j],Spec[k]))
                                {
                                    Spec.erase(Spec.begin()+j);
                                    j--;
                                    break;
                                }
                            }
                        }
                    }
                }
                //cout<<"Spec: ";
                //print_vv(Spec);

            }


        }
        else
        {
            //Negative example
            cout<<"Negative!"<<endl;
            for(int j=0;j<Spec.size();j++)
            {
                if(!consistent(Spec[j],training[i],class_val))
                {
                    cout<<"Spec ";
                    print_v(Spec[j]);
                    cout<<" inconsistent!"<<endl;
                    Spec.erase(Spec.begin()+j);
                    j--;
                }
            }
            for(int p=0;p<Gen.size();p++)
            {
                if(!consistent(Gen[p],training[i],class_val))
                {
                    cout<<"Gen inconsistent ..!"<<endl;
                    vector<int> x=Gen[p];
                    cout<<"Removing:";
                    print_v(Gen[p]);
                    cout<<"\n";
                    Gen.erase(Gen.begin()+p);
                    p--;
                    vector<vector<int> > add=minimal_specialization(x,training[i],Spec);
                    print_vv(add);
                    for(int j=0;j<add.size();j++)
                        Gen.push_back(add[j]);
                    //Remove from G any hypothesis less general than any in G
                    for(int j=0;j<Gen.size();j++)
                    {
                        for(int k=0;k<Gen.size();k++)
                        {
                            if(!vector_equal(Gen[j],Gen[k]))
                            {
                                if(is_gen(Gen[j],Gen[k]))
                                {
                                    print_v(Gen[j]);
                                    cout<<" more gen than ";
                                    print_v(Gen[k]);
                                    cout<<" .Removing\n";
                                    Gen.erase(Gen.begin()+k);
                                    k--;
                                    break;
                                }
                            }
                        }
                    }

                }
            }
        }
        cout<<"Gen_fin:";
        print_vv(Gen);
        cout<<"Spec_fin:";
        print_vv(Spec);
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
        if(hypot1[i]==-1 || hypot2[i]==100)
            continue;
        if(hypot1[i]!=hypot2[i])
        {
            gen=false;
            break;
        }
    }
    return gen;
}
vector<vector<int> > minimal_generlization(vector<int> hypothesis,vector<int> example,vector<vector<int> > Gen)
{
    //return all minimal generalizations of hypothesis consistent with example
    cout<<"Min-gen"<<endl;
    for(int i=0;i<hypothesis.size();i++)
    {
        if(hypothesis[i]==100)
            hypothesis[i]=example[i];
        if(hypothesis[i]==-1)
            continue;
        if(hypothesis[i]!=example[i])
        {
            hypothesis[i]=-1;
        }
    }
    vector<vector<int> > ret;
    bool add=false;
    for(int i=0;i<Gen.size();i++)
    {
        if(is_gen(Gen[i],hypothesis))
        {
            add=true;
            break;
        }
    }
    if(add)
        ret.push_back(hypothesis);
    return ret;
}
vector<vector<int> > minimal_specialization(vector<int> hypothesis,vector<int> example,vector<vector<int> > Spec)
{
    //return all minimal specializations of hypothesis consistent with example
    cout<<"Min-spec"<<endl;
    vector<vector<int> > ret;
    for(int i=0;i<hypothesis.size();i++)
    {
        if(hypothesis[i]==example[i])
            continue;
        if(hypothesis[i]==-1)
        {

            for(set<int>::iterator it=attribute_val[i].begin();it!=attribute_val[i].end();it++)
            {
                vector<int> add=hypothesis;
                if(example[i]!=*it)
                {
                    add[i]=*it;
                    bool flag=false;
                    for(int i=0;i<Spec.size();i++)
                    {
                        if(is_gen(hypothesis,Spec[i]))
                        {
                            flag=true;
                            break;
                        }
                    }
                    if(flag)
                        ret.push_back(add);
                    continue;
                }

            }
        }
    }
    return ret;
}
bool consistent(vector<int> hypothesis, vector<int> ex,int target_class)
{
    //cout<<"hypo:";
    //print_v(hypothesis);
    //cout<<"example:";
    // print_v(ex);
    if(ex.size()!=hypothesis.size()+1)
    {
        cout<<"Error in size of hypothesis and example!"<<endl;
        return false;
    }
    int result_class=ex[ex.size()-1];
    bool cons=true;
    for(int i=0;i<hypothesis.size() && i<ex.size()-1;i++)
    {
        if(hypothesis[i]==-1)
            continue;
        if(hypothesis[i]!=ex[i])
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
void print_vv(vector<vector<int> >  add)
{
    for(int p=0;p<add.size();p++)
    {
        for(int d=0;d<add[p].size();d++)
            cout<<add[p][d]<<",";
            cout<<endl;
    }
}
void print_v(vector<int> add)
{
    for(int p=0;p<add.size();p++)
    {
            cout<<add[p]<<",";
    }
     //cout<<endl;
}
bool vector_equal(vector<int> a,vector<int> b)
{
    if(a.size()!=b.size())
        return false;
    for(int i=0;i<a.size();i++)
    {
        if(a[i]!=b[i])
            return false;
    }
    return true;
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
