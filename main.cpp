#include <map>
#include <iostream>
#include <ctime>
#include <locale>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

struct Object
{
public:
    std::string name;
    std::string type;
    float x;
    float y;
    time_t creationTime;
};

std::vector<Object> getObjects(std::fstream& file) {
    std::vector<Object> objects;
    std::string line;

    while (std::getline(file, line)) {
        Object object;
        std::stringstream ss(line);
        if (ss >> object.name >> object.x >> object.y >> object.type >> object.creationTime)
        {
            objects.push_back(object);
        }
        else
        {
            std::cerr << "Error reading line " << line << std::endl;
        }
    }
    return objects;
}

std::multimap<std::string, Object> getObjects2(std::fstream& file) {
    std::multimap<std::string, Object> objects;
    std::string line;

    while (std::getline(file, line)) {
        Object object;
        std::stringstream ss(line);
        if (ss >> object.name >> object.x >> object.y >> object.type >> object.creationTime)
        {
            objects.insert(std::make_pair(object.name, object ));
        }
        else
        {
            std::cerr << "Error reading line " << line << std::endl;
        }
    }
    return objects;
}


void showObjects(const std::vector<Object>& a)
{
    for(auto i : a)
    {
        std::cout << i.name  << "\t" << i.x << " " << i.y << " "  << i.type << " " << i.creationTime << std::endl;
    }
}
void showObjects2(const std::multimap<std::string, Object>& a)
{
    for(auto i : a)
    {
        std::cout << i.second.name  << "\t" << i.second.x << " " << i.second.y << " "
                  << i.second.type << " " << i.second.creationTime << std::endl;
    }
}


//std::map<char, std::map<std::string, Object>> sorting
//    (std::vector<Object>& allObj, std::string& name, std::string& sortingType )

//std::map<char, Object> sorting(std::vector<Object>& allObj)
//{
//    std::map<char, Object> alfabet;
//    for(auto i : allObj)
//    {
//        if(alfabet.find(i.name[0]) != alfabet.end() )
//    }
//}


int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    std::fstream file("obj.txt");
    std::vector<Object> allObj;
    std::multimap<std::string ,Object> allObj2;

    allObj2 = getObjects2(file);
    std::cout << "We having base" << std::endl;
    showObjects2(allObj2);

    return 0;
}
