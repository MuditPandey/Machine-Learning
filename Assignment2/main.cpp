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
double calcEntropyGain(vector <vector <int> > &data, pair <int, int> &attribute);
double calcEntropy(int, int);
void cutData(vector <vector <int> > &data, int, int, vector <vector <int> > &vdata); //also removes the attribute maxAtt from data
void printTree(node*, int tabs = 0);
void printData(vector <vector <int> >);

int main()
{
    valTrue.attribute.first = -1;
    valFalse.attribute.first = -2;

    vector <vector <int> > data;
    loadData(data);
    set <pair <int, int> > attributes; //first int is the attribute number and the second is the number of values of the attribute
    setAttributes(attributes);
    node *root = ID3(data, attributes);
    printTree(root);
}

void loadData(vector <vector <int> > &data)
{
    data = {
        {0, 1, 2, 3, 4}, //attribute numbers
        {1, 0, 0, 0, 0},
        {1, 0, 0, 1, 0},
        {2, 0, 0, 0, 1},
        {0, 2, 0, 0, 1},
        {0, 1, 1, 0, 1},
        {0, 1, 1, 1, 0},
        {2, 1, 1, 1, 1},
        {1, 2, 0, 0, 0},
        {1, 1, 1, 0, 1},
        {0, 2, 1, 0, 1},
        {1, 2, 1, 1, 1},
        {2, 2, 0, 1, 1},
        {2, 0, 1, 0, 1},
        {0, 2, 0, 1, 0}
    };
}

void setAttributes(set <pair <int, int> > &attributes)
{
    vector <int> numValues = {3, 3, 2, 2, -1};
    //{1, 8, 1, 16, 7, 14, 6, 5, 2, 1, 1, 1, 41, 2};
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
    double splitEntropy = 10000, entropy;
    node *root = new node();
    pair <int, int> bestAtt;
    set <pair <int, int> > newAttributes = attributes; //for: attributes - {bestAtt}

    //calculating the optimal attribute to make a root
    for (auto attribute: attributes)
    {
        entropy = calcEntropyGain(data, attribute); //returns the entropy
        if (entropy < splitEntropy)
        {
            splitEntropy = entropy;
            bestAtt = attribute;
        }
    }
    root->attribute = bestAtt;
    cout << "bestAtt found! " << bestAtt.first << ", Entropy " << splitEntropy << endl;

    //now to make its children
    newAttributes.erase(bestAtt);
    for (int val = 0; val < bestAtt.second; ++val) //iterating through all the possible values of the attribute
    {
        vector <vector <int> > vdata;
        cutData(data, bestAtt.first, val, vdata);
        cout << "__________DATA(" << bestAtt.first << "=" << val << "_________" << endl;
        printData(vdata);
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
    for (int recordNum = 1; recordNum < data.size(); ++recordNum)
    {
        if (data[recordNum][lastCol] != result)
            return NULL;
    }
    return result? &valTrue: &valFalse;
}

node *doVoting(vector <vector <int> > &data)
{
    int count = 0;
    int lastCol = data[0].size() - 1;
    for (int recordNum = 1; recordNum < data.size(); ++recordNum)
        count += 2 * data[recordNum][lastCol] - 1;
    return count > 0? &valTrue: &valFalse;
}

double calcEntropy(int countTrue, int countFalse)
{
    if (countTrue == 0 || countFalse == 0) return 0;
    double p = (double) countTrue / (countTrue + countFalse);
    p = -1 * p * log2(p) - (1 - p) * log2(1 - p);
    return p * (countTrue + countFalse);
}

double calcEntropyGain(vector <vector <int> > &data, pair <int, int> &attribute)
{
    if (attribute.second == -1) return 10000;
    vector <pair <int, int> > table(attribute.second); //first int is true, second is false
    int lastCol = data[0].size() - 1, attCol;
    for (attCol = 0; attCol < lastCol; ++attCol)
        if (data[0][attCol] == attribute.first)
            break;
    for (int recordNum = 1; recordNum < data.size(); ++recordNum)
    {
        if (data[recordNum][lastCol] == 1)
            table[data[recordNum][attCol]].first += 1;
        else
            table[data[recordNum][attCol]].second += 1;
    }
    double sum = 0;
    cout << "****************Table (att" << attribute.first << ")**************" << endl;
    for (int val = 0; val < attribute.second; ++val)
    {
        cout << "val" << val << "-> true: " << table[val].first << ", false: " << table[val].second;
        cout << ", entropy: " << calcEntropy(table[val].first, table[val].second) << endl;
    }

    cout << endl;
    for (int val = 0; val < attribute.second; ++val)
        sum += calcEntropy(table[val].first, table[val].second);
    return sum;
}

void cutData(vector <vector <int> > &data, int attNum, int val, vector <vector <int> > &vdata)
{
    int attCol;
    vector <int> attNums;
    for (attCol = 0; attCol < data[0].size(); ++attCol)
        if (attNum == data[0][attCol])
            break;
    for (int col = 0; col < data[0].size(); ++col)
        if (col != attCol)
            attNums.push_back(data[0][col]);
    vdata.push_back(attNums);
    for (int recordNum = 1; recordNum < data.size(); ++recordNum)
    {
        vector <int> newRecord;
        if (data[recordNum][attCol] == val)
        {
            for (int col = 0; col < data[recordNum].size(); ++col)
            {
                if (col != attCol)
                    newRecord.push_back(data[recordNum][col]);
            }
            vdata.push_back(newRecord);
        }
    }
}

void printTree(node* root, int tabs)
{
    for (int tab = 0; tab < tabs; ++tab)
        cout << "  ";
    cout << root->attribute.first << endl;
    for (auto child: root->nextAtt)
        printTree(child, tabs + 1);
}

void printData(vector <vector <int> > x)
{
    for (int i = 0; i < x.size(); ++i)
    {
        for (int j = 0; j < x[0].size(); ++j)
        {
            cout << x[i][j] << " ";
        }
        cout << endl;
    }
}


