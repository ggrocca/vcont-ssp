#include "trackdisplay.hh"
#include "CImg.h"

using namespace cimg_library;

double white[3] =   {1.0,1.0,1.0};
double black[3] =   {0.0,0.0,0.0};
double red[3] =     {1.0,0.0,0.0};
double green[3] =   {0.0,1.0,0.0};
double blue[3] =    {0.0,0.0,1.0};
double yellow[3] =  {1.0,1.0,0.0};
double cyan[3] =    {0.0,1.0,1.0};
double magenta[3] = {1.0,0.0,1.0};

int signs_w , signs_h;
int height_w , height_h;
int shape_w , shape_h;
int val_w , val_h;
Grid<char> signs;
Grid<double> heights;
Grid<unsigned int> vals;
vector<Grid<double> > shapeIndices;
double max_h;

// Scale colore se ne trovano tante in giro, una che di solito
// funziona bene, su un intervallo [0,1] e':
// - R a zero fino a 0.5 e poi a salire lineare fino a 1 da li' in poi
// - B esattamente l'opporto (scende fino a 0.5 poi resta zero)
// - G parte da 0, sale fino a 1 a 0.5 e poi ridiscende fino a 0

void time2color (double t, double tmin, double tmax, double color[])
{
    double st = (t - tmin) / tmax;

    color[0] = st < 0.5? 0.0 : (st - 0.5) * 2.0;
    color[1] = st < 0.5? st * 2.0 : 1.0 - ((st - 0.5) * 2.0);
    color[2] = st < 0.5? (0.5 - st) * 2.0 : 0.0;
    
    // printf ("@ st: %.2lf @ color: {%.2lf , %.2lf , %.2lf}\n",
    // 	    st, color[0], color[1], color[2]);
}


int TrackDisplay::searchNoseTip()
{
  double thresh=-DBL_MAX;
  double scale= -DBL_MAX;
  int cand=-1;
  for (int i=0; i<spots_maxima.size(); i++)
    {
      //		if (signs(track->lines[spots_maxima[i].crit].entries[0].c)!='p')
      //continue;
      double life= spots_maxima[i].elix;
      double imp = track->lines[spots_maxima[i].crit].strength;
      double t_scale= track->lines[spots_maxima[i].crit].scale;
      if (isinf(imp))
	continue;
      double dist=fabs(track->width/2-track->lines[spots_maxima[i].crit].entries[0].c.x);
      double dist_y=fabs(track->height/2-track->lines[spots_maxima[i].crit].entries[0].c.y);
      if (dist_y>track->height*0.1)// || heights(track->lines[spots_maxima[i].crit].entries[0].c)<(max_h*0.99))
       	continue;
      //if (t_scale > scale && (pow(life,10)*imp)-(pow(1000,dist)+pow(100,dist_y))>thresh)
      // if (t_scale*imp*life>thresh)
      // 	{
      // 	  // thresh=(pow(life,10)*imp)-(pow(2,dist)+pow(2,dist_y));
      // 	  cand=i;
      // 	  thresh=t_scale*life*imp;
      // 	  //scale=t_scale;
      // 	}
      //		spots_fiducial.push_back(CritNikolas(spots_maxima[i].crit,spots_maxima[i].elix, track->lines[spots_maxima[i].crit].strength,track->lines[spots_maxima[i].crit].scale));
      if (heights(track->lines[spots_maxima[i].crit].entries[0].c)>thresh)
	{
	  cand=i;
	  thresh=heights(track->lines[spots_maxima[i].crit].entries[0].c);
	}
      
    }

  return cand;
}

void TrackDisplay::searchNoseLimits(double x_nose, double y_nose, int under, int& cand1, int& cand2)
{
  double thresh=-DBL_MAX;
  double scale= -DBL_MAX;
  double soft = DBL_MAX;
  int cand=-1;
  for (int i=0; i<spots_minima.size(); i++)
    {
	
      double life= spots_minima[i].elix;
      double imp = track->lines[spots_minima[i].crit].strength;
      double t_scale= track->lines[spots_minima[i].crit].scale;
      //		if (isinf(imp))
      //continue;
      double x=track->lines[spots_minima[i].crit].entries[0].c.x;
      double y=track->lines[spots_minima[i].crit].entries[0].c.y;
      double dist=fabs(x-x_nose);
      double dist_y=fabs(y-y_nose);
      if (dist>track->width*0.2 || dist_y>track->height*0.05 || x<=x_nose || under==i)
	continue;
      for (int j=0; j<spots_minima.size(); j++)
	{
	   double life2= spots_minima[j].elix;
	   double imp2 = track->lines[spots_minima[j].crit].strength;
	   double t_scale2 = track->lines[spots_minima[j].crit].scale;
	   //		if (isinf(imp))
	   //continue;
	   double x2=track->lines[spots_minima[j].crit].entries[0].c.x;
	   double y2=track->lines[spots_minima[j].crit].entries[0].c.y;
	   double dist2=fabs(x2-x_nose);
	   double dist_y2=fabs(y2-y_nose);
	   if (dist2>track->width*0.2 || dist_y2>track->height*0.05 || x2>=x_nose  || under==j)
	     continue;

	   //	   if (fabs(dist2-dist)<track->width*0.05 && fabs(y-y2) < track->height * 0.02  && min(life*1,life2*1)*dist2*dist > thresh)
	   if (life*life2>thresh)  
	   {
	     //thresh=min(life*imp,life2*imp2)*dist2*dist;
	     thresh=life*life2;
	     cand1=i;
	       cand2=j;
	     }
     
	}
   } 
}


void TrackDisplay::searchMouth(double x_nose,  double y_nose, double x_l, double x_r, int& cand1, int& cand2, int& cand3, int& cand4)
{
  double y_cand;
  double thresh_l, thresh_r;
  thresh_l=thresh_r=-DBL_MAX;
  /* Prima cerco i due punti laterali */
  for (int i=0; i<spots_maxima.size(); i++)
    {
	
      double life= spots_maxima[i].elix;
      double imp = track->lines[spots_maxima[i].crit].strength;
      //imp=(imp-min_imp)/(max_imp-min_imp)*track->time_of_life;
      double t_scale= track->lines[spots_maxima[i].crit].scale;
      //		if (isinf(imp))
      //continue;
      double x=track->lines[spots_maxima[i].crit].entries[0].c.x;
      double y=track->lines[spots_maxima[i].crit].entries[0].c.y;
      if (/*fabs(x-x_nose)<15*/ y>y_nose || heights(x,y)<border_cut || y>y_nose-50 || y<track->height/6 || x>=x_nose ) continue;

      double x_half=track->width/2;
      double y2=y;
      double x2=x_half+(x_half-x);

      for (int j=0; j<spots_maxima.size(); j++)
	{
	  if (i==j)
	    continue;
	   double life2= spots_maxima[j].elix;
	   double imp2 = track->lines[spots_maxima[j].crit].strength;
	   //   imp2=(imp2-min_imp)/(max_imp-min_imp)*track->time_of_life;
	   double t_scale2 = track->lines[spots_maxima[j].crit].scale;
	   //		if (isinf(imp))
	   //continue;
	   double x22=track->lines[spots_maxima[j].crit].entries[0].c.x;
	   double y22=track->lines[spots_maxima[j].crit].entries[0].c.y;
	   double dist2=fabs(x2-x_nose);
	   if (/* fabs(x22-x_nose)<1*/  y22>y_nose || heights(x22,y22)<border_cut || y22>y_nose-50  || y22<track->height/6 || x22<=x_nose) continue;
	   double distance=sqrt(pow(x2-x22,2)+pow(y2-y22,2));
	   if (fabs((x22-x)-fabs(x_r-x_l))>20)
	     continue;
	   if (distance<30)
	     {
	       if ((imp*imp2)/*(x22-x)*/>thresh_l && fabs(y-y22)<10)
		 {
		   thresh_l=(imp*imp2)/**(x22-x)*/;//life*life2;
		   cand1=i;
		   cand2=j;
		   y_cand=y;
		 }
	     }	
}
    }
  
  double thresh_up, thresh_down;
  thresh_up=thresh_down=-DBL_MAX;
  /* Ora cerco i centrali */
  for (int i=0; i<spots_maxima.size(); i++)
    {
      double life= spots_maxima[i].elix;
      double imp = track->lines[spots_maxima[i].crit].strength;
      double t_scale= track->lines[spots_maxima[i].crit].scale;
      //		if (isinf(imp))
      //continue;
      double x=track->lines[spots_maxima[i].crit].entries[0].c.x;
      double y=track->lines[spots_maxima[i].crit].entries[0].c.y;
      if (fabs(x-x_nose)>25 || y>y_nose || heights(x,y)<border_cut || y>y_nose-20) continue;
      
      double val=imp*life;
      if (life>thresh_up && y>y_cand)
	{
	  cand3=i;
	  thresh_up = imp*life;
	}
      else if (life>thresh_down && y<y_cand && y>y_cand-50)
	{
	  cand4=i;
	  thresh_down = imp*life;
	}

    }
}


// void TrackDisplay::bestFitVertical(double x_nose, double y_nose, double x_root, double y_root, double x_under, double y_nose)
// {
// }

void TrackDisplay::searchEyesAngles(double x_nose, double y_nose, int& cand1, int& cand2, int& cand3, int& cand4)
{
  double thresh=-DBL_MAX;
  double thresh2=-DBL_MAX;
  double scale= -DBL_MAX;
  double soft = DBL_MAX;
  int cand=-1;
  for (int i=0; i<spots_maxima.size(); i++)
    {
	
      double life= spots_maxima[i].elix;
      double imp = track->lines[spots_maxima[i].crit].strength;
      double t_scale= track->lines[spots_maxima[i].crit].scale;
      //		if (isinf(imp))
      //continue;
      double x=track->lines[spots_maxima[i].crit].entries[0].c.x;
      double y=track->lines[spots_maxima[i].crit].entries[0].c.y;
      double dist=fabs(x-x_nose);
      double dist_y=fabs(y-y_nose);
      if (life < track->time_of_life*0.95 ||  x<=x_nose || y<=y_nose) 
	continue;
      double x_half=track->width/2;
      double y2=y;
      double x2=x_half+(x_half-x);


      for (int j=0; j<spots_maxima.size(); j++)
	{
	  if (i==j)
	    continue;
	   double life2= spots_maxima[j].elix;
	   double imp2 = track->lines[spots_maxima[j].crit].strength;
	   double t_scale2 = track->lines[spots_maxima[j].crit].scale;
	   //		if (isinf(imp))
	   //continue;
	   double x22=track->lines[spots_maxima[j].crit].entries[0].c.x;
	   double y22=track->lines[spots_maxima[j].crit].entries[0].c.y;
	   double dist2=fabs(x2-x_nose);
	   double dist_y2=fabs(y2-y_nose);
	   if (life < track->time_of_life*0.95 || x2>=x_nose  || y2<=y_nose)
	     continue;
	   double distance=sqrt(pow(x2-x22,2)+pow(y2-y22,2));
	   if (distance<25)
	     {
		    if (/*min(life_left,life_right)>0.7*max(life_left,life_right) && */(heights(x22,y22)>border_cut-1000)) /*&& min(strength_left,strength_right)>0.7*max(strength_left,strength_right)*/
		      {
			if (imp*imp2>thresh)
			  {
			    thresh=imp*imp2;
			    //thresh=distance;
			    cand1=i;
			    cand2=j;
			  }
		      }
	     }
	}
    }
 
  /*Devo capire se ho preso gli esterni o gli interni... */
  isInner=abs(track->lines[spots_maxima[cand1].crit].entries[0].c.x-track->lines[spots_maxima[cand2].crit].entries[0].c.x)<track->width/3;
  thresh=-DBL_MAX;
    for (int i=0; i<spots_maxima.size(); i++)
    {
      if (i==cand1) continue;
      double life= spots_maxima[i].elix;
      double imp = track->lines[spots_maxima[i].crit].strength;
      double t_scale= track->lines[spots_maxima[i].crit].scale;
      //		if (isinf(imp))
      //continue;
      double x=track->lines[spots_maxima[i].crit].entries[0].c.x;
      double y=track->lines[spots_maxima[i].crit].entries[0].c.y;
      double dist=fabs(x-x_nose);
      double dist_y=fabs(y-y_nose);
      if (/*life < track->time_of_life*0 || */ x<=x_nose || (!isInner &&x>=track->lines[spots_maxima[cand1].crit].entries[0].c.x) || (isInner &&x<=track->lines[spots_maxima[cand1].crit].entries[0].c.x) || fabs(y-track->lines[spots_maxima[cand1].crit].entries[0].c.y)>55)
	continue;
      double x_half=track->width/2;
      double y2=y;
      double x2=x_half+(x_half-x);


      for (int j=0; j<spots_maxima.size(); j++)
	{
	  if (i==j || j==cand2)
	    continue;
	   double life2= spots_maxima[j].elix;
	   double imp2 = track->lines[spots_maxima[j].crit].strength;
	   double t_scale2 = track->lines[spots_maxima[j].crit].scale;
	   //		if (isinf(imp))
	   //continue;
	   double x22=track->lines[spots_maxima[j].crit].entries[0].c.x;
	   double y22=track->lines[spots_maxima[j].crit].entries[0].c.y;
	   double dist2=fabs(x2-x_nose);
	   double dist_y2=fabs(y2-y_nose);

	   if (/*life < track->time_of_life*0 || */ x22>=x_nose || (!isInner &&x22<=track->lines[spots_maxima[cand2].crit].entries[0].c.x) || (isInner &&x22>=track->lines[spots_maxima[cand2].crit].entries[0].c.x) || fabs(y22-track->lines[spots_maxima[cand2].crit].entries[0].c.y)>55)
	     continue;
	   double distance=sqrt(pow(x2-x22,2)+pow(y2-y22,2));
	   if (distance<25)
	     {
		    if (/*min(life_left,life_right)>0.7*max(life_left,life_right) && */(heights(x22,y22)>border_cut-1000)) /*&& min(strength_left,strength_right)>0.7*max(strength_left,strength_right)*/
		      {
			if (imp*imp2>thresh)
			  {
			    thresh=imp*imp2;
			    //thresh=distance;
			    cand3=i;
			    cand4=j;
			  }
		      }
	     }
	}
    }
}


void TrackDisplay::searchEyebrows(double y_lower, int& cand1, int& cand2)
{
  double thresh=-DBL_MAX;
    for (int i=0; i<spots_maxima.size(); i++)
    {
	
      double life= spots_maxima[i].elix;
      double imp = track->lines[spots_maxima[i].crit].strength;
      double t_scale= track->lines[spots_maxima[i].crit].scale;
      //		if (isinf(imp))
      //continue;
      double x=track->lines[spots_maxima[i].crit].entries[0].c.x;
      double y=track->lines[spots_maxima[i].crit].entries[0].c.y;
      double dist_y=fabs(y-y_lower);
      if (y<=y_lower) 
	continue;
      double x_half=track->width/2;
      double y2=y;
      double x2=x_half+(x_half-x);
      if (x>x_half) continue;

      for (int j=0; j<spots_maxima.size(); j++)
	{
	  if (i==j)
	    continue;
	   double life2= spots_maxima[j].elix;
	   double imp2 = track->lines[spots_maxima[j].crit].strength;
	   double t_scale2 = track->lines[spots_maxima[j].crit].scale;
	   //		if (isinf(imp))
	   //continue;
	   double x22=track->lines[spots_maxima[j].crit].entries[0].c.x;
	   double y22=track->lines[spots_maxima[j].crit].entries[0].c.y;
	   double dist_y2=fabs(y2-y_lower);
	   if (y22<=y_lower) 
	     continue;
	   if (x<x_half) continue;
	   double distance=sqrt(pow(x2-x22,2)+pow(y2-y22,2));
	   if (distance<25)
	     {
		    if (/*min(life_left,life_right)>0.7*max(life_left,life_right) && */(heights(x22,y22)>border_cut)) /*&& min(strength_left,strength_right)>0.7*max(strength_left,strength_right)*/
		      {
			if (fabs(imp*imp2)>thresh)
			  {
			    thresh=fabs(imp*imp2);
			    //			    thresh=life*life2*imp*imp2;
			    //thresh=distance;
			    cand1=i;
			    cand2=j;
			  }
		      }
	     }
	}
    }

}

int TrackDisplay::searchNoseRoot (double x_nose, double y_nose, double y_eyes=0.0f)
{
  double thresh=-DBL_MAX;
  int cand = -1;

  for (int i=0; i<spots_minima.size(); i++)
    {
      double life= spots_minima[i].elix;
      double imp = track->lines[spots_minima[i].crit].strength;
      double t_scale= track->lines[spots_minima[i].crit].scale;
      //if (isinf(imp))
      //continue;
      double x=track->lines[spots_minima[i].crit].entries[0].c.x;
      double y=track->lines[spots_minima[i].crit].entries[0].c.y;
      // 		double distL=x-x_sideleft;
      //double distR=x_sideright-x;
      double distN= y-y_nose;
      double distXN=fabs(x-x_nose);
      double distUpper(track->height-y);
      /*if (distN>distUpper*1.2 || distN<distUpper*0.8)
	continue;*/
      if ( y<y_nose || distXN > 0.05 * track->width)
	continue;
      if (y_eyes!=0.0f && fabs(y-y_eyes)>track->height*0.02)
	continue;
      if (imp*life>thresh)
	{
	  cand=i;
	  thresh=imp*life;
	}
    }
  return cand;
}

int TrackDisplay::searchUnderNose(double x_nose, double y_nose)
{


  double thresh=-DBL_MAX;
  int cand = -1;

  for (int i=0; i<spots_minima.size(); i++)
    {
      double life= spots_minima[i].elix;
      double imp = track->lines[spots_minima[i].crit].strength;
      double t_scale= track->lines[spots_minima[i].crit].scale;
      //if (isinf(imp))
      //continue;
      double x=track->lines[spots_minima[i].crit].entries[0].c.x;
      double y=track->lines[spots_minima[i].crit].entries[0].c.y;
      //	double distL=x-x_sideleft;
      //double distR=x_sideright-x;
      double distYN= y_nose-y;
      double distXN=fabs(x-x_nose);
      if (/*x<x_sideleft || x>x_sideright ||*/ y>y_nose  || distXN > 0.03 * track->width || distYN > track->height *0.1)
	continue;
      if (imp>thresh)
	{
	  cand=i;
	  thresh=imp;
	}

    }
  return cand;
}

void TrackDisplay::computeAxis(double& slope1,double&  yint1,double& slope2, double& yint2, vector<CritNikolas> spots_fiducial)
{
  double nose_x=track->lines[spots_fiducial[0].crit].entries[0].c.x;
  double nose_y=track->lines[spots_fiducial[0].crit].entries[0].c.y;

  double nose_Rootx=track->lines[spots_fiducial[1].crit].entries[0].c.x;
  double nose_Rooty=track->lines[spots_fiducial[1].crit].entries[0].c.y;
 
  double nose_LimitsRx=track->lines[spots_fiducial[2].crit].entries[0].c.x;
  double nose_LimitsRy=track->lines[spots_fiducial[2].crit].entries[0].c.y;
  double nose_LimitsLx=track->lines[spots_fiducial[3].crit].entries[0].c.x;
  double nose_LimitsLy=track->lines[spots_fiducial[3].crit].entries[0].c.y;
 
  double nose_UnderX=track->lines[spots_fiducial[4].crit].entries[0].c.x;
  double nose_UnderY=track->lines[spots_fiducial[4].crit].entries[0].c.y;


  /* Linea tra limits */
  int c = 2;
  double sumX=nose_LimitsRx+nose_LimitsLx;
  double sumY=nose_LimitsRy+nose_LimitsLy;
  double sumX2=pow(nose_LimitsRx,2)+pow(nose_LimitsLx,2);
  double sumXY=nose_LimitsRx*nose_LimitsRy+nose_LimitsLx*nose_LimitsLy;
  double Xmean = sumX/c;
  double Ymean = sumY/c;
  slope1=(sumXY-sumX*Ymean)/(sumX2-sumX*Xmean);
  yint1=Ymean-slope1*Xmean;

  /*Best fit altri tre */
  c = 3;
  sumX=nose_x+nose_Rootx+nose_UnderX;
  sumY=nose_y+nose_Rooty+nose_UnderY;
  sumX2=pow(nose_x,2)+pow(nose_Rootx,2)+pow(nose_UnderX,2);
  sumXY=nose_x*nose_y+nose_Rootx*nose_Rooty+nose_UnderX*nose_UnderY;
  Xmean = sumX/c;
  Ymean = sumY/c;
  double slopeTemp1=(sumXY-sumX*Ymean)/(sumX2-sumX*Xmean);
  double yintT2=Ymean-slopeTemp1*Xmean;
  
  //cout << "slopteTemp 1 " <<slopeTemp1<<endl;
  
  double slopeTemp2=(-1/(slope1));
  if (slopeTemp2*slopeTemp1<0)
    slopeTemp2*=-1;
//cout << "slopteTemp 2 " <<slopeTemp2<<endl;

  double dem1= sqrt(pow(slopeTemp1,2)+1);
  double dem2= sqrt(pow(slopeTemp2,2)+1);

  //cout << "dem1 " << dem1 << endl;
  //cout << "dem2 " << dem2 << endl;


  double A=slopeTemp1/dem1+slopeTemp2/dem2;
  double B=(-1)/dem1+(-1)/dem2;

  //cout << "A " << A << endl;
  //cout << "B " << B << endl;

  double medX=0.5*(nose_x+0.5*(nose_LimitsRx+nose_LimitsLx));
  double medY=0.5*(nose_y+0.5*(nose_LimitsRy+nose_LimitsLy));

  slope2=-A/B;
  yint2=medY-(slope2*medX);//-slope2*nose_x;

  // slope2=slopeTemp2;
  // yint2=Ymean-slope2*Xmean;
}


TrackDisplay::TrackDisplay()
{

  max_imp=-DBL_MAX;
  min_imp=DBL_MAX;
    // draw_time_labels = false;
    draw_terrain = true;
    draw_track = false;
    draw_query = false;
    // draw_csv = false;
    clip_black = 0.0;
    clip_white = 65535.0;
    query_scale = 0.1;
    query_mult = 1.0;
    query_cur_pos = true;
    query_death = true;
    track_scale = 0.1;
    track_mult = 1.0;
    vquery = std::vector<TrackRenderingEntry>(0);
    draw_lines = false;
    lines_width = 1.0;
    lines_size_clip = 0;
    lines_life_clip = 0.0;
    lines_query = false;

    draw_nikolas=true;
    draw_maxima=false;
    bool showSigns=false;
    show_axis=false;
    fiducialShow=0;
    draw_minima=false;
    draw_sellae=false;
    scale_by_life=true;
    scale_by_size=false;
    life_imp=0.01;
    strength_imp=0.01;
    show_nose=true;
    show_noseLimits=true;
    show_noseRoot=true;
    show_underNose=true;
    show_eyesOuterAngles=true;
    show_mouth=true;
    draw_elixir = false;
    elixir_mult = 1.0;
    elixir_scale = 0.5;
    elixir_cut = 0.0;
    importance_mult = 0.05;
    importance_scale = 0.3;
    importance_cut = 0.0;
    normal_lives = false;

    border_cut=15000;
    //density_maxima_num = 0;
    draw_spots = true;
    multiply_elix_spots = false;
    draw_final = false;
    draw_always_selected = false;
    draw_density_selected = false;
    draw_density_pool = false;
    
    spot_scale = 0.2;
    density_maxima_val = 300.0;
    // spots_maxima_life_cut = 0;
    // spots_minima_life_cut = 0;
    // spots_sellae_life_cut = 0;
    // spots_maxima_life_add = 0;
    // spots_minima_life_add = 0;
    // spots_sellae_life_add = 0;

    maxima_always_selected_num = 12;
    maxima_density_pool_num = 60;
    spots_maxima_imp_cut = 0.0;
    minima_always_selected_num = 2;
    minima_density_pool_num = 10;
    spots_minima_imp_cut = 0.0;
    sellae_always_selected_num = 6;
    sellae_density_pool_num = 30;
    spots_sellae_imp_cut = 0.0;
    simmetry_imp_cut_max=0.005;
    simmetry_imp_cut_min=0.005;
    simmetry_life_cut_max=2.5;
    simmetry_life_cut_min=6.5;


    swpts_display = false;
    swpts_active = false;
};


// void TrackDisplay::read_dem (char *file)
// {
//     dems.push_back(DEMSelector::get (file));
// }

void TrackDisplay::readShapes(vector<string> files)
{
  int i;
  for (i=0; i<files.size(); i++)
    {
      cout << "FILE: " << files[i].c_str() << endl;
      FILE * fp = fopen(files[i].c_str(), "r");
      fread (&shape_w, sizeof (int), 1, fp);
      fread (&shape_h, sizeof (int), 1, fp);

      double* shapeT=(double*)malloc(sizeof(double)*shape_w*shape_h);

      fread (&(shapeT[0]), sizeof (double), shape_w*shape_h, fp);
      Grid<double> temp(shape_w,shape_h,-DBL_MAX);

      shapeIndices.push_back(temp);


      for (int j=0; j<shape_w*shape_h; j++)
	shapeIndices[i].data[j]=shapeT[j];

      fclose(fp);
    } 
}

void TrackDisplay::read_ssp (char *file)
{
    ssp = new ScaleSpace (file, ScaleSpaceOpts());

    clip_white = ssp->dem[0]->max;
    clip_black = ssp->dem[0]->min;

    clip_black=-10;
    clip_white=10;
    multiply=150;
}

void TrackDisplay::read_track (char *file)
{
    track_reader (file, &track, &track_order);
    // track = new Track (file);

    // for (unsigned i = 0; i < track->lines.size(); i++)
    // 	if (track->is_original (i))
    // 	    track->lines[i].strength = track->lines[i].strength * multiply;
}


void TrackDisplay::read_signs(char *file)
{
  printf("Opening file %s\n" , file);
  FILE * fp = fopen(file, "r");
  fread (&signs_w, sizeof (int), 1, fp);
  fread (&signs_h, sizeof (int), 1, fp);

  char* signsT=(char*)malloc(sizeof(char)*signs_w*signs_h);

  fread (&(signsT[0]), sizeof (char), signs_w*signs_h, fp);
  //cout << "W: " << signs_w << " H: " << signs_h << endl;
  signs=Grid<char>(signs_w,signs_h,'n');

  for (int i=0; i<signs_w*signs_h; i++)
    signs.data[i]=signsT[i];

}

void TrackDisplay::read_height(char *file)
{
  printf("Opening file %s\n" , file);
  FILE * fp = fopen(file, "r");
  fread (&height_w, sizeof (int), 1, fp);
  fread (&height_h, sizeof (int), 1, fp);

  double* heightT=(double*)malloc(sizeof(double)*height_w*height_h);

  fread (&(heightT[0]), sizeof (double), height_w*height_h, fp);
  //cout << "W: " << height_w << " H: " << height_h << endl;
  heights=Grid<double>(height_w,height_h,-DBL_MAX);

  int c=0;
  double min_H=heightT[0], max_H=heightT[0];
  for (int i=0; i<height_w*height_h; i++)
    {
      if (heightT[i]==0 && c++);
	
      if (heightT[i]<min_H)
	min_H=heightT[i];
      if (heightT[i]>max_H && heightT[i]<22000)
	max_H=heightT[i];
      heights.data[i]=heightT[i];
    }
  //cout << "MinH: " << min_H << "\t MaxH: " << max_H << "\t numero di entry a 0: " << c << endl;
  max_h=max_H;
}

// void TrackDisplay::read_val(char *file)
// {
//   printf("Opening file %s\n" , file);
//   FILE * fp = fopen(file, "r");
//   fread (&val_w, sizeof (int), 1, fp);
//   fread (&val_h, sizeof (int), 1, fp);

//   unsigned int* valT=(unsigned int*)malloc(sizeof(unsigned int)*val_w*val_h);

//   fread (&(valT[0]), sizeof (unsigned int), val_w*val_h, fp);
//   //cout << "W: " << val_w << " H: " << val_h << endl;
//   vals=Grid<unsigned int>(val_w,val_h,UINT_MAX);

//     for (int i=0; i<val_w*val_h; i++)
//     {
//       vals.data[i]=valT[i];
//     }
  
// }


void TrackDisplay::query (double t)
{
    track->query (t, vquery);
}


void TrackDisplay::getbb (double* cx, double* cy, double* diam)
{
    // *cx = dems[0]->width / 2.0;
    // *cy = dems[0]->height / 2.0;
    // *diam = dems[0]->width * 1.2;
    *cx = ssp->dem[0]->width / 2.0;
    *cy = ssp->dem[0]->height / 2.0;
    *diam = ssp->dem[0]->width * 1.2;    
}

void TrackDisplay::getbb (Point* a, Point* b)
{
    // *a = Point (0.0, 0.0);
    // b->x = (double) dems[0]->width;
    // b->y = (double) dems[0]->height;
    *a = Point (0.0, 0.0);
    b->x = (double) ssp->dem[0]->width;
    b->y = (double) ssp->dem[0]->height;
}

void __draw_quad ()
{
    double qs = 10.0;
    double qh = (qs / 2.0);
    double st = 1.0;
    
    glBegin (GL_QUADS);
    glVertex2f (-qh, -qh);
    glVertex2f (-qh,  qh);
    glVertex2f ( qh,  qh);
    glVertex2f ( qh, -qh);
    glEnd ();

    glColor3f (0.0, 0.0, 0.0);

    glBegin (GL_QUADS);
    // 1 left
    glVertex2f (-qh     , -qh);
    glVertex2f (-qh     ,  qh);
    glVertex2f (-qh + st,  qh);
    glVertex2f (-qh + st, -qh);
    // 2 top
    glVertex2f (-qh,  qh - st);
    glVertex2f (-qh,  qh     );
    glVertex2f ( qh,  qh     );
    glVertex2f ( qh,  qh - st);
    // 3 right
    glVertex2f ( qh - st, -qh);
    glVertex2f ( qh - st,  qh);
    glVertex2f ( qh     ,  qh);
    glVertex2f ( qh     , -qh);
    // 4 bottom
    glVertex2f (-qh, -qh);
    glVertex2f (-qh, -qh + st);
    glVertex2f ( qh, -qh + st);
    glVertex2f ( qh, -qh);
    glEnd ();
}

void __draw_critical_nikolas (CriticalType t, bool sign)
{
    switch (t)
    {
    case MIN:
	glColor3dv (blue);
 	break;

    case MAX:
      if (sign)
	glColor3dv(red);
      else
	glColor3dv(red);
	break;

    case SA2:
    case SA3:
	glColor3dv(green);
	break;

    // case SA3:
    // 	glColor3dv(yellow);
    // 	break;
	
    default:
	fprintf (stderr, "__draw_critical_color() problem\n");
    }

    __draw_quad ();
    
 }


void __draw_critical_color (CriticalType t)
{
    switch (t)
    {
    case MIN:
	glColor3dv (blue);
 	break;

    case MAX:
	glColor3dv(red);
	break;

    case SA2:
    case SA3:
	glColor3dv(green);
	break;

    // case SA3:
    // 	glColor3dv(yellow);
    // 	break;
	
    default:
	fprintf (stderr, "__draw_critical_color() problem\n");
    }

    __draw_quad ();
    
 }

void __draw_cross ()
{
    // double qs = 10.0;
    // double qh = (qs / 2.0);

    double ss = 8.0;
    double sg = 2.0;
    // double sd = 1.5;
    // double st = 1.0;
    double sh = (ss / 2.0);

    glBegin (GL_QUADS);
    glVertex2f (-sh+sg, -sh);
    glVertex2f (-sh   , -sh+sg);
    glVertex2f ( sh-sg,  sh);
    glVertex2f ( sh   ,  sh-sg);
    glEnd ();

    
    glBegin (GL_QUADS);
    glVertex2f (-sh+sg,  sh);
    glVertex2f (-sh   ,  sh-sg);
    glVertex2f ( sh-sg, -sh);
    glVertex2f ( sh   , -sh+sg);
    glEnd ();
}

void __draw_critical_sym (CriticalType t)
{
    double qs = 10.0;
    double qh = (qs / 2.0);

    double ss = 8.0;
    double sg = 2.0;
    double sd = 1.5;
    double st = 1.0;
    double sh = (ss / 2.0);

    glBegin (GL_QUADS);
    glVertex2f (-qh, -qh);
    glVertex2f (-qh,  qh);
    glVertex2f ( qh,  qh);
    glVertex2f ( qh, -qh);
    glEnd ();

    glColor3f (0.0, 0.0, 0.0);

    // char sym[5][5];

    switch (t)
    {
    case MIN:
	glBegin (GL_QUADS);
	// 1 left
	glVertex2f (-sh     , -sh);
	glVertex2f (-sh     ,  sh);
	glVertex2f (-sh + sg,  sh);
	glVertex2f (-sh + sg, -sh);
	// 2 top
	glVertex2f (-sh,  sh - sg);
	glVertex2f (-sh,  sh     );
	glVertex2f ( sh,  sh     );
	glVertex2f ( sh,  sh - sg);
	// 3 right
	glVertex2f ( sh - sg, -sh);
	glVertex2f ( sh - sg,  sh);
	glVertex2f ( sh     ,  sh);
	glVertex2f ( sh     , -sh);
	// 4 bottom
	glVertex2f (-sh, -sh);
	glVertex2f (-sh, -sh + sg);
	glVertex2f ( sh, -sh + sg);
	glVertex2f ( sh, -sh);
	glEnd ();
 	break;

    case MAX:
	glBegin (GL_QUADS);
	glVertex2f (-sh+st, -sh+st);
	glVertex2f (-sh+st,  sh-st);
	glVertex2f ( sh-st,  sh-st);
	glVertex2f ( sh-st, -sh+st);
	glEnd ();
	break;

    case SA3:
	glBegin (GL_QUADS);
	glVertex2f (-sh, -st);
	glVertex2f (-sh,  st);
	glVertex2f ( sh,  st);
	glVertex2f ( sh, -st);
	glEnd ();	
	glBegin (GL_QUADS);
	glVertex2f (-sh+sd, -sh);
	glVertex2f (-sh   , -sh+sd);
	glVertex2f ( sh-sd,  sh);
	glVertex2f ( sh   ,  sh-sd);
	glEnd ();	
	glBegin (GL_QUADS);
	glVertex2f (-sh+sd,  sh);
	glVertex2f (-sh   ,  sh-sd);
	glVertex2f ( sh-sd, -sh);
	glVertex2f ( sh   , -sh+sd);
	glEnd ();
	break;
	
    case SA2:
	glBegin (GL_QUADS);
	glVertex2f (-sh+sg, -sh);
	glVertex2f (-sh   , -sh+sg);
	glVertex2f ( sh-sg,  sh);
	glVertex2f ( sh   ,  sh-sg);
	glEnd ();	
	glBegin (GL_QUADS);
	glVertex2f (-sh+sg,  sh);
	glVertex2f (-sh   ,  sh-sg);
	glVertex2f ( sh-sg, -sh);
	glVertex2f ( sh   , -sh+sg);
	glEnd ();	
	break;

    default:
	fprintf (stderr, "__draw_critical_sym() problem\n");
    }
}

void __draw_death (bool is_alive)
{
    double qs = 10.0;
    double qh = (qs / 2.0);

    double ss = 8.0;
    double sg = 2.0;
    // double sd = 1.5;
    // double st = 1.0;
    double sh = (ss / 2.0);

    glColor3dv (is_alive? cyan : magenta);

    glBegin (GL_QUADS);
    glVertex2f (-qh, -qh);
    glVertex2f (-qh,  qh);
    glVertex2f ( qh,  qh);
    glVertex2f ( qh, -qh);
    glEnd ();

    glColor3dv (black);

    glBegin (GL_QUADS);
    glVertex2f (-sh+sg, -sh);
    glVertex2f (-sh   , -sh+sg);
    glVertex2f ( sh-sg,  sh);
    glVertex2f ( sh   ,  sh-sg);
    glEnd ();	
    glBegin (GL_QUADS);
    glVertex2f (-sh+sg,  sh);
    glVertex2f (-sh   ,  sh-sg);
    glVertex2f ( sh-sg, -sh);
    glVertex2f ( sh   , -sh+sg);
    glEnd ();	

}

void __draw_critical_simple (Coord c, CriticalType t, bool dead, double scale)
{
    if (dead)
	glColor3f (1.0, 0.0, 0.0);
    else
	glColor3f (0.0, 1.0, 0.0);

    glPushMatrix ();

    glTranslated ((double) c.x, (double) c.y, 0.0);
    glScaled (scale, scale, 1.0);

    __draw_critical_sym (t);

    glPopMatrix();
}

void __draw_critical_track (Coord c, CriticalType t,
			    bool start, bool special, double tol,
			    double scale, double tol_mult)
{
    double scale_tol = tol_mult * (1.0 + tol);

    // initial position original
    if (start && !special)
	glColor3dv (green);
    // initial position born
    else if (start && special)
	glColor3dv (yellow);
    // final position alive
    else if (!start && !special)
	glColor3dv (cyan);
    // final position dead
    else if (!start && special)
	glColor3dv (blue);

    glPushMatrix ();

    glTranslated ((double) c.x, (double) c.y, 0.0);
    glScaled (scale, scale, 1.0);

    if (scale_tol != 0.0)
	glScaled (scale_tol, scale_tol, 1.0);

    __draw_critical_sym (t);

    glPopMatrix();
}

void __draw_critical_query (TrackRenderingEntry r,  double scale, double tol_mult,
			    bool cur_pos, bool draw_death)
{
    double x = r.oc.x;
    double y = r.oc.y;

    if (cur_pos)
    {
	x = r.tc.x;
	y = r.tc.y;
    }

    double scale_tol = tol_mult * (1.0 + r.tol);

    glPushMatrix ();
    glTranslated ((double) x, (double) y, 0.0);
    glScaled (scale, scale, 1.0);
    if (scale_tol != 0.0)
	glScaled (scale_tol, scale_tol, 1.0);
    __draw_critical_color (r.type);
    glPopMatrix();

    if (draw_death)
    {
	glPushMatrix ();
	glTranslated ((double) r.fx, (double) r.fy, 0.0);
	glScaled (scale, scale, 1.0);
	if (scale_tol != 0.0)
	    glScaled (scale_tol, scale_tol, 1.0);
	__draw_death (r.is_alive);
	glPopMatrix();
    }
}

void __draw_critical_elixir (Coord c, CriticalType type, double elixir,  double scale, double tol_mult)
{
    double x = c.x;
    double y = c.y;

    double scale_tol = tol_mult * (1.0 + elixir);
  
    glPushMatrix ();
    glTranslated ((double) x, (double) y, 0.0);
    glScaled (scale, scale, 1.0);
    if (scale_tol != 0.0)
	glScaled (scale_tol, scale_tol, 1.0);
    

    __draw_critical_nikolas (type, signs(c)=='p');
//__draw_critical_color (type);
    glPopMatrix();
}

// std::vector<int> spots_maxima;
// std::vector<int> spots_minima;
// std::vector<int> spots_sellae;

void TrackDisplay::init_spots ()
{
    // track->drink_elixir ();

    for (unsigned i = 0; i < track->lines.size(); i++)
    {
	if (track->is_original (i))
	{
	  if (track->lines[i].strength>max_imp)
	    max_imp=track->lines[i].strength;
	 
	  if (track->lines[i].strength<min_imp)
	    min_imp=track->lines[i].strength;
	  if (track->original_type (i) == MAX)
	    spots_maxima.push_back (CritElix (i, track->lifetime_elixir (i)));
	  if (track->original_type (i) == MIN)
	    spots_minima.push_back (CritElix (i, track->lifetime_elixir (i)));
	  if (track->original_type (i) == SA2 ||
	      track->original_type (i) == SA3)
	    spots_sellae.push_back (CritElix (i, track->lifetime_elixir (i)));
	}
    }

    std::sort (spots_maxima.begin(), spots_maxima.end());
    std::sort (spots_minima.begin(), spots_minima.end());
    std::sort (spots_sellae.begin(), spots_sellae.end());
}

// double density_distance (Point a, Point b)
// {
//     return sqrt ((a.x - b.x) * (a.x - b.x)) + ((a.y - b.y) * (a.y - b.y));
// }

bool TrackDisplay::is_density (double val, int idx, vector<int>& spots_current)
{
    Point pa = track->start_point (idx);
    for (unsigned i = 0; i < spots_current.size (); i++)
    {
    	Point pb = track->start_point (spots_current[i]);
    	if (point_distance (pa, pb) < val)
    	    return false;
    }
    return true;
}

// double __map (double v, double min, double max)
// {
//     v = v < min? min : v;
//     v = v > max? max : v;

//     return (v - min) / (max - min);
// }

static double __clip (double v, double min, double max, double mul)
{
    v *= mul;

    v = v < min? min : v;
    v = v > max? max : v;

    return (v - min) / (max - min);
}


void TrackDisplay::draw (int dem_idx)
{
  //multiply=150;
  //scale_by_life=false;
  vector<CritNikolas> spots_fiducial;
    glMatrixMode (GL_PROJECTION);
    glPushMatrix ();
    glMatrixMode (GL_MODELVIEW);
    glPushMatrix ();
    if (draw_terrain)
    {
	// DEMReader* dem = dems[dem_idx];
	// double min = clip_black;
	// double max = clip_white;

	// glBegin (GL_TRIANGLES);
	// for (unsigned int i = 0; i < dem->width - 1; i++)
	//     for (unsigned int j = 0; j < dem->height - 1; j++)
	//     {
	// 	double vij = dem->get_pixel (i, j);
	// 	double vipj = dem->get_pixel (i+1, j);
	// 	double vijp = dem->get_pixel (i, j+1);
	// 	double vipjp = dem->get_pixel (i+1, j+1);
	// 	vij = __map (vij, min, max);
	// 	vipj = __map (vipj, min, max);
	// 	vijp = __map (vijp, min, max);
	// 	vipjp = __map (vipjp, min, max);

	// 	glColor3f (vij, vij, vij);
	// 	glVertex2f (i, j);
	// 	glColor3f (vipj, vipj, vipj);
	// 	glVertex2f (i+1, j);
	// 	glColor3f (vipjp, vipjp, vipjp);
	// 	glVertex2f (i+1, j+1);

	// 	glColor3f (vij, vij, vij);
	// 	glVertex2f (i, j);
	// 	glColor3f (vijp, vijp, vijp);
	// 	glVertex2f (i, j+1);
	// 	glColor3f (vipjp, vipjp, vipjp);
	// 	glVertex2f (i+1, j+1);
	//     }
	// glEnd();

      int level = dem_idx;
      // CImg<double> pic(ssp->dem[level]->width,ssp->dem[level]->height,3);
	glBegin (GL_TRIANGLES);
	

	for (int i = 0; i < ssp->dem[level]->width - 1; i++)
	    for (int j = 0; j < ssp->dem[level]->height - 1; j++)
	    {
		double min = clip_black;
		double max = clip_white;
		double mul = multiply;
		
		double vij = (*ssp->dem[level]) (i, j);
		double vipj = (*ssp->dem[level]) (i+1, j);
		double vijp = (*ssp->dem[level]) (i, j+1);
		double vipjp = (*ssp->dem[level]) (i+1, j+1);
		
		vij = __clip (vij, min, max, mul);
		vipj = __clip (vipj, min, max, mul);
		vijp = __clip (vijp, min, max, mul);
		vipjp = __clip (vipjp, min, max, mul);

		
		// pic(i,j,0)=vij*10000;
		// pic(i,j,1)=vij*10000;
		// pic(i,j,2)=vij*10000;


		glColor3f (vij, vij, vij);
		glVertex2f (i, j);
		glColor3f (vipj, vipj, vipj);
		glVertex2f (i+1, j);
		glColor3f (vipjp, vipjp, vipjp);
		glVertex2f (i+1, j+1);

		glColor3f (vij, vij, vij);
		glVertex2f (i, j);
		glColor3f (vijp, vijp, vijp);
		glVertex2f (i, j+1);
		glColor3f (vipjp, vipjp, vipjp);
		glVertex2f (i+1, j+1);
	    }
	glEnd();
	// pic.save("Out.tiff");
	// exit(0);
    }
    if (draw_track)
    {
	for (unsigned i = 0; i < track->lines.size(); i++)
	{
	    CriticalType t = track->lines[i].type;
	    __draw_critical_track (track->lines[i].entries[0].c, t, true,
				   track->lines[i].is_born(),
				   track->lifetime(i),
				   track_scale, track_mult);
	    __draw_critical_track (track->lines[i].entries.back().c, t, false,
				   track->lines[i].is_dead(),
				   track->lifetime(i),
				   track_scale, track_mult);
	}
    }

    if (draw_query)
    {
	for (unsigned i = 0; i < vquery.size(); i++)
	    __draw_critical_query (vquery[i], query_scale, query_mult,
				   query_cur_pos, query_death);
    }


    if (draw_elixir)
    {
	for (unsigned i = 0; i < track->lines.size(); i++)
	{
	    double life_multiplier = normal_lives? track->lifetime (i) : track->lifetime_elixir (i);
	    if (track->is_original (i) && life_multiplier > elixir_cut)
		__draw_critical_elixir (track->lines[i].entries[0].c,
					track->original_type (i),
					life_multiplier,
					elixir_scale, elixir_mult);
	}
    }

    if (draw_importance)
    {
	for (unsigned i = 0; i < track->lines.size(); i++)
	{
	    if (track->is_original (i) && track->lines[i].strength > importance_cut)
		__draw_critical_elixir (track->lines[i].entries[0].c,
					track->original_type (i),
					track->lines[i].strength,
					importance_scale, importance_mult);
	}
    }

    if (draw_nikolas)
      {
	maxima_draw_num = minima_draw_num = sellae_draw_num = 0;
	vector<CritNikolas> spots_add;
	maxima_total_num = spots_maxima.size();
	if (draw_maxima)
	  for (int i=0; i<spots_maxima.size(); i++)
	  {
	    if (spots_maxima[i].elix >= spots_maxima_life_cut && track->lines[spots_maxima[i].crit].strength  >=
		spots_maxima_imp_cut && track->lines[spots_maxima[i].crit].is_original())
	      {
		spots_add.push_back(CritNikolas(spots_maxima[i].crit,spots_maxima[i].elix, track->lines[spots_maxima[i].crit].strength,track->lines[spots_maxima[i].crit].scale ));
		maxima_draw_num++;
	      }
	  }
	if (draw_minima)
	  for (int i=0; i<spots_minima.size(); i++)
	  {
	    if (spots_minima[i].elix >= spots_minima_life_cut && track->lines[spots_minima[i].crit].strength  >=
		   spots_minima_imp_cut && track->lines[spots_minima[i].crit].is_original())
	      {
		spots_add.push_back(CritNikolas(spots_minima[i].crit,spots_minima[i].elix, track->lines[spots_minima[i].crit].strength,track->lines[spots_minima[i].crit].scale));
		minima_draw_num++;
	      }  
	  }
	if (draw_sellae)
	  for (int i=0; i<spots_sellae.size(); i++)
	  {
	    if (spots_sellae[i].elix >= spots_sellae_life_cut && track->lines[spots_sellae[i].crit].strength  >=
		spots_sellae_imp_cut && track->lines[spots_sellae[i].crit].is_original())
	      {
		spots_add.push_back(CritNikolas(spots_sellae[i].crit,spots_sellae[i].elix, track->lines[spots_sellae[i].crit].strength,track->lines[spots_sellae[i].crit].scale));
		sellae_draw_num++;
	      }  
	  }

	


	if (show_nose)
	  {
	    int cand=searchNoseTip();
	   spots_fiducial.push_back(CritNikolas(spots_maxima[cand].crit,spots_maxima[cand].elix, track->lines[spots_maxima[cand].crit].strength,track->lines[spots_maxima[cand].crit].scale));
	  }

 	if (show_noseRoot)
 	  {
	    int cand=searchNoseRoot(track->lines[spots_fiducial[0].crit].entries[0].c.x, track->lines[spots_fiducial[0].crit].entries[0].c.y);
	    spots_fiducial.push_back(CritNikolas(spots_minima[cand].crit,spots_minima[cand].elix, track->lines[spots_minima[cand].crit].strength,track->lines[spots_minima[cand].crit].scale));
	  }	
	
	int under;
	if (show_underNose)
 	  {
	    int cand=searchUnderNose(track->lines[spots_fiducial[0].crit].entries[0].c.x, track->lines[spots_fiducial[0].crit].entries[0].c.y);
	    spots_fiducial.push_back(CritNikolas(spots_minima[cand].crit,spots_minima[cand].elix, track->lines[spots_minima[cand].crit].strength,track->lines[spots_minima[cand].crit].scale));
	    under=cand;
	  }

	if (show_noseLimits)
	  {
	    int cand1, cand2;
	    searchNoseLimits(track->lines[spots_fiducial[0].crit].entries[0].c.x, track->lines[spots_fiducial[0].crit].entries[0].c.y,under, cand1,cand2);
	   spots_fiducial.push_back(CritNikolas(spots_minima[cand1].crit,spots_minima[cand1].elix, track->lines[spots_minima[cand1].crit].strength,track->lines[spots_minima[cand1].crit].scale));
	   spots_fiducial.push_back(CritNikolas(spots_minima[cand2].crit,spots_minima[cand2].elix, track->lines[spots_minima[cand2].crit].strength,track->lines[spots_minima[cand2].crit].scale));
	  }

 	//       }
 	//     spots_fiducial.push_back(CritNikolas(spots_minima[cand].crit,spots_minima[cand].elix, track->lines[spots_minima[cand].crit].strength,track->lines[spots_minima[cand].crit].scale));
 	//   }

 	
	if (show_eyesOuterAngles)
	  {
	    int cand1, cand2,cand3,cand4;
	    searchEyesAngles(track->lines[spots_fiducial[0].crit].entries[0].c.x, track->lines[spots_fiducial[0].crit].entries[0].c.y,cand1,cand2,cand3,cand4);
	    spots_fiducial.push_back(CritNikolas(spots_maxima[cand1].crit,spots_maxima[cand1].elix, track->lines[spots_maxima[cand1].crit].strength,track->lines[spots_maxima[cand1].crit].scale));

	    spots_fiducial.push_back(CritNikolas(spots_maxima[cand2].crit,spots_maxima[cand2].elix, track->lines[spots_maxima[cand2].crit].strength,track->lines[spots_maxima[cand2].crit].scale));
	    spots_fiducial.push_back(CritNikolas(spots_maxima[cand3].crit,spots_maxima[cand3].elix, track->lines[spots_maxima[cand3].crit].strength,track->lines[spots_maxima[cand3].crit].scale));

	    spots_fiducial.push_back(CritNikolas(spots_maxima[cand4].crit,spots_maxima[cand4].elix, track->lines[spots_maxima[cand4].crit].strength,track->lines[spots_maxima[cand4].crit].scale));
	    /*if (show_noseRoot)
	      {
		double y_eyes=(track->lines[spots_maxima[cand1].crit].entries[0].c.y+track->lines[spots_maxima[cand2].crit].entries[0].c.y+track->lines[spots_maxima[cand3].crit].entries[0].c.y+track->lines[spots_maxima[cand4].crit].entries[0].c.y)/4;
		int cand=searchNoseRoot(track->lines[spots_fiducial[0].crit].entries[0].c.x, track->lines[spots_fiducial[0].crit].entries[0].c.y, y_eyes);
		 spots_fiducial[1]=CritNikolas(spots_minima[cand].crit,spots_minima[cand].elix, track->lines[spots_minima[cand].crit].strength,track->lines[spots_minima[cand].crit].scale);
		 }*/
	      }
	if (show_mouth)
	  {
	    int cand1, cand2,cand3,cand4;
	    searchMouth(track->lines[spots_fiducial[0].crit].entries[0].c.x, track->lines[spots_fiducial[0].crit].entries[0].c.y, track->lines[spots_fiducial[3].crit].entries[0].c.x, track->lines[spots_fiducial[4].crit].entries[0].c.x, cand1,cand2,cand3,cand4);
	    spots_fiducial.push_back(CritNikolas(spots_maxima[cand1].crit,spots_maxima[cand1].elix, track->lines[spots_maxima[cand1].crit].strength,track->lines[spots_maxima[cand1].crit].scale));

	    spots_fiducial.push_back(CritNikolas(spots_maxima[cand2].crit,spots_maxima[cand2].elix, track->lines[spots_maxima[cand2].crit].strength,track->lines[spots_maxima[cand2].crit].scale));
	    spots_fiducial.push_back(CritNikolas(spots_maxima[cand3].crit,spots_maxima[cand3].elix, track->lines[spots_maxima[cand3].crit].strength,track->lines[spots_maxima[cand3].crit].scale));

	    spots_fiducial.push_back(CritNikolas(spots_maxima[cand4].crit,spots_maxima[cand4].elix, track->lines[spots_maxima[cand4].crit].strength,track->lines[spots_maxima[cand4].crit].scale));

	  }
	// if (show_eyebrows)
 // 	  {
 // 	    int cand1, cand2;
 // 	    searchEyebrows(track->lines[spots_fiducial[6].crit].entries[0].c.y,cand1,cand2);
 // 	    spots_fiducial.push_back(CritNikolas(spots_maxima[cand1].crit,spots_maxima[cand1].elix, track->lines[spots_maxima[cand1].crit].strength,track->lines[spots_maxima[cand1].crit].scale));
 // spots_fiducial.push_back(CritNikolas(spots_maxima[cand2].crit,spots_maxima[cand2].elix, track->lines[spots_maxima[cand2].crit].strength,track->lines[spots_maxima[cand2].crit].scale));
 // 	  }
	for (unsigned i = 0; i < spots_add.size(); i++)
	    {
	      int idx = spots_add[i].crit;
	      double scale=0.0;
		if (scale_by_life)
		  scale+=life_imp*spots_add[i].life;
		if (scale_by_size)
		  scale+=strength_imp*spots_add[i].strength;
		__draw_critical_elixir (track->lines[idx].entries[0].c,
					track->original_type (idx),
					spots_add[i].life,
					spot_scale, scale);
	    }

	//std::sort(spots_fiducial.begin(), spots_fiducial.end());
	for (unsigned i = 0; i < min(fiducialShow,(int)spots_fiducial.size()); i++)
	  {
	    int idx = spots_fiducial[i].crit;
	    double scale=0.0;
	    if (scale_by_life)
	      scale+=life_imp*spots_fiducial[i].life;
	    if (scale_by_size)
	      scale+=strength_imp*spots_fiducial[i].strength;

	    __draw_critical_elixir (track->lines[idx].entries[0].c,
				    track->original_type (idx),
				    spots_fiducial[i].life,
				    spot_scale, scale);

	    if (i==min(fiducialShow-1,(int)spots_fiducial.size()-1))
	      cout <<  spots_fiducial[i].life << "," << track->lines[spots_fiducial[i].crit].strength <<  "," << track->lines[spots_fiducial[i].crit].scale << "," << spots_fiducial[i].life* track->lines[spots_fiducial[i].crit].strength*track->lines[spots_fiducial[i].crit].scale <<  "," << track->lines[spots_fiducial[i].crit].entries[0].c.x << "," << track->lines[spots_fiducial[i].crit].entries[0].c.y << " height: " << heights(track->lines[spots_fiducial[i].crit].entries[0].c) << "\n";// << "\t Shape index at scale 0: " << shapeIndices[0](track->lines[spots_fiducial[i].crit].entries[0].c) << endl;

	  }
	
      }
    
    if (draw_spots)
    {
	vector<CritElix> spots_cut;
	vector<int> spots_add;
	// vector<int> spots_current;
	spots_current.clear();

	total_num = spots_maxima.size() + spots_minima.size() + spots_sellae.size();
	maxima_total_num = spots_maxima.size();
	minima_total_num = spots_minima.size();
	sellae_total_num = spots_sellae.size();
	
	// maxima
	for (int i = 0; i < maxima_always_selected_num; i++)
	{
	    int k = spots_maxima.size() - (i+1);
	    if (k < 0)
	    {
		maxima_always_selected_num = spots_maxima.size();
		break;
	    }
	    spots_add.push_back (spots_maxima[k].crit);
	    maxima_always_life = spots_maxima[k].elix;
	}
	int i = 0;
	for (int ii = 0; ii < maxima_density_pool_num; ii++)
	{
	    int offset = spots_maxima.size() - maxima_always_selected_num;
	    int k;
	    do
	    {
		k = offset - (++i);
		maxima_always_discarded_num = k >= 0? k : spots_maxima.size();
		if (k < 0)
		    break;
	    }
	    while (track->lines[spots_maxima[k].crit].strength * multiply <
		   spots_maxima_imp_cut);

	    if (k < 0)
	    {
		maxima_density_pool_num = offset - (i - ii-1);
		break;
	    }

	    spots_cut.push_back (spots_maxima[k]);
	    maxima_excluded_life = spots_maxima[k].elix;
	}

	// minima
	for (int i = 0; i < minima_always_selected_num; i++)
	{
	    int k = spots_minima.size() - (i+1);
	    if (k < 0)
	    {
		minima_always_selected_num = spots_minima.size();
		break;
	    }
	    spots_add.push_back (spots_minima[k].crit);
	    minima_always_life = spots_minima[k].elix;
	}
	i = 0;
	for (int ii = 0; ii < minima_density_pool_num; ii++)
	{
	    int offset = spots_minima.size() - minima_always_selected_num;
	    int k;
	    do
	    {
		k = offset - (++i);
		minima_always_discarded_num = k >= 0? k : 0;
		if (k < 0)
		    break;
	    }
	    while (track->lines[spots_minima[k].crit].strength * multiply <
		   spots_minima_imp_cut);

	    if (k < 0)
	    {
		minima_density_pool_num = offset - (i - ii-1);
		break;
	    }

	    spots_cut.push_back (spots_minima[k]);
	    minima_excluded_life = spots_minima[k].elix;
	}

	// sellae
	for (int i = 0; i < sellae_always_selected_num; i++)
	{
	    int k = spots_sellae.size() - (i+1);
	    if (k < 0)
	    {
		sellae_always_selected_num = spots_sellae.size();
		break;
	    }
	    spots_add.push_back (spots_sellae[k].crit);
	    sellae_always_life = spots_sellae[k].elix;
	}
	i = 0;
	for (int ii = 0; ii < sellae_density_pool_num; ii++)
	{
	    int offset = spots_sellae.size() - sellae_always_selected_num;
	    int k;
	    do
	    {
		k = offset - (++i);
		sellae_always_discarded_num = k >= 0? k : 0;
		if (k < 0)
		    break;
	    }
	    while (track->lines[spots_sellae[k].crit].strength * multiply <
		   spots_sellae_imp_cut);

	    if (k < 0)
	    {
		sellae_density_pool_num = offset - (i - ii-1);
		break;
	    }

	    spots_cut.push_back (spots_sellae[k]);
	    sellae_excluded_life = spots_sellae[k].elix;
	}

	maxima_importance_discarded_num =
	    maxima_total_num - maxima_always_discarded_num -
	    maxima_always_selected_num - maxima_density_pool_num;
	minima_importance_discarded_num =
	    minima_total_num - minima_always_discarded_num -
	    minima_always_selected_num - minima_density_pool_num;
	sellae_importance_discarded_num =
	    sellae_total_num - sellae_always_discarded_num -
	    sellae_always_selected_num - sellae_density_pool_num;
	
	if ((int) spots_cut.size() != sellae_density_pool_num +
	    maxima_density_pool_num + minima_density_pool_num)
	    fprintf (stderr, "Warning: spots_cut.size()=%zu, sum mx/mn/sd=%d\n",
		     spots_cut.size(), sellae_density_pool_num +
		     maxima_density_pool_num + minima_density_pool_num);
	
	// sort spots_cut
	std::sort (spots_cut.begin(), spots_cut.end());
	
	// add from spots_add to spots_current
	for (unsigned i = 0; i < spots_add.size(); i++)
	    spots_current.push_back (spots_add[i]);
	
	// add from spots_cut to spots_current density-based
	maxima_density_selected_num = 0;
	minima_density_selected_num = 0;
	sellae_density_selected_num = 0;
	for (int i = (int)spots_cut.size()-1; i >= 0 ; i--)
	    if (is_density (density_maxima_val, spots_cut[i].crit, spots_current))
	    {
		spots_current.push_back (spots_cut[i].crit);

		switch (track->get_type (spots_cut[i].crit))
		{
		case MAX:
		    maxima_density_selected_num++;
		    break;
		case MIN:
		    minima_density_selected_num++;
		    break;
		case SA2:
		case SA3:
		    sellae_density_selected_num++;
		    break;
		default:
		    break;
		}
	    }
	

	always_selected_num = spots_add.size();
	density_selected_num = spots_current.size() - spots_add.size();
	density_pool_num = spots_cut.size();

	double mmm = multiply_elix_spots? 0.5 : 0.0; // 0.0); elixir_mult);
	
	if (draw_final) 
	    for (unsigned i = 0; i < spots_current.size(); i++)
	    {
		int idx = spots_current[i];
		__draw_critical_elixir (track->lines[idx].entries[0].c,
					track->original_type (idx),
					track->lifetime_elixir (idx),
					spot_scale, mmm);
	    }

	if (draw_always_selected)
	    for (unsigned i = 0; i < spots_add.size(); i++)
	    {
		int idx = spots_add[i];
		__draw_critical_elixir (track->lines[idx].entries[0].c,
					track->original_type (idx),
					track->lifetime_elixir (idx),
					spot_scale, mmm);
	    }	

	if (draw_density_selected)
	    for (unsigned i = spots_add.size(); i < spots_current.size(); i++)
	    {
	    	int idx = spots_current[i];
	    	__draw_critical_elixir (track->lines[idx].entries[0].c,
	    				track->original_type (idx),
	    				track->lifetime_elixir (idx),
	    				spot_scale, mmm);
	    }

	if (draw_density_pool)
	    for (unsigned i = 0; i < spots_cut.size(); i++)
	    {
	    	int idx = spots_cut[i].crit;
	    	__draw_critical_elixir (track->lines[idx].entries[0].c,
	    				track->original_type (idx),
	    				track->lifetime_elixir (idx),
	    				spot_scale, mmm);
	    }	

    }
    
    if (draw_lines)
    {
	glEnable(GL_LINE_SMOOTH);
 	glLineWidth (lines_width);

	for (unsigned i = 0; i < track->lines.size(); i++)
	{
	    if (track->lines[i].entries.size() < lines_size_clip)
		continue;

	    if (track->lifetime(i) < lines_life_clip)
		continue;

	    if (lines_query && !track->lines[i].mark)
		continue;

	    glBegin (GL_LINES);
	    for (unsigned j = 1; j < track->lines[i].entries.size(); j++)
	    {
		TrackEntry pte = track->lines[i].entries[j-1];
		TrackEntry te = track->lines[i].entries[j];
		double color[3];
		time2color (pte.t, 0.0, track->time_of_life, color);
		glColor3dv (color);
		glVertex2d (pte.c.x, pte.c.y);
		time2color (te.t, 0.0, track->time_of_life, color);
		glColor3dv (color);
		glVertex2d (te.c.x, te.c.y);
		// draw last stretch to die point
		if (j == track->lines[i].entries.size() - 1)
		{
		    glVertex2d (te.c.x, te.c.y);
		    Point f = track->final_point (i);
		    glVertex2d (f.x, f.y); 
		}
	    }
	    glEnd();
	}
    }

    // if (draw_csv)
    // 	swpts_draw ();

    if (show_axis)
      {
	double slope1, yint1, slope2, yint2;
	computeAxis(slope1, yint1, slope2, yint2,spots_fiducial);
	glColor3f(0,1,0);
	glEnable(GL_LINE_SMOOTH);
 	glLineWidth (lines_width);
	glBegin (GL_LINES);
	for (double x=1; x<track->width; x++)
	  {
	    glVertex2d(x,slope1*x+yint1);
	  }
	glEnd();
	glBegin (GL_LINES);
	for (double x=0; x<track->width; x+=0.1)
	  {
	    glVertex2d(x,slope2*x+yint2);
	  }
	glEnd();
	vector<int> pool_left, pool_right;
	/* Prendo due punti sull'asse verticale */
	double px1, px2, py1, py2;
	px1=200, px2=400;
	py1=px1*slope2+yint2;
	py2=px2*slope2+yint2;
	/* Metto in pool solo i punti che stanno a destra (o sopra)
	   (Bx - Ax) * (Cy - Ay) - (By - Ay) * (Cx - Ax)
	 */
	for (int i=0; i<spots_maxima.size(); i++)
	  {
	    /* Ciclo sui massimi, se sono ~vicini all'asse di simmetria li visualizzo
	     */
	    int idx = spots_maxima[i].crit;
	    double x=track->lines[idx].entries[0].c.x;
	    double y=track->lines[idx].entries[0].c.y;

	    double orientation= (px2 - px1) * (y - py1) - (py2 - py1) * (x - px1);
	    //if (track->lines[idx].strength>simmetry_imp_cut_max && track->lines[idx].elixir>simmetry_life_cut_max)
	      if(orientation >= -10e-10)
		pool_left.push_back(idx);
	      else
		pool_right.push_back(idx);


	  }
	for (int i=0; i<spots_minima.size(); i++)
	  {
	    /* Ciclo sui minimi, se sono ~vicini all'asse di simmetria li visualizzo
	     */
	    int idx = spots_minima[i].crit;
	    double x=track->lines[idx].entries[0].c.x;
	    double y=track->lines[idx].entries[0].c.y;
	    double orientation= (px2 - px1) * (y - py1) - (py2 - py1) * (x - px1);
	    //if (track->lines[idx].strength>simmetry_imp_cut_min && track->lines[idx].elixir>simmetry_life_cut_min)
	      if(orientation >= -10e-10)
		pool_left.push_back(idx);
	      else
		pool_right.push_back(idx);
	  }
	vector<int> pool_final;
	for (unsigned i = 0; i <pool_left.size(); i++)
	  {

	    int idx=pool_left[i];
	    double life_left = track->lines[idx].elixir;
	    double strength_left = track->lines[idx].strength;
	    double x=track->lines[idx].entries[0].c.x;
	    double y=track->lines[idx].entries[0].c.y;
	    /* ---------------- TEMP ------------ */
	    double slope3=-1/slope2;
	    double yint3=y+(x/slope2);
	    double xI, yI;
	    xI=(y+(x/slope2)-yint2)/(slope2+(1/slope2));
	    yI=slope3*xI+yint3;
	    double x2,y2;
	    x2=2*xI-x;
	    y2=2*yI-y;
	    /*--------------------------------------*/
	    /* Il simmetrico di left ha coordinate (x2,y2),
	       ora ciclo su right */
	    
	    /* Provo col simmetrico rispetto all'asse verticale esatto */
	    double x_half=track->width/2;
	    y2=y;
	    x2=x_half+(x_half-x);

	    if (track->lines[idx].strength<simmetry_imp_cut_max || track->lines[idx].elixir<simmetry_life_cut_max) continue;
	    int candidateIndex=-1;
	    double thresh=-DBL_MAX;
	    for (unsigned j = 0; j<pool_right.size(); j++)
	      {
		int idx2=pool_right[j];
		if (track->lines[idx].type!=track->lines[idx2].type)
		  continue;
		double x22=track->lines[idx2].entries[0].c.x;
		double y22=track->lines[idx2].entries[0].c.y;
		double life_right = track->lines[idx2].elixir;
		double strength_right = track->lines[idx2].strength;
		if (track->lines[idx2].strength<simmetry_imp_cut_max || track->lines[idx2].elixir<simmetry_life_cut_max) continue;
		/* TODO la divisione per la distanza sputtana tutto, però devo usare qualcosa del genere... */
		double distance=sqrt(pow(x2-x22,2)+pow(y2-y22,2));
		if (distance<25)
		  {
		    //cout<< "distance : " << distance << endl;
		    /* Confronto internamente life/imp */ 
		    /* Dovrei trovare una misura di similarità ed usare questa per prendere il max */
		    if (/*min(life_left,life_right)>0.7*max(life_left,life_right) && */(heights(x22,y22)>border_cut)) /*&& min(strength_left,strength_right)>0.7*max(strength_left,strength_right)*/
		      {
			if (fabs(strength_left-strength_right)>thresh && 1/fabs(strength_left-strength_right)>thresh > sim_cut && std::find(pool_final.begin(),pool_final.end(),idx2)==pool_final.end())
			  {
			    thresh=fabs(strength_left-strength_right);
			    //thresh=distance;
			    candidateIndex=idx2;
			  }
		      }
		  }
		
	      }
	    if (candidateIndex!=-1)
	      {
		pool_final.push_back(idx);
		pool_final.push_back(candidateIndex);

	      }
	  }
	//cout << "SIZE FINAL : " << pool_final.size() << endl;
	for (int i=0; i<pool_final.size(); i++)
	  {
	    double scale=0.0;
	    int idx=pool_final[i];
	    if (scale_by_life)
	      scale+=life_imp*track->lines[idx].elixir;
	    if (scale_by_size)
	      scale+=strength_imp*track->lines[idx].strength;

	    
	    __draw_critical_elixir (track->lines[idx].entries[0].c,
	    			    track->original_type (idx),
	    			    track->lines[idx].elixir,
	    			    spot_scale, scale);


	  }
      }
    if (automatic)
      {
	int punta_naso= track->lines[spots_fiducial[0].crit].entries[0].c.x;
	FILE * fp=fopen(autoFile->c_str(),"w");
	/* qui stampo nell'ordine giusto su file */
	
	int occhioSxOut, occhioSxIn, occhioDxOut,occhioDxIn;
	if (isInner) 
	  {
	    occhioSxOut=8;
	    occhioSxIn=6;
	    occhioDxOut=7;
	    occhioDxIn=5;
	  }
	else 
	  {
	    occhioSxOut=6;
	    occhioSxIn=8;
	    occhioDxOut=5;
	    occhioDxIn=7;
	  }
	/* Occhio sx - esterno if (!isInner) 6 else 8*/
	int idx=spots_fiducial[occhioSxOut].crit;
	fprintf(fp, "%d\t%d\n", track->lines[idx].entries[0].c.x, track->height-track->lines[idx].entries[0].c.y);

	/* Occhio sx - interno if (isInner) 6 else 8*/
	idx=spots_fiducial[occhioSxIn].crit;
	fprintf(fp, "%d\t%d\n", track->lines[idx].entries[0].c.x, track->height-track->lines[idx].entries[0].c.y);

	/* Radice naso 1*/
	idx=spots_fiducial[1].crit;
	fprintf(fp, "%d\t%d\n", punta_naso /*track->lines[idx].entries[0].c.x*/, track->height-track->lines[idx].entries[0].c.y);


	/* Occhio dx - interno  if (isInner) 5 else 7*/
      	idx=spots_fiducial[occhioDxIn].crit;
	fprintf(fp, "%d\t%d\n", track->lines[idx].entries[0].c.x, track->height-track->lines[idx].entries[0].c.y);
	/* Occhio dx - esterno if (!isInner) 5 else 7*/
	idx=spots_fiducial[occhioDxOut].crit;
	fprintf(fp, "%d\t%d\n", track->lines[idx].entries[0].c.x, track->height-track->lines[idx].entries[0].c.y);

	/* Naso sx 3*/
	idx=spots_fiducial[4].crit;
	fprintf(fp, "%d\t%d\n", track->lines[idx].entries[0].c.x, track->height-track->lines[idx].entries[0].c.y);

	/* Naso Punta 0*/
	idx=spots_fiducial[0].crit;
	fprintf(fp, "%d\t%d\n", track->lines[idx].entries[0].c.x, track->height-track->lines[idx].entries[0].c.y);

	/* Sotto naso 4*/
	idx=spots_fiducial[2].crit;
	fprintf(fp, "%d\t%d\n", punta_naso /*track->lines[idx].entries[0].c.x*/, track->height-track->lines[idx].entries[0].c.y);

	/* Naso dx 2 */
	idx=spots_fiducial[3].crit;
	fprintf(fp, "%d\t%d\n", track->lines[idx].entries[0].c.x, track->height-track->lines[idx].entries[0].c.y);

	/* Bocca sx  9*/
	idx=spots_fiducial[9].crit;
	fprintf(fp, "%d\t%d\n", track->lines[idx].entries[0].c.x, track->height-track->lines[idx].entries[0].c.y);

	/* PRova per fixare sto1 e sto2 */

	int x_med=(track->lines[spots_fiducial[9].crit].entries[0].c.x+track->lines[spots_fiducial[10].crit].entries[0].c.x)/2;

	/* Bocca sopra 11*/
	idx=spots_fiducial[11].crit;
	//fprintf(fp, "%d\t%d\n", track->lines[idx].entries[0].c.x, track->height-track->lines[idx].entries[0].c.y);
	fprintf(fp, "%d\t%d\n", x_med, track->height-track->lines[idx].entries[0].c.y);

	/* Bocca sotto 12*/
	idx=spots_fiducial[12].crit;
	//fprintf(fp, "%d\t%d\n", track->lines[idx].entries[0].c.x, track->height-track->lines[idx].entries[0].c.y);
	fprintf(fp, "%d\t%d\n", x_med, track->height-track->lines[idx].entries[0].c.y);

	/* Bocca dx 10 */
	idx=spots_fiducial[10].crit;
	fprintf(fp, "%d\t%d\n", track->lines[idx].entries[0].c.x, track->height-track->lines[idx].entries[0].c.y);

	exit(0);
      }
    glMatrixMode (GL_PROJECTION);
    glPopMatrix ();
    glMatrixMode (GL_MODELVIEW);
    glPopMatrix ();
}

void TrackDisplay::draw_line_test (double d)
{
    double color[3];
    glLineWidth (lines_width);

    d /= 1.2;
    glEnable(GL_LINE_SMOOTH);
    glBegin (GL_LINES);
    // glBegin (GL_POINTS);
    for (int i = 0; i < 100; i++)
    {
	double ti = track->time_of_life/100.0 * ((double)i);
	time2color (ti, 0.0, track->time_of_life, color);
	glColor3dv (color);
	glVertex2d (d/100.0 * ((double)i), d/100.0 * ((double)i));
	
	double tip1 = track->time_of_life/100.0 * ((double)i+1.0);
	time2color (tip1, 0.0, track->time_of_life, color);
	glColor3dv (color);
	glVertex2d (d/100.0 * ((double)i+1.0), d/100.0 * ((double)i+1.0));
	// printf ("$$$ %d: %lf %lf -- %lf\n", i, ti, tip1, track->time_of_life);
    }
    glEnd();
    //   exit (0);
}

//void TrackDisplay::swpts_load_asc (char* filename)
//{
    // swpts_active = true;

    // ASCReader ascr (filename);

    // swpts_xllcorner = ascr.xllcorner;
    // swpts_yllcorner = ascr.yllcorner;
    // swpts_cellsize = ascr.cellsize;
//}

//void TrackDisplay::swpts_draw ()
//{
    // // static bool check = true;
    
    // for (unsigned i = 0; i < swpts_ground_truth.size(); i++)
    // {
    // 	double x = swpts_ground_truth[i].p.x;
    // 	double y = swpts_ground_truth[i].p.y;

    // 	// if (check)
    // 	//     printf ("GRUNTH: %lf,%lf %s ", x, y,
    // 	// 	    i < swpts_ground_truth.size()-1? "--" : "|\n");
	
    // 	glPushMatrix ();
    // 	glTranslated ((double) x, (double) y, 0.0);
    // 	glScaled (spot_scale, spot_scale, 1.0);
    // 	glColor3dv(magenta);	
    // 	__draw_cross ();
    // 	glPopMatrix();

    // 	glPushMatrix ();
    // 	glTranslated ((double) x, (double) y, 0.0);
    // 	glScaled (spot_scale / 2.0, spot_scale / 2.0, 1.0);
    // 	glColor3dv(black);
    // 	__draw_cross ();

    // 	glPopMatrix();

    // }

    // // check = false;
//}


// void TrackDisplay::swpts_load_csv (char* filename)
// {
//     // CSVReader csvio (ssp->dem[0]->width, ssp->dem[0]->height,
//     // 		     swpts_cellsize, swpts_xllcorner, swpts_yllcorner);
//     // csvio.load (filename, swpts_ground_truth);
    
//     // swpts_display  = true;
// }

#include "../common/strlcat.h"

// void TrackDisplay::swpts_save_csv (char* filename)
// {
//     char cwd[2048] = "\0";
    
//     // printf ("cwd: %s\n", cwd);
//     // printf ("filename: %s\n", filename);

//     if (filename != NULL && filename[0] != '/')
//     {
// 	if (getcwd(cwd, sizeof(cwd)) == NULL)
// 	    fprintf(stdout, "Could not get working dir: %s\n", strerror(errno));
	
// 	// printf ("cwd: %s\n", cwd);
// 	// printf ("filename: %s\n", filename);

// 	char *app_ending = ".app/Contents/Resources";
// 	if (ends_with (cwd, app_ending))
// 	{
// 	    cwd[0] = '\0';
// 	    strlcat (cwd, "../../../", 2048);
// 	}
// 	else
// 	    cwd[0] = '\0';	    
//     }
//     strlcat (cwd, filename, 2048);

//     // printf ("final: %s\n", cwd);

//     // CSVReader csvio (ssp->dem[0]->width, ssp->dem[0]->height,
// // 		     swpts_cellsize, swpts_xllcorner, swpts_yllcorner);
// //     csvio.save (cwd, spots_current, track, ssp);
//  }
