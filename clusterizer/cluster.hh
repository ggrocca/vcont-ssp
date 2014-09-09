#ifndef CLUSTER_H_
#define CLUSTER_H_

#include <vector>
#include <Eigen/Core>
#include <vector>
#include "miniBall.hh"

using std::vector;

//classe i cui oggetti rappresentano un cluster
class Cluster
{
private:
  bool mark;
  bool markQuery;
  Point3 center;
  float radius;
  Cluster* father;
public:
  vector<int> vertices;
  vector<Cluster*> neighborhood;
  vector<Cluster*> children;

  Cluster();
  Cluster(int vertex, Point3); //costruttore che crea un cluster contenente solo vertex (con centro vertex e raggio nullo)
  Cluster(Cluster* cluster);	//costruttore che crea un cluster inserendo come figli il cluster passato come parametro e i suoi vicini

  //metodi getter e setter
  Point3 getCenter();
  Cluster* getFather();
  bool getMark();
  bool getMarkQuery();
  float getRadius();
  vector<Cluster*>* getNeighborhood();
  vector<Cluster*>* getChildren();
  vector<int>* getVertices();
  void setFather(Cluster* father);
  void setCenter(Point3 center);
  //  void setCenter(Eigen::Vector3d center);
  void setRadius(float radius);
  void setMark(bool mark);
  void setMarkQuery(bool markQuery);

  void markChildren(bool mark); //segna tutti i cluster figli
  void insertVertex(int vertex);
  void insertVertices(vector<int>* vertices);
  void insertChild(Cluster* child);
  void insertChildren(vector<Cluster*>* children);
  void insertNeighbor(Cluster* cluster, bool control); //inserisce il cluster passato fra i vicini (control=true -> viene effettuato il controllo per evitare di inserire duplicati)
  void deleteChild(vector<Cluster*>::iterator child);

  int getChildrenVertices();

};

class ClusterPointerComparison
{
private:
  int avg;
  int mode;
public:
  ClusterPointerComparison(int val[2])
  {
    this->mode=val[0];
    this->avg=val[1];
  } 
  ClusterPointerComparison(int mode)
  {
    this->mode=mode;
  } 
  bool operator()(Cluster* x, Cluster* y)
  {
    switch (mode)
      {
      case 0:
      case 1:
      case 4:
      case 5:
	return  abs((int)avg-(int)(x->getChildrenVertices())) > abs((int)avg-(int)(y->getChildrenVertices()));
      case 2:
	return x->getChildren()->size() < y->getChildren()->size();
      case 3:
	return x->getChildren()->size() > y->getChildren()->size();
      }
  }
};


#endif /* CLUSTER_H_ */
