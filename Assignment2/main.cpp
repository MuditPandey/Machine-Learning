#include <iostream>
#include <set>

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
node *calcEntropy(vector <vector <int> > &data, set <pair <int, int> > &attributes);
void cutData(vector <vector <int> > &data, int, int, vector <vector <int> > &vdata); //also removes the attribute maxAtt from data
void printTree(node*)

int main()
{
    vector <vector <int> > data;
    loadData(data);
    set <pair <int, int> > attributes; //first int is the attribute number and the second is the number of values of the attribute
    setAttributes(attributes);
    node *root = ID3(attributes);
    printTree(root);
}

node *ID3(data, attributes)
{
    node *check = checkSame(data);
    if (check != NULL)
        return check;
    if (attributes.empty())
        return doVoting(data);
    //now the not so trivial part

    //declarations
    int maxEntropy = 0, entropy;
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
        if (vdata.empty())
            root->nextAtt.push_back(doVoting(data));
        else //recursive call
            root->nextAtt.push_back(ID3(vdata, newAttributes));
    }
    return root;
}
