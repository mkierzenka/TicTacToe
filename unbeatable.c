#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum { empty = -1, x = 1, o = 0 } spot; 
typedef enum { xwin, owin, tie, nowin } gamestate;

typedef struct nodetag node;

struct nodetag {
	spot board[9];
	spot movesNext;
	void (*mPrintNode) (const node* self);
	node* nextMoves[9];
	int score;

};



void printSpot(const node* self, int i, int j) {	
	spot curSpot = self->board[3 * i + j];
	if(curSpot == x) { printf("X"); }
	else if(curSpot == o) { printf("O"); }
	else printf(" ");
};



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


node* node_create(spot firstmove) {
	node* n = (node*) malloc(sizeof(node));
	spot* tmpB = n->board;
	node** tmpN = n->nextMoves;
	for(int i = 0; i < 9; ++i) {
		tmpB[i] = empty;
		tmpN[i] = NULL;
	}
	n->movesNext = firstmove;
	n->mPrintNode = printBoard;
	n->score = 0;
	return n;
}


//does either player win on a column? xwin, owin, nowin -> maybe tie, maybe full
gamestate col_win(const node* n) {
	const spot* b = n->board;
	for(int i = 0; i < 3; ++i) {
		if(b[i] == b[3 + i] && b[3 + i] == b[3*2 + i]) {
			if(b[i] == x) { return xwin; }
			else if(b[i] == o) { return owin; }
		}
	}
	return nowin;
}

//does either player win on a row?
gamestate row_win(const node* n) {
	const spot* b = n->board;
	for(int i = 0; i < 3; ++i) {
		if(b[3*i] == b[3*i + 1] && b[3*i + 1] == b[3*i + 2]) {
			if(b[3*i] == x) { return xwin; }
			else if(b[3*i] == o) { return owin; }
		}
	}
	return nowin;
}

//does either player win on a diagonal? or no win, if tie or no spots
gamestate diag_win(const node* n) {
	const spot* b = n->board;
	bool majorDiagWin = b[0] == b[3*1 + 1] && b[3*1 + 1] == b[3*2 + 2] && b[0] != empty;
	bool minorDiagWin = b[2] == b[3*1 + 1] && b[3*1 + 1] == b[3*2] && b[2] != empty;
	if(majorDiagWin) {
		if(b[0] == x) { return xwin; }
		else if(b[0] == o) { return owin; }
	}
	else if(minorDiagWin) {
		if(b[2] == x) { return xwin; }
		else if(b[2] == o) { return owin; }
	}
	return nowin;
}

//return true if no empty spaces left
bool no_moves(const node* n) {
	const spot* b = n->board;
	for(int i = 0; i < 9; ++i) {
		if(b[i] == empty) { return false; }
	}
	return true;
}




gamestate whowon(const node* n) {
	if(row_win(n) != nowin) { return row_win(n); }
	else if(col_win(n) != nowin) { return col_win(n); }
	else if(diag_win(n) != nowin) { return diag_win(n); }
	else if(no_moves(n)) { return tie; }
	else { return nowin; }
}	


//is the game over at this node?
bool gameover(const node* n) {
	return whowon(n) != nowin;
}


spot getNextMover(spot justMoved) {
	if(justMoved == x) { return o; }
	else { return x; }
}


//makes a new node that is prev but with the new move made (copy)
node* node_move(const node* prev, int m) {
	node* newNode = (node*) malloc(sizeof(node));
	const spot* prevBoard = prev->board;
	spot* newBoard = newNode->board;
	for(int i = 0; i < 9; ++i) {
		newBoard[i] = prevBoard[i];
	}
	newBoard[m] = prev->movesNext;
	newNode->movesNext = getNextMover(prev->movesNext);
	newNode->mPrintNode = printBoard;
	newNode->score = 0;

	node** newNextMoves = newNode->nextMoves;
	for(int i = 0; i < 9; ++i) {
		newNextMoves[i] = NULL;
	}
	return newNode;
}


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


//if a spot already occupied is chosen, no move made
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

void printScore(const node* who) {
	printf("score: %d", who->score);
}

	
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


//computer is o
node* make_tree() {
	node* first = node_create(x);
	make_tree_helper(first);
	update_scores(first);
	return first;
}


void printFinal(const node* n) {
	printBoard(n);
	gamestate result = whowon(n);
	if(result == xwin) { printf("X Wins!\n"); }
	else if(result == owin) { printf("O Wins!\n"); }
	else if(result == tie) { printf("Tie!\n"); }
}


void playGame() {
	node* game = make_tree();
	printf("Tic Tac Toe\n");
	printf("X : User\n");
	printf("O : Comp\n");
	//printBoard(game);
	int userMove;
	//game = computerResponse(game); if comp goes first, change make_tree to o
	while(game != NULL && !gameover(game)) {
		printBoard(game);
		printf("\nEnter move (0-8): ");
		scanf("%d", &userMove);
		game = make_move(game, userMove);
	}

	printFinal(game);
}


void tester() {
	node* a = make_tree();
	//printBoard(a);
	//a = make_move(a, 2);
	//a = node_create(x);
	
	//node* a = node_create(x);
	//a->mPrintNode(a);
	//node* b = make_tree();
	//a = a->nextMoves[1]->nextMoves[0]->nextMoves[2]->nextMoves[2]->nextMoves[0]->nextMoves[2]->nextMoves[2];
	//a = a->nextMoves[0]->nextMoves[0]->nextMoves[6]->nextMoves[2]->nextMoves[4];//->nextMoves[2];//->nextMoves[1];
	//a = a->nextMoves[4]->nextMoves[0]->nextMoves[0];//->nextMoves[0];
	//a = a->nextMoves[0]->nextMoves[0]->nextMoves[6]->nextMoves[2];//->nextMoves[4];//->nextMoves[3];//->nextMoves[1];
	a = a->nextMoves[1]->nextMoves[0]->nextMoves[0]->nextMoves[0]->nextMoves[4];
	printBoard(a);
	for(int i = 0; i < 9; ++i) {
		if(a->nextMoves[i] != NULL) {
			a->nextMoves[i]->mPrintNode(a->nextMoves[i]);
			printf("\n");
			printScore(a->nextMoves[i]);
			printf("\n");
		}
	}
	//printBoard(head);
	//node* b = node_move(a, o);
	//b->mPrintNode(b);
}


int main() {
	//tester();
	playGame();
	return 0;
}


