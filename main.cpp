#include <iostream>
#include <vector>
#include <map>
#include "math.h"
#include <sstream>
#include <fstream>
#include <ctime>
#include <algorithm>
#include <locale>

using namespace  std;

std::time_t currentTime = std::time(NULL);
std::time_t today = currentTime - 20000;
std::time_t yesterday = currentTime - 86400;
std::time_t sevenDaysAgo = currentTime - 7 * 86400;
std::time_t thirtyDaysAgo = currentTime - 30 * 86400;
std::time_t oneYearAgo = currentTime - 365 * 86400;

struct Object
{
    string name;
    float x;
    float y;
    string type;
    time_t time;
};

bool isRussianLetter(char c) {
      return (c >= 'А' && c <= 'я');
}

int calcDistance(const float& a, const float& b)
{
    return sqrt(a*a + b*b);
}

std::multimap<string, Object> getObj(fstream& file) //return sorted by name
{
    string line;
    multimap<string, Object> objects;

    while (std::getline(file, line)) {
        Object object;
        std::stringstream ss(line);

        if (ss >> object.name >> object.x >> object.y >> object.type >> object.time)
        {
            objects.insert(std::make_pair(object.name, object));
        } else {
            std::cerr << "Error reading file: " << line << std::endl;
        }
    }
    return objects;
}

std::map<char, std::vector<Object>> nameGroups(const multimap<string, Object> objects) //group by name
{
    char prevFirstLetter = 0;
    std::map<char, std::vector<Object>> groups;
    for (auto i=objects.begin(); i!=objects.end(); ++i) {

        char firstLetter = tolower(i->second.name[0]);
        if (isRussianLetter(firstLetter))
        {
            if (firstLetter == tolower(prevFirstLetter)) {
                groups[firstLetter].push_back(i->second);
            }
            else {
                groups[firstLetter].push_back(i->second);
                prevFirstLetter = firstLetter;
            }
        }
        else
        {
           groups['#'].push_back(i->second);
           prevFirstLetter = 0;
        }
    }
    return groups;
}

std::map<time_t,std::vector<Object>> timeGroups(const std::multimap<string, Object>& objects) //group by time
{
    std::map<time_t, std::vector<Object>> groups;

    for (auto it = objects.begin(); it != objects.end(); ++it) {

        if (it->second.time <= oneYearAgo) {
            groups[oneYearAgo].push_back(it->second);
        } else if (it->second.time <= thirtyDaysAgo) {
            groups[thirtyDaysAgo].push_back(it->second);
        } else if (it->second.time <= sevenDaysAgo) {
            groups[sevenDaysAgo].push_back(it->second);
        } else if (it->second.time <= yesterday) {
            groups[yesterday].push_back(it->second);
        } else {
            groups[today].push_back(it->second);
        }
    }

    return groups;
}

std::map<std::string, std::vector<Object>> typeGroups(const std::map<char, std::vector<Object>>& group, int n) //group by type
{
    std::map<std::string, std::vector<Object>> typeGroups;

    for (const auto& pair : group) {
        const std::vector<Object>& objects = pair.second;

        for (const Object& obj : objects) {
            const std::string& objectType = obj.type;
            typeGroups[objectType].push_back(obj);
        }
    }

    for (const auto& pair : typeGroups) {
        const std::string& typeName = pair.first;
        std::vector<Object>& objects = typeGroups[typeName];

        if (objects.size() <= n) {
            typeGroups["разное"].insert(typeGroups["разное"].end(), objects.begin(), objects.end());
            typeGroups.erase(typeName);
        }
    }

    return typeGroups;
}

std::map<std::string, std::vector<Object>> distanceGroups(const multimap<string, Object> objects) {
    std::map<std::string, std::vector<Object>> groups;

    for (auto it = objects.begin(); it != objects.end(); ++it) {
        if (calcDistance(it->second.x, it->second.y) < 100)
            groups["100"].push_back(it->second);
        else if (calcDistance(it->second.x, it->second.y) < 1000)
            groups["1000"].push_back(it->second);
        else if (calcDistance(it->second.x, it->second.y) < 10000)
            groups["10000"].push_back(it->second);
        else if (calcDistance(it->second.x, it->second.y) > 10000)
            groups["over 10000"].push_back(it->second);
    }

    return groups;
} //group by type

template <typename KeyType>
void showVec(const std::map<KeyType, std::vector<Object>>& groups) {
    if constexpr (std::is_same<KeyType, time_t>::value)
    {
        std::string dat;
        for (auto it = groups.rbegin(); it != groups.rend(); ++it){
            if (it->first <= oneYearAgo)
                dat = "year ago";
            else if (it->first <= thirtyDaysAgo)
                dat = "month ago";
            else if (it->first <= sevenDaysAgo)
                dat = "week ago";
            else if (it->first <= yesterday)
                dat = "yesterday";
            else if (it->first <= today)
                dat = "today";
            cout << dat << endl;
            for (const auto &j : it->second ) {
                std::cout << "\t" << j.name << " " << j.x << " " << j.y << " " << j.type << " " << j.time << std::endl;
            }
        }
    }
    else
    {
        for (const auto& i : groups) {
            std::cout << i.first << std::endl;
            for (const auto& j : i.second) {
                std::cout << "\t" << j.name << " " << j.x << " " << j.y << " " << j.type << " " << j.time << std::endl;
            }
        }
    }
}

template <typename KeyType>
void sortVec (std::map<KeyType, std::vector<Object>>& groups, string& command)
{
    if(command == "time")
    {
        for (auto& group : groups) {
            std::sort(group.second.begin(), group.second.end(), [](const Object& a, const Object& b) {
                return a.time > b.time;
            });
        }
    }
    else if(command == "distance")
    {
        for (auto& group : groups) {
            std::sort(group.second.begin(), group.second.end(), [](const Object& a, const Object& b) {
                return calcDistance(a.x,a.y) < calcDistance(b.x,b.y);
            });
        }
    }
    else if(command == "name")
    {
        for (auto& group : groups) {
            std::sort(group.second.begin(), group.second.end(), [](const Object& a, const Object& b) {
                return a.name < b.name;
            });
        }
    }
    else if(command == "type") {
        if constexpr (std::is_same<KeyType, time_t>::value)
        {
            for (auto &group: groups) {
                std::sort(group.second.begin(), group.second.end(), [](const Object &a, const Object &b) {
                    return a.type < b.type;
                });
            }
        }
        else
        {
            for (auto &group: groups) {
                std::sort(group.second.begin(), group.second.end(), [](const Object &a, const Object &b) {
                    return a.type < b.type;
                });
            }
        }
    }
}

template <typename KeyType>
void save(std::map<KeyType, std::vector<Object>>& groups)
{
    ofstream outputFile("sorted_objects.txt");
    if constexpr (std::is_same<KeyType, time_t>::value)
    {
        if (outputFile.is_open()) {
            for (auto i= groups.rbegin(); i!=groups.rend(); ++i) {
                for(auto j : i->second)
                {
                    outputFile << j.name << " " << j.x << " " << j.y << " " << j.type << " " << j.time << std::endl;
                }
            }
            outputFile.close();
        } else {
            cerr << "Error opening file" << endl;
        }
    }
    else
    {
        if (outputFile.is_open()) {
            for (const auto i : groups) {
                for(auto j : i.second)
                {
                    outputFile << j.name << " " << j.x << " " << j.y << " " << j.type << " " << j.time << std::endl;
                }
            }
            outputFile.close();
        } else {
            cerr << "Error opening file" << endl;
        }
    }
}

int main() {

    setlocale(LC_ALL , "en");

    fstream file("obj.txt");
    multimap<string,Object> total = getObj(file);

    string command;
    string command2;

    std::map<char, std::vector<Object>> groupNm;
    std::map<time_t,std::vector<Object>> groupTm;
    std::map<std::string, std::vector<Object>> groupTp;
    std::map<std::string, std::vector<Object>> groupDs;

    while (command != "q")
    {
        cout << "We grouping data by: name, type, distance, time, save, q-exit " <<endl;
        cin >> command;
        if(command == "name"){
            groupNm = nameGroups(total);
            showVec(groupNm);
            cout << "We sorting data by: name, type, distance, time, no... " <<endl;
            cin >> command;
            if(command != "no")
            {
                sortVec(groupNm,command);
                showVec(groupNm);
            }
            else
                continue;
        }
        else if(command == "type"){
            groupTp = typeGroups(groupNm,2);
            showVec(groupTp);
            cout << "We sorting data by: name, type, distance, time, no... " <<endl;
            cin >> command;
            if(command != "no"){
                sortVec(groupTp,command);
            showVec(groupTp);
            }
            else
                continue;
        }
        else if(command == "distance"){
            groupDs = distanceGroups(total);
            showVec(groupDs);
            cout << "We sorting data by: name, type, distance, time, no... " <<endl;
            cin >> command;
            if(command != "no"){
                sortVec(groupDs,command);
            showVec(groupDs);
            }
            else
                 continue;
        }
        else if(command == "time"){
            groupTm = timeGroups(total);
            showVec(groupTm);
            cout << "We sorting data by: name, type, distance, time, no... " <<endl;
            cin >> command;
            if(command != "no") {
                sortVec(groupTm,command);
            showVec(groupTm);
            }
            else
                continue;
        }
        else if(command == "save")
        {
            cout << "1-namesGroup, 2-typesGroup, 3-timesGroup, 4-distanceGroup " <<endl;
            cin >> command;
            if(command == "1")
                save(groupNm);
            if(command == "2")
                save(groupTp);
            if(command == "3")
                save(groupTm);
            if(command == "4")
                save(groupDs);
            cout << "saving " <<endl;
        }
    }
    return 0;
}
