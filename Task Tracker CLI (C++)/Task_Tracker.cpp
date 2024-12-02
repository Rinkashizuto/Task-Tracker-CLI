#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <set>

class TaskManager {
private:
    struct Task {
        int id;
        std::string description;
        std::string createdAt;
        bool isDone = false;
    };

    int nextavailableID = 1;
    std::vector<Task> tasks;

    std::string GetCurrentTime() {
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::tm local_time = *std::localtime(&now_c);

        bool Is_pm = local_time.tm_hour >= 12;
        if (local_time.tm_hour > 12) local_time.tm_hour -= 12;
        if (local_time.tm_hour == 0) local_time.tm_hour = 12;

        std::ostringstream oss;
        oss << std::put_time(&local_time, "%I:%M:%S ") << (Is_pm ? "PM" : "AM");
        return oss.str();
    }

    void loadTask() {
        std::ifstream data("System.json");
        if (!data.is_open()) {
            std::cerr << "Unable to open System.json to read.\n";
            return;
        }
        
        tasks.clear();
        std::string line;
        Task task;
        std::set<int> usedIDs;

        while (std::getline(data, line)) {
            if (line.find("Task : ") != std::string::npos) {
                task.description = line.substr(7);
            } 
            else if (line.find("Created at : ") != std::string::npos) {
                task.createdAt = line.substr(13);
            } 
            else if (line.find("ID : ") != std::string::npos) {
                task.id = std::stoi(line.substr(5));
                usedIDs.insert(task.id);
                tasks.push_back(task);
            }
            else if(line.find("Status : ") != std::string::npos)
            {
                task.isDone = (line.substr(7) == "Done");
            }
        }
        data.close();

        // Find the next available ID
        nextavailableID = 1;
        while (usedIDs.count(nextavailableID) > 0) {
            nextavailableID++;
        }
    }

    void saveTask() {
        std::sort(tasks.begin(), tasks.end(), 
            [](const Task& a, const Task& b) { return a.id < b.id; });

        std::ofstream data("System.json");
        if (!data.is_open()) {
            std::cerr << "Unable to open System.json to write.\n";
            return;
        }
        
        for (const auto& task : tasks) {
            data << "Task : " << task.description << std::endl
                 << "Created at : " << task.createdAt << std::endl
                 << "ID : " << task.id << std::endl
                 << "Status : " << (task.isDone ? "Done" : "Not Done") << std::endl << std::endl;
        }
        data.close();
    }

public:

    TaskManager() {
        loadTask();
    }

    void addTask() {
        Task task;
        task.createdAt = GetCurrentTime();
        task.id = nextavailableID;

        std::cout << "Enter task description : ";
        std::cin.ignore();
        std::getline(std::cin, task.description);

        tasks.push_back(task);
        
        nextavailableID++;
        while (std::any_of(tasks.begin(), tasks.end(), 
               [this](const Task& t) { return t.id == nextavailableID; })) {
            nextavailableID++;
        }

        saveTask();
        std::cout << "Task successfully added!\n"
        << "ID : " << task.id << '\n';
    }

    void updateTask() {
        int id;
        std::string newDescription;

        std::cout << "Enter the task ID to update: ";
        std::cin >> id;
        std::cin.ignore();

        auto it = std::find_if(tasks.begin(), tasks.end(), 
            [id](const Task& task) { return task.id == id; });

        if (it != tasks.end()) {
            std::cout << "Current Task: " << it->description << "\n";
            std::cout << "Enter new description: ";
            std::getline(std::cin, newDescription);

            it->description = newDescription;
            saveTask();
            std::cout << "Task successfully updated!\n";
        }
        else {
            std::cerr << "Task ID not found.\n";
        }
    }

    void deleteTask() {
        int id;
        std::cout << "Enter the ID of the task you want to delete: ";
        std::cin >> id;

        auto it = std::remove_if(tasks.begin(), tasks.end(), 
            [id](const Task& task) { return task.id == id; });

        if (it != tasks.end()) {
            tasks.erase(it, tasks.end());
            saveTask();

            std::set<int> usedIDs;
            for (const auto& task : tasks) {
                usedIDs.insert(task.id);
            }

            nextavailableID = 1;
            while (usedIDs.count(nextavailableID) > 0) {
                nextavailableID++;
            }

            std::cout << "Task successfully deleted!\n";
        }
        else {
            std::cerr << "Task ID not found.\n";
        }
    }

    void displayTask(){
        std::cout << "=============================\n";
        std::cout << "\tCurrent Tasks\n";
        std::cout << "=============================\n";
        for(const auto& task : tasks)
        {
            std::cout << "ID : " << task.id << '\n';
            std::cout << "Description : " << task.description << '\n';
            std::cout << "Created at : " << task.createdAt << '\n';
            std::cout << "Status : " << (task.isDone ? "Done":"Not Done") << '\n';
            std::cout << "-----------------------------\n";
        }
    }

    void displayFinishedTask()
    {
        std::cout << "=============================\n";
        std::cout << "\tFinished Tasks\n";
        std::cout << "=============================\n";
        for(const auto& task : tasks)
        {
            if(task.isDone)
            {
                std::cout << "ID : " << task.id << '\n';
                std::cout << "Description : " << task.description << '\n';
                std::cout << "Created At : " << task.createdAt << '\n';
                std::cout << "-----------------------------\n";
            }
        }
    }

    void markTaskAsDone()
    {
        int id;
        std::cout << "Enter the id you want to mark as done\n";
        std::cin >> id;

        auto it = std::find_if(tasks.begin(), tasks.end(), 
        [id](const Task& task) { return task.id == id; });

        if (it != tasks.end()) {
        it->isDone = true;
        saveTask();
        std::cout << "Task successfully marked as done!\n";
    }
        else{
        std::cerr << "Task ID not found.\n";
        }
    }

    void markTaskAsUndone()
    {
        int id;
        std::cout << "Enter the task you want to mark as not done\n";
        std::cin >> id;

        auto it = std::find_if(tasks.begin(), tasks.end(), 
        [id](const Task& task) { return task.id == id; });

    if (it != tasks.end()) {
        it->isDone = false;
        saveTask();
        std::cout << "Task successfully marked as not done!\n";
    }
    else {
        std::cerr << "Task ID not found.\n";
    }

    }


};

int main() {
    TaskManager taskmanage;
    int choice;
    do {
        std::cout << "=============================\n";
        std::cout << "\tTASK MANAGER\n";
        std::cout << "=============================\n";
        std::cout << "\t1. Add Tasks\n";
        std::cout << "\t2. Update Tasks\n";
        std::cout << "\t3. Delete Task\n";  
        std::cout << "\t4. Display Task\n";
        std::cout << "\t5. Mark Task As Done\n";
        std::cout << "\t6. Mark Task As Not Done\n";
        std::cout << "\t7. Display Finished Task\n";
        std::cout << "\t8. Exit\n";
        
        std::cin >> choice;

        switch (choice) {
            case 1:
                taskmanage.addTask();
                break;
            case 2:
                taskmanage.updateTask();
                break;
            case 3:
                taskmanage.deleteTask();
                break;
            case 4:
                taskmanage.displayTask();
                break;
            case 5:
                taskmanage.markTaskAsDone();
                break;
            case 6:
                taskmanage.markTaskAsUndone();
            case 7:
                taskmanage.displayFinishedTask();     
        }
    } while (choice != 8);
}