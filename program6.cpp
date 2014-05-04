/*
 *  prog6.cpp
 *     Text-based version of the game 1024, a variant of 2048 available online at:
 *        http://gabrielecirulli.github.io/2048/
 *     Object of game is to combine number tiles with the same value, accumulating
 *     points as the game progresses, to try and create the tile with the value 1024.
 *     Each move consists of sliding pieces to the left, up, right or down.
 *
 *  Class: Program #6 for CS 141, Spring 2014.
 *  System: Windows 7
 *  IDE: Cod::Blocks 13.12
 *  Author: Gabriel A. Mendoza
 *
 * 	This code uses Professor Reeds displayIDInfo and displayBoard functions. everything else
 *  is my own code. also i used some of his comments to separate my sliding functions
*/

#include <iostream>  // input / output
#include <iomanip>   // used for setting output field size using setw
#include <cstdlib>	 // standard c library
#include <time.h>	 // for seeding random values w/ time( ) funciton
using namespace std;

#define GAME_W0N 1
#define GAME_LOST 2
#define GAME_NOT_OVER 0

#define MOVE_IS_NOT_VALID false
#define MOVE_IS_VALID true

struct Node {
	int size;				// board size
	long long moveCount; 	// current number of moves
	long long score; 		// current total score
	int maxTileValue;		// value of winning tile
	bool validMoves[4] ;		// 0=A, 1=W, 2=S. 3=D
	int* board;   			// current board w/o random char
	Node* pPrev; 			// points to the previous Node
};




/*************************************************************************************
*                        USER INPUT FUNCTIONS
*************************************************************************************/
void manualInjection(Node* &currentNode){        // manually input characters into board
	int index = -1, value = -1;
	cin >> index >> value;
	currentNode->board[index] = value;
}
int  getSize(){
	cout << "\nEnter the board size you want, between 4 and 12: ";
	int x;
	cin >> x;
	if( x < 4 || x > 12){
		cout << "Invalid board size. Try again.\n";
		x = getSize();
	}
	return x;
}
void getUserInput( Node* currentNode, char &moveChar){
	cout << "\n" << currentNode->moveCount << ". Your move: ";
	cin >> moveChar;

	moveChar = toupper(moveChar);			// make uppercase

	// check for valid moves
	if( moveChar == 'A' || moveChar == 'W' || moveChar == 'S' ||
		moveChar == 'D' || moveChar == 'P' || moveChar == 'X' ||
		moveChar == 'R' || moveChar == 'U' /*|| moveChar == 'I'*/) {
		// do nothing. everything is already taken care of
	}
	// if not valid, call function recursively to prompt for new valid input
	else {
		cout << "Input is not valid. Please try again.\n";
		getUserInput(currentNode, moveChar);
	}
}
/*************************************************************************************
*                        END OF USER INPUT FUNCTIONS
*************************************************************************************/




/*************************************************************************************
*                       RANDOM NUMBERS, COMPUTATIONS, AND DYNAMIC ALLOCATIONS
*************************************************************************************/
void setBoard( Node* &node) {
	node->board = new int[node->size * node->size];   // create new 1d int array
	if( node->board == NULL ) {								// if failed try again
		setBoard( node);
	}
	for( int index =0; index < node->size * node->size; index ++){
		node->board[index] = 0;
	}
}
void copyNodeBoard( Node* &originalNode, Node* &nodeCopy) {
	for( int index =0; index < originalNode->size * originalNode->size; index ++){
		nodeCopy->board[index] =  originalNode->board[index];
	}
}
void copyNode( Node* originalNode, Node* &nodeCopy){
	nodeCopy->size = originalNode->size;             // set board size
	setBoard(nodeCopy); 							// initializes nodeCopy->board to all zeroes
	copyNodeBoard(originalNode, nodeCopy); 			// copies the current gameBoard
	nodeCopy->moveCount = originalNode->moveCount ;	// copy moveCount
	nodeCopy->score =   originalNode->score;      	// copy score
	nodeCopy->maxTileValue = originalNode->maxTileValue;
}
void determineMaxTileValue(Node* &node){
	node->maxTileValue = 1024;
	for( int i = 4; i < node->size; i++) {
       node->maxTileValue += node->maxTileValue;
	}
}
void randNumOnBoard( Node* node){
	int randomTile = rand() % (node->size * node->size);
	int randNum =-1;

	//	these if-else statements generates a random 2 or 4
	int x = rand()%100 ;
	if ( x >= 0 &&  x < 75 ){
		randNum = 2;
	}
	else if ( x >= 75 && x < 100 ){
		randNum = 4;
	}

	// if position is not a empty, call function again recursively
	if ( node->board[randomTile] != 0 ) {
		randNumOnBoard( node);
	}

	// else if position is empty, assign randNum to that position
	else {
		node->board[randomTile] =  randNum;
	}
}
/*************************************************************************************
*                       END OF RANDOM NUMBERS, COMPUTATIONS, AND DYNAMIC ALLOCATIONS
*************************************************************************************/




/*************************************************************************************
*                     	 SLIDING FUNCTIONS
*************************************************************************************/
//--------------------------------------------------------------------
// Slide all tiles UP, combining matching values, updating the score
int  mergeUp          ( Node* &node, int col){
	int greatestTileChange = -1;
	int row = 0;
	int sum = 0;
	while(row < node->size) {
		if( node->board[node->size*row + col] == node->board[node->size*(row+1) + col]) {
			sum = node->board[node->size*row + col] * 2;
			node->board[node->size*row + col] = sum;
			node->board[node->size*(row+1) + col] = 0;
			row += 2;
			node->score = node->score + sum;
			if( sum > greatestTileChange) {
				greatestTileChange = sum;
			}
		} else row++;

	}
	return greatestTileChange;
}
int  findNextTileUp   ( Node* &node, int row, int col){
	// -1 means the rest of the column is empty
	int index = -1;
	for(; row < node->size; row++){
		if( node->board[node->size*row + col] != 0 ){
			index = row;
			break;
		}
	}
	return index;
}
void slideUp          ( Node* &node, int col){
	int nextRowIndex = -1;
	for(int row = 0; row < node->size; row++){
		if( node->board[node->size*row + col] == 0 ){
			// if rest of column is empty exit for loop
			nextRowIndex = findNextTileUp(node, row + 1, col);
			if( nextRowIndex == -1 ){
				break;
			}
			node->board[node->size*row + col] = node->board[node->size*nextRowIndex + col];
			node->board[node->size*nextRowIndex + col] = 0;
		}
	}
}
int  moveUp           ( Node* node){
	int greatestTileChange = -1;
	for(int col = 0; col < node->size; col++){
		slideUp(node ,col);
		greatestTileChange = mergeUp(node, col);
		slideUp(node, col);
	}
	return greatestTileChange;
}

//--------------------------------------------------------------------
// Slide all tiles DOWN, combining matching values, updating the score
int  mergeDown         ( Node* &node, int col) {
	int greatestTileChange = -1;
	int row = node->size-1;
	long sum = 0;
	while(row >= 0) {
		if( node->board[node->size*row + col] == node->board[node->size*(row-1) + col]) {
			sum = node->board[node->size*row + col] * 2;
			node->board[node->size*row + col] = sum;
			node->board[node->size*(row-1) + col] = 0;
			row-=2;
			node->score = node->score + sum;
			if( sum > greatestTileChange) {
				greatestTileChange = sum;
			}
		} else row--;

	}
	return greatestTileChange;
}
int  findNextTileDown  ( Node* &node, int row, int col){
	// -1 means the rest of the column is empty
	int index = -1;
	for(; row >= 0; row--){
		if( node->board[node->size*row + col] != 0 ){
			index = row;
			break;
		}
	}
	return index;
}
void slideDown         ( Node* &node, int col){
	for(int row = node->size-1; row >= 0; row--){
		if( node->board[node->size*row + col] == 0 ){
			// if rest of column is empty exit for loop
			int nextRowIndex = findNextTileDown(node, row-1, col);
			if( nextRowIndex == -1 )
				break;
			node->board[node->size*row + col] = node->board[node->size*nextRowIndex + col];
			node->board[node->size*nextRowIndex + col] = 0;
		}
	}
}
int  moveDown          ( Node* &node){
	int greatestTileChange = -1;
	for(int col = 0; col < node->size; col++){
		slideDown(node, col);
		greatestTileChange = mergeDown(node, col);
		slideDown(node, col);
	}
	return greatestTileChange;
}

//--------------------------------------------------------------------
// Slide all tiles LEFT, combining matching values, updating the score
int  mergeLeft         ( Node* &currentNode, int row){
	int greatestTileChange = -1;
	int col = 0;
	int sum = 0;
	while(col < currentNode->size) {
		if( currentNode->board[currentNode->size*row + col] == currentNode->board[currentNode->size*row + col +1]) {
			sum = currentNode->board[currentNode->size*row + col] * 2;
			currentNode->board[currentNode->size*row + col] = sum;
			currentNode->board[currentNode->size*row + col+1] = 0;
			col+=2;
			currentNode->score = currentNode->score + sum;
			if( sum > greatestTileChange) {
				greatestTileChange = sum;
			}
		}
		else col++;
	}
	return greatestTileChange;
}
int  findNextTileLeft  ( Node* &node, int row, int col){
	int index = -1;
	for(; col < node->size ; col++)
		if( node->board[node->size*row + col] != 0 ){
			index = col;
			break;
		}
	return index;
}
void slideLeft         ( Node* &node, int row ){
	for(int col=0; col <= (node->size-1); col++){
		if( node->board[node->size*row + col] == 0){
			int nextColIndex = findNextTileLeft(node, row, col+1) ;
			if( nextColIndex == -1 ) {
				break;
			}
			else {
				node->board[node->size*row + col] = node->board[node->size*row + nextColIndex];
				node->board[node->size*row + nextColIndex] = 0;
			}
		}
	}
}
int  moveLeft          ( Node* &node){
	int greatestTileChange = -1;
	for(int row = 0; row< node->size; row++){
		slideLeft( node, row);
		greatestTileChange = mergeLeft( node, row );
		slideLeft( node, row);
	}
	return greatestTileChange;
}

//--------------------------------------------------------------------
// Slide all tiles RIGHT, combining matching values, updating the score
int  mergeRight        ( Node* &node, int row){
	int greatestTileChange = -1;
	int col = node->size;
	int sum = 0;
	while( col > 0) {
		if( node->board[node->size*row + col] == node->board[node->size*row + col -1]) {
			sum = node->board[node->size*row + col] * 2;
			node->board[node->size*row + col] = sum;
			node->board[node->size*row + col-1] = 0;
			node->score = node->score + sum;      // update score
			col -=2;
			if( sum > greatestTileChange) {
				greatestTileChange = sum;
			}
		}
		else col--;

	}
	return greatestTileChange;
}
int  findNextTileRight ( Node* &node, int row, int col){
	int index = -1;
	for(; col >=0 ; col--){
		if( node->board[node->size*row + col] != 0 ){
			index = col;
			break;
		}
	} return index;
}
void slideRight        ( Node* &node, int row ){
	for(int col= node->size-1; col>0; col--){
		if(node->board[node->size*row + col] == 0) {
			int nextColIndex = findNextTileRight(node, row, col-1) ;
			if( nextColIndex == -1 ){
				break;
			} else {
				node->board[node->size*row + col] = node->board[node->size*row + nextColIndex ];
				node->board[node->size*row + nextColIndex] = 0;
			}
		}
	}
}
int  moveRight         ( Node* &node){
	int greatestTileChange = -1;
	for(int row = 0; row< node->size; row++){
		slideRight( node, row);
		greatestTileChange = mergeRight( node, row);
		slideRight( node, row);
	}
	return greatestTileChange;
}
/*************************************************************************************
*                       END OF SLIDING FUNCTIONS
*************************************************************************************/




/*************************************************************************************
*                        CHECKING FOR VALID MOVES AND GAMEOVER/GAMEWIN
*************************************************************************************/
bool areNodesTheSame   (Node* currentNode,Node* nodeCopy ){
	int bSize = currentNode->size;
	for(int i=0; i < bSize*bSize; i++){
		if( currentNode->board[i] != nodeCopy->board[i] ){ // if 1 not equal tile
			return false;
		}
	}
	return true;
}
bool checkIfGameIsWon  ( Node* currentNode) {
	for(int i=0; i < currentNode->size * currentNode->size; i++){
		if( currentNode->board[i] == currentNode->maxTileValue) {
			return true;
		}
	}
	return false;
}
bool checkIfGameIsLost ( Node* currentNode){
	if( currentNode->validMoves[0] == MOVE_IS_NOT_VALID &&
		currentNode->validMoves[1] == MOVE_IS_NOT_VALID &&
		currentNode->validMoves[2] == MOVE_IS_NOT_VALID &&
		currentNode->validMoves[3] == MOVE_IS_NOT_VALID) {
		return true;
	}
	return false;
}
int  checkGameEnd      ( Node* currentNode ){
	if( checkIfGameIsWon( currentNode ) ){            // check if game is won
		return GAME_W0N;          // =1
	} else if ( checkIfGameIsLost( currentNode ) ){  // else, check if game is lost
		return GAME_LOST;  	 	  // =2
	} else {									  // if neither, continue
		return GAME_NOT_OVER;     // =0
	}
}
void possibleMoves     ( Node* &currentNode){
	for( int i=0; i < 4; i++) {
		Node* tempNode = new Node;
		copyNode( currentNode, tempNode);
		switch(i){
			case 0: moveLeft (tempNode); break;
			case 1: moveUp   (tempNode); break;
			case 2: moveDown (tempNode); break;
			case 3:	moveRight(tempNode); break;
		}

		if( areNodesTheSame( currentNode, tempNode) ) {    // move is NOT valid
			currentNode->validMoves[i] = MOVE_IS_NOT_VALID;
		} else {	       								   // move IS valid
			currentNode->validMoves[i] = MOVE_IS_VALID;
		}

		delete [] tempNode->board;				   // free memory for board
		delete tempNode;						   // and node
	}
}
/********************************************************************************d*****
*                        END OF CHECKING FOR VALID MOVES AND GAMEOVER/GAMEWIN
*************************************************************************************/




/*************************************************************************************
*                        LINKED LIST FUNCTIONS
*************************************************************************************/
void deleteNode       ( Node* &currentNode){
	Node* pTemp = currentNode->pPrev;		// point to currentnode
	delete [] currentNode->board;   		// delelte the gameboard
	delete currentNode;		 			    // delete currentNode
	currentNode = pTemp;                    // reassign pHead
}
void deleteLinkedList ( Node* &currentNode){
	while( currentNode != NULL){
		deleteNode(currentNode);
	}
}
void addNewNode       ( Node* &currentNode){
	Node* newNode = new Node;
	if( newNode == NULL){
		cout << "\nError: Failed to allocate new node.\nExiting...\n";
		exit(EXIT_FAILURE);
	}

	if( currentNode != NULL) { 			     // if list is not empty
		copyNode( currentNode, newNode );    // make copy of node
		newNode->moveCount = currentNode->moveCount + 1;  // update moveCount
	}

	newNode->pPrev = currentNode;            // insert node at front of list
	currentNode = newNode; 				     // make currentNode point to front of list
}
void undoLastMove     ( Node* &currentNode){
	deleteNode(currentNode);  			// deleting the latest node undos 1 move
}
/*************************************************************************************
*                        END OF LINKED LIST FUNCTIONS
*************************************************************************************/




/*************************************************************************************
*                        DISPLAY FUNCTIONS
*************************************************************************************/
void displayIDInfo() {
    cout << "Gabriel A. Mendoza              \n";
    cout << "UIC CS 141, Spring 2014           \n";
    cout << "Welcome to 1024 and More. \n";
    cout << "This program is based off of Gabriele Cirulli's game online at \n";
    cout << "    bit.ly/great2048 \n";
    cout << " \n";
}//end displayIDInfo()
void displayInstructions() {
    cout << "For each move enter a direction as a letter key, as follows: \n"
         << "    W    \n"
         << "  A S D  \n"
         << "where A=left,W=up, D=right and S=down. \n"
         << "  \n"
         << "After a move, when two identical valued tiles come together they    \n"
         << "join to become a new single tile with the value of the sum of the   \n"
         << "two originals. This value gets added to the score.  On each move    \n"
         << "one new randomly chosen value of 2 or 4 is placed in a random open  \n"
         << "square.  User input of r restarts the game, x exits the game. User input  \n"
         << "of u undos the last move made."
         << "  \n";
}//end displayInstructions()
void displayBoardAndScore( Node* currentNode) {
	cout << "\nScore: " << currentNode->score ;    		// displaysScore

	// Set display width value depending on boardSize (and corresponding max value)
    int displayWidth;
    switch( currentNode->size) {
        case 4:
        case 5:
        case 6:
        case 7:  displayWidth = 5; break; // up to   8,192  // 4-7
        case 8:
        case 9:
        case 10: displayWidth = 6; break; // up to  65,536  // 8-10
        case 11:
        case 12: displayWidth = 7; break; // up to 262,144  // 11-12
        default:
             // Sanity check, should never get here
            cout << "Invalid boardSize of " << currentNode->size
                  << ". Exiting program..\n" << endl;
			exit( -1);
            break;
    }
	cout << "\n\n";
    for( int row=0; row< currentNode->size; row++) {
        cout << "   ";
        for( int col=0; col< currentNode->size; col++ ) {
            int current = currentNode->size * row + col;  // 1-d index corresponding to row & col

            // set output field size to 6 (Requires #include <iomanip> )
            cout << setw( displayWidth);
            // display '.' if board value is 0

            if( currentNode->board[ current] == 0) {
               cout << '.';
            }
            else {
               cout << currentNode->board[ current];
            }
        }
        cout << "\n\n";
    }
}//end displayBoard()
void displayAndInitializeGameData( Node* &currentNode, bool &moveMade, char moveChar){
	system("cls"); 								// clear the screen
	displayIDInfo(); 							// display name and other info
	displayInstructions();						// display game instructions

	if( currentNode == NULL) { 					// if start of game or if it was reset
		int boardSize = getSize();				// get board size
		addNewNode( currentNode );        		// add a node to empty list
		currentNode->size = boardSize; 			// set board size
		currentNode->moveCount = 1;			    // reset moveCount
		currentNode->score = 0;             	// reset score
		setBoard(currentNode);					// allocate board

		determineMaxTileValue(currentNode);		// determine the game winning tile value

		randNumOnBoard( currentNode);           // set random Numbers
		randNumOnBoard( currentNode);
	}

	if ( moveMade == true && moveChar != 'P'){ 	// if a valid move was made without manually
		randNumOnBoard( currentNode);			// inserting a number, put random number on board
	}

	moveMade = false;							// reset moveMade

	if( moveChar != '\0'){
		cout << "\nEnter the board size you want, between 4 and 12: " << currentNode->size << "\n";
	}
	cout << "Game ends when you reach " << currentNode->maxTileValue << "." << endl;

	displayBoardAndScore( currentNode );   		// self explanatory
}
/*************************************************************************************
*                        END OF DISPLAY FUNCTIONS
*************************************************************************************/




/*************************************************************************************
*                        ARTIFICIAL INTELLIGENCE FUNCTIONS
*************************************************************************************/
void artificialIntelligence (Node* &currentNode){
	possibleMoves( currentNode );

	int greatestTileChange[4] = {-1,-1,-1,-1};
	for( int i=0; i<4; i++){
		if( currentNode->validMoves[i] ) {
			Node* AI_node = new Node;
			copyNode( currentNode, AI_node);
			switch(i){
				case 0: greatestTileChange[i] = moveLeft (AI_node); break;
				case 1: greatestTileChange[i] = moveUp   (AI_node); break;
				case 2: greatestTileChange[i] = moveDown (AI_node); break;
				case 3:	greatestTileChange[i] = moveRight(AI_node); break;
			} // end of switch(i) {...

			delete [] AI_node->board;			   // free memory for board
			delete AI_node;					   // and node
		}
		else {
			greatestTileChange[i] = 0;
		}
	}

	int indexOfMax = -1;
	int max = 0;
	for(int i=0; i<4; i++){
        if( max < greatestTileChange[i]){
			max = greatestTileChange[i];
			indexOfMax = i;
        }
	}

	switch(indexOfMax){
		case 0: moveLeft (currentNode); break;
		case 1: moveUp   (currentNode); break;
		case 2: moveDown (currentNode); break;
		case 3:	moveRight(currentNode); break;
	} // end of switch(i) {...

	possibleMoves( currentNode );
}
/*************************************************************************************
*                        END OF ARTIFICIAL INTELLIGENCE FUNCTIONS
*************************************************************************************/


/*************************************************************************************
*                        MAIN GAME CODE
*************************************************************************************/
int  main() {
	srand(time(NULL));			// seeding random values

	Node* currentNode = NULL;   // declare pHead, will point to linked list game later

	int size = -1;              // to get the size at game start/reset
	char moveChar = '\0';		// to get the move character at game start/reset

	int gameEnd = GAME_NOT_OVER;
	bool wasMoveMade = false;
	bool undoCheck = false;

	do{
		// displays ID info
		// if start of game, or if game was reset, then it also initializes game data
		displayAndInitializeGameData(currentNode, wasMoveMade , moveChar);

		if( undoCheck ){
			cout << "Cannot undo at game start\n" ;
			undoCheck = false;
		}
		possibleMoves( currentNode ); 	   // DETERMINE POSSIBLE MOVES, SAVE TO STRUCT
        getUserInput(currentNode, moveChar);       // gets user input and acts accordingly
		switch( moveChar){
			case 'A':
				if( currentNode->validMoves[0] ){  // move is valid
					addNewNode( currentNode );     // add a node to non empty list
					moveLeft(currentNode);
					wasMoveMade = true;
				}
				break;
			case 'W':
				if( currentNode->validMoves[1] ){  // move is valid
					addNewNode( currentNode );     // add a node to non empty list
					moveUp(currentNode);
					wasMoveMade = true;
				}
				break;
			case 'S':
				if( currentNode->validMoves[2] ){  // move is valid
					addNewNode( currentNode );     // add a node to non empty list
					moveDown(currentNode);
					wasMoveMade = true;
				}
				break;
			case 'D':
				if( currentNode->validMoves[3] ){  // move is valid
					addNewNode( currentNode );     // add a node to non empty list
					moveRight(currentNode);
					wasMoveMade = true;
				}
				break;
			case 'P':
				manualInjection( currentNode );
				wasMoveMade = true;                // only for some display purposes
				break;
			case 'U':
				if( currentNode->pPrev == NULL ) {
					undoCheck = true;
				} else {
					undoLastMove( currentNode );   // delete latest node to undo move
				}
				break;
			case 'R':
				deleteLinkedList( currentNode );   // deletes list and restarts game
				moveChar = '\0';				   // reset moveChar
				break;
			case 'X':
				deleteLinkedList(currentNode);
				cout << "Thank you for playing. Exiting...\n";
				exit(-1);
				break;
			case 'I':
				int i = 0;
				while( i < 9 && gameEnd == GAME_NOT_OVER){
					displayAndInitializeGameData(currentNode, wasMoveMade, moveChar);
					artificialIntelligence (currentNode);
					gameEnd = checkGameEnd(currentNode);
					system("sleep 1");
					i++;
				}
				break;
		} // end of switch(moveChar) {}
		if(moveChar != '\0'){ 	                  // DETERMINE POSSIBLE MOVES, SAVE TO STRUCT
			possibleMoves( currentNode );
		}

		if( currentNode == NULL) {                // if game is reset, do not check game end
			gameEnd = GAME_NOT_OVER;
		} else {
			gameEnd = checkGameEnd(currentNode);
		}

	} while( gameEnd == GAME_NOT_OVER );

	// display board and info 1last time before terminating program
	displayAndInitializeGameData(currentNode, wasMoveMade, moveChar);

	deleteLinkedList( currentNode ); 			// free memory

	if( gameEnd == GAME_W0N) {
		cout << "\n\nCongratulations! You Won!\n";
		cout << "Thank you for playing. Exiting...\n";
	}
	else if ( gameEnd == GAME_LOST  ){
		cout << "\nGame Over! You Lost!\n";
		cout << "Thank you for playing. Exiting...\n";
	}
	else ;
	return 0;
}
/*************************************************************************************
*                        END OF MAIN GAME CODE
*************************************************************************************/
