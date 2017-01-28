#include <cstdio>
#include <string>
#include <fstream>
#include <vector>
#include <cstring>

using namespace std;

inline float dens(unsigned int n, unsigned int e)
{
  return ((float)e)/((float)(n*(n-1)/2));
}

int main(int argc, char* argv[])
{
  ios::sync_with_stdio();
  if(argc < 3)
  {
    printf("Too few arguments.\n");
    return 0;
  }
//  string dir = argv[1];

  ifstream files(argv[2]);
  if(!files.is_open())
  {
    fprintf(stderr, "Cannot open %s\n", argv[2]);
    return 1;
  }
  vector<string> s; string line;
  while(getline(files, line))
  {
    s.push_back(line);
  }
  files.close();
  vector<string> modes; 
  modes.push_back("-c");
  modes.push_back("-s");
  modes.push_back("-d 1");
  modes.push_back("-d 2");
  modes.push_back("-d 3");
  modes.push_back("-p 2");
  modes.push_back("-p 3");
  modes.push_back("-p 4");
  modes.push_back("-iuc");
  for(auto mode = modes.begin(); mode != modes.end(); ++mode)
  {
    printf("Mode %s\n", mode->c_str());
    for(auto graph_file = s.begin(); graph_file != s.end(); ++graph_file)
    {
      char buf[256];
      snprintf(buf, sizeof(buf), "./rds -t 600 %s %s/%s", mode->c_str(), argv[1], graph_file->c_str());
      fprintf(stderr, "Running %s\n", buf);
      FILE* f = popen(buf, "r");
      float time_el = 0.;
      unsigned int nr_n = 1, nr_e = 0;
      unsigned int res = 0;
      while(fgets(buf, sizeof(buf), f) != NULL)
      {
        string pr1("rds: time elapsed =");
        if(!strncmp(buf, pr1.c_str(), pr1.size()))
          sscanf(buf, "rds: time elapsed = %f secs", &time_el);
        string pr2("graph: ");
        if(!strncmp(buf, pr2.c_str(), pr2.size()))
          sscanf(buf, "graph: %u nodes, %u edges", &nr_n, &nr_e);
        string pr3("Solution verification:");
        if(!strncmp(buf, pr3.c_str(), pr3.size()))
        {
          pr3 += " CORRECT";
          if(strncmp(buf, pr3.c_str(), pr3.size()))
            fprintf(stderr, "Warning : incorrect sol for %s mode %s, parsed %s\n", graph_file->c_str(), mode->c_str(), buf);
        }
        string pr4("RDS returned res = ");
        if(!strncmp(buf, pr4.c_str(), pr4.size()))
          sscanf(buf, "RDS returned res = %u", &res);
      }
      pclose(f);
      printf("%s & %u & %u & %.2f & %u & %.2f \\\\\n", graph_file->c_str(), nr_n, nr_e, dens(nr_n, nr_e), res, time_el);
      fflush(stdout);
    }
  }

  return 0;
}
