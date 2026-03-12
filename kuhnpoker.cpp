#include <iostream>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <random>
#include <vector>
using namespace std;

enum choice 
{
    pass=0,bet=1
};
enum Kuhncards 
{
    jack=0,queen=1,king=2
};

class Node 
{
    public:
    double regretsum[2];
    double strategy[2];
    double strategysum[2];
    void getstrategy(double my_prob)
    {
       if(regretsum[0]==0 && regretsum[1]==0) {
         strategy[0]=strategy[1]=0.5;
       }
       else {
         double a=(regretsum[0]>0) ? regretsum[0] : 0;
         double b=(regretsum[1]>0) ? regretsum[1] : 0;
         if(a==0 && b==0){
             strategy[0]=strategy[1]=0.5;
         }
         else {
         strategy[0]=a/(a+b);
         strategy[1]=b/(a+b);
         }
       }
       for(int i=0;i<2;i++) {
         strategysum[i]+=my_prob*strategy[i];
       }
    }
    void final_avg_strategy() {
        double normalizing_sum=strategysum[0]+strategysum[1];
        if(normalizing_sum>0){
            strategy[0]=strategysum[0]/normalizing_sum;
            strategy[1]=strategysum[1]/normalizing_sum;
        }
        else {
            strategy[0]=strategy[1]=0.5;
        }

    }
    Node () {
        regretsum[0]=regretsum[1]=0;
        strategy[0]=strategy[1]=0;
        strategysum[0]=strategysum[1]=0;
    }
};
class bot_trainer
{
    unordered_map<string,Node>node_data;
    mt19937 numgen;
    double cfr(string past_actions,vector<int>&cards,double p0,double p1 ){
        int payoff=getpayoff(past_actions,cards);
        if(past_actions.size()>=2 && payoff!=0){       //if payoff=0 then  game is still not over
            return (double)payoff;
        }
        int active_player=(past_actions.size())%2;
        string infoset=to_string(cards[active_player])+past_actions; 
        Node& node=node_data[infoset];
        double p_self=(active_player==0) ? p0 : p1;
        double p_opp=(active_player==1) ? p0 : p1;
        double rec_values[2];
        node.getstrategy(p_self);
        for(int i=0;i<2;i++) {
           double new_p0=p0;
           double new_p1=p1;
           if(active_player==0){
            new_p0*=node.strategy[i];
           }
           else {
            new_p1*=node.strategy[i];
           }
           string t=(i==0)? "p" :"b";
           rec_values[i]=-cfr(past_actions+t,cards,new_p0,new_p1);
        }
        double node_evalue=0;
        for(int i=0;i<2;i++) {
            node_evalue+=rec_values[i]*node.strategy[i];
        }
        for(int i=0;i<2;i++){
            node.regretsum[i]+=p_opp*(rec_values[i]-node_evalue);
        }
        return node_evalue;

    } 
    public:
    bot_trainer(){
      numgen.seed(random_device{}());
    }
    double trainerloop(long long iterations) {
        double total_utility=0;
        
       for(int i=0;i<iterations;i++){
         vector<int>cards={0,1,2};
         shuffle(cards.begin(),cards.end(),numgen);
         total_utility+=cfr("",cards,1,1);
         
       }
       return total_utility/(double)iterations;
    }
    int getpayoff(string past_actions,vector<int>&cards){
        int len=past_actions.size();
        if(len<2) {
            return 0;
        }
        int next_player=len%2;
        int opp_player=1-next_player;
        bool fold=(past_actions.back()=='p'&& past_actions.find("b")!=string::npos);  //check past_actions leads to "fold" or not
        bool checkcards=(past_actions=="bb"|| past_actions=="pbb"|| past_actions=="pp"); //check past_actions leads to "showdown" between players
        if(fold){
            return 1;
        }
        if(checkcards){
            int pot=(past_actions=="pp") ? 1:2;
            return cards[next_player]>cards[opp_player]?pot:-pot;
        }
        return 0;    // Not reached terminal node state yet

    }
    void print_final_strategies() {

    vector<string> keys;

    for(auto &p : node_data){
        keys.push_back(p.first);
    }

    sort(keys.begin(), keys.end());

    cout << "\nInfoset Strategies\n";
    cout << "Infoset\tPass\tBet\n";

    for(auto &k : keys){
        Node &node = node_data[k];
        node.final_avg_strategy();

        cout << k <<"\t"<< node.strategy[0] << "\t"<< node.strategy[1] << endl;
    }
   }


};

int main()
{
    bot_trainer mentor;
    long long iterations =1000000;
    double expected_value=mentor.trainerloop(iterations);
    cout<<"Average game value for 1st player "<<expected_value<<endl;
    mentor.print_final_strategies();
}