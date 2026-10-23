#include<iostream>
#include<map>
#include<list>
#include<set>

using namespace std;


struct my_node {
  double node;
  double x;
  double y;
};


map<double,list<my_node>> graph_a;
map<double,list<my_node>> graph_wall_distance;
map<double,pair<double,double>> coordinates_map;


void push_edge(double a, double b, double x, double y){
  graph_a[a].push_back({b,x,y});
}

void push_edge2(double a, double b, double x, double y){
  graph_wall_distance[a].push_back({b,x,y});
}

void full_dive_graph(list<my_node> node_edges,double previous_node){
  for (auto j:node_edges){
      coordinates_map[j.node]=make_pair(coordinates_map[previous_node].first + j.x, coordinates_map[previous_node].second+ j.y);
  //  cout << j.node << " pos = (" << coordinates_map[j.node].first << "," << coordinates_map[j.node].second << ")" << endl;  
    full_dive_graph(graph_a[j.node],j.node);
  }
  return;
}





int main(){
  

  push_edge(1,5,0,1.9992);
  push_edge(5,4,0,1.8620);
  push_edge(1,2,10.2499,0.04);
  push_edge(2,8,0,1.9130);
  push_edge(8,3,0,1.8580);
  push_edge(8,8.1,2.0000,0);
  push_edge(8.1,8.2,2.0000,0);
  push_edge(8.2,8.3,13.5380,0);
  push_edge(8,10.1,-12.0499,0);
  push_edge(10.1,10.2,-1.8,0);
  push_edge(8,7.3,-5.3799,0);
  push_edge(7.3,7,0,-0.7900);
  push_edge(7,7.2,0,-1.5250);
  push_edge(7.3,7.4,0,1.8850);
  push_edge(7.4,7.5,0,1.4995);
  push_edge(7.5,7.1,0,2.1000);
  push_edge(7,6,0.2260,2.0330);

  // push_edge(1,5,0,19530);
  // push_edge(5,4,0,18620);
  // push_edge(1,2,102499,400);
  // push_edge(2,8,0,19130);
  // push_edge(8,3,0,18580);
  // push_edge(8,8.1,20000,0);
  // push_edge(8.1,8.2,20000,0);
  // push_edge(8.2,8.3,135380,0);
  // push_edge(8,10.1,-120499,0);
  // push_edge(10.1,10.2,-156499,0);
  // push_edge(8,7.3,53799,0);
  // push_edge(7.3,7,0,7900);
  // push_edge(7,7.2,0,15250);
  // push_edge(7.3,7.4,0,18850);
  // push_edge(7.4,7.5,0,14995);
  // push_edge(7.5,7.1,0,21000);
  // push_edge(7,6,-2260,20330);





  // for (auto i:graph_a){
  //   cout << i.first << "=>";
  //   for (auto j:i.second){
  //      cout << "\t" << j.node << "(" << j.x <<"," << j.y << ")" << endl;
  //   }
  //   cout << endl;
  // }

  set<pair<double,double>>used_edge;
  

  coordinates_map[1]=make_pair(0,0);

   cout << 1 << " pos = (" << coordinates_map[1].first << "," << coordinates_map[1].second << ")" << endl;  
  full_dive_graph(graph_a[1],1);

  // plt.plot([1, 2, 3, 4], [1, 4, 9, 16], 'ro')
  cout << "X = [";
  bool first = true;
  for (auto i: coordinates_map){
    if (!first){
      cout << ", ";
    } 
    cout << i.second.first;
    first=false;
  }
  cout << "]"<< endl;
  cout << "Y = ["; 
  first = true;
  for (auto i: coordinates_map){
    if (!first){
      cout << ", ";
    } 
    cout << i.second.second;
    first=false;
  }

   cout << "]" << endl;

  cout << "P = ["; 
  first = true;
  for (auto i: coordinates_map){
    if (!first){
      cout << ", ";
    } 
    cout << i.first;
    first=false;
  }
cout << "]" << endl;

  return 0;
}