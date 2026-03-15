#  Kuhn_Poker Solver :Counterfactual Regret Minimization (CFR)
## Project Overview
An optimized C++ implementation of the CFR algorithm to solve Kuhn Poker. This project demonstrates the application of iterative regret matching to reach a Nash Equilibrium in a zero-sum, imperfect-information game.  It implements Regret Matching, an iterative approach that allows the algorithm to converge toward a Nash Equilibrium. By simulating millions of game states, the solver minimizes "counterfactual regret"—the difference between what the agent could have earned and what it actually earned—to derive an unexploitable strategy.
## Game : Kuhn Poker 
Kuhn Poker is a zero-sum game played with a three-card deck (Jack, Queen, King)
**Deck** :Three cards (0, 1, 2). Each player is dealt one.
**Actions**: Players can Pass (fold) or Bet(call).
**Imperfect Information Game** : Player only knows their own card and the betting history ,doesn't know the opponents card. 
**Objective**: Converge to a Nash Equilibrium where neither player can improve their expected utility by changing their strategy.
## Counterfactual Regret Minimization (CFR) Algorithm 
In a game of imperfect information, we cannot simply look at the "best move", we must look at how our choices across an entire Information Set minimize our vulnerability to an opponent.The algorithm works as a recursive game tree traversal which calculates the expected payoff for player i at that node, assuming the player tried their best to reach that node.At a decision Node the Infoset Contains 'present card' +'betting history' .

The cfr method is a recursive implementation of a Depth-First Search (DFS) through the game tree. It is responsible for "training" the bot by simulating self-play.
**Terminal Payoff Calculation**: When the betting sequence ends, it calls getpayoff to determine the utility based on whether the game ended in a fold or a showdown.
**Probability Weighting**: The function tracks reach probabilities (p0 and p1). This ensures that the regret updates are properly weighted by the likelihood of reaching a specific node given the players' current strategies.
As the recursion unwinds, the "Expected Value" (EV) of each action is passed back up the tree. This allows parent nodes to calculate their own regret by comparing specific action values against the node's average value.

**Regret[Action]**=Expected Utility of an action-average utility of the node

This " regret" is added to the regretsum. If an action would have performed better than the average, its regret increases, making the algorithm to choose it more often in the future.the Average Strategy calculated by summing all strategies used across all the iterations is guaranteed to converge to a Nash Equilibrium. This is why the code maintains a _strategysum_ array: it captures the stable, long-term equilibrium that survives the fluctuations of the training process.

# Algorithm Implementation
Node Class represents the **Information set** used to store the data of a node
**regretsum** array is used to store the cummulative regret of both the actions pass and bet.
**strategy** array is used to store the current strategy for an action which is obtained by using regretsum stored for that action.
In Kuhn Poker, players make decisions based on "Information Sets"—everything they can see (their card + betting history). My code converts this into a unique string key.The cfr function performs millions of lookups; using a hash map ensures the simulation remains high-speed regardless of how many game states are discovered.The final_avg_strategy() method is a crucial  step. It ensures that the final output is the average strategy over time, which is mathematically guaranteed to reach Nash Equilibrium, unlike the "current" strategy which can be fluctuating.

To simulate the deck, I use std::shuffle, which implements the Fisher-Yates shuffle algorithm.It ensures that every one of the  6 possible card permutations is equally likely.Each round’s deal is independent of the previous one, simulating a true "fresh deck" environment.I used the trainer loop for a million iterations so that the algorithm converges to Nah Equilibrium. By the end of the loop, the results converge toward the theoretical value of Kuhn Poker (-1/18), proving the mathematical correctness of the implementation.The bot_trainer iterates through all discovered information sets. It calls final_avg_strategy() on each node to normalize the accumulated strategysum. This produces the final Nash Equilibrium output, which defines the optimal probability for every possible action in the game.





