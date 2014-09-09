#include "topologyIndex.hh"
#include "htimer.hh"
#include "miniBall.hh"
#include <iostream>
#include <fstream>

#define IGL_HEADER_ONLY
#include <igl/adjacency_list.h>

void TopologyIndex::gg_print_stats (std::string meshname)
{
  printf ("\ntopoindex generated %d levels.\n", (int) index.size ());
  printf ("  level \t size    \t ave_vn \t max_vn \t min_vn \tave_rad \tmax_rad \tmin_rad \tspace\n");

  unsigned int totSpace=0;
  for (unsigned i = 0; i < index.size (); i++)
    {
      unsigned int tmpSpace=0;
      vector<Cluster*>* level=index[i].getLevel();
      for (unsigned j = 0; j < level->size (); j++)
        {
	  tmpSpace+=(21+level->at(j)->children.size()*4+level->at(j)->neighborhood.size()*4+level->at(j)->vertices.size()*4);
        }
      printf ("     %d \t %12d \t %12d \t %12d \t %12d \t  %12f \t  %12f \t  %12f \t %12u\n",
	      i, index[i].gg_size(), index[i].gg_ave_vn(),
	      index[i].gg_max_vn(), index[i].gg_min_vn(), index[i].gg_ave_rad(averageEdge), index[i].gg_max_rad(averageEdge), index[i].gg_min_rad(averageEdge), tmpSpace);
      totSpace+=tmpSpace;
    }
  printf("\n");
  std::ofstream myfile;
  char path[100];
  for (unsigned i = 1; i < index.size (); i++)	
    {
      switch (priorityMode)
	{
	case 0:
	  sprintf(path, "media-level-%03d.dat", i);
	  break;
	case 1:
	  sprintf(path, "media-stimata-level-%03d.dat", i);
	  break;
	case 2:
	  if (!fusionIsSet)
	    sprintf(path, "Normal-NoFusion/%s-level-%03d.dat", meshname.c_str(), i);
	  else
	    sprintf(path, "priorità-normale-level-%03d.dat", i);
	  break;
	case 3:
	  if (!fusionIsSet)
	    sprintf(path, "PrioritaInvertita-NoFusion/%s-level-%03d.dat", meshname.c_str(), i);
	  else
	    sprintf(path, "PrioritaInvertita-%d_%d/%s-level-%03d.dat", fusion_base_factor, fusion_progression_factor, meshname.c_str(), i);
	  break;
	case 4: 
	  if (!fusionIsSet)
	    sprintf(path, "MediaFigli-NoFusion/%s-level-%03d.dat", meshname.c_str(), i);
	  else
	    sprintf(path, "MediaFigli-%d_%d/%s-level-%03d.dat", fusion_base_factor, fusion_progression_factor, meshname.c_str(), i);
	  break;
	case 5: 
	  sprintf(path, "media-stimata-figli-level-%03d.dat", i);
	  break;
	default:
	  sprintf(path, "unknown-path-%03d.dat", i);
	  break;
	}
      myfile.open(path);
      unsigned int min=index[i].gg_min_vn();
      unsigned int max=index[i].gg_max_vn();
      for (unsigned int j=min; j<=max; j++)
	{ 
	  int tmp=0;
	  for (int k = 0; k < index[i].gg_size (); k++)
	    if( index[i].getLevel()->at(k)->getVertices()->size() == j)
	      tmp++;
	  myfile << j << " " << tmp << std::endl;
	}
      myfile.close();
    }
  if (verbose)
    printf("Occupazione Spaziale: %u, with mesh attrs: %u\n", totSpace, getTotSpace());
}

unsigned int TopologyIndex::getTotSpace()
{
  unsigned int totSpace=0;
  for (unsigned i = 0; i < index.size (); i++)
    {
      vector<Cluster*>* level=index[i].getLevel();
      for (unsigned j = 0; j < level->size (); j++)
	totSpace+=(21+level->at(j)->children.size()*4+level->at(j)->neighborhood.size()*4+level->at(j)->vertices.size()*4);
    }
  return totSpace + V.rows() * 4;
}

int Level::gg_size ()
{
  return level.size ();
}


int Level::gg_ave_vn ()
{
  unsigned vn = 0;
  for (unsigned i = 0; i < level.size (); i++)
    vn += level[i]->vertices.size ();
  return vn / level.size ();
}

int Level::gg_max_vn ()
{
  unsigned vn = 0;
  for (unsigned i = 0; i < level.size (); i++)
    if (vn < level[i]->vertices.size ())
      vn = level[i]->vertices.size ();
  return vn;
}

int Level::gg_min_vn ()
{
  unsigned vn = ~((unsigned)0);
  for (unsigned i = 0; i < level.size (); i++)
    if (vn > level[i]->vertices.size ())
      vn = level[i]->vertices.size ();
  return vn;
}

int Level::gg_ave_children ()
{
  unsigned children = 0;
  for (unsigned i = 0; i < level.size (); i++)
    children += level[i]->getChildren()->size ();
  return children / level.size ();
}

double Level::gg_ave_rad (double averageEdge)
{
  double vn = 0;
  for (unsigned i = 0; i < level.size (); i++)
    vn += level[i]->getRadius()/averageEdge;
  return vn / (double) level.size ();
}

double Level::gg_max_rad (double averageEdge)
{
  double vn = 0;
  for (unsigned i = 0; i < level.size (); i++)
    if (vn < level[i]->getRadius()/averageEdge)
      vn = level[i]->getRadius()/averageEdge;
  return vn;
}

double Level::gg_min_rad (double averageEdge)
{
  double vn = FLT_MAX;
  for (unsigned i = 0; i < level.size (); i++)
    if (vn > level[i]->getRadius()/averageEdge)
      vn = level[i]->getRadius()/averageEdge;
  return vn;
}

double Level::getAveRad()
{
  return this->aveRad;
}

void Level::setAveRad(double aveRad)
{
  this->aveRad=aveRad;
}

vector<Cluster*>* Level::getLevel()
{
  return &level;
}

void Level::insert(Cluster* cluster)
{
  level.push_back(cluster);
}

vector<Cluster*>::iterator Level::begin()
{
  return level.begin();
}

vector<Cluster*>::iterator Level::end()
{
  return level.end();
}

void Level::mark(bool mark)
{
  for (vector<Cluster*>::iterator cluster = level.begin(); cluster < level.end(); cluster++)
    (*cluster)->setMark(mark);
}

void Level::markQuery(bool markQuery)
{
  for (vector<Cluster*>::iterator cluster = level.begin(); cluster < level.end(); cluster++)
    (*cluster)->setMarkQuery(markQuery);
}

Cluster* Level::at(int position)
{
  return level.at(position);
}

Cluster* Level::operator [](int position)
{
  return level[position];
}


void TopologyIndex::insertLevel(Level* level)
{
  index.push_back(*level);
}

Level* TopologyIndex::getLastLevel()
{
  return &(index.at(index.size() - 1));
}

Level* TopologyIndex::getLevel(int position)
{
  return &(index.at(position));
}

void TopologyIndex::base()
{
  Level* level0 = new Level();
  //per ogni vertice della mesh
  //creo un cluster contenente il solo vertice, la cui sfera sottesa ha centro nel vertice stesso e raggio nullo
  //setto l'assocazione tra il vertice e il cluster a livello 0
  //inserisco il nuovo cluster nel livello 0
  /*   for (CMesh::VertexIterator vertex = mesh->vert.begin(); vertex != mesh->vert.end(); ++vertex)
       {
       Cluster* cluster = new Cluster(&(*vertex));
       level0->insert(cluster);
       this->cluster[&(*vertex)] = cluster;
       }*/
  for (int i=0; i<V.rows(); i++)
    {
      Cluster* cluster = new Cluster(i,eigenToPoint(Eigen::Vector3d(V.row(i))));
      level0->insert(cluster);
      this->cluster[i]=cluster;
    }
  // vector<CVertex*> vv;
  //impostazione dei vicini di ogni cluster a livello 0 usando la VV
  /*for (vector<Cluster*>::iterator cluster = level0->begin(); cluster != level0->end(); ++cluster)
    {
    expander->ring((*cluster)->getVertices()->at(0), &vv);
    for (vector<CVertex*>::iterator v = vv.begin(); v != vv.end(); ++v)
    (*cluster)->insertNeighbor(this->cluster[*v], false); //false->evito di controllare l'eventuale di duplicazione dei vicini
    }*/
  vector<vector<int> > adj;
  igl::adjacency_list(F,adj);
  for (vector<Cluster*>::iterator cluster = level0->begin(); cluster != level0->end(); ++cluster)
    {
      for (int j=0; j<adj[(*cluster)->getVertices()->at(0)].size(); j++)
	{

	  (*cluster)->insertNeighbor(this->cluster[adj[(*cluster)->getVertices()->at(0)][j]], false); //false->evito di controllare l'eventuale di duplicazione dei vicini
	}
    }
  //inserimento del livello creato nell'indice
  insertLevel(level0);
}

priorityQueue* TopologyIndex::create()
{
  Level* level = getLastLevel();
  //per ogni cluster del livello precedentemente creato
  //marco a false il cluster
  //ne creo uno nuovo con figli il cluster stesso e i suoi vicini
  //e inserico il nuovo cluster nella coda di priorità (ordinata in modo decrescente in base al numero di figli)
  vector<Cluster*>* candidates=new vector<Cluster*>();
  priorityQueue* queue;
  int values[2];
  unsigned tmp;
  switch (priorityMode)
    {
    case 0:
      {
	for (vector<Cluster*>::iterator cluster = level->begin(); cluster != level->end(); ++cluster)
	  {
	    candidates->push_back(new Cluster(*cluster));
	  }
	tmp=0;
	for (vector<Cluster*>::iterator cluster = candidates->begin(); cluster != candidates->end(); ++cluster)
	  {
	    tmp+=(*cluster)->getChildrenVertices();
	  }
	values[0]=0;
	values[1]=tmp/candidates->size();
	queue = new priorityQueue(values);
	break;
      }
    case 1:
      {
	values[0]=1;
	values[1]=6*level->gg_ave_vn();
	queue = new priorityQueue(values);
	break;
      }
    case 4: //Media figli
      {
	for (vector<Cluster*>::iterator cluster = level->begin(); cluster != level->end(); ++cluster)
	  {
	    candidates->push_back(new Cluster(*cluster));
	  }
	tmp=0;
	for (vector<Cluster*>::iterator cluster = candidates->begin(); cluster != candidates->end(); ++cluster)
	  {
	    tmp+=(*cluster)->getChildren()->size();
	  }
	values[0]=4;
	values[1]=tmp/candidates->size();
	queue = new priorityQueue(values);
	break;
      }
    case 5: //Media stimata figli
      {
	values[0]=5;
	values[1]=6*level->gg_ave_children();
	queue = new priorityQueue(values);
	break;
      }
    default:
      queue = new priorityQueue(priorityMode);
      break;
    }

  for (vector<Cluster*>::iterator cluster = level->begin(); cluster != level->end(); ++cluster)
    {
      (*cluster)->setMark(false);
      Cluster* newCluster = new Cluster(*cluster);
      queue->push(newCluster);
    }
  delete candidates;
  return queue;
}

Level* TopologyIndex::select(priorityQueue* queue)
{
  Level* levelNN = new Level;
  while (!queue->empty())
    {
      //si prende il cluster dalla cima della quota
      Cluster* cluster = queue->top();
      queue->pop();
      bool reinsert = false;
      //per ogni figlio del cluster
      //se è marcato a true
      //lo si elimina tra i figli del cluster
      //e setto il flag reinsert a true
      for (vector<Cluster*>::iterator child = cluster->getChildren()->begin(); child < cluster->getChildren()->end(); ++child)
        {
	  if ((*child)->getMark())
            {
	      cluster->deleteChild(child--);
	      reinsert = true;
            }
        }
      //se reinsert è true e il cluster ha figli
      //dovrà essere reiserito nella coda
      if (reinsert)
        {
	  if (cluster->getChildren()->size() != 0) queue->push(cluster);
        }
      //altrimenti il cluster viene inserito nel nuovo livello
      //e tutti i suoi figli vengono segnati
      else
        {
	  levelNN->insert(cluster);
	  cluster->markChildren(true);
        }
    }
  delete queue;
  return levelNN;
}


void TopologyIndex::link(Level* levelNN)
{
  //per ogni cluster del nuovo livello
  //i suoi vertici saranno dati dall'unione di tutti i vertici dei suoi figli
  //e per ogni figlio viene settato il padre al cluster considerato
  for (vector<Cluster*>::iterator cluster = levelNN->begin(); cluster != levelNN->end(); ++cluster)
    {
      for (vector<Cluster*>::iterator child = (*cluster)->getChildren()->begin(); child != (*cluster)->getChildren()->end(); ++child)
        {
	  (*cluster)->insertVertices((*child)->getVertices());
	  (*child)->setFather(*cluster);
        }
    }
  //per ogni cluster del nuovo livello
  //i suoi vicini saranno i padri dei vicini dei suoi figli
  for (unsigned int j=0; j<levelNN->getLevel()->size(); j++)
    {
      vector<Cluster*>::iterator cluster=levelNN->begin()+j;
      for (vector<Cluster*>::iterator child = (*cluster)->getChildren()->begin(); child != (*cluster)->getChildren()->end(); ++child)
	for (vector<Cluster*>::iterator neighbor = (*child)->getNeighborhood()->begin(); neighbor != (*child)->getNeighborhood()->end(); ++neighbor)
	  (*cluster)->insertNeighbor((*neighbor)->getFather(), true); //true->viene effettuato il controllo per evitare i duplicati
      if ((*cluster)->neighborhood.size() < 1 && levelNN->getLevel()->size()>1)
        {
	  delete (*cluster);
	  levelNN->getLevel()->erase(levelNN->getLevel()->begin()+j);
	  j--;
        }
    }
}

void TopologyIndex::miniball(Level* levelNN)
{
  //per ogni cluster del nuovo livello
  //viene calcolata la sfera sottesa
  //impostando il centro e il raggio della sfera
  for (vector<Cluster*>::iterator cluster = levelNN->begin(); cluster != levelNN->end(); ++cluster)
    {
      //TODO: qui devo estrarre i punti, ciclare, metterli in un array e chiamare miniball */
      vector<int>* vert = (*cluster)->getVertices();
      //Point P[vert->size()];
      Point3 *P = new Point3[vert->size()];

      for (int i=0; i<vert->size(); i++)
	P[i]=Point3(V.row((*vert)[i])[0],V.row((*vert)[i])[1],V.row((*vert)[i])[2]);
      
      miniBall ms = MinSphere(P,vert->size());
      (*cluster)->setCenter(ms.center);
      (*cluster)->setRadius(ms.radius);

      delete P;
    }
}

void TopologyIndex::fusion(Level* levelNN)
{
  int level=index.size();
  int factor=fusion_base_factor*pow(fusion_progression_factor,level);
  //Se il fattore di eliminazione dei cluster è maggiore del numero dei vertici della mesh non eseguiamo nulla
  if (factor >index.at(0).gg_size()) return;
  for (unsigned int j=0; j<levelNN->getLevel()->size(); j++)
    {
      Cluster* cluster=levelNN->at(j);
      //Se il cluster rispetta i criteri di eliminazione
      if ((cluster)->getVertices()->size()<factor)
        {
	  //Ciclo su tutti i suoi figli
	  for (vector<Cluster*>::iterator child = (cluster)->getChildren()->begin(); child != (cluster)->getChildren()->end(); ++child)
            {
	      //centro miniball del figlio:
	      Point3 center_f=(*child)->getCenter();
	      //Cerco il vicino migliore
	      float min_distance=FLT_MAX;
	      Cluster* candidate_neighbor=NULL;
	      for (vector<Cluster*>::iterator neighbor = (cluster)->getNeighborhood()->begin(); neighbor !=  (cluster)->getNeighborhood()->end(); ++neighbor)
                {
		  if ( (center_f-(*neighbor)->getCenter()).length() < min_distance)
                    {
		      candidate_neighbor=(*neighbor);
		      min_distance=(center_f-(*neighbor)->getCenter()).length();
                    }
                }
	      //Aggiungo vertici del figlio al vicino trovato
	      candidate_neighbor->insertVertices((*child)->getVertices());

	      vector<int>* vert = candidate_neighbor->getVertices();
	      //Point P[vert->size()];
	      Point3 *P = new Point3[vert->size()];
	      
	      for (int i=0; i<vert->size(); i++)
		P[i]=Point3(V.row((*vert)[i])[0],V.row((*vert)[i])[1],V.row((*vert)[i])[2]);
      
	      miniBall ms = MinSphere(P,vert->size());

	      candidate_neighbor->setCenter(ms.center);
	      candidate_neighbor->setRadius(ms.radius);

	      delete P;

	      //Il vicino trovato è il nuovo padre del figlio
	      (*child)->setFather(candidate_neighbor);
	      //E viceversa
	      candidate_neighbor->insertChild(*child);
	      //Giro tra i vicini del figlio per aggiornare la relazione di vicinanza
	      for (vector<Cluster*>::iterator neighbor = (*child)->getNeighborhood()->begin(); neighbor !=  (*child)->getNeighborhood()->end(); ++neighbor)
                {
		  if ((*neighbor)->getFather()!=candidate_neighbor && (*neighbor)->getFather()!=(cluster))
                    {
		      candidate_neighbor->insertNeighbor((*neighbor)->getFather(),true);
		      (*neighbor)->getFather()->insertNeighbor(candidate_neighbor,true);
                    }
                }
            }
	  //Elimino il rapporto di vicinanza del cluster con tutti i suoi vicini
	  for (vector<Cluster*>::iterator neighbor = (cluster)->getNeighborhood()->begin(); neighbor !=  (cluster)->getNeighborhood()->end(); ++neighbor)
            {
	      for (unsigned int k=0; k<(*neighbor)->getNeighborhood()->size(); k++)
                {
		  if((*neighbor)->getNeighborhood()->at(k)==(cluster))
                    {
		      (*neighbor)->getNeighborhood()->erase((*neighbor)->getNeighborhood()->begin() + k);
                    }
                }
            }
	  levelNN->getLevel()->erase(levelNN->getLevel()->begin()+j);
	  j--;
        }
    }
}

void TopologyIndex::step()
{
  priorityQueue* queue = create();
  Level* levelNN = select(queue);
  link(levelNN);
  assert(checkNeighborhood(levelNN));
  miniball(levelNN);
  if (fusionIsSet) 
    fusion(levelNN);
  levelNN->setAveRad(levelNN->gg_ave_rad(1.0));
  insertLevel(levelNN);

}

void TopologyIndex::build()
{
  if (verbose)
    printf("creating index... ");
  htimer zving;
  htimer_start(&zving);

  //creo il livello 0
  base();
  int i=1;
  //iterativamente costruisco i nuovi livelli a partire da quelli precedenti
  while (getLastLevel()->at(0)->getNeighborhood()->size() != 0 && (i<pruneLevel+numberOfLevels || numberOfLevels==0))
    {
      step();
      i++;
    }


  // for (CMesh::VertexIterator vertex = mesh->vert.begin(); vertex != mesh->vert.end(); ++vertex)
  // {
  // 	Cluster* cluster = this->cluster[&(*vertex)];
  //     for (int i=0; i<pruneLevel; i++)
  // 	    if (cluster->getFather() != NULL) 
  // 		cluster = cluster->getFather();
  //     this->cluster[&(*vertex)] = cluster;
  // }

  for (int k=0; k<V.rows(); k++)
    {
      Cluster* cluster = this->cluster[k];
      for (int i=0; i<pruneLevel; i++)
	if (cluster->getFather() != NULL) 
	  cluster = cluster->getFather();
      this->cluster[k] = cluster;
    }
    
  index.erase(index.begin(),index.begin()+pruneLevel);


  if (verbose)
    printf("[%s ms elapsed]\n", htimer2string(htimer_get(&zving)));
}

void TopologyIndex::public_build()
{
  index.erase(index.begin(),index.end());
  build();
}

//funzione ricorsiva ausiliaria per la query
void TopologyIndex::auxQuery(Cluster* cluster, vector<vector<int>*>* candidates, int pick, float sphereRadius)
{
  //controllo che il cluster non sia già stato visitato
  if (cluster->getMarkQuery()) return;
  //se la sfera del cluster interseca quella di ricerca
  //visito il cluster
  //inserendo fra i candidati tutti i suoi vertici
  //e propago la ricerca ai suoi vicini
  //TODO  pick to Vector - Point to Eigen
  Eigen::Vector3d punto = Eigen::Vector3d(V.row(pick));
  Point3 p = eigenToPoint(punto);
  if ((cluster->getCenter() - p).length() < (cluster->getRadius() + sphereRadius))
    {
      cluster->setMarkQuery(true);
      candidates->push_back(cluster->getVertices());
      for (vector<Cluster*>::iterator neighbor = cluster->getNeighborhood()->begin(); neighbor < cluster->getNeighborhood()->end(); ++neighbor)
	auxQuery(*neighbor, candidates, pick, sphereRadius);
    }
}

//funzione che filtra i candidati eliminando quelli esterni alla sfera di ricerca
vector<int> TopologyIndex::filterCandidates(vector<vector<int>*>* candidates, int pick, float sphereRadius)
{
  vector<int> result;
  Eigen::Vector3d punto = Eigen::Vector3d(V.row(pick));
  Point3 p = eigenToPoint(punto);
  for (vector<vector<int>*>::iterator candidates_vector = candidates->begin(); candidates_vector < candidates->end(); ++candidates_vector)
    for (vector<int>::iterator candidate = (*candidates_vector)->begin(); candidate < (*candidates_vector)->end(); ++candidate)
      {
	//TODO candidate to my vec
	Eigen::Vector3d punto2 = Eigen::Vector3d(V.row(*candidate));
	Point3 p2 = eigenToPoint(punto2);
	float distance = (p - p2).length();
	if (distance <= sphereRadius/* && distance != 0*/) result.push_back(*candidate);
      }
  return result;
}

TopologyIndex::TopologyIndex(Eigen::MatrixXd V, Eigen::MatrixXi F, int mode, int fusion_base_factor, int fusion_progression_factor, bool fusion, bool edgeQuery, double averageEdge, bool verbose, int pruneLevel, int numberOfLevels)
{
  //vengono settati i puntatori alla mesh e all'oggetto nav del visualizer
  this->V=V;
  this->F=F;
  this->priorityMode=mode;
  this->fusion_base_factor=fusion_base_factor;
  this->fusion_progression_factor=fusion_progression_factor;
  this->fusionIsSet=fusion;
  this->averageEdge=averageEdge;
  this->edgeQuery=edgeQuery;
  this->verbose=verbose;
  this->pruneLevel=pruneLevel;
  this->numberOfLevels=numberOfLevels;
    
  //vengono settati gli attributi ai vertici della mesh
  //clusterV = tri::Allocator<CMesh>::AddPerVertexAttribute<bool>(*mesh, std::string("clusterV"));
  //cluster = tri::Allocator<CMesh>::AddPerVertexAttribute<Cluster*>(*mesh, std::string("cluster"));
  clusterV = (bool*) calloc (V.rows(), sizeof(bool));
  cluster = (Cluster**) calloc (V.rows(), sizeof(Cluster*));
  //tutti i vertici vengono marcati a false (per la visualizzazione dei vertici risultati della query sull'indice)
  //clearClusterV();

  //viene costruito l'indice spaziale
  build();
  queryRadiusFactor = 10.0;
}

int TopologyIndex::query(int pick, float sphereRadius,vector<int> * vv, htimer* filterTime)
  {
  htimer zving;

  htimer_start(&zving);
  vv->clear();
  if (verbose && profiling)
  printf("[%s ms elapsed - Clear]\n", htimer2string(htimer_get(&zving)));

  //recupero il cluster a livello 0 corrispondente al vertice selezionato
  Cluster* cluster = this->cluster[pick];
  int level = 0;


  //risalgo la gerarchia fino a trovare un cluster la cui sfera ha un raggio maggiore di quello della sfera di ricerca
  htimer_start(&zving);
  if (!edgeQuery)
  while (cluster->getRadius() < sphereRadius / queryRadiusFactor  && cluster->getFather() != NULL)
  {
  cluster = cluster->getFather();
  level++;
  }
  else
  while (index.at(level).getAveRad()  <  sphereRadius / queryRadiusFactor && cluster->getFather() != NULL)
  {
  cluster = cluster->getFather();
  level++;
  }
  if (verbose && profiling)
  printf("[%s ms elapsed - Risalita della struttura]\n", htimer2string(htimer_get(&zving)));

  //mantengo i riferimenti ai candidati come puntatori ai vertici dei cluster che intersecano la sfera di ricerca
  vector<vector<int>*> candidates;

  //cerco i possibili vertici del risultato operando una BFS sui vicini del cluster
  htimer_start(&zving);
  auxQuery(cluster, &candidates, pick, sphereRadius);
  if (verbose && profiling)
  printf("[%s ms elapsed - AuxQuery]\n", htimer2string(htimer_get(&zving)));

  //filtro i candidati eliminando quelli esterni alla sfera di ricerca
  htimer_start(&zving);
  *vv = filterCandidates(&candidates, pick, sphereRadius);
  if (verbose && profiling)
  printf("[%s ms elapsed - filtercandidates]\n", htimer2string(htimer_get(&zving)));
  if (filterTime)
  {
  *filterTime=htimer_get(&zving);
  }

  if (verbose)
    printf ("query at level %d, trovati %d vertici ", level, vv->size());

  return level;

  }

/*int TopologyIndex::query(unsigned int intpick, float sphereRadius, vector<CVertex*> *vv, htimer* filterTime)
  {
  CVertex* pick=&(mesh->vert[intpick]);
  return query(pick,sphereRadius,vv, filterTime);
  }*/

bool TopologyIndex::isSelected(int v)
{
  return clusterV[v];
}

void TopologyIndex::clearClusterV()
{
  /*   for (CMesh::VertexIterator vi = mesh->vert.begin(); vi != mesh->vert.end(); ++vi)
       clusterV[vi] = false;*/
  for (int i=0; i<V.rows(); i++)
    clusterV[i]=false;
}

void TopologyIndex::selectClusterV(int v)
{
  clusterV[v] = true;
}


double TopologyIndex::getAverageEdge()
{
  return averageEdge;
}

void TopologyIndex::setEdgeQuery(bool edgeQuery)
{
  this->edgeQuery=edgeQuery;
}

bool TopologyIndex::checkNeighborhood(Level *level)
{
  if (level->getLevel()->size()==1)
    return true;
  int t=0;
  for (vector<Cluster*>::iterator cluster = level->getLevel()->begin(); cluster !=level->getLevel()->end() ; ++cluster)
    {
      if ((*cluster)->getNeighborhood()->size()==0)
        {
	  t++;
        }
    }
  if (t)
    {
      std::cout << "Cluster senza vicini: " << t << std::endl;
      return false;
    }
  return true;
}
