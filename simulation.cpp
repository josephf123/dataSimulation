#include <iostream>
#include <string>
#include <fstream>
#include <stdint.h>
using namespace std;

class Agent {       
    public:
        int startMoney;
        double XorNothing;
        double winRate;
        double currMoney;
        Agent(int startM, double betting, double winR) {
            startMoney = startM;
            XorNothing = betting;
            winRate = winR;
            currMoney = startM;
        }
};

bool simulateCoinFlip(Agent agent) {
    if (rand() % 100 < agent.winRate) {
        return true;
    } else {
        return false;
    }
}

double placeBet(double moneyPlaced, Agent agent) {
    // it will either return +moneyPlaced or -moneyPlaced
    if (simulateCoinFlip(agent)){
        return moneyPlaced;
    } else {
        return -moneyPlaced;
    }
}

int64_t* untilBankrupt(Agent agent){
    static int64_t r[2];
    // useful stats to know
    int64_t maxCurrMoney = agent.currMoney;
    int64_t betsPlaced = 0;
    
    // since currMoney is relative to 1
    long double betAmount = 1;
    while (agent.currMoney > 0) {
        // std::cout << agent.currMoney << "\n";
        betsPlaced += 1;
        if (betAmount > agent.currMoney) {
            // if doing double or nothing and bet is
            // greater than currMoney
            betAmount = 1;
        }

        long double currentBet = placeBet(betAmount, agent);
        agent.currMoney += currentBet;
        if (agent.currMoney > maxCurrMoney){
            maxCurrMoney = agent.currMoney;
        }
        if (currentBet < 0){
            betAmount *= agent.XorNothing;
        }
    }
    r[0] = betsPlaced;
    r[1] = maxCurrMoney;
    // std::cout << r[0] << " , " << r[1] << "\n";
    return r;
}



void simulateEvents(string stratergy, int startStartMoney, int diffStartMoney,
int startMoneyInc, int startXor, int diffXor, double xorInc, int repeats, string extra) {

    std::ofstream MyFile("data/" + to_string(startStartMoney) + "-" + to_string(diffStartMoney) + "-" +
    to_string(startMoneyInc) + "-" + to_string(startXor) + "-"  + to_string(diffXor) + "-"  + to_string(xorInc) + "-" +
    to_string(repeats) + "-" + "gambleSimulation" + stratergy + extra + ".csv");
    MyFile << "StartMoney,XorNothing,BetsPlaced,MaxMoney\n";

    int * validStartMoneyRange = (int *) malloc(sizeof(int) * diffStartMoney);
    double * xOrNothingRange = (double *) malloc(sizeof(double) * diffXor);;
    for (int i={0}; i < diffStartMoney; ++i) {
        validStartMoneyRange[i] = startMoneyInc * i + startStartMoney;
    }
    for (int i={0}; i < diffXor; ++i) {
        xOrNothingRange[i] = xorInc * i + startXor;
    }
    if (stratergy == "Standard") {
        for (int x={0}; x < diffStartMoney; ++x) {
            for (int y={0}; y < diffXor; ++y) {
                Agent myAgent(validStartMoneyRange[x],xOrNothingRange[y],48.6);
                for (int z={0}; z < repeats; z++){
                    int64_t *arr = untilBankrupt(myAgent);
                    MyFile << validStartMoneyRange[x] << "," << xOrNothingRange[y]
                    << "," << arr[0] << "," << arr[1] << "\n";
                }
                std::cout << y << "\n";
            }
        }
    }

    MyFile.close();
}

int main() {
    srand( (unsigned)time(NULL) );

    // Create and open a text file
    // int xOrNRange = {500};
    // int * xOrNothingRange = (int *) malloc(sizeof(int) * xOrNRange);;
    
    // for (int i={0}; i < xOrNRange; ++i) {
    //     xOrNothingRange[i] = i * 10 + 1;
    // }
    // for (int i = {0}; i < xOrNRange; ++i) {
    //     std::cout << xOrNothingRange[i] << "\n";
    // }
    // stratergy, start SM, num of diff SM, SM inc, start xor, num of diff xor, xor inc, repeats
    simulateEvents("Standard", 5000, 1, 1, 500, 4500, 1, 1, "");

    /*
    Way to document file name:
    Variables: 
    1 start startMoney
    2 number of different startMoney
    3 startMoney increments
    4 start xor
    5 number of different xor 
    6 xor increments
    
    7 total repeats of each

    Stratergy used:
    - Standard
    - gambleMore 

    Extras (for future):
    - probability

    STANDARD STRUCTURE
    gambleSimulationStandard-1-2-3-4-5-6-7
    E.g if start startMoney is 5000, number of different startMoney 1,
    increments 1, start xor is 2, number of different xor 200, xor increments 1,
    total repeats of each 500 and if statergy is standard
    
    The file would be called
    gambleSimulationStandard_5000_1_1_2_200_1_500

    */


    // Close the file
    

   return 0;
}


