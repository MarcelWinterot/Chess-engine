from chess import *
import time
from functools import lru_cache

board = Board()
pieceValues = {
    "P": 1, "N": 3, "B": 3, "R": 5, "Q": 9, "K": 100_000,
    "p": -1, "n": -3, "b": -3, "r": -5, "q": -9, "k": -100_000
}

def evaluate(board, values=pieceValues):
    value = 0

    # Material count
    for square in range(64):
        piece = board.piece_at(square)
        if piece:
            value += values[piece.symbol()]

    # Piece activity
    value += piece_activity_bonus(board)

    return value

def piece_activity_bonus(board):
    # Piece activity bonus
    piece_activity = 0
    for square in SquareSet(BB_ALL):
        piece = board.piece_at(square)
        if not piece:
            continue
        piece_activity += 0.01 * len(board.attacks(square))
    return piece_activity

def alphaBeta(board, depth, alpha, beta, maximizingPlayer, evaluateFunc):
    if depth == 0 or board.is_game_over():
        return evaluateFunc(board)

    if maximizingPlayer:
        maxEval = float('-inf')
        for move in board.legal_moves:
            board.push(move)
            eval = alphaBeta(board, depth-1, alpha, beta, False, evaluateFunc)
            board.pop()
            maxEval = max(maxEval, eval)
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

def getBestMove(board, maxDepth, evaluateFunc, whoAsked, timeLimit=10.0):
    if whoAsked == 'white':
        startTime = time.time()
        bestMove = None
        maxEval = float('-inf')
        alpha = float('-inf')
        beta = float('inf')
        for depth in range(1, maxDepth):
            for move in board.legal_moves:
                board.push(move)
                eval = alphaBeta(board, depth-1, alpha, beta, True, evaluateFunc)
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
    else:
        startTime = time.time()
        bestMove = None
        minEval = float('inf')
        alpha = float('-inf')
        beta = float('inf')
        for depth in range(1, maxDepth):
            for move in board.legal_moves:
                board.push(move)
                eval = alphaBeta(board, depth-1, alpha, beta, False, evaluateFunc)
                board.pop()
                if eval < minEval:
                    minEval = eval
                    bestMove = move
                beta = min(beta, eval)
                elapsedTime = time.time() - startTime
                if elapsedTime >= timeLimit:
                    return bestMove
            elapsedTime = time.time() - startTime

            if elapsedTime >= timeLimit:
                break
        return bestMove

with open('Games.txt', 'r') as f:
    games = f.read().split('\n\n')

def openingBook(movesPlayed, games, board, side):
    for game in games:
        if movesPlayed in game:
            moveIndex = game.index(movesPlayed) + len(movesPlayed) + 1
            if moveIndex >= len(game):
                return getBestMove(board, 3, evaluate, side), False
            nextMove = game[moveIndex:].split()[0]
            return nextMove, True
    return getBestMove(board, 7, evaluate, side), False

def gameLoop(board, movesPlayed, side):
    if movesPlayed != '':
        movesPlayed = '1. ' + movesPlayed
    else:
        movesPlayed = '1.'
    if side == 'white':
        while not board.is_game_over():
            if board.turn:
                move = input('Your move: ')
                board.push_san(move)
                movesPlayed += ' ' + move
                print(movesPlayed)
            else:
                move, isSan = openingBook(movesPlayed, games, board, side)
                if isSan:
                    board.push_san(move)
                else:
                    board.push(move)
                print(move)
                movesPlayed += ' ' + str(move)

    else:
        while not board.is_game_over():
            if board.turn:
                move, isSan = openingBook(movesPlayed, games, board, side)
                if isSan:
                    board.push_san(move)
                else:
                    board.push(move)
                print(move)
                movesPlayed += ' ' + move
            else:
                move = input('Your move: ')
                board.push_san(move)
                movesPlayed += ' ' + str(move)
                print(movesPlayed)

board.set_fen('r1bqk2r/ppppbppp/2nn4/1B2N3/8/8/PPPP1PPP/RNBQR1K1 w kq - 0 1')
board.push_san('Nxc6')
print(board)
print(openingBook('1. e4 e5 Nf3 Nc6 Bb5 Nf6 O-O Nxe4 Re1 Nd6 Nxe5 Be7 Nc6', games, board, 'black'))