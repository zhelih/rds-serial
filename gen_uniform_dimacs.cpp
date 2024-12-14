#include <random>
#include <chrono>
#include <string>
#include <iostream>
#include <sstream>

#define VERBOSE 0

int main(int argc, char* argv[])
{
  if (argc < 3)
  {
    std::cout << "Usage: " << argv[0] << " <p> <n>" << std::endl;
    return 0;
  }

  std::chrono::milliseconds ms = std::chrono::duration_cast< std::chrono::milliseconds >( std::chrono::system_clock::now().time_since_epoch() );
  std::mt19937 generator (ms.count());
  std::uniform_real_distribution<double> dis(0.0, 1.0);

  const double p = std::stod( argv[1] );
  const int n = std::stoi( argv[2] );

  int nr_edges = 0;

  std::stringstream buffer;

  for (int v1 = 1; v1 <= n; ++v1)
  {
    for (int v2 = v1+1; v2 <= n; ++v2)
    {
      double r = dis(generator);
      if (r <= p)
      {
        buffer << "e " << v1 << " " << v2 << std::endl;
        nr_edges++;
      }
    }
  }

  std::cout << "p n " << n << " " << nr_edges << std::endl;
  std::cout << buffer.str();

  if(VERBOSE)
  {
    int max_edges = n*(n-1)/2;
    printf("generated %d edges, density %f\n", nr_edges, static_cast<double>(nr_edges) / static_cast<double>(max_edges));
  }
  return 0;
}
