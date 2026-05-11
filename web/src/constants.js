export const SIZE = 9;

export const TURN = {
  BLACK: 0,
  WHITE: 1,
  BLACK_WIN: 2,
  WHITE_WIN: 3,
  DRAW: 4,
};

export const PIECE = {
  EMPTY: 0,
  BLACK: 1,
  WHITE: 2,
  KING: 3,
};

export const CAMPS_MASK = [
  [false, false, false, true, true, true, false, false, false],
  [false, false, false, false, true, false, false, false, false],
  [false, false, false, false, false, false, false, false, false],
  [true, false, false, false, false, false, false, false, true],
  [true, true, false, false, false, false, false, true, true],
  [true, false, false, false, false, false, false, false, true],
  [false, false, false, false, false, false, false, false, false],
  [false, false, false, false, true, false, false, false, false],
  [false, false, false, true, true, true, false, false, false],
];

export const THRONE = { x: 4, y: 4 };
