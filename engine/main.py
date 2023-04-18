from chess import *
import time

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

def mobility(board):
    wM = len(list(board.legal_moves))

    board.turn = not board.turn
    bM = len(list(board.legal_moves))

    board.turn = not board.turn
    return (wM - bM) * 0.1


def evaluate(board, values=pieceValues):
    value = 0

    # Counting pieces
    for square in range(64):
        piece = board.piece_at(square)
        if piece:
            value += values[piece.symbol()]

    # Hanging pieces
    value -= hangingPieces(board)

    # Mobility
    value += mobility(board)

    return value

def alphaBeta(board, depth, alpha, beta, maximizingPlayer, evaluateFunc):
    if depth == 0 or board.is_game_over():
        return evaluateFunc(board)

    if maximizingPlayer:
        maxEval = float('-inf')
        for move in board.legal_moves:
            board.push(move)
            eval = alphaBeta(board, depth-1, alpha, beta, False, evaluateFunc)
            board.pop()
            max_eval = max(max_eval, eval)
            alpha = max(alpha, eval)
            if beta <= alpha:
                break
        return maxEval

    else:
        minEval = float('inf')
        for move in board.legal_moves:
            board.push(move)
            eval = alphaBeta(board, depth-1, alpha, beta, True, evaluateFunc)
            board.pop()
            minEval = min(minEval, eval)
            beta = min(beta, eval)
            if beta <= alpha:
                break
        return minEval

def getBestMove(board, maxDepth, evaluateFunc, timeLimit=10.0):
    startTime = time.time()
    bestMove = None
    maxEval = float('-inf')
    alpha = float('-inf')
    beta = float('inf')
    for depth in range(1, maxDepth):
        for move in board.legal_moves:
            board.push(move)
            eval = alphaBeta(board, depth-1, alpha, beta, False, evaluateFunc)
            board.pop()
            if eval > maxEval:
                maxEval = eval
                bestMove = move
            alpha = max(alpha, eval)
            elapsedTime = time.time() - startTime
            if elapsedTime >= timeLimit:
                return bestMove
        elapsedTime = time.time() - startTime

        if elapsedTime >= timeLimit:
            break
    return bestMove


"""
TODO:
1. Add pawn structure to evaluate (don't know if I will do it)
2. Change minMax to alpha beta -- Working on
3. Add move ordering
4. Convert this into an api
5. Make a web application for it that sends requests to api

DONE:
1. Fixed hangingPieces - 17.04.2023
2. Added mobility to evaluate function - 17.04.2023
"""

board.set_fen('rq5r/pb1pk2p/2n1pp1B/2P5/2P5/P1Q2N1P/5PP1/R3KB1R w KQ - 0 1')
print(board)
print(evaluate(board))
print(getBestMove(board, maxDepth=5, evaluateFunc=evaluate, timeLimit=30.0))