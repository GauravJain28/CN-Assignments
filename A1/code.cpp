#include <bits/stdc++.h>
#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>

std::string exec(const char* cmd) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}

std::string router_rtt(std::string route_ip){
    std::string cmd = "ping -4 -c 1 -s 60 -W 2 -n -t 100 "+route_ip;
    std::string res = exec(cmd.c_str());

    std::vector<std::string> lines;
    std::string line = ""; 
    for(auto x : res){
        if(x=='\n'){
            lines.push_back(line);
            line = "";
        }
        else line += x;
    }

    std::string rtt = "";
    if(lines.size()==6){
        int k=23;
        while(lines[5][k]!='/')
        rtt += lines[5][k++];
    }
    else rtt = "0.0";

    //std::cout<<"\tRound Trip Time : "<<rtt<<" seconds\n";    

    return rtt;
}

int main(int argc, char const *argv[]){

    std::string domain = "www.google.com";

    if (argc!=2){       // unexpected no. of parameters
        std::cout << "Please run the program using this command format \"./a.out WEBSITE NAME\"\n";
        return 1;
    }

    domain = argv[1];
    std::ofstream outFile("out.txt");
    //outFile<<"NoOfHops"<<" "<<"RoundTripTime"<<"\n";
    bool succ = false;
    for(int i=1;i<=30;i++){

        std::string cmd = "ping -4 -c 1 -s 60 -W 2 -n -t "+std::to_string(i)+" "+domain;
        
        std::string res = exec(cmd.c_str());

        std::vector<std::string> lines;
        std::string line = ""; 
        for(auto x : res){
            if(x=='\n'){
                lines.push_back(line);
                line = "";
            }
            else line += x;
        }

        std::cout<<"hop "<<i<<"\t";
        if(lines.size()==6 && lines[5]==""){
            //ping the router
            std::string route_ip = "";
            int k=5;
            while(lines[1][k]!=' ')
                route_ip += lines[1][k++];
                
            std::cout<<route_ip<<"\n";
            //call ping for route_ip
            std::string t = router_rtt(route_ip);
            outFile<<i<<" "<<t<<" "<<t<<"ms\n";
        }
        else if(lines.size()==5){
            //no router available
            std::cout<<"*\n";
            outFile<<i<<" "<<0.0<<" "<<0.0<<"ms\n";
        }
        else{
            //ping successful
            std::string route_ip = "";
            int k=14;
            while(lines[1][k]!=':')
                route_ip += lines[1][k++];
                
            std::cout<<route_ip<<"\n";

            std::string rtt = "";
            k=23;
            while(lines[5][k]!='/')
                rtt += lines[5][k++];

            outFile<<i<<" "<<rtt<<" "<<rtt<<"ms\n";
            std::cout<<"TRACEROUTE for "<<domain<<" ("<<route_ip<<") successful!\n";
            std::cout<<"Round Trip Time : "<<rtt<<" milliseconds\n";
            succ=true;
            break;
        }
    }
    outFile.close();

    if(!succ){
        std::cout<<"Cannot reach the target in 30 hops...\n";
        return 0;
    }
    std::string ss = "python3 graph.py "+domain;
    if(system(ss.c_str())==0) std::cout<<"plot saved successfully!\n";
    else std::cout<<"error while saving plot!\n";

    return 0;

}