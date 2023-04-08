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


def evaluate(board, values):
    value = 0

    # Counting pieces
    for square in range(64):
        piece = board.piece_at(square)
        if piece:
            value += values[piece.symbol()]

    # Hanging pieces
    value += hangingPieces(2, board, 0)

    return value