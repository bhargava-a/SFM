#include <iostream>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <iomanip>
                                            // \(ASCII value)=92
namespace fs = std::filesystem;             // C:\Windows\System32

class SysInfo{
    public:
    void Describe(const std::string& dir){
        try{
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
    float sizeMB(const std::string& path){
        try{
            auto sizeMB=fs::file_size(path)/(1024.0f*1024.0f);
            return sizeMB;
        }catch(const fs::filesystem_error &e){
            std::cerr<<"Error : "<<e.what()<<std::endl;
        }
        return 0.0f;
    }
    float sizeGB(const std::string& path){
        try{
            auto sizeGB=fs::file_size(path)/(1024.0f*1024.0f*1024.0f);
            return sizeGB;
        }catch(fs::filesystem_error &e){
            std::cerr<<"Error : "<<e.what()<<std::endl;
        }
        return 0.0f;
    }
    
    void Modification_details(const std::string& path){
        try{
            auto ftime=fs::last_write_time(path);

            //converting to system's clock timming;
            auto sctp=std::chrono::time_point_cast<std::chrono::system_clock::duration>(ftime-fs::file_time_type::clock::now()+std::chrono::system_clock::now());
            
            std::time_t cftime=std::chrono::system_clock::to_time_t(sctp);
            std::cout<<"Last modified: "<<std::asctime(std::localtime(&cftime));
        }catch(fs::filesystem_error &e){
            std::cerr<<"Error : "<<e.what()<<std::endl;
        }
    }

    void Extension(const std::string& loc){
        //fs::path is an object representing a file or directory path

        fs::path p(loc);
        auto ext=p.extension();
        std::cout<<"File extension : "<<ext<<std::endl;
    }
};
int main() {
    SysInfo obj;
    std::string dir;/*"C:\\Users\\bharg\\Videos\\DL-videos\\15 Backpropagation in Deep Learning _ Part 1 _ The What_.mp4";*/
    std::cout<<"Enter the file or directory path : ";
    std::getline(std::cin, dir);
    if(!fs::exists(dir)){
        std::cout<<"File not found.\n";
        return 1;
    }
    obj.Describe(dir);
    std::cout<<std::fixed<<std::setprecision(2);
    std::cout<<"file size in "<<obj.sizeMB(dir)<<"MB or "<<obj.sizeGB(dir)<<"GB"<<std::endl;
    obj.Modification_details(dir);
    obj.Extension(dir);
    return 0;
}