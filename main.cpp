#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <queue>
#include <list>
#include <unordered_set>
#include <math.h>
#include <algorithm>
#include <chrono> //https://www.geeksforgeeks.org/cpp/measure-execution-time-function-cpp/

struct node{
    node * child;
    float accuracy;
    std::vector<int> features;
    node * prev;

    node(node* prev, float accuracy) : prev(prev), accuracy(accuracy){
    }
    node(node* prev, float accuracy, node* child, std::vector<int> features) : prev(prev), accuracy(accuracy), child(child), features(features){
    }

    ~node(){
        delete child;
    }
};

float kfold(std::vector<std::vector<float>>& data, std::vector<int>& features, int addedfeature, int type){
    float sum;
    float correct = 0;
    float closest = std::numeric_limits<float>::max();
    int indexClosest = -1;
    std::cout << "Checking Feature(s): ";
    for(int i = 0; i < features.size(); i++){
        if(type == 2 && features.at(i) == addedfeature){

        }
        else{
           std::cout << features.at(i) << " "; 
        }
    }
    if(type == 1){
        std::cout << addedfeature << " ";
    }
    for(int i = 0; i < data.size(); i++){
        closest = std::numeric_limits<float>::max();
        indexClosest = -1;
        for(int j = 0; j < data.size(); j++){
            sum = 0;
            if(i != j){
                for(int k = 0; k < features.size(); k++){
                    if(type == 2 && features.at(k) == addedfeature){
                        
                    }
                    else{
                        sum += pow((data.at(i).at(features.at(k)) - data.at(j).at(features.at(k))), 2);
                    }
                }
                if(type == 1){
                    sum += pow((data.at(i).at(addedfeature) - data.at(j).at(addedfeature)), 2);
                }
                sum = sqrt(sum);
                if(sum < closest){
                    closest = sum;
                    indexClosest = j;
                }
            }
        }
        if(data.at(i).at(0) == data.at(indexClosest).at(0)){
            correct++;
        }
    }
    correct = correct/data.size();
    std::cout << " Accuracy is " << correct << std::endl;
    return correct;
}


node* forwardSearch(std::vector<std::vector<float>>& data, std::vector<int>& features, std::unordered_set<int>& checker, node* currNode, float& best, std::vector<int>& bestFeatures){
    float accuracyfinal = std::numeric_limits<float>::lowest();
    float accuracychecker = 0;
    int index = -1;
    std::cout << "On level " << currNode->features.size() << " Level Accuracy " << currNode->accuracy << std::endl;
    if(features.size() == data.at(0).size() - 1){
        return nullptr;
    }
    for(int i = 1; i < data.at(0).size(); i++){
        if(checker.count(i) == 0){
            accuracychecker = kfold(data, features, i, 1);
            if(accuracychecker > accuracyfinal){
                accuracyfinal = accuracychecker;
                index = i;
            }
        }
    }
    std::vector<int> newfeatures = features;
    std::unordered_set<int> newchecker = checker;
    if(index != -1){
        newfeatures.push_back(index);
        newchecker.insert(index);
    }
    std::cout << "Best Feature is " << index << " with an accuracy of " << accuracyfinal << std::endl;
    if(accuracyfinal > best){
        best = accuracyfinal;
        bestFeatures = newfeatures;
    }
    node * temp = new node(currNode, accuracyfinal, nullptr, newfeatures);
    temp->child = forwardSearch(data, newfeatures, newchecker, temp, best, bestFeatures);
    return temp;
}


node* backwardSearch(std::vector<std::vector<float>>& data, std::vector<int>& features, std::unordered_set<int>& checker, node* currNode, float& best, std::vector<int>& bestFeatures){
    float accuracyfinal = std::numeric_limits<float>::lowest();
    float accuracychecker = 0;
    int index = -1;
    std::cout << "On level " << currNode->features.size() << " Best Accuracy " << best << std::endl;
    if(features.size() <= 1){
        return nullptr;
    }
    for(int i = 1; i < data.at(0).size(); i++){
        if(checker.count(i) == 1){
            accuracychecker = kfold(data, features, i, 2);
            if(accuracychecker > accuracyfinal){
                accuracyfinal = accuracychecker;
                index = i;
            }
        }
    }
    std::vector<int> newfeatures = features;
    std::unordered_set<int> newchecker = checker;
    if(index != -1){
        newfeatures.erase(std::remove(newfeatures.begin(), newfeatures.end(), index), newfeatures.end()); //https://en.cppreference.com/w/cpp/container/vector/erase2.html
        newchecker.erase(index);
    }
    std::cout << "Best Feature is " << index << " with an accuracy of " << accuracyfinal << std::endl;
    if(accuracyfinal > best){
        best = accuracyfinal;
        bestFeatures = newfeatures;
    }
    node * temp = new node(currNode, accuracyfinal, nullptr, newfeatures);
    temp->child = backwardSearch(data, newfeatures, newchecker, temp, best, bestFeatures);
    return temp;
}


int main(int argc, char*  argv[]){
    if(argc < 2){
        std::cout << "No file" << std::endl;
        return 1;
    }
    std::ifstream file(argv[1]);
    if(!file){
        std::cout << "No file" << std::endl;
        return 1;
    }
    //https://www.geeksforgeeks.org/cpp/tokenizing-a-string-cpp/
    //https://www.geeksforgeeks.org/cpp/stringstream-c-applications/
    std::vector<std::vector<float>> data;
    std::string line;
    float defaultcount = 0;
    while(std::getline(file, line)){
        std::stringstream ss(line);
        std::vector<float> row;
        float value;
        while(ss >> value){
            if(row.empty()){
                defaultcount++;
            }
            row.push_back(value);
        }
        data.push_back(row);
    }
    int type = 0;
    std::cout << "Type 1 for forward selection and type 2 for backward elimination" << std::endl;
    std::cin >> type;
    if(type < 1 || type > 2){
        std::cout << "invalid type" << std::endl;
        return 1;
    }
    std::vector<int> features;
    std::vector<int> featuresordered;
    std::unordered_set<int> checker;
    if(type == 2){
       for(int i = 1; i < data.at(0).size(); i++){
            features.push_back(i);
            checker.insert(i);
       } 
    }

    int temp1 = 0;
    int temp2 = 0;
    for(int i = 0; i < data.size(); i++){
        if(data.at(i).at(0) == 1){
            temp1++;
        }
        else{
            temp2++;
        }
    }
    float best = 0;
    std::vector<int> bestFeatures;
    auto start = std::chrono::high_resolution_clock::now();
    node* first = new node(nullptr, std::max(temp1, temp2)/(float)data.size(), nullptr, features);
    if(type == 1){
        first->child = forwardSearch(data, features, checker, first, best, bestFeatures);
    }
    else{
        first->child = backwardSearch(data, features, checker, first, best, bestFeatures);
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Best features are ";
    for(int i = 0; i < bestFeatures.size(); i++){
        std::cout << bestFeatures.at(i) << " ";
    }
    std::cout << "with an accuracy of " << best << std::endl;
    float timevar = duration.count()/1000000;
    timevar = timevar/3600;
    std::cout << "Total Time: " << timevar << " hours or " << timevar * 60 << " minutes" << std::endl;
    return 0;
}