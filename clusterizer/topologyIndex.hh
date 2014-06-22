#ifndef TOPOLOGYINDEX_H_
#define TOPOLOGYINDEX_H_

#include "cluster.hh"
//#include "mymesh.hh"
//#include "expand.hh"
#include "htimer.hh"
#include <queue>
#include <cfloat>
#include <igl/principal_curvature.h>

typedef std::priority_queue<Cluster*, vector<Cluster*>, ClusterPointerComparison> priorityQueue;

//Classe per rappresentare i livelli della gerarchia di cluster
//E' semplicimente un vettore di puntatori a cluster
class Level
{
private:
  vector<Cluster*> level;
  double aveRad;

public:
  vector<Cluster*>* getLevel();
  void insert(Cluster* cluster);
  vector<Cluster*>::iterator begin();
  vector<Cluster*>::iterator end();
  void mark(bool mark); //per marcare ogni cluster del livello
  void markQuery(bool markQuery);
  Cluster* at(int position);
  Cluster* operator [](int position);
  int gg_size ();
  int gg_ave_vn ();
  int gg_max_vn ();
  int gg_min_vn ();
  double gg_ave_rad (double averageEdge);
  double gg_max_rad (double averageEdge);
  double gg_min_rad (double averageEdge);
  double getAveRad();
  void setAveRad(double aveRad);
  int gg_ave_children();
};

//Classe che rappresenta l'indice spaziale basato su cluster di vertici
class TopologyIndex
{
private:
  vector<Level> index;
  //CMesh* mesh;
  Eigen::MatrixXd V;
  Eigen::MatrixXi F;
  //Expander* expander;
  

  bool * clusterV;
  Cluster** cluster;
  

  double averageEdge;

  /* Funzioni ausiliarie per la conversione di tipo */
  Eigen::Vector3d pointToEigen(Point p)
  {
    return Eigen::Vector3d(p.x,p.y,p.z);
  }

  Point eigenToPoint(Eigen::Vector3d p)
  {
    return Point(p[0],p[1],p[2]);
  }

  //metodi per inserire/accedere ai livelli della gerarchia
  void insertLevel(Level* level);
  Level* getLastLevel();

  //funzione che costruisce l'indice spaziale
  //crea tramite base() il livello 0 e applica step() N volte fino a costruire N livelli
  void build();

  //funzione che genera il livello 0 dell'indice
  void base();

  //costruisce il livello L a partire dall'ultimo livello L-1 inserito nell'indice
  //richiama le funzioni create, select, link e miniball
  void step();

  //costruisce un cluster a livello L per ogni cluster C a livello L-1 con figli C e tutti i suoi vicini
  //restituisce la coda di priorità di cluster così costruiti, ordinata in modo decrescente secondo il numero di figli che contiene
  priorityQueue* create();

  //dai cluster ottenuti dalla create() vengono eliminati i "doppioni"
  //restituisce il nuovo livello della gerarchia che dovrà essere inserito nell'indice
  Level* select(priorityQueue* queue);

  //vengono aggiustati i collegamenti dei cluster del nuovo livello in maniera appropriata (link ai vertici, padre e figli)
  void link(Level* levelNN);

  //viene impostata la sfera sottesa per ogni nuovo cluster creato
  void miniball(Level* levelNN);

  //esegue la fusione tra i cluster più piccoli di una certa dimensione
  void fusion(Level* levelNN);

  //funzione ausiliaria per la query
  void auxQuery(Cluster* cluster, vector<vector<int>*>* candidates, int pick, float sphereRadius);

  //filtra i vertici candidati a essere il risultato della query
  //eliminando quelli che sono esterni alla sfera di query
  vector<int> filterCandidates(vector<vector<int>*>* candidates, int pick, float sphereRadius);

  //Controllo che non ci siano cluster senza vicii
  bool checkNeighborhood(Level* level);


public:
  int pruneLevel;
  unsigned int priorityMode;
  double queryRadiusFactor;
  bool profiling;
  bool verbose;
  bool filteringBenchmark;
  int numberOfLevels;
  int pickedVertex;
  int fusion_base_factor;
  int fusion_progression_factor;
  bool fusionIsSet;
  bool edgeQuery;

  TopologyIndex(Eigen::MatrixXd, Eigen::MatrixXi, int mode, int fusion_base_factor, int fusion_progression_factor, bool factor, bool edgeQuery, double averageEdge, bool verbose, int pruneLevel, int numberOfLevels); //crea l'indice prendendo come parametri un puntatore alla mesh (utile per accedere ai vertici) e un oggetto expander (per eseguire la 1-ring)
  int query(int pick, float sphereRadius, vector<int> *vv, htimer* = NULL);
  //int query(unsigned int intpick, float sphereRadius, vector<int> *vv, htimer* = NULL);
  bool isSelected(int v);
  void clearClusterV(); //per marcare tutti i cluster a false
  void selectClusterV(int v); //per marcare a true un particolare cluster
  void gg_print_stats (std::string meshname);
  void public_build();
  double getAverageEdge();
  void setEdgeQuery(bool edgeQuery);
  unsigned int getTotSpace();
  Level* getLevel(int position);
};

#endif /* TOPOLOGYINDEX_H_ */
