#include <iostream>
#include <set>
#include <algorithm>
#include <vector>

using namespace std;

struct node
{
    pair <int, int> attribute; //attribute #, and number of values
    vector <node*> nextAtt;
} valTrue, valFalse;

void loadData(vector <vector <int> > &data); //for fixing continuous data and ? and marking all attributes from 0 to n -> load the data
void setAttributes(set <pair <int, int> > &attributes); //just initializes all the attributes... nothing much (have to hard code this)
node *ID3(vector <vector <int> > &data, set <pair <int, int> > &attributes);
node *checkSame(vector <vector <int> > &data); //returns valTrue/valFalse if all the data have the same val, otherwise returns NULL
node *doVoting(vector <vector <int> > &data); //returns valTrue/valFalse based on voting
double calcEntropy(vector <vector <int> > &data, pair <int, int> &attribute);
void cutData(vector <vector <int> > &data, int, int, vector <vector <int> > &vdata); //also removes the attribute maxAtt from data
void printTree(node*);

int main()
{
    vector <vector <int> > data;
    loadData(data);
    set <pair <int, int> > attributes; //first int is the attribute number and the second is the number of values of the attribute
    setAttributes(attributes);
    node *root = ID3(data, attributes);
    printTree(root);
}

void loadData(vector <vector <int> > &data)
{

}

void setAttributes(set <pair <int, int> > &attributes)
{
    vector <int> numValues = {1, 8, 1, 16, 7, 14, 6, 5, 2, 1, 1, 1, 41, 2};
    for (int i = 0; i < numValues.size(); ++i)
    {
        pair <int, int> attribute = pair <int, int> (i, numValues[i]);
        attributes.insert(attribute);
    }
}

node *ID3(vector <vector <int> > &data, set <pair <int, int> > &attributes)
{
    node *check = checkSame(data);
    if (check != NULL)
        return check;
    if (attributes.empty())
        return doVoting(data);
    //now the not so trivial part

    //declarations
    double maxEntropy = 0, entropy;
    node *root = new node();
    pair <int, int> bestAtt;
    set <pair <int, int> > newAttributes = attributes; //for: attributes - {bestAtt}

    //calculating the optimal attribute to make a root
    for (auto attribute: attributes)
    {
        entropy = calcEntropy(data, attribute); //returns the entropy
        if (entropy > maxEntropy)
        {
            maxEntropy = entropy;
            bestAtt = attribute;
        }
    }
    root->attribute = bestAtt;

    //now to make its children
    newAttributes.erase(bestAtt);
    for (int val = 0; val < bestAtt.second; ++val) //iterating through all the possible values of the attribute
    {
        vector <vector <int> > vdata;
        cutData(data, bestAtt.first, val, vdata);
        if (vdata.size() <= 1)
            root->nextAtt.push_back(doVoting(data));
        else //recursive call
            root->nextAtt.push_back(ID3(vdata, newAttributes));
    }
    return root;
}

node *checkSame(vector <vector <int> > &data)
{
    int lastCol = data[0].size() - 1;
    bool result = data[1][lastCol];
    for (auto record: range(data.begin() + 1, data.end()))
    {
        if (record[lastCol] != result)
            return NULL;
    }
    return result? valTrue: valFalse;
}

node *doVoting(vector <vector <int> > &data)
{
    int count = 0;
    int lastCol = data[0].size() - 1;
    for (auto record: range(data.begin() + 1, data.end()))
        count += 2 * record[lastCol] - 1;
    return count > 0? valTrue: valFalse;
}

double calcEntropy(vector <vector <int> > &data, pair <int, int> &attribute)
{
    int countTrue = 0, countFalse = 0;
    int lastCol = data[0].size() - 1;
    for (auto record: range(data.begin() + 1, data.end()))
    {
        countTrue += record[lastCol];
        countFalse += 1 - record[lastCol];
    }
    double p = countTrue / (countTrue + countFalse);
    return -1 * p * log2(p) - (1 - p) * log2(1 - p);
}

void cutData(vector <vector <int> > &data, int attNum, int val, vector <vector <int> > &vdata)
{

}

void printTree(node*)
{

}


