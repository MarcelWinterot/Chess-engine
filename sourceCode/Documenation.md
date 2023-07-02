## Core functions
### ChessPosition()
#### This is the constructor function for creating a new ChessPosition object. The object represents a chess position.
```c++
thc::ChessPosition chessPos;
```

### Forsyth()
#### This function converts the current chess position into Forsyth-Edwards Notation (FEN), a standard notation for describing a particular board position of a chess game.
```c++
std::string fen = chessPos.Forsyth();
```

### ReadFen(const char *fen)
#### This function sets the current chess position from a Forsyth-Edwards Notation (FEN) string.
```c++
bool isFenValid = chessPos.ReadFen(fen.c_str());
```

### ToMove()
#### This function returns the color of the side to move next.
```c++
bool isWhiteToMove = chessPos.ToMove();
```

## Move functions
### ChessMove()
#### This is the constructor function for creating a new ChessMove object. The object represents a chess move.
```c++
thc::ChessMove chessMove;
```

### NaturalInFast(const char *natural)
#### This function sets the move from a string in natural coordinate notation (e.g., "e2e4").
```c++ 
bool isMoveValid = chessMove.NaturalInFast("e2e4");
```

### NaturalOut(char *natural)
#### This function gets the move as a string in natural coordinate notation.
```c++
char moveStr[6];
chessMove.NaturalOut(moveStr);
```

## Game functions
### ChessRules()
#### This is the constructor function for creating a new ChessRules object. The object represents a chess game with rules.
```c++
thc::ChessRules chessGame;
```

### PlayMove(ChessMove move)
#### This function plays a move in the current game position.
```c++
bool isMovePlayed = chessGame.PlayMove(chessMove);
```

### GenerateMoves()
#### This function generates all legal moves for the current game position.
```c++
std::vector<thc::ChessMove> legalMoves = chessGame.GenerateMoves();
```
