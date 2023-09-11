#include <iostream>
#include <vector>
#include <map>
#include "math.h"
#include <sstream>
#include <fstream>
#include <ctime>
#include <locale>

using namespace  std;


std::time_t currentTime = std::time(0);
std::time_t today = currentTime - 40000;
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
        time_t cur = it->second.time;

        if (cur >= today ) {
            groups[today].push_back(it->second);
        } else if (cur >= yesterday) {
            groups[yesterday].push_back(it->second);
        } else if (cur >= sevenDaysAgo) {
            groups[sevenDaysAgo].push_back(it->second);
        } else if (cur >= thirtyDaysAgo) {
            groups[thirtyDaysAgo].push_back(it->second);
        } else {
            groups[oneYearAgo].push_back(it->second);
        }
    }
    return groups;
}

std::map<std::string, std::vector<Object>> typeGroups(const std::map<char, std::vector<Object>>& group, int n) {
    std::map<std::string, std::vector<Object>> typeGroups;

    for (const auto& pair : group) {

        const std::vector<Object>& objects = pair.second;

        for (const Object& obj : objects) {
            const std::string& objectType = obj.type;

            // Поместить объект в соответствующую группу
            typeGroups[objectType].push_back(obj);
        }
    }

    // Теперь, если объектов в группе меньше чем n, объединим их в группу "Разное"
    for (const auto& pair : typeGroups) {
        const std::string& typeName = pair.first;
        std::vector<Object>& objects = typeGroups[typeName];

        if (objects.size() <= n) {
            typeGroups["Разное"].insert(typeGroups["Разное"].end(), objects.begin(), objects.end());
            typeGroups.erase(typeName); // Удалить текущую группу
        }
    }

    return typeGroups;
}

template <typename KeyType>
void showVec(const std::map<KeyType, std::vector<Object>>& groups) {
    for (const auto& i : groups) {
        if constexpr (std::is_same<KeyType, time_t>::value)
        {
            cout << std::ctime(&i.first) <<endl;
        }
        else
            std::cout << i.first << std::endl;

        for (const auto& j : i.second) {
            std::cout << "\t" << j.name << " " << j.x << " " << j.y << " " << j.type << " " << j.time << std::endl;
        }
    }
}

int main() {

    setlocale(LC_ALL , "ru");

    fstream file("obj.txt");
    multimap<string ,Object> total = getObj(file);

    std::map<char, std::vector<Object>> group = nameGroups(total);
    showVec(group);
    cout << endl;

    std::map<time_t, std::vector<Object>> timeGroup = timeGroups(total);
    showVec(timeGroup);
    cout << endl;

    std::map<string, std::vector<Object>> typeGroup = typeGroups(group,2);
    showVec(typeGroup);
    cout << endl;

    return 0;
}
