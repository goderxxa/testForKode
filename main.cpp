#include <iostream>
#include <vector>
#include <map>
#include "math.h"
#include <sstream>
#include <fstream>
#include <ctime>
#include <algorithm>

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

std::multimap<string, Object> getObj(fstream& file)
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

std::map<char, std::vector<Object>> nameGroups(const multimap<string, Object> objects)
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

std::map<time_t,std::vector<Object>> timeGroups(const std::multimap<std::string, Object>& objects)
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
    for (auto& group : groups) {
        std::sort(group.second.begin(), group.second.end(), [](const Object& a, const Object& b) {
            return a.time > b.time;
        });
    }

    return groups;
}

std::map<std::string, std::vector<Object>> typeGroups(const std::map<char, std::vector<Object>>& group, int n) {
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
    for (auto& group : groups) {
        std::sort(group.second.begin(), group.second.end(), [](const Object& a, const Object& b) {
            return calcDistance(a.x,a.y) > calcDistance(b.x,b.y);
        });
    }
    return groups;
}

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

int main() {

    setlocale(LC_ALL , "en");

    fstream file("obj.txt");
    multimap<string,Object> total = getObj(file);

    std::map<char, std::vector<Object>> group = nameGroups(total);
    showVec(group);
    cout << endl;

    std::map<std::string, std::vector<Object>> toOut = distanceGroups(total);
    showVec(toOut);

    std::ofstream outputFile("sorted_objects.txt");
    if (outputFile.is_open()) {
        for (const auto i : toOut) {
            for(auto j : i.second)
            {
                outputFile << j.name << " " << j.x << " " << j.y << " " << j.type << " " << j.time << std::endl;
            }
        }
        outputFile.close();
    } else {
        std::cerr << "Error opening file" << std::endl;
    }

    return 0;
}
