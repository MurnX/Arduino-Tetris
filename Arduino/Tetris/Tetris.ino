#include <Arduino.h>
#include "Charliplexing.h"

const int WIDTH = 9;
const int HEIGHT = 14;

const unsigned long DROP_MS = 600;
const unsigned long SOFT_DROP_MS = 70;
const unsigned long RENDER_MS = 20;

byte board[HEIGHT][WIDTH] = {};

int pieceX = 0;
int pieceY = 0;
int pieceType = 0;
int rotation = 0;
int score = 0;

bool gameOver = false;
bool softDrop = false;

unsigned long lastDrop = 0;
unsigned long lastRender = 0;

const byte shapes[7][4][4][2] = {
  {
    {{0,1},{1,1},{2,1},{3,1}},
    {{2,0},{2,1},{2,2},{2,3}},
    {{0,2},{1,2},{2,2},{3,2}},
    {{1,0},{1,1},{1,2},{1,3}}
  },
  {
    {{1,0},{2,0},{1,1},{2,1}},
    {{1,0},{2,0},{1,1},{2,1}},
    {{1,0},{2,0},{1,1},{2,1}},
    {{1,0},{2,0},{1,1},{2,1}}
  },
  {
    {{1,0},{0,1},{1,1},{2,1}},
    {{1,0},{1,1},{2,1},{1,2}},
    {{0,1},{1,1},{2,1},{1,2}},
    {{1,0},{0,1},{1,1},{1,2}}
  },
  {
    {{1,0},{2,0},{0,1},{1,1}},
    {{1,0},{1,1},{2,1},{2,2}},
    {{1,1},{2,1},{0,2},{1,2}},
    {{0,0},{0,1},{1,1},{1,2}}
  },
  {
    {{0,0},{1,0},{1,1},{2,1}},
    {{2,0},{1,1},{2,1},{1,2}},
    {{0,1},{1,1},{1,2},{2,2}},
    {{1,0},{0,1},{1,1},{0,2}}
  },
  {
    {{0,0},{0,1},{1,1},{2,1}},
    {{1,0},{2,0},{1,1},{1,2}},
    {{0,1},{1,1},{2,1},{2,2}},
    {{1,0},{1,1},{0,2},{1,2}}
  },
  {
    {{2,0},{0,1},{1,1},{2,1}},
    {{1,0},{1,1},{1,2},{2,2}},
    {{0,1},{1,1},{2,1},{0,2}},
    {{0,0},{1,0},{1,1},{1,2}}
  }
};

void setPixel(int x, int y, byte state) {
  if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
    LedSign::Set(HEIGHT - 1 - y, x, state);
  }
}

bool validPosition(int nx, int ny, int nr) {
  for (int i = 0; i < 4; i++) {
    int x = nx + shapes[pieceType][nr][i][0];
    int y = ny + shapes[pieceType][nr][i][1];

    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
      return false;
    }

    if (board[y][x] != 0) {
      return false;
    }
  }

  return true;
}

void drawBoard() {
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      setPixel(x, y, board[y][x]);
    }
  }
}

void drawPiece() {
  if (gameOver) return;

  for (int i = 0; i < 4; i++) {
    int x = pieceX + shapes[pieceType][rotation][i][0];
    int y = pieceY + shapes[pieceType][rotation][i][1];

    setPixel(x, y, 1);
  }
}

void render() {
  drawBoard();
  drawPiece();
}

void spawnPiece() {
  pieceType = random(0, 7);
  rotation = 0;
  pieceX = (WIDTH - 4) / 2;
  pieceY = 0;

  if (!validPosition(pieceX, pieceY, rotation)) {
    gameOver = true;
    Serial.println("GAMEOVER");
  }
}

void lockPiece() {
  for (int i = 0; i < 4; i++) {
    int x = pieceX + shapes[pieceType][rotation][i][0];
    int y = pieceY + shapes[pieceType][rotation][i][1];

    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
      board[y][x] = 1;
    }
  }
}

void clearLines() {
  int cleared = 0;
  int y = HEIGHT - 1;

  while (y >= 0) {
    bool full = true;

    for (int x = 0; x < WIDTH; x++) {
      if (board[y][x] == 0) {
        full = false;
        break;
      }
    }

    if (full) {
      cleared++;

      for (int row = y; row > 0; row--) {
        for (int x = 0; x < WIDTH; x++) {
          board[row][x] = board[row - 1][x];
        }
      }

      for (int x = 0; x < WIDTH; x++) {
        board[0][x] = 0;
      }
    } else {
      y--;
    }
  }

  score += cleared * 100;

  if (cleared > 0) {
    Serial.print("SCORE:");
    Serial.println(score);
  }
}

void moveDown() {
  if (validPosition(pieceX, pieceY + 1, rotation)) {
    pieceY++;
  } else {
    lockPiece();
    clearLines();
    spawnPiece();
  }
}

void restartGame() {
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      board[y][x] = 0;
    }
  }

  score = 0;
  gameOver = false;
  softDrop = false;

  spawnPiece();
  lastDrop = millis();

  Serial.println("READY");
  Serial.println("SCORE:0");
}

void handleCommand(char c) {
  if (c == 'S') {
    softDrop = true;
    return;
  }

  if (c == 's') {
    softDrop = false;
    return;
  }

  if (gameOver) {
    if (c == 'R') restartGame();
    return;
  }

  if (c == 'L') {
    if (validPosition(pieceX - 1, pieceY, rotation)) {
      pieceX--;
    }
  } else if (c == 'D') {
    if (validPosition(pieceX + 1, pieceY, rotation)) {
      pieceX++;
    }
  } else if (c == 'W') {
    int nr = (rotation + 1) % 4;

    if (validPosition(pieceX, pieceY, nr)) {
      rotation = nr;
    }
  } else if (c == 'N') {
    moveDown();
  } else if (c == 'F') {
    while (validPosition(pieceX, pieceY + 1, rotation)) {
      pieceY++;
    }

    moveDown();
  }
}

void setup() {
  Serial.begin(115200);

  LedSign::Init();
  randomSeed(analogRead(A0));

  spawnPiece();
  render();

  lastDrop = millis();
  lastRender = millis();

  Serial.println("READY");
  Serial.println("SCORE:0");
}

void loop() {
  while (Serial.available() > 0) {
    handleCommand((char)Serial.read());
  }

  unsigned long now = millis();
  unsigned long interval = softDrop ? SOFT_DROP_MS : DROP_MS;

  if (!gameOver && now - lastDrop >= interval) {
    moveDown();
    lastDrop = now;
  }

  if (now - lastRender >= RENDER_MS) {
    render();
    lastRender = now;
  }
}
