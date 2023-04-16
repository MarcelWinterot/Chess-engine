from chess import *

board = Board()
pieceValues = {
    "P": 1, "N": 3, "B": 3, "R": 5, "Q": 9, "K": 100_000,
    "p": -1, "n": -3, "b": -3, "r": -5, "q": -9, "k": -100_000
}

def hangingPieces(ply, board,currentValue, values=pieceValues):
    if ply == 0:
        return currentValue

    for move in board.legal_moves:
        capturedPiece = board.piece_at(move.to_square)
        if capturedPiece:
            currentValue -= values[capturedPiece.symbol()]
            global hypotheticalBoard;
            hypotheticalBoard = board.copy()
            hypotheticalBoard.push(move)

    return hangingPieces(ply-1, hypotheticalBoard, currentValue)

def evaluate(board, values=pieceValues):
    value = 0

    # Counting pieces
    for square in range(64):
        piece = board.piece_at(square)
        if piece:
            value += values[piece.symbol()]

    # Hanging pieces
    value += hangingPieces(2, board, 0)

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
1. Fix evaluation as hangingPieces isn't working correctly
2. Finish minMax
"""
