#include <iostream>
#include <string>
#include <fstream>
#include <stdint.h>
using namespace std;

class Agent {       
    public:
        int startMoney;     
        int XorNothing;      // Could be double or nothing, triple or nothing etc. This will be used as the bet progression.
                                // If equal to zero, fibonacci, if equal to 1, single bets, if equal to startMoney, all in.
        double winRate;
        long int currMoney;
        long int maxMoney[2];        // [Amount of bets needed to get to maxMoney, the maxMoney value]
        long int betsPlaced;         // Total amount of bets placed before termination
        long int prevBetAmount;   // This is only for fibonacci sequence as prevBetAmount is needed
        long int currBetAmount;  // Starts off as 1, then depending on xOrNothing, will progress
        string whenToStop;
        long int knownMaxMoney;
        long int knownMaxMoneyBets;
        bool hasAchievedGoal;



        bool simulateCoinFlip();

        double placeBet(long int betAmount);

        void agentGamble();

        void agentGambleScenario();

        Agent(int startM, int betting, double winR, string stoppingTime, long int kMM, long int kMMB) {
            startMoney = startM;
            XorNothing = betting;
            winRate = winR;
            prevBetAmount = 1;
            currBetAmount = 1;
            whenToStop = stoppingTime;
            currMoney = startM;
            knownMaxMoney = kMM;
            knownMaxMoneyBets = kMMB;
            hasAchievedGoal = true;
        }
        void resetVals(){
            currMoney = startMoney;
            maxMoney[0] = 0;
            maxMoney[1] = 0;
            prevBetAmount = 1;
            currBetAmount = 1;
            hasAchievedGoal = true;
            betsPlaced = 0;
        }
};


bool Agent::simulateCoinFlip() {
    if (rand() % 100 < winRate) {
        return true;
    } else {
        return false;
    }
}

double Agent::placeBet(long int betAmount) {
    ++betsPlaced;
    long int winLossAmount = betAmount;
    // it will either return +moneyPlaced or -moneyPlaced
    if (!simulateCoinFlip()){
        // This means that they lost the bet, so the 
        winLossAmount *= -1;
    }
    currMoney += winLossAmount;
    return winLossAmount;
}

void Agent::agentGamble() {

    if (betsPlaced % 100000000 == 0 && betsPlaced != 0) {
        printf("have placed %lu bets with %lu\n", betsPlaced, currMoney);
    }

    if (currBetAmount > currMoney) {
        // For fibonacci                
        if (XorNothing == 0) {
            long int tmp = currBetAmount;
            // This is the formula for 2 places back on the fibonacci sequence
            currBetAmount = currBetAmount - prevBetAmount;
            // This is the formula for 3 places back on the fibonacci sequence
            prevBetAmount = 2 * prevBetAmount - tmp;
        } else {
            // if doing double or nothing and bet is
            // greater than currMoney
            currBetAmount = 1;
        }

    }
    long int winLossAmount;
    // This means that the agent will gamble all their money every time
    if (XorNothing == startMoney) {
        winLossAmount = placeBet(currMoney);
    } else {
        winLossAmount = placeBet(currBetAmount);
    }


    // printf("%f currMon\n", currMoney);
    if (currMoney > maxMoney[1]) {
        maxMoney[0] = betsPlaced;
        maxMoney[1] = currMoney;
    }

    if (winLossAmount < 0) {
        // If the agent lost the bet, do something depending on xor
        if (XorNothing == 0) {
            // This means fibonacci
            long int tmp = prevBetAmount;
            prevBetAmount = currBetAmount;
            currBetAmount += tmp;
        } else {
            currBetAmount *= XorNothing;
        }

    }
}


void Agent::agentGambleScenario() {

    // When the agent loses all their money
    if (whenToStop == "bankrupt") {
        // Keep betting until the agent has no money left
        while (currMoney > 0) {
            // This means that the updates of the gamble
            // will be set in the agent (change in money etc.)
            agentGamble();

        }

    }
    // When the agent reaches 80% of what the average maxMoney
    else if (whenToStop == "80MaxMoney") {
        if (knownMaxMoney == 0) {
            // error message
            printf("There is no known maxMoney\n");
            exit(1);
        }
        // Either the agent stops when they have 80% of the known maxMoney
        // or they run out of money
        while (currMoney <= 0.8 * knownMaxMoney && currMoney != 0) {
            agentGamble();
        }
        if (currMoney == 0) {
            hasAchievedGoal = false;
        }

    }
    // When the agent reaches the amount of bets to get to maxMoney
    else if (whenToStop == "avgMaxMoneyBets") {
        if (knownMaxMoneyBets == 0) {
            // error message
            printf("There is no known maxMoney bet\n");
            exit(1);
        }
        // Either the agent stops when the amount of bets placed is the same as the 
        // amount of bets placed needed to reach the known maxMoney amount of bets 
        // or they run out of money
        while (betsPlaced < knownMaxMoneyBets && currMoney != 0) {
            agentGamble();
        }
        if (currMoney == 0) {
            hasAchievedGoal = false;
        }

    }
    // When the agent doubles their money
    else if (whenToStop == "doubled") {

        // Either the agent stops when their current money is double what they started with 
        // or they run out of money
        while (currMoney < 2 * startMoney && currMoney != 0) {
            agentGamble();

        }
        if (currMoney == 0) {
            hasAchievedGoal = false;
        }

    }
}

int * create_arr_from_file(string fileName){
    FILE *myFile;
    const char *nameOfFile = fileName.c_str();
    myFile = fopen(nameOfFile, "r");
    static int arr[49];
    for (int i=0; i<49; i++) {
        fscanf(myFile, "%d\n", &arr[i]);
    }
    for (int i=0; i<49; i++) {
        printf("%d\n", arr[i]);
    }
    return arr;

}

void withMaxMoney(int *knownMaxMoneyArr) {
    int count = 0;
    ofstream MyFile("./random/knownMaxMoneyMedian80_2to50.csv");

    MyFile << "XOR,80% of maxMoney,passOrFail\n";
    for (int i=2; i < 51; i++) {
        Agent myAgent(5000,i,48.64, "80MaxMoney" ,knownMaxMoneyArr[i-2],0);
        while (count < 200000) {
            double eightMaxMon = 0.8 * knownMaxMoneyArr[i-2];
            myAgent.agentGambleScenario();
            MyFile << myAgent.XorNothing << "," << eightMaxMon
             << "," << myAgent.hasAchievedGoal << "\n";
            count++;
            myAgent.resetVals();
        }
        printf("finished %d, %d\n", i, myAgent.XorNothing);
        count = 0;
    }
    MyFile.close();
}

void findKnownMaxMoney(){
    int count = 0;
    ofstream MyFile("./random/maxMoneyXOR2to50NONOO.csv");

    MyFile << "max money\n";
    int maxMoney = 0;
    int whereMaxMoney = 0;
    for (int i=2; i < 51; i++) {
        Agent myAgent(5000,i,48.64, "bankrupt" ,0,0);
        while (count < 2000000) {
            myAgent.agentGambleScenario();
            MyFile << myAgent.maxMoney[1] << "\n";
            count++;
            myAgent.resetVals();
        }
        printf("finished %d, %d\n", i, myAgent.XorNothing);
        count = 0;
    }
    MyFile.close();
}

void test() {
    ofstream MyFile("data2/fibonacci_test_bankrupt.csv");
    MyFile << "StartMoney,XorNothing,BetsPlaced,MaxMoney\n";

    int count = 0;
    while (count < 10000) {
        Agent myAgent(5000,0,48.64, "bankrupt" ,0,0);
        myAgent.agentGambleScenario();
        MyFile << myAgent.startMoney << "," << myAgent.XorNothing << ","
        << myAgent.betsPlaced << "," << myAgent.maxMoney;
        count++;

    }
    MyFile.close();
}

// Testing
int main (void) {
    srand( (unsigned)time(NULL) );
    int *fileArr;
    fileArr = create_arr_from_file("average_medianp_2to50");
    for (int i=0; i < 49; i++) {
        printf("%d\n", fileArr[i]);
    }
    withMaxMoney(fileArr);
    // findKnownMaxMoney();
    return 0;
}

// Variable input
    // 1. startM. start money
    // 2. betting. XorNothing (if 0, it will be fibonacci)
    // 3. winR. the win rate
    // 4. stoppingTime. When the agent should stop betting. 
    // It can either be when the money is doubled, when there is no money, when they have made 80% of kMM,
    // when the amount of bets is the kMMB
    // 5. kMM. Known max money. (the max money that that the agent makes previously)
    // 6. kMMB. Known max money bets (how many bets it takes to get to the max money previously)