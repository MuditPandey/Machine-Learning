#include <fstream>
#include <sstream>
#include <iostream>
#include <set>
#include <algorithm>
#include <vector>
#include <type_traits>

using namespace std;

struct node
{
    pair <int, int> attribute; //attribute #, and number of values
    vector <node*> nextAtt;
} valTrue, valFalse;

void loadData(vector <vector <int> > &data, set <pair <int, int> > &attributes); //for fixing continuous data and ? and marking all attributes from 0 to n -> load the data
node *ID3(vector <vector <int> > &data, set <pair <int, int> > &attributes);
node *checkSame(vector <vector <int> > &data); //returns valTrue/valFalse if all the data have the same val, otherwise returns NULL
node *doVoting(vector <vector <int> > &data); //returns valTrue/valFalse based on voting
double calcEntropyGain(vector <vector <int> > &data, pair <int, int> &attribute);
double calcEntropy(int, int);
void cutData(vector <vector <int> > &data, int, int, vector <vector <int> > &vdata); //also removes the attribute maxAtt from data
void printTree(node*, int tabs = 0);
void printData(vector <vector <int> >);
void discretizeData(int attNum, vector <vector <int> > &data);
void fillMissingData(vector <vector <int> > &data, set <pair <int, int> > &attributes);

void space_modifier()
{
    ifstream fin;
    ofstream fout;
    fin.open("data_small.txt");
    fout.open("data_small_output.txt");
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

    vector <vector <int> > data;
    set <pair <int, int> > attributes; //first int is the attribute number and the second is the number of values of the attribute
    loadData(data, attributes);
    node *root = ID3(data, attributes);
    printTree(root);
}

void loadData(vector <vector <int> > &data, set <pair <int, int> > &attributes)
{

    ifstream fin;
    fin.open("data_small_output.txt");
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

    for (int i = 0; i < valNames.size() - 1; ++i)
        attributes.insert(pair <int, int> (i, valNames[i].size()));
    attributes.insert(pair <int, int> (valNames.size() - 1, -1));
    cout << "ATTRIBUTES" << endl;
    for (auto attribute: attributes)
        cout << attribute.first << " " << attribute.second << endl;

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
    cout << "doing missing data" << endl;
    fillMissingData(data, attributes);
    discretizeData(0, data);
    discretizeData(2, data);
    discretizeData(4, data);
    discretizeData(10, data);
    discretizeData(11, data);
    discretizeData(12, data);
    vector <int> attNums;
    for (int i = 0; i < 15; ++i)
        attNums.push_back(i);
    data.insert(data.begin(), attNums);
    for (int i = 0; i < data.size(); ++i)
    {
        cout << i << "-> ";
        for (int j = 0; j < data[i].size(); ++j)
            cout << data[i][j] << " ";
        cout << endl;
    }

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
    cout << "part 1 done" << endl;
    for (int i = 0; i < data.size(); ++i)
    {
        for (int j = 0; j < data[i].size() - 1; ++j)
        {
            cout << data[i][j] << " ";
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
        cout << endl;
    }
    for (int j = 0; j < data[0].size() - 1; ++j)
    {

    }
    cout << "part 2 done" << endl;
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
    cout << endl << "part 3 done" << endl;
    for (int i = 0; i < data.size(); ++i)
    {
        for (int j = 0; j < data[i].size() - 1; ++j)
        {
            if (data[i][j] == -1)
                data[i][j] = bestVal[j];
        }
    }
    cout << "part 4 done" << endl;
}

void discretizeData(int attNum, vector <vector <int> > &data)
{
    vector <vector <int> > tempData = data;
    cout << "starting the sort" << endl;
    sort(tempData.begin(), tempData.end(),
         [&attNum](const vector <int> &a, const vector <int> &b)
         {
             return a[attNum] < b[attNum];
         });
    cout << "done " << attNum << endl;
    int lastCol = tempData[0].size() - 1, cIndex;
    int count0True = 0, count1True = 0, count0False = 0, count1False = 0;
    for (int i = 0; i < tempData.size(); ++i)
        count1True += tempData[i][lastCol];
    count1False = tempData.size() - count1True;
    count0True = tempData[0][lastCol];
    count0False = 1 - tempData[0][lastCol];
    double maxEntropy = 10000, entropy;
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
    cout << "c value = " << c << endl;
    for (int i = 0; i < data.size(); ++i)
        data[i][attNum] = (data[i][attNum] <= c ? 0: 1);
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
    double maxEntropy = 10000, entropy;
    node *root = new node();
    pair <int, int> bestAtt;
    set <pair <int, int> > newAttributes = attributes; //for: attributes - {bestAtt}

    //calculating the optimal attribute to make a root
    cout << "hello" << endl;
    for (auto attribute: attributes)
    {
        cout << "Now checking entropy of att" << attribute.first << endl;
        entropy = calcEntropyGain(data, attribute); //returns the entropy
        cout << "yo i'm back" << endl;
        if (entropy < maxEntropy)
        {
            maxEntropy = entropy;
            bestAtt = attribute;
        }
        cout << "finished checking entropy of att" << attribute.first << endl;
    }
    root->attribute = bestAtt;
    cout << "bestAtt found! " << bestAtt.first << ", Entropy " << maxEntropy << endl;

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
    vector <pair <int, int> > table(attribute.second); //first int is countTrue, second is countFalse
    int lastCol = data[0].size() - 1, attCol;
    for (attCol = 0; attCol < lastCol; ++attCol)
        if (data[0][attCol] == attribute.first)
            break;
    cout << "attribute column number = " << attCol << endl;
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
    cout << "*************FINI**************" << endl;
    cout << endl;
    for (int val = 0; val < attribute.second; ++val)
        sum += calcEntropy(table[val].first, table[val].second);
    cout << "returning from function..." << endl;
    cout << "2 + 2 = " << sum << endl;
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


