#include <fstream>
#include <sstream>
#include <iostream>
#include <math.h>
#include <set>
#include <algorithm>
#include <vector>
#include <time.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#define FOREST_SIZE 5
#define VALRATIO 0.3

using namespace std;

struct node
{
    pair <int, int> attribute; //attribute #, and number of values
    vector <node*> nextAtt;
    bool guess;
} valTrue, valFalse;

void loadData(vector <vector <int> > &data, set <pair <int, int> > &attributes, bool test = false); //for fixing continuous data and ? and marking all attributes from 0 to n -> load the data
node *ID3(vector <vector <int> > &data, set <pair <int, int> > &attributes,bool random_forest=false);
node *checkSame(vector <vector <int> > &data); //returns valTrue/valFalse if all the data have the same val, otherwise returns NULL
node *doVoting(vector <vector <int> > &data); //returns valTrue/valFalse based on voting
double calcEntropyGain(vector <vector <int> > &data, pair <int, int> &attribute);
double calcEntropy(int, int);
void cutData(vector <vector <int> > &data, int, int, vector <vector <int> > &vdata); //also removes the attribute maxAtt from data
void printTree(node*, int tabs = 0);
void printData(vector <vector <int> > &);
void discretizeData(int attNum, vector <vector <int> > &data);
void fillMissingData(vector <vector <int> > &data, set <pair <int, int> > &attributes);
double testData(vector <vector <int> > &testData, vector <node*> root, bool printDetails = false);
void get_new_data(vector<vector<int> > &data, vector<vector <int> > &sampleData);
set<pair<int,int> > get_new_attributes(set< pair<int,int> > &att);
void get_validation_set(vector<vector<int> > &data, vector <vector <int> > &validationData);
double reduced_error_pruning(vector <vector <int> > &vdata, node *, node *, double acc);
int treeSize(node *root);

void space_modifier()
{
    ifstream fin;
    ofstream fout;
    fin.open("test.txt");
    fout.open("test_output.txt");
    string str;
    while(getline(fin, str))
    {
        bool space = false;
        for (int i = 0; i < str.size(); ++i)
        {
            if ((str[i] == ' ' || str[i] == '\t') && !space)
            {
                fout << ",";
                space = true;
            }
            else if (str[i] != ' ' && str[i] != '\t')
            {
                if (str[i] != ',' && str[i] != '.')
                    fout << str[i];
                space = false;
            }
        }
        fout << endl;
    }
    fin.close();
    fout.close();
}

int main()
{
    valTrue.attribute.first = -1;
    valFalse.attribute.first = -2;

    //space_modifier();
    //return 0;

    vector <vector <int> > data, dataTest;
    set <pair <int, int> > attributes; //first int is the attribute number and the second is the number of values of the attribute
    cout << "preprocessing given data" << endl;
    loadData(data, attributes);
    int ch;
    bool again = false;
    double acc;

    do{
        cout << "1.ID3\n2.ID3 with pruning\n3.ID3 with random forest\nChoice? ";
        cin >> ch;
        cout << endl;
        vector <node *> rootTree;
        vector<vector<int> > validationData;
        node* root;
        time_t t;
        double time_taken;
        switch(ch)
        {
            case 1: again = false;
                    cout << "running ID3" << endl;
                    t = clock();
                    root = ID3(data, attributes);
                    time_taken=((double)clock()-t)/CLOCKS_PER_SEC;
                    cout << "    Time taken for training:" << time_taken << endl;
                    cout << "preprocessing test data" << endl;
                    loadData(dataTest, attributes, true);
                    cout << "testing data with decision tree" << endl;
                    rootTree.push_back(root);
                    testData(dataTest, rootTree, true);
                    break;

            case 2: again = false;
                    cout << "Partitioning validation set" << endl;
                    get_validation_set(data, validationData);
                    //build normal tree using modified grow set
                    cout << "Running ID3" << endl;
                    root = ID3(data, attributes);
                    cout << "    Tree size: " << treeSize(root) << endl;
                    cout << "preprocessing test data" << endl;
                    loadData(dataTest, attributes, true);
                    cout << "testing data with decision tree" << endl;
                    rootTree.push_back(root);
                    cout << "    Before pruning: " << endl;
                    testData(dataTest, rootTree, true);
                    cout << "Pruning tree for optimum accuracy" << endl;
                    t = clock();
                    cout << "    Optimized Accuracy on validation set: " << reduced_error_pruning(validationData, root, root,acc) << endl;
                    time_taken=((double)clock()-t)/CLOCKS_PER_SEC;
                    cout << "    Total Time taken for training: " << time_taken << endl;
                    rootTree.clear();
                    rootTree.push_back(root);
                    cout << "    Accuracy on test data: " << testData(dataTest, rootTree, true) << endl;
                    cout << "    Tree size: " << treeSize(root) << endl;
                    break;
            case 3:
                    again = false;
                    cout << "running ID3 with random forest" << endl;
                    t = clock();
                    for(int i=0;i<FOREST_SIZE;i++)
                    {
                        cout<<"    Making TREE:"<<i<<endl;
                        vector< vector<int> > sampled_data;
                        get_new_data(data, sampled_data);
                        rootTree.push_back(ID3(sampled_data, attributes,true));
                    }
                    time_taken=((double)clock()-t)/CLOCKS_PER_SEC;
                    cout<<"Time taken for training:"<<time_taken<<endl;
                    cout << "preprocessing test data" << endl;
                    loadData(dataTest, attributes, true);
                    cout << "testing data with random forest" << endl;
                    cout << "Results on test data: " << endl;
                    testData(dataTest, rootTree, true);
                    break;
            default:
                    cout<<"Invalid Choice!\n";
                    again=true;
        }
    }while(again);

}

int treeSize(node *root)
{
    int sum = 0;
    for (auto child: root->nextAtt)
    {
        if (child == &valTrue || child == &valFalse)
            continue;
        sum += treeSize(child);
    }
    return sum + 1;
}

double reduced_error_pruning(vector <vector <int> > &vdata, node *root, node *n, double maxAcc)
{
    for (int child = 0; child < n->nextAtt.size(); ++child)
    {
        if (n->nextAtt[child] == &valTrue || n->nextAtt[child] == &valFalse)
            continue;
        double newAcc = reduced_error_pruning(vdata, root, n->nextAtt[child], maxAcc);
        if (newAcc > maxAcc)
            maxAcc = newAcc;
    }
    vector <node *> childBuffer;
    move(n->nextAtt.begin(), n->nextAtt.end(), back_inserter(childBuffer));
    n->nextAtt.resize(1);
    n->nextAtt[0] = n->guess? &valTrue: &valFalse;

    vector <node *> rootForest;
    rootForest.push_back(root);
    double currAcc = testData(vdata, rootForest);

    if (currAcc > maxAcc)
        return currAcc;

    n->nextAtt.clear();
    move(childBuffer.begin(), childBuffer.end(), back_inserter(n->nextAtt));
    return maxAcc;
}

set<pair<int,int> > get_new_attributes(set< pair<int,int> > &att)
{
    vector<pair<int,int> > temp;
    set<pair<int,int> > ret;
    for(set<pair<int,int> >::iterator it=att.begin();it!=att.end();it++)
    {
        if (it->second == -1)
            continue;
        pair<int,int> add;
        add.first=it->first;
        add.second=it->second;
        temp.push_back(add);
    }
    srand(time(NULL));
    int sizes=0;
    while(sizes!=(int)sqrt(att.size() - 1))
    {
        int index=rand()%temp.size();
        ret.insert(temp[index]);
        temp.erase(temp.begin()+index);
        sizes++;
    }
    return ret;
}

void get_new_data(vector<vector<int> > &data, vector <vector <int> > &ret)
{
    srand(time(NULL));
    for(int i=0;i<data.size();i++)
    {
        int index=rand()%(data.size() - 1) + 1;
        ret.push_back(data[index]);
    }
    ret.insert(ret.begin(), data[0]);
}

double testData(vector <vector <int> > &dataTest, vector <node *> rootTree, bool printDetails)
{
    int tp = 0, fn = 0, fp = 0, tn = 0, lastCol = dataTest[0].size() - 1;
    for (int i = 1; i < dataTest.size(); ++i)
    {
        int vote = 0;
        for (auto root: rootTree)
        {
            node *traverse = root;
            while (traverse->attribute.first >= 0)
                traverse = traverse->nextAtt[dataTest[i][traverse->attribute.first]];

            vote += 2*traverse->attribute.first + 3;
        }
        if (vote > 0 && dataTest[i][lastCol] == 1)
            ++tp;
        else if (vote < 0 && dataTest[i][lastCol] == 0)
            ++tn;
        else if (vote > 0 && dataTest[i][lastCol] == 0)
            ++fp;
        else
            ++fn;
    }
    if (printDetails)
    {
        int dataSize = dataTest.size() - 1;
        double accuracy = (double)(tp + tn) / dataSize;
        double precision = (double)tp / (tp + fp);
        double recall = (double)tp / (tp + fn);
        double fmeausre = 2 * precision * recall / (precision + recall);
        cout << "        Accuracy: " << accuracy << endl;
        cout << "        Precision: " << precision << endl;
        cout << "        Recall: " << recall << endl;
        cout << "        F-Measure: " << fmeausre << endl;
    }
    return (double)(tp + tn) / (dataTest.size() - 1);
}

void loadData(vector <vector <int> > &data, set <pair <int, int> > &attributes, bool test)
{
    ifstream fin;
    if (test)
        fin.open("test_output.txt");
    else
        fin.open("data_output.txt");
    string line;
    vector <vector <string> > valNames = {
        {"0", "1"},
        {"Private", "Self-emp-not-inc", "Self-emp-inc", "Federal-gov", "Local-gov", "State-gov", "Without-pay", "Never-worked"},
        {"0", "1"},
        {"Bachelors", "Some-college", "11th", "HS-grad", "Prof-school", "Assoc-acdm", "Assoc-voc", "9th", "7th-8th", "12th", "Masters", "1st-4th", "10th", "Doctorate", "5th-6th", "Preschool"},
		{"0", "1"},
        {"Married-civ-spouse", "Divorced", "Never-married", "Separated", "Widowed", "Married-spouse-absent", "Married-AF-spouse"},
        {"Tech-support", "Craft-repair", "Other-service", "Sales", "Exec-managerial", "Prof-specialty", "Handlers-cleaners", "Machine-op-inspct", "Adm-clerical",
            "Farming-fishing", "Transport-moving", "Priv-house-serv", "Protective-serv", "Armed-Forces"},
        {"Wife", "Own-child", "Husband", "Not-in-family", "Other-relative", "Unmarried"},
        {"White", "Asian-Pac-Islander", "Amer-Indian-Eskimo", "Other", "Black"},
        {"Female", "Male"},
        {"0", "1"},
        {"0", "1"},
        {"0", "1"},
        {"United-States", "Cambodia", "England", "Puerto-Rico", "Canada", "Germany", "Outlying-US(Guam-USVI-etc)", "India", "Japan", "Greece", "South", "China",
            "Cuba", "Iran", "Honduras", "Philippines", "Italy", "Poland", "Jamaica", "Vietnam", "Mexico", "Portugal", "Ireland", "France", "Dominican-Republic", "Laos",
            "Ecuador", "Taiwan", "Haiti", "Columbia", "Hungary", "Guatemala", "Nicaragua", "Scotland", "Thailand", "Yugoslavia", "El-Salvador", "Trinadad&Tobago",
            "Peru", "Hong", "Holand-Netherlands"},
        {"<=50K", ">50K"}
    };

    if (!test)
    {
        for (int i = 0; i < valNames.size() - 1; ++i)
            attributes.insert(pair <int, int> (i, valNames[i].size()));
        attributes.insert(pair <int, int> (valNames.size() - 1, -1));
    }

    while(getline(fin, line))
    {
        stringstream linestream(line);
        string value;
        vector <int> record;
        char space;
        for (int att = 0; getline(linestream, value, ','); ++att)
        {
            switch(att)
            {
                case 0: case 2: case 4: //continuous data
                case 10: case 11: case 12:
                        int data;
                        if(stringstream(value) >> data)
                            record.push_back(data);
                        else
                            record.push_back(-1);
                        break;
                case 1: case 3: case 5: //discrete data
                case 6: case 7: case 8:
                case 9: case 13: case 14:
                        auto iter = find(valNames[att].begin(), valNames[att].end(), value);
                        if (iter != valNames[att].end())
                            record.push_back(distance(valNames[att].begin(), iter));
                        else
                            record.push_back(-1);
                        break;
            }
        }
        data.push_back(record);
    }
    cout << "doing missing data" << endl << "    ";
    fillMissingData(data, attributes);
    cout << endl << "discretizing data" << endl;
    discretizeData(0, data);
    discretizeData(2, data);
    discretizeData(4, data);
    discretizeData(10, data);
    discretizeData(11, data);
    discretizeData(12, data);
    cout << endl;
    vector <int> attNums;
    for (int i = 0; i < 15; ++i)
        attNums.push_back(i);
    data.insert(data.begin(), attNums);
}

void get_validation_set(vector<vector<int> > &data, vector <vector <int> > &vdata)
{
    float valSize = VALRATIO * data.size();
    move(data.begin() + (data.size() - valSize), data.end(), back_inserter(vdata));
    data.resize(data.size() - valSize);
}

void fillMissingData(vector <vector <int> > &data, set <pair <int, int> > &attributes)
{
    vector <vector <int> > countVal;
    vector <int> countRec(attributes.size());
    for (auto attribute: attributes)
    {
        if (attribute.first == attributes.size() - 1)
            continue;
        vector <int> countValAtt(attribute.second);
        countVal.push_back(countValAtt);
    }
    for (int i = 0; i < data.size(); ++i)
    {
        for (int j = 0; j < data[i].size() - 1; ++j)
        {
            if (data[i][j] == -1)
                continue;
            switch(j)
            {
                case 0: case 2: case 4: //continuous data
                case 10: case 11: case 12:
                        countRec[j] += 1;
                        countVal[j][0] += data[i][j]; //finds the average
                        break;
                case 1: case 3: case 5: //discrete data
                case 6: case 7: case 8:
                case 9: case 13: case 14:
                        countVal[j][data[i][j]] += 1; //finds the mode
                        break;
            }
        }
    }
    vector <int> bestVal;
    for (int i = 0; i < attributes.size() - 1; ++i)
    {
        switch(i)
        {
            case 0: case 2: case 4: //continuous data
            case 10: case 11: case 12:
                    bestVal.push_back(countVal[i][0] / countRec[i]);
                    break;
            case 1: case 3: case 5: //discrete data
            case 6: case 7: case 8:
            case 9: case 13: case 14:
                    int maxElement = 0, maxj = 0;
                    for (int j = 0; j < countVal[i].size(); ++j)
                        if (countVal[i][j] > maxElement)
                        {
                            maxElement = countVal[i][j];
                            maxj = j;
                        }

                    bestVal.push_back(maxj);
                    break;
        }
        cout << bestVal[i] << " ";
    }
    for (int i = 0; i < data.size(); ++i)
    {
        for (int j = 0; j < data[i].size() - 1; ++j)
        {
            if (data[i][j] == -1)
                data[i][j] = bestVal[j];
        }
    }
}

void discretizeData(int attNum, vector <vector <int> > &data)
{
    vector <vector <int> > tempData = data;
    sort(tempData.begin(), tempData.end(),
         [&attNum](const vector <int> &a, const vector <int> &b)
         {
             return a[attNum] < b[attNum];
         });
    int lastCol = tempData[0].size() - 1, cIndex;
    int count0True = 0, count1True = 0, count0False = 0, count1False = 0;
    for (int i = 0; i < tempData.size(); ++i)
        count1True += tempData[i][lastCol];
    count1False = tempData.size() - count1True;
    count0True = tempData[0][lastCol];
    count0False = 1 - tempData[0][lastCol];
    double maxEntropy = INT_MAX, entropy;
    for (int i = 1; i < tempData.size(); ++i)
    {
        if (tempData[i][lastCol] != tempData[i - 1][lastCol])
        {
            entropy = calcEntropy(count0True, count0False) + calcEntropy(count1True, count1False);
            if (entropy < maxEntropy)
            {
                maxEntropy = entropy;
                cIndex = i;
            }
        }
        count0True += tempData[i][lastCol];
        count0False += 1 - tempData[i][lastCol];
        count1True -= tempData[i][lastCol];
        count1False -= 1 - tempData[i][lastCol];
    }
    double c = (double) ((double) tempData[cIndex][attNum] + tempData[cIndex - 1][attNum]) / 2;
    cout << "    (att = " << attNum << ", c = " << c << ")" << endl;
    for (int i = 0; i < data.size(); ++i)
        data[i][attNum] = (data[i][attNum] <= c ? 0: 1);
}

node *ID3(vector <vector <int> > &data, set <pair <int, int> > &attributes, bool random_forest)
{
    node *check = checkSame(data);
    if (check != NULL)
        return check;
    if (attributes.size() == 1)
        return doVoting(data);

    //now the not so trivial part

    //declarations
    double maxEntropy = INT_MAX, entropy;
    node *root = new node();
    root->guess = (doVoting(data) == &valTrue? true: false);
    pair <int, int> bestAtt;
    set <pair <int, int> > newAttributes = attributes;
    set <pair <int, int> > copyAttributes;

    //calculating the optimal attribute to make a root
    if (random_forest)
        copyAttributes = get_new_attributes(attributes);
    else
        copyAttributes=attributes;

    for (auto attribute: copyAttributes)
    {
        entropy = calcEntropyGain(data, attribute); //returns the entropy
        if (entropy < maxEntropy)
        {
            maxEntropy = entropy;
            bestAtt = attribute;
        }
    }
    root->attribute = bestAtt;

    //now to make its children
    for (auto it = newAttributes.begin(); it != newAttributes.end(); ++it)
    {
        if (it->first == bestAtt.first)
            newAttributes.erase(it);
    }

    for (int val = 0; val < bestAtt.second; ++val) //iterating through all the possible values of the attribute
    {
        vector <vector <int> > vdata;
        cutData(data, bestAtt.first, val, vdata);
        if (vdata.size() <= 1)
            root->nextAtt.push_back(doVoting(data));
        else //recursive call
            root->nextAtt.push_back(ID3(vdata, newAttributes,random_forest));
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
    if (attribute.second == -1) return INT_MAX;
    vector <pair <int, int> > table(attribute.second); //first int is countTrue, second is countFalse
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
    for (int val = 0; val < attribute.second; ++val)
        sum += calcEntropy(table[val].first, table[val].second);
    return sum;
}

void cutData(vector <vector <int> > &data, int attNum, int val, vector <vector <int> > &vdata)
{
    int attCol;
    vector <int> attNums;
    for (attCol = 0; attCol < data[0].size() - 1; ++attCol)
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
    if (tabs > 2) return;
    for (int tab = 0; tab < tabs; ++tab)
        cout << "  ";
    cout << root->attribute.first << endl;
    for (auto child: root->nextAtt)
        printTree(child, tabs + 1);
}

void printData(vector <vector <int> > &x)
{
    cout << "i'm in the function" << endl;
    for (int i = 0; i < x.size(); ++i)
    {
        for (int j = 0; j < x[0].size(); ++j)
        {
            cout << x[i][j] << " ";
        }
        cout << endl;
    }
}
