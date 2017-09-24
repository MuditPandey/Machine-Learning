#include <iostream>
#include <set>

using namespace std;
typedef pair<string, vector <string> > attType;

struct attNode
{
    int attribute; //attribute #
    valNode **values;
};

struct valNode
{
    string value;
    attNode *nextAtt;
    bool result, known;
};

int main()
{
    vector <attType> attNames;
    setAttributeNames(attNames);
    data = fixData(); //for fixing continuous data and ? and marking all attributes from 0 to n
    set <pair <int, int> > attributes; //first int is the attribute number and the second is the number of values of the attribute
    setAttributes(attributes);
    ID3(attributes);
}

attNode *ID3(data, attributes)
{
    for each attribute
        calcEntropy(attribute)
        if entropy > maxEntropy
            maxEntropy = entropy
            maxAtt = attribute
    root = makeNode(maxAtt);
    for each value of maxAtt
        child = root.makeChild(value)
        vdata = cutData(data, maxAtt, val);
        if vdata is empty
            findDecision(data);
        else
            node = ID3(vdata, attributes - {maxAtt})
            child.makeChild(node)
    return root
}


void setAttributeNames(set <attType> &attributes)
{
    vector <string> x;
    x = {"10-19", "20-29", "30-39", "40-49"}
    attributes.push_back("age", x);
    x = {"Private", "Self-emp-not-inc", "Self-emp-inc", "Federal-gov", "Local-gov", "State-gov", "Without-pay", "Never-worked"}
    attributes.push_back("workclass", x);
    x = {}
}
