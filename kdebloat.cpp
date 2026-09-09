// kdebloat.cpp lol
// be careful with this thing gng
// lol this shit is gonna cook your pc

#include <iostream>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <cstdio>
using namespace std;

bool dryRun=false;
bool skipAsk=false;


string grabOutput(const string& cmd)
{
    string out;
    FILE *p=popen(cmd.c_str(),"r");
    if(!p)return out;

    char buf[512];

    while(fgets(buf,sizeof(buf),p)!=NULL)
    {
        out+=buf;
    }

    pclose(p);
    return out;
}


int main(int argc,char** argv)
{
for(int i=1;i<argc;i++)
{
string arg=argv[i];

if(arg=="-n"||arg=="--dry-run") dryRun=true;
else if(arg=="-y"||arg=="--yes") skipAsk=true;
else if(arg=="-h"||arg=="--help")
{
cout<<"usage: kdebloat [-n] [-y]\n";
cout<<"  -n dry run only\n";
cout<<"  -y dont ask me stufff\n";
return 0;
}
}

cout<<"==================================\n";
cout<<"   labubuubububuubu kernel debloat\n";
cout<<"==================================\n";

if(geteuid()!=0)
{
cout<<"\nneeds root lol use sudo\n";
return 1;
}

if(access("/usr/bin/dpkg",F_OK)!=0)
{
cout<<"\nno dpkg here, probably not debian/ubuntu\n";
return 1;
}


string currentKernel=grabOutput("uname -r");

while(!currentKernel.empty() &&
      (currentKernel.back()=='\n'||currentKernel.back()=='\r'))
    currentKernel.pop_back();

if(currentKernel.empty())
{
cout<<"\ncouldnt get kerrnel not touching anything\n";
return 1;
}

cout<<"\nrunning kernel (staying): "<<currentKernel<<"\n";

string listCmd="dpkg --list | grep -E 'linux-(image|headers)-[0-9]' | awk '{print $2}' | grep -v \""+currentKernel+"\"";

string oldPkgs=grabOutput(listCmd);

if(oldPkgs.empty())
{
cout<<"\nnothing old found we chillin\n";
return 0;
}

cout<<"\nold kernel stuff found:\n";
cout<<oldPkgs;

if(!dryRun && !skipAsk)
{
cout<<"\nremove all that? type yes: ";

string ans;
getline(cin,ans);

if(ans!="yes" && ans!="y")
{
cout<<"ok leaving it alone\n";
return 0;
}
}

string purgeCmd=listCmd+" | xargs -r apt-get -y purge";
string autoremoveCmd="apt-get autoremove -y --purge";
string grubCmd="update-grub";

int stepsRun=0;

if(dryRun)
{
cout<<"\n[dry-run] "<<purgeCmd<<"\n";
cout<<"[dry-run] "<<autoremoveCmd<<"\n";
cout<<"[dry-run] "<<grubCmd<<"\n";
}
else
{
cout<<"\npurging old kernels...\n";

int result = system(purgeCmd.c_str());
if(result == 0)
    stepsRun++;

cout<<"doing autoremove...\n";

result = system(autoremoveCmd.c_str());
if(result == 0)
    stepsRun++;

cout<<"updating grub...\n";

result = system((grubCmd+" 2>/dev/null").c_str());
}

cout<<"\n-------------------------------\n";

if(dryRun)
    cout<<"dry run finished, nothing removed\n";
else
    cout<<"done. ran "<<stepsRun<<" commands\n";

cout<<"-------------------------------\n";

return 0;
}

