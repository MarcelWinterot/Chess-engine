from chess import *

board = Board()
pieceValues = {
    "P": 1, "N": 3, "B": 3, "R": 5, "Q": 9, "K": 100_000,
    "p": -1, "n": -3, "b": -3, "r": -5, "q": -9, "k": -100_000
}

def hangingPieces(ply, board, currentValue, values=pieceValues):
    if ply == 0:
        return currentValue

    for move in board.legal_moves:
        capturedPiece = board.piece_at(move.to_square)
        if capturedPiece:
            capturingPiece = board.piece_at(move.from_square)
            if capturingPiece:
                safetyValue = 1 if board.is_attacked_by(not board.turn, move.to_square) else 0
                captureValue = values[capturedPiece.symbol()]
                currentValue += captureValue - safetyValue * captureValue
                hypotheticalBoard = board.copy()
                hypotheticalBoard.remove_piece_at(move.to_square)
                currentValue = hangingPieces(ply-1, hypotheticalBoard, currentValue)

    return currentValue


def mobility(board, multiplier = 0.1):
    wM = 0
    for _ in board.legal_moves:
        wM += 1

    board.turn = not board.turn 
    bM = 0
    for _ in board.legal_moves:
        bM += 1

    board.turn = not board.turn 
    return (wM - bM) * multiplier

def evaluate(board, values=pieceValues):
    value = 0

    # Counting pieces
    for square in range(64):
        piece = board.piece_at(square)
        if piece:
            value += values[piece.symbol()]

    # Hanging pieces
    value += hangingPieces(2, board, 0)

    # Mobility
    value += mobility(board)

    return value

def minMax(board, depth, bestMove, bestScore, maximingPlayer):
    if depth == 0 or board.is_game_over():
        return bestMove
    
    if maximingPlayer:
        for move in board.legal_moves:
            hypotheticalBoard = board.copy()
            hypotheticalBoard.push(move)
            score = minMax(board, depth-1, move, evaluate(hypotheticalBoard), False)
            if score > bestScore:
                bestMove, bestScore = move, score

        return minMax(board, depth-1, bestMove, bestScore, True)

    else:
        opponentBestScore = -100_000
        for opponentMove in board.legal_moves:
            hypotheticalBoard = board.copy()
            hypotheticalBoard.push(opponentMove)
            opponentScore = evaluate(hypotheticalBoard)
            if opponentScore > opponentBestScore:
                opponentBestScore = opponentScore
            
        print(f"Opponents best score is {opponentBestScore} after white's move {bestMove}")
        return opponentBestScore

"""
TODO:
1. Add pawn structure to evaluate (don't know if I will do it)
2. Finish minMax -- Working on
3. Change minMax to alpha beta
4. Add move ordering
5. Convert this into an api
6. Make a web application for it that sends requests to api

DONE:
1. Fixed hangingPieces - 17.04.2023
2. Added mobility to evaluate function - 17.04.2023
"""
