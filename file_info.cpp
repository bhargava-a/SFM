#include <iostream>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <thread>
#include <mutex>
                                            // \(ASCII value)=92
namespace fs = std::filesystem;             // C:\Windows\System32

class SysInfo{
    public:
    void Describe(const std::string& dir,std::mutex& mtx){
        try{
            std::lock_guard<std::mutex> lock(mtx); //synchronize output to console
            if(fs::is_directory(dir)){
                for (const auto &entry : fs::directory_iterator(dir)){
                    std::cout << entry.path() <<std::endl;
                }
            }else{
                std::cout<<"Note: Skipping directory listing since it's a file.\n";
            }
        }catch(const fs::filesystem_error& e){
            std::cerr<<"Error : "<<e.what()<<std::endl;
        }
    }
    float size(const std::string& path,std::mutex& mtx){
        try{
            auto sizeMB=fs::file_size(path)/(1024.0f*1024.0f);
            auto sizeGB=sizeMB/1024.0f;
            std::lock_guard<std::mutex> lock(mtx);  //synchronize output to console
            std::cout<<"File size: "<<sizeMB<<" MB"<<" or "<<sizeGB<<" GB"<<std::endl;
            //return sizeMB;
        }catch(const fs::filesystem_error &e){
            std::cerr<<"Error : "<<e.what()<<std::endl;
        }
        return 0.0f;
    }
    
    void Modification_details(const std::string& path,std::mutex& mtx){
        try{
            auto ftime=fs::last_write_time(path);

            //converting to system's clock timming;
            auto sctp=std::chrono::time_point_cast<std::chrono::system_clock::duration>(ftime-fs::file_time_type::clock::now()+std::chrono::system_clock::now());
            
            std::time_t cftime=std::chrono::system_clock::to_time_t(sctp);
            std::lock_guard<std::mutex> lock(mtx); 
            std::cout<<"Last modified: "<<std::asctime(std::localtime(&cftime));
        }catch(fs::filesystem_error &e){
            std::cerr<<"Error : "<<e.what()<<std::endl;
        }
    }

    void Extension(const std::string& loc,std::mutex& mtx){
        //fs::path is an object representing a file or directory path

        fs::path p(loc);
        auto ext=p.extension();
        std::lock_guard<std::mutex> lock(mtx);
        std::cout<<"File extension : "<<ext<<std::endl;
    }
};

void runParallel(SysInfo& obj,const std::string& path){
    std::mutex mtx;

    std::thread t1(&SysInfo::Describe,&obj,path,std::ref(mtx));
    std::thread t2(&SysInfo::size,&obj,path,std::ref(mtx));
    std::thread t3(&SysInfo::Modification_details,&obj,path,std::ref(mtx));
    std::thread t4(&SysInfo::Extension,&obj,path,std::ref(mtx));

    t1.join();
    t2.join();
    t3.join();
    t4.join();
}


int main() {
    SysInfo obj;
    std::string dir; /* "C:\\Users\\bharg\\Videos\\DL-videos\\15 Backpropagation in Deep Learning _ Part 1 _ The What_.mp4"; */
    std::cout<<"Enter the file or directory path : ";
    std::getline(std::cin, dir);
    if(!fs::exists(dir)){
        std::cout<<"File not found.\n";
        return 1;
    }
    std::cout<<std::fixed<<std::setprecision(2);
    runParallel(obj,dir);
    system("pause");
    return 0;
}
