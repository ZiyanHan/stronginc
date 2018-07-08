#include <gflags/gflags.h>
#include <glog/logging.h>
#include<boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
//#include "cpp/core/graphapi.h"
#include "cpp/serial/dualsimulation.h"
#include "cpp/serial/dual_incremental.h"
#include "cpp/serial/strongsimulation.h"
#include "cpp/serial/strong_incremental.h"
#include "cpp/utils/util.h"
#include "cpp/io/io_local.h"
#include "cpp/core/global.h"
#include "cpp/core/strongr.h"
#include<iostream>
#include <fstream>
#include<ctime>
//#include <sstream>
#define random(a,b) (rand()%(b-a+1)+a)


class Serial{
public:
    std::string get_query_vfile(int index){
        return "../data/synmticquery/q"+std::to_string(index)+".v";
    }

    std::string get_query_efile(int index){
        return "../data/synmticquery/q"+std::to_string(index)+".e";
    }
public:

    void test_dualsimulation(){
        Graph dgraph;
        GraphLoader dgraph_loader,qgraph_loader;
        dgraph_loader.LoadGraph(dgraph,graph_vfile,graph_efile);
        DualSim dualsim;
        int index = 1;
        while(index<2){
            Graph qgraph;
            qgraph_loader.LoadGraph(qgraph,get_query_vfile(index),get_query_efile(index));
            std::unordered_map<VertexID, std::unordered_set<VertexID>>  sim;
            bool initialized_sim = false;
            dualsim.dual_simulation(dgraph,qgraph,sim,initialized_sim);
            for(auto u :qgraph.GetAllVerticesID()){
                cout<<sim[u].size()<<endl;
            }
            save_sim_result(qgraph,sim,"../data/dualresult"+std::to_string(index));
            index+=1;
        }
    }

    void test_dual_incremental(){
        DualInc dualinc;
        DualSim dualsim;
        Graph dgraph;
        GraphLoader dgraph_loader,qgraph_loader;
        dgraph_loader.LoadGraph(dgraph,graph_vfile,graph_efile);
        int index =1;
        while(index <200){
            Graph qgraph;
            qgraph_loader.LoadGraph(qgraph,get_query_vfile(index),get_query_efile(index));
            std::unordered_map<VertexID, std::unordered_set<VertexID>> sim;
            bool initialized_sim=false;
            dualsim.dual_simulation(dgraph,qgraph,sim,initialized_sim);
            int j=1;
            while (j<20){
            std::set<std::pair<VertexID,VertexID>> add_edges,rm_edges;
            Load_bunch_edges(add_edges,base_add_file,j);
            Load_bunch_edges(rm_edges,base_remove_file,j);
            for (auto e:add_edges){
               dgraph.AddEdge(Edge(e.first,e.second,1));
            }
            std::unordered_map<VertexID, std::unordered_set<VertexID>> incdsim,direct_sim;
            for(auto u :qgraph.GetAllVerticesID()){
                incdsim[u]=std::unordered_set<VertexID>();
                for(auto v:sim[u]){
                incdsim[u].insert(v);
                }
            }
            dualsim.dual_simulation(dgraph,qgraph,direct_sim,initialized_sim);
            for (auto e:add_edges){
               dgraph.AddEdge(Edge(e.first,e.second,1));
            }
            dualinc.incremental_addedges(dgraph,qgraph,incdsim,add_edges);
            std::cout<<index<<' '<<j<<' '<<dual_the_same(qgraph,direct_sim,incdsim)<<std::endl;
            for(auto e :rm_edges){
                dgraph.RemoveEdge(Edge(e.first,e.second,1));
            }
            direct_sim.clear();
            dualsim.dual_simulation(dgraph,qgraph,direct_sim,initialized_sim);
            dualinc.incremental_removeedgs(dgraph,qgraph,incdsim,rm_edges);
            std::cout<<index<<' '<<j<<' '<<dual_the_same(qgraph,direct_sim,incdsim)<<std::endl;
            for(auto e:rm_edges){
                dgraph.AddEdge(Edge(e.first,e.second,1));
            }
            for (auto e:add_edges){
               dgraph.RemoveEdge(Edge(e.first,e.second,1));
            }
            j+=1;
           }
           index +=1;
        }

    }

    void test_strongsimulation(){
    StrongSim strongs;
    Graph dgraph;
    GraphLoader dgraph_loader,qgraph_loader;
    dgraph_loader.LoadGraph(dgraph,graph_vfile,graph_efile);
    int index = 1;
    while (index <20){
      Graph qgraph;
      qgraph_loader.LoadGraph(qgraph,get_query_vfile(index),get_query_efile(index));
      clock_t start,end;
      start =clock();
      strongs.strong_simulation_sim(dgraph,qgraph);
      end = clock();
//      std::cout<<"strong simulation time "<<(float)(end-start)/CLOCKS_PER_SEC<<"s"<<std::endl;
      index +=1;
   }
 }
private:
    std::string graph_vfile ="../data/synmtic.v";
    std::string graph_efile ="../data/synmtic.e";
    std::string r_file = "../data/synmtic.r";
    std::string base_add_file = "../data/incsynmtic/add_e";
    std::string base_remove_file="../data/incsynmtic/rm_e";
};

int main(int argc, char *argv[]) {
  google::SetUsageMessage("Usage: test [gflags_opt]");
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::ShutDownCommandLineFlags();
  google::InitGoogleLogging("test for working");
  google::ShutdownGoogleLogging();
//  init_workers();
  Serial serial;
//  serial.test_dualsimulation();
//  serial.test_dual_incremental();
  serial.test_strongsimulation();
//  worker_finalize();
  return 0;
}

using namespace std;