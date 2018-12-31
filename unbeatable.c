#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


// Defines a spot on the board
typedef enum { empty = -1, x = 1, o = 0 } spot; 

// Defines all possible game(board) states
typedef enum { xwin, owin, tie, nowin } gamestate;

// Defines a node, used to make a tree of all possible board states
typedef struct nodetag node;

struct nodetag {
	spot board[9];
	spot movesNext;
	node* nextMoves[9];
	int score;

};


// Prints a spot on the board, row-col 0-indexed
// eg. (0, 1) is top middle spot
void printSpot(const node* self, int i, int j) {	
	spot curSpot = self->board[3 * i + j];
	if(curSpot == x) { printf("X"); }
	else if(curSpot == o) { printf("O"); }
	else printf(" ");
};

//Prints the board at this node
void printBoard (const node* self) {
	printf("\n");
	for(int i = 0; i < 3; ++i) {
		for(int j = 0; j < 3; ++j) {
			printSpot(self, i, j);
			printf("|");
		}
		printf("\n------\n");
	}
	printf("\n");
};

// Create (allocate) a new node with given next move
// ie. creates the head of the tree
node* node_create(spot firstmove) {
	node* n = (node*) malloc(sizeof(node));
	spot* tmpB = n->board;
	node** tmpN = n->nextMoves;
	for(int i = 0; i < 9; ++i) {
		tmpB[i] = empty;
		tmpN[i] = NULL;
	}
	n->movesNext = firstmove;
	n->score = 0;
	return n;
}

// Return which player what wins on ith column.
// If no clear winner, return nowin (note, could be a tie board)
gamestate ith_col_win(const node* n, int i) {
	const spot* b = n->board;
	if(b[i] == b[3 + i] && b[3 + i] == b[3*2 + i]) {
		if(b[i] == x) { return xwin; }
		else if(b[i] == o) { return owin; }
	}
	return nowin;
}

// Return which player wins on any column, if none then nowin.
gamestate col_win(const node* n) {
	for(int i = 0; i < 3; ++i) {
		gamestate res = ith_col_win(n, i);
		if(res != nowin) { return res; }
	}
	return nowin;
}

// Return which player wins on ith row, if no winner return nowin.
gamestate ith_row_win(const node* n, int i) {
	const spot* b = n->board;
	if(b[3*i] == b[3*i + 1] && b[3*i + 1] == b[3*i + 2]) {
		if(b[3*i] == x) { return xwin; }
		else if(b[3*i] == o) { return owin; }
	}
	return nowin;
}

// Return which player wins on a row, else nowin.
gamestate row_win(const node* n) {
	const spot* b = n->board;
	for(int i = 0; i < 3; ++i) {
		gamestate res = ith_row_win(n, i);
		if(res != nowin) { return res; }
	}
	return nowin;
}

// Return which player wins on a diagonal, if none then nowin.
gamestate diag_win(const node* n) {
	const spot* b = n->board;
	bool majorDiagWin = b[0] == b[4] && b[4] == b[8] && b[0] != empty;
		if(majorDiagWin) {
		if(b[0] == x) { return xwin; }
		else if(b[0] == o) { return owin; }
	}
	
	bool minorDiagWin = b[2] == b[4] && b[4] == b[6] && b[2] != empty;
	if(minorDiagWin) {
		if(b[2] == x) { return xwin; }
		else if(b[2] == o) { return owin; }
	}
	return nowin;
}

// Are there no moves left? ie. true if no empty spots on board
bool no_moves(const node* n) {
	const spot* b = n->board;
	for(int i = 0; i < 9; ++i) {
		if(b[i] == empty) { return false; }
	}
	return true;
}

// Return the game result-> xwin, owin, tie, or nowin(meaning game not over)
gamestate whowon(const node* n) {
	if(row_win(n) != nowin) { return row_win(n); }
	else if(col_win(n) != nowin) { return col_win(n); }
	else if(diag_win(n) != nowin) { return diag_win(n); }
	else if(no_moves(n)) { return tie; }
	else { return nowin; }
}	


// Is the game over at this node?
bool gameover(const node* n) {
	return whowon(n) != nowin;
}


// Given who just moved, get who should move next. (x -> o and o/empty -> x)
spot getNextMover(spot justMoved) {
	if(justMoved == x) { return o; }
	else { return x; }
}


// Makes/allocates a new node that is like the previous but with the new move made.
// m is 0 - 8 for which spot to play the piece
node* node_move(const node* prev, int m) {
	node* newNode = (node*) malloc(sizeof(node));
	const spot* prevBoard = prev->board;
	spot* newBoard = newNode->board;
	for(int i = 0; i < 9; ++i) {
		newBoard[i] = prevBoard[i];
	}
	newBoard[m] = prev->movesNext;
	newNode->movesNext = getNextMover(prev->movesNext);
	newNode->score = 0;

	node** newNextMoves = newNode->nextMoves;
	for(int i = 0; i < 9; ++i) {
		newNextMoves[i] = NULL;
	}
	return newNode;
}


// Return the computer's response to the given state
node* computerResponse(const node* start) {
	int bestIdx = 0;
	float bestScore = -100;
	for(int i = 0; i < 9; ++i) {
		node* candidate = start->nextMoves[i];
		if(candidate != NULL && candidate->score > bestScore) {
			bestIdx = i;
		 	bestScore = candidate->score;
		}
	}
	return start->nextMoves[bestIdx];
}


// Makes the given move (ie. places a piece at position m).
// Then, if the game isn't over the computer makes a move. Outputs final board state.
// If the position m is already taken, return prev.
node* make_move(node* prev, int m) {
	if(prev->board[m] != empty) { return prev; }
	node* afterUserMoveM = NULL;
	node** nextMoves = prev->nextMoves;
	for(int i = 0; i < 9; ++i) {
		if(nextMoves[i] != NULL && nextMoves[i]->board[m] != empty) {
			afterUserMoveM = nextMoves[i];
			break;
		}
	}

	//if the user just won, computer cannot win
	if(gameover(afterUserMoveM)) { return afterUserMoveM; }
	return computerResponse(afterUserMoveM);
}


// Helper function to continue making a tree, starting from the given node.
// Note, this is via mutation of prev. If the game is over, no further moves made.
void make_tree_helper(node* prev) {
	if(gameover(prev)) { 
		return;
	}
	spot* b = prev->board;
	node** nextMoves = prev->nextMoves;
	int cntr = 0;
	for(int i = 0; i < 9; ++i) {
		if(b[i] == empty) {
			node* nextN = node_move(prev, i);
			make_tree_helper(nextN);
			nextMoves[cntr] = nextN;
			++cntr;
		}
	}
}

// Print the score associated with node who. For testing/debugging.
void printScore(const node* who) {
	printf("score: %d", who->score);
}


// Update the scores of nodes starting from n and going down the tree.
void update_scores(node* n) {
	gamestate res = whowon(n);
	if(res == xwin) { n->score = -1; }
	else if(res == owin) { n->score = 10; }
	else if(res == tie) { n->score = 0; }
	else {
		spot mN = n->movesNext;
		int nextScore = 0;
		int maxScore = -100;
		int minScore = 100;
		node** nextMoves = n->nextMoves;
		for(int i = 0; i < 9; ++i) {
			if(nextMoves[i] != NULL) {
				update_scores(nextMoves[i]);
				if(mN == o && nextMoves[i]->score > maxScore) {
					maxScore = nextMoves[i]->score;
				} else if(mN == x && nextMoves[i]->score < minScore) {
					minScore = nextMoves[i]->score;}
				
			}
		}
		
		if(mN == o) {
			nextScore = maxScore; 
			if(maxScore > 1) { nextScore = maxScore - 1; }
		}
		else if(mN == x) { nextScore = minScore; }
		n->score = nextScore;
	}
}


// Makes a tic tac toe move tree. First person to move is player firstMove.
node* make_tree(spot firstMove) {
	node* first = node_create(firstMove);
	make_tree_helper(first);
	update_scores(first);
	return first;
}


// Prints a board and the result (winner or tie)
void printFinal(const node* n) {
	printBoard(n);
	gamestate result = whowon(n);
	if(result == xwin) { printf("X Wins!\n"); }
	else if(result == owin) { printf("O Wins!\n"); }
	else if(result == tie) { printf("Tie!\n"); }
}


// Plays the tic tac toe game.
void playGame() {
	node* game = make_tree(x);
	printf("Tic Tac Toe\n");
	printf("X : User\n");
	printf("O : Comp\n");
	//printBoard(game);
	int userMove;
	//game = computerResponse(game); if comp goes first, change make_tree to o
	while(game != NULL && !gameover(game)) {
		printBoard(game);
		printf("\nEnter move (1-9): ");
		scanf("%d", &userMove);
		game = make_move(game, userMove - 1); //real board is 0-indexed
	}

	printFinal(game);
}


void tester() {
	node* a = make_tree(x);
	//printBoard(a);
	//a = make_move(a, 2);
	//a = node_create(x);
	
	//node* a = node_create(x);
	//a = a->nextMoves[1]->nextMoves[0]->nextMoves[2]->nextMoves[2]->nextMoves[0]->nextMoves[2]->nextMoves[2];
	//a = a->nextMoves[0]->nextMoves[0]->nextMoves[6]->nextMoves[2]->nextMoves[4];//->nextMoves[2];//->nextMoves[1];
	//a = a->nextMoves[4]->nextMoves[0]->nextMoves[0];//->nextMoves[0];
	//a = a->nextMoves[0]->nextMoves[0]->nextMoves[6]->nextMoves[2];//->nextMoves[4];//->nextMoves[3];//->nextMoves[1];
	a = a->nextMoves[1]->nextMoves[0]->nextMoves[0]->nextMoves[0]->nextMoves[4];
	printBoard(a);
	for(int i = 0; i < 9; ++i) {
		if(a->nextMoves[i] != NULL) {
			printBoard(a->nextMoves[i]);
			printf("\n");
			printScore(a->nextMoves[i]);
			printf("\n");
		}
	}
}


int main() {
	//tester();
	playGame(x);
	return 0;
}


