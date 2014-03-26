#include "trackdisplay.hh"

TrackSortedPoint::TrackSortedPoint( double lifePerc, int originalIndex ){
	this->life = lifePerc;
	this->baseIndex = originalIndex; 
}

void TrackDisplay::loadTerrain( char* file){

    FILE* r = fopen(file, "r");
	int trash;
	trash = fread(&this->w, 4, 1, r);
	trash = fread(&this->h, 4, 1, r);

	this->terrain = (float*) malloc(w*h*sizeof(float));
	double val;
	for( int i=0; i<(this->w*this->h); i++){
		trash = fread(&val, 8, 1, r);
		*(this->terrain+i) = (float)(val/256.0);
	}
	fclose(r);
}

void TrackDisplay::loadTracks( char* gaussFile, char* bauerFile){
    track_reader( gaussFile, &this->gaussTrack, &this->gaussTrOrder);
    track_reader( bauerFile, &this->bauerTrack, &this->bauerTrOrder);
	this->gaussTrack->drink_elixir();
	this->bauerTrack->drink_elixir();

	this->InitSortedVectorByTrack( &(this->gaussSorted), this->gaussTrack, "gaussPlot.dat");
	this->InitSortedVectorByTrack( &(this->bauerSorted), this->bauerTrack, "bauerPlot.dat");
}

void TrackDisplay::InitSortedVectorByTrack(std::vector<TrackSortedPoint*> *sortedVector, Track* track, char* filename){

	// Inizializzazione vettore e ordinamento rispetto alla vita
	for( int i=0; i<track->lines.size() && track->is_original(i); i++){
		sortedVector->push_back( new TrackSortedPoint(track->lifetime_elixir(i)/track->time_of_life, i ) );
	}
	std::sort (sortedVector->begin(), sortedVector->end(), this->myLifeSorter);

	// Salvataggio su File
	FILE* fp = fopen(filename, "w");
	for( int i=0; i<sortedVector->size(); i++){
		fprintf( fp,"%lf %d\n", sortedVector->at(i)->life, ((int)sortedVector->size())-i);
	}
	fclose(fp);

	// Salvataggio dell'indice vita e riordinamento come in origine
	for( int i=0; i<sortedVector->size(); i++){
		sortedVector->at(i)->lifeIndex = i;
	}
	std::sort (sortedVector->begin(), sortedVector->end(), this->myBaseSorter);
}

float TrackDisplay::getLife( int i, int tMode, Track* t, std::vector<TrackSortedPoint*> v, double min, double max){
	double trackLife;
	if(tMode == TrackDisplay::ON_LIFE){
		trackLife = t->lifetime_elixir(i)/t->time_of_life;
	}else{
		trackLife = ((double)v.at(i)->lifeIndex)/ ((double)v.size());
	}

	if(trackLife >= min && trackLife <= max){
		return (trackLife-min)/(max-min);
	}
	return -1;
}

float TrackDisplay::getCriticalWeight( int i, int tMode, int pMode){
	double g = this->getLife(i, tMode, this->gaussTrack, this->gaussSorted, this->gaussWFrom, this->gaussWTo);
	double b = this->getLife(i, tMode, this->bauerTrack, this->bauerSorted, this->bauerWFrom, this->bauerWTo);

	if(pMode == TrackDisplay::R_DEFAULT){
		if(g >= 0 && b >= 0){
			if(this->choosedWeightType == W_AVG){
				return g*(1.0f-this->weightBalance) + b*(this->weightBalance);
			}
			else if(this->choosedWeightType == W_MAX){
				return fmax( g, b);
			}
			else if(this->choosedWeightType == W_MIN){
				return fmin( g, b);
			}
		}

	}else if(pMode == TrackDisplay::R_GAUSS){
		if(g >= 0) return g;

	}else if(pMode == TrackDisplay::R_BAUER){
		if(b >= 0) return b;
	}

	return -1;
}

void TrackDisplay::draw(){

    glMatrixMode (GL_PROJECTION);
    glPushMatrix ();
    glMatrixMode (GL_MODELVIEW);
    glPushMatrix ();

    if (this->draw_terrain){
		float col;
		glBegin (GL_QUADS);
		for( int i=0; i<this->w; i++){
			for( int j=0; j<this->h; j++){
				col = *(this->terrain+(i*this->h)+j);
				glColor3f (col, col, col);

				glVertex2f (i-0.5, j-0.5);
				glVertex2f (i-0.5, j+0.5);
				glVertex2f (i+0.5, j+0.5);
				glVertex2f (i+0.5, j-0.5);
			}
		}
	    glEnd ();
    }

	if(this->draw_criticals){
		float weight;
		Point p;
		for( int i=0; i<this->gaussTrack->lines.size() && this->gaussTrack->is_original(i); i++){
			if(this->getCriticalWeight(i, this->thresholdMode, TrackDisplay::R_DEFAULT) >= this->weightThreshold){
				p = this->gaussTrack->start_point(i);
				if(this->pointNeedToBeDrawn(p)){

					if(this->draw_as_spheres){
						this->drawCircle(p, this->getCriticalWeight(i, this->thresholdMode, this->printMode), this->gaussTrack->get_type(i));
					}else{
						this->drawCross(p, this->getCriticalWeight(i, this->thresholdMode, TrackDisplay::R_GAUSS),
											this->getCriticalWeight(i, this->thresholdMode, TrackDisplay::R_BAUER), this->gaussTrack->get_type(i));
					}
				}
			}
		}
	}

    glMatrixMode (GL_PROJECTION);
    glPopMatrix ();
    glMatrixMode (GL_MODELVIEW);
    glPopMatrix ();
}

void TrackDisplay::save( char* file, bool viewedOnly){

	float weight;
	Point p;
	FILE* fp = fopen(file, "w");

	for( int i=0; i<this->gaussTrack->lines.size() && this->gaussTrack->is_original(i); i++){
		if(this->getCriticalWeight(i, this->thresholdMode, TrackDisplay::R_DEFAULT) >= this->weightThreshold || (!viewedOnly)){
			p = this->gaussTrack->start_point(i);
			if(this->pointNeedToBeDrawn(p)){
				double g = this->getLife(i, TrackDisplay::ON_LIFE, this->gaussTrack, this->gaussSorted, this->gaussWFrom, this->gaussWTo);
				double b = this->getLife(i, TrackDisplay::ON_LIFE, this->bauerTrack, this->bauerSorted, this->bauerWFrom, this->bauerWTo);
				fprintf( fp, "%d [%d;%d] %3.1f %3.1f\n", this->gaussTrack->get_type(i), (int)p.x, (int)p.y, g * 100.0, b * 100.0);
			}
		}
	}

	fclose(fp);
}

bool TrackDisplay::pointNeedToBeDrawn(Point p){
	return (this->draw_boundary_criticals || (p.x>0 && p.x<this->w-1 && p.y>0 && p.y<this->h-1));
}

void TrackDisplay::drawCircle(Point p, float weight, int color){

	glPushMatrix ();
	float radius = this->baseRadius + (weight * this->weightedRadius);

	if(color==1) glColor3f (1.0, 0.0, 0.0);
	else if(color==2) glColor3f (0.0, 0.0, 1.0);
	else glColor3f (0.0, 1.0, 0.0);

	glBegin (GL_POLYGON);
	for( int i=0; i<360; i+=20){
		// ATTENZIONE: le righe sono volutamente invertite con le colonne per raddrizzare l'immagine ruotata in fase di tracking
		//glVertex2f (p.x + radius*cos(((float)i)*PI/180.0f), p.y + radius*sin(((float)i)*PI/180.0f));
		glVertex2f (p.y + radius*cos(((float)i)*PI/180.0f), p.x + radius*sin(((float)i)*PI/180.0f));
	}
	glEnd ();
	glPopMatrix ();
}

void TrackDisplay::drawCross(Point p, float wGauss, float wBauer, int color){

	glPushMatrix ();
	float gRadius = this->baseRadius + (wGauss * this->weightedRadius);
	float bRadius = this->baseRadius + (wBauer * this->weightedRadius);

	if(color==1) glColor3f (1.0, 0.0, 0.0);
	else if(color==2) glColor3f (0.0, 0.0, 1.0);
	else glColor3f (0.0, 1.0, 0.0);

	glBegin (GL_LINES);
		// ATTENZIONE: le righe sono volutamente invertite con le colonne per raddrizzare l'immagine ruotata in fase di tracking
		glVertex2f (p.y - gRadius, p.x - gRadius);
		glVertex2f (p.y + gRadius, p.x + gRadius);
		glVertex2f (p.y - gRadius, p.x + gRadius);
		glVertex2f (p.y + gRadius, p.x - gRadius);

		glVertex2f (p.y - bRadius, p.x);
		glVertex2f (p.y + bRadius, p.x);
		glVertex2f (p.y, p.x - bRadius);
		glVertex2f (p.y, p.x + bRadius);
	glEnd ();
	glPopMatrix ();
}

void TrackDisplay::getBounds( Point* a, Point* b)
{
    a->x = 0.0;
    a->y = 0.0;
    b->x = this->w;
    b->y = this->h;
}


TrackDisplay::TrackDisplay()
{
    this->draw_terrain = true;
    this->draw_criticals = true;
    this->draw_boundary_criticals = false;
    this->draw_as_spheres = true;
    this->baseRadius = 1.5;
    this->weightedRadius = 7.0;
	this->weightThreshold = 0.35;
	this->weightBalance = 0.5;
	this->choosedWeightType = W_AVG;

	this->gaussWFrom = 0.0;
	this->gaussWTo = 1.0;
	this->bauerWFrom = 0.0;
	this->bauerWTo = 1.0;
};


