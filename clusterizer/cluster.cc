#include "cluster.hh"

Cluster::Cluster()
{
  father = NULL;
}

Cluster::Cluster (int vertex, Point3 point)
{
  radius = 0;
  center = point;
  vertices.push_back(vertex);
  father = NULL;
  markQuery=false;
}

Cluster::Cluster(Cluster* cluster)
{
  insertChild(cluster);
  insertChildren(cluster->getNeighborhood());
  father = NULL;
  markQuery=false;
}

Point3 Cluster::getCenter()
{
  return center;
}

Cluster* Cluster::getFather()
{
  return father;
}

bool Cluster::getMark()
{
  return mark;
}

bool Cluster::getMarkQuery()
{
  return markQuery;
}

float Cluster::getRadius()
{
  return radius;
}

vector<Cluster*>* Cluster::getNeighborhood()
{
  return &neighborhood;
}

vector<Cluster*>* Cluster::getChildren()
{
  return &children;
}

vector<int>* Cluster::getVertices()
{
  return &vertices;
}

void Cluster::setFather(Cluster* father)
{
  this->father = father;
}

void Cluster::setCenter(Point3 center)
{
  this->center = center;
}

/*void Cluster::setCenter(Point33d center)
  {
  this->center = Eigen::Vector3d(center.X(), center.Y(), center.Z());
  }*/

void Cluster::setRadius(float radius)
{
  this->radius = radius;
}

void Cluster::setMark(bool mark)
{
  this->mark = mark;
}

void Cluster::setMarkQuery(bool markQuery)
{
  this->markQuery = markQuery;
}

void Cluster::markChildren(bool mark)
{
  for (vector<Cluster*>::iterator child = children.begin(); child != children.end(); ++child)
    (*child)->setMark(mark);
}

void Cluster::insertVertex(int vertex)
{
  vertices.push_back(vertex);
}

void Cluster::insertVertices(vector<int>* vertices)
{
  for (vector<int>::iterator vertex = vertices->begin(); vertex != vertices->end(); ++vertex)
    insertVertex(*vertex);
}

void Cluster::insertChild(Cluster* child)
{
  children.push_back(child);
}

void Cluster::insertChildren(vector<Cluster*>* children)
{
  for (vector<Cluster*>::iterator child = children->begin(); child != children->end(); ++child)
    insertChild(*child);
}

//Inserisce il cluster cluster fra i vicini
//control viene messo a true se si vuole effettuare un controllo per evitare di inserire duplicati
void Cluster::insertNeighbor(Cluster* cluster, bool control)
{
  if (cluster != this)
    {
      if (control) for (vector<Cluster*>::iterator neighbor = neighborhood.begin(); neighbor != neighborhood.end(); ++neighbor)
		     if (*neighbor == cluster) return;
      neighborhood.push_back(cluster);
    }
}

void Cluster::deleteChild(vector<Cluster*>::iterator child)
{
  children.erase(child);
}

int Cluster::getChildrenVertices()
{
  int tmp=0;
  for (vector<Cluster*>::iterator child = children.begin(); child != children.end(); ++child)
    tmp+=(*child)->vertices.size();
  return tmp;
}
