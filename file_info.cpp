#include <iostream>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <thread>
#include <mutex>

namespace fs = std::filesystem;         // \(ASCII value)=92 , // C:\Windows\System32

class SysInfo{
    public:
    void size(const std::string& path, std::mutex& mtx){
        try{
            auto sizeMB=fs::file_size(path)/(1024.0f * 1024.0f);
            auto sizeGB=sizeMB/1024.0f;
            std::lock_guard<std::mutex> lock(mtx);          //Mutexes to sync output to prevent jumbling of outputs
            std::cout<<"File: "<<path<<"\n";
            std::cout<<"  Size: "<<sizeMB<<" MB or "<<sizeGB<<" GB\n";
        }catch(const fs::filesystem_error& e){
            std::lock_guard<std::mutex> lock(mtx);          //Mutexes to sync ERROR output to prevent jumbling of outputs
            std::cerr<<"  [Size] Error: "<<e.what()<<"\n";
        }
    }

    void Modification_details(const std::string& path,std::mutex& mtx){
        try{
            auto ftime=fs::last_write_time(path);

            //converting to system's clock timming;
            auto sctp=std::chrono::time_point_cast<std::chrono::system_clock::duration>(ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now());
            std::time_t cftime = std::chrono::system_clock::to_time_t(sctp);
            std::lock_guard<std::mutex> lock(mtx);          //Mutexes to sync output to prevent jumbling of outputs
            std::cout<<"  Last modified: "<<std::asctime(std::localtime(&cftime));
        }catch(const fs::filesystem_error& e){
            std::lock_guard<std::mutex> lock(mtx);          //Mutexes to sync ERROR output to prevent jumbling of outputs
            std::cerr<<"  [Modify] Error: "<<e.what()<<"\n";
        }
    }

    void Extension(const std::string& path, std::mutex& mtx){
        fs::path p(path);
        auto ext=p.extension();
        std::lock_guard<std::mutex> lock(mtx);              //Mutexes to sync output to prevent jumbling of outputs
        std::cout<<"  Extension: "<<ext<<"\n\n";
    }

    void processFile(const std::string& path, std::mutex& mtx){
        std::thread t1(&SysInfo::size, this, path, std::ref(mtx));
        std::thread t2(&SysInfo::Modification_details, this, path, std::ref(mtx));
        std::thread t3(&SysInfo::Extension, this, path, std::ref(mtx));
        t1.join();
        t2.join();
        t3.join();
    }
};

int main() {
    SysInfo obj;
    std::string dir;
    std::mutex mtx;

    std::cout << "Enter the directory path to scan recursively: ";
    std::getline(std::cin, dir);

    if (!fs::exists(dir)) {
        std::cout << "File or directory not found.\n";
        return 1;
    }
    
    std::cout << std::fixed << std::setprecision(2);        //to format file sizes to only 2 decimal points;

    try{
        for(const auto& entry : fs::recursive_directory_iterator(dir)){
            try{
                if(fs::is_regular_file(entry)){
                    obj.processFile(entry.path().string(),mtx);
                }
            }catch(const std::exception& inner){
                std::lock_guard<std::mutex> lock(mtx);      //Mutexes to sync output to prevent jumbling of outputs
                std::cerr<<"[access denied]"<<inner.what()<<std::endl;
            }
        }
    } catch(const fs::filesystem_error& e){
        std::lock_guard<std::mutex> lock(mtx);              //Mutexes to sync ERROR output to prevent jumbling of outputs
        std::cerr<<"Traversal error : "<<e.what()<<std::endl;
    }

    system("pause");
    return 0;
}
