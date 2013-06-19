// g++ test_trackstring.cc ../scaletracker/trackstats.cc ../scaletracker/flipper.cc ../common/track.cc ../common/dem.cc ../common/scalespace.cc

#include "../scaletracker/trackstats.hh"

CriticalPair RR = {REG,REG};
CriticalPair RM = {REG,MAX};
CriticalPair MR = {MAX,REG};
CriticalPair Rm = {REG,MIN};
CriticalPair mR = {MIN,REG};

CriticalPair Ru = {REG,SA2};
CriticalPair uR = {SA2,REG};
CriticalPair Rd = {REG,SA3};
CriticalPair dR = {SA3,REG};

CriticalPair Mu = {MAX,SA2};
CriticalPair uM = {SA2,MAX};

CriticalPair md = {MIN,SA3};
CriticalPair dm = {SA3,MIN};


int main (void)
{
    TrackString ts (1);

    ts.insert (0, RR,RM); 
    ts.insert (0, RR,RM); 

    ts.insert (0, RR,mR);
    ts.insert (0, RR,mR);

    ts.insert (0, RM,RR); // 1

    ts.insert (0, RR,RM);
    ts.insert (0, RR,RM); // 4

    ts.insert (0, Mu,Ru);

    ts.insert (0, RR,mR);

    ts.insert (0, Mu,Ru); // 2

    ts.insert (0, RR,mR); // 4

    // total 11

    ts.print ();
}
