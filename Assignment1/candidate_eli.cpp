#include<bits/stdc++.h>

using namespace std;

bool consistent(vector<int> , vector<int> ,int);
void candidate_elimination(vector<vector<int> > ,int );

int main()
{
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
        S.push_back(2);
    }
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
                    vector<vector<int> > add=minimal_generlization(Spec[i],traning[i],class_val);
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
                    vector<vector<int> > add=minimal_specialization(Gen[i],traning[i],class_val);
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

bool is_gen(vector<int> hypot1,vector<int> hypot2)
{
    //Checks if hypot1 is more general than hypot2
    if(hypot1.size()!=hypot2.size())
    {
        cout<<"Hypothesis of different sizes!"<<endl;\
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
            vector<int> add=hypothesis;
            add[i]=example[i]==0?1:0;
            ret.push_back(add);
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
    if((result_class==target_class && cons) || (result_class!==target_class && !cons))
        return true;
    else
        return false;
}
