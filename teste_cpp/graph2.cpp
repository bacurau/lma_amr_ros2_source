#include<iostream>
#include<map>
#include<list>
#include<set>
#include<cmath>
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
  
  // There measured distance were made from the wall
  // However, there is a difference in the position of the fixed beacon, wall, and hedhog beacon.
  // Therefore, nodes will be added to provide pose correction to compare real pose and marvelmind program poses correctly.
  // Example: 
  // - 1 representes fixed beacon 1 pose.  
  // - 1.1 represents the wall 1 pose.
  // - 1.2 represents hedhog beacon 1 pose.


 


  double fixed_beacon1_to_wall_correction=-0.082 ;// y
  double fixed_beacon1_correction_to_mobile=-0.042; // y
  double wall1_to_pose5=1.992;
  double fixed_beacon1_to_pose5 = wall1_to_pose5 + fixed_beacon1_to_wall_correction;
  double fixed_beacon2_to_wall_correction=-0.023 ;// y
  double fixed_beacon2_correction_to_mobile=0.027; // y
  double fixed_beacon3_and_4_to_wall_correction=0.023 ;// y
  double fixed_beacon3_and_4_correction_to_mobile=-0.0045; // y

  //push_edge(1,1.1,0,fixed_beacon1_to_wall_correction);
  //push_edge(1,1.2,0,fixed_beacon1_correction_to_mobile);
  push_edge(1,5,0,fixed_beacon1_to_pose5);
  
  //push_edge(4,4.1,0,fixed_beacon3_and_4_to_wall_correction);
  //push_edge(4,4.2,0,fixed_beacon3_and_4_correction_to_mobile);
  double wall4_to_pose5=1.8620;
  double fixed_beacon4_to_pose5 = wall4_to_pose5 - fixed_beacon3_and_4_to_wall_correction;
  push_edge(5,4,0,fixed_beacon4_to_pose5);
 

  
  // Use pithagoras to get x distance between 1 and 2.
  // We took the laser and aligned it to pose 8, then measured the distance to the walls 1 (1.953m) and 2 (1.913m).
  double wall2_to_pose8=1.913;
  double y_distance_1_and_2= (wall2_to_pose8 - fixed_beacon2_to_wall_correction)-(1.953-fixed_beacon1_to_wall_correction) ;
  double x_distance_1_and_2 = sqrt((10.250*10.250) - (y_distance_1_and_2*y_distance_1_and_2)); // 10.250 is the hypotenuse
  push_edge(1,2,x_distance_1_and_2,y_distance_1_and_2);
  //push_edge(2,2.1,0,fixed_beacon2_to_wall_correction);
  //push_edge(2.1,2.2,0,fixed_beacon2_correction_to_mobile);
  double fixed_beacon2_to_pose8 = wall2_to_pose8 + fixed_beacon2_to_wall_correction;
  push_edge(2,8,0,fixed_beacon2_to_pose8);
  double wall3_to_pose8=1.8620;
  double fixed_beacon3_to_pose8 = wall3_to_pose8 - fixed_beacon3_and_4_to_wall_correction;

  push_edge(8,3,0,fixed_beacon3_to_pose8);
  push_edge(8,8.1,2.0000,0);
  push_edge(8.1,8.2,2.0000,0);
  push_edge(8.2,8.3,13.5380,0);
  double distance_pose_8_and_10_1_in_x=-(x_distance_1_and_2 + 1.8);
  push_edge(8,10.1,distance_pose_8_and_10_1_in_x,0);
  push_edge(10.1,10.2,distance_pose_8_and_10_1_in_x-1.8,0);
  double distance_pose_5_and_7_3_in_x= 4.87;
  double distance_pose_8_and_7_3_in_x=-(x_distance_1_and_2 - distance_pose_5_and_7_3_in_x);
  push_edge(8,7.3,distance_pose_8_and_7_3_in_x,0);
  push_edge(7.3,7,0,-0.7900);
  push_edge(7,7.2,0,-1.5250);
  push_edge(7.3,7.4,0,1.8850);
  push_edge(7.4,7.5,0,1.4995);
  push_edge(7.5,7.1,0,2.1000);
  double distance_pose_5_and_6_in_x= 5.096;
  double distance_pose_8_and_6_in_x=-(x_distance_1_and_2 - distance_pose_5_and_6_in_x);
  double distance_pose_7_and_6_in_x=(distance_pose_8_and_7_3_in_x - distance_pose_8_and_6_in_x);
  push_edge(7,6,distance_pose_7_and_6_in_x,2.0330);





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





  for (auto i:graph_a){
    cout << i.first << "=>";
    for (auto j:i.second){
       cout << "\t" << j.node << "(" << j.x <<"," << j.y << ")" << endl;
    }
    cout << endl;
  }

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