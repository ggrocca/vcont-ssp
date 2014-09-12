#include "../../common/dem.hh"
#include "../../common/scalespace.hh"

void print_help()
{
  fprintf(stderr, "Usage: ./dems2ssp n dem1 .. demN out.ssp\n");
}

using namespace std;

int main(int argc, char * argv[])
{
  int n_dems=atoi(argv[1]);
  ScaleSpace ssp;
  ssp.dem.reserve(n_dems);
  ssp.levels = n_dems;
  for (int i=0; i<n_dems; i++)
    {
      FILE * fp = fopen(argv[i+2], "r");
      cout << "read " << argv[i+2] << endl;
      ssp.dem.push_back(new Dem(fp));
    }

      
  ssp.write_scalespace(argv[n_dems+2]);
}

