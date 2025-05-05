package org.example;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.Random;
import java.util.Set;

public class SnakeController {
    public final Set<Integer> pressedKeys = new HashSet<>();
    public int boardWidth;
    public int boardHeight;
    public int squareSize = 25;

    public Square snakeHead; // snake
    public ArrayList<Square> snakeBody;

    public Square food; // food
    public Random random;

    public int velocityX; //game logic
    public int velocityY;
    public boolean gameOver = false;

    public int maxScore = 0;

    public SnakeController(int boardWidth, int boardHeight) {
        this.boardWidth = boardWidth;
        this.boardHeight = boardHeight;

        snakeHead = new Square(5, 5);
        snakeBody = new ArrayList<>();

        food = new Square(0, 0);
        random = new Random();
        placeFood();

        velocityX = 1;
        velocityY = 0;

        updatePositions(); // инициализация позиций
    }

    public void placeFood() {
        food.x = random.nextInt(boardWidth / squareSize);
        food.y = random.nextInt(boardHeight / squareSize);
    }

    public boolean collision(Square square1, Square square2) { // столкновение
        return square1.x == square2.x && square1.y == square2.y;
    }

    public void move() {
        if (collision(snakeHead, food)) { // съела ли еду
            snakeBody.add(new Square(food.x, food.y)); // добавляем тело там, где была еда
            placeFood();
        }

        for (int i = snakeBody.size() - 1; i >= 0; i--) { // проходимся по каждому элементу тела, начиная с конца
            Square snakePart = snakeBody.get(i);
            if (i == 0) { // первый сегмент после головы
                snakePart.x = snakeHead.x;
                snakePart.y = snakeHead.y;
            } else {
                Square prevSnakePart = snakeBody.get(i - 1); // сегмент i получает координаты от i - 1 — от сегмента перед ним
                snakePart.x = prevSnakePart.x; // хвост идёт туда, где был предыдущий кусок тела
                snakePart.y = prevSnakePart.y;
            }
        }

        snakeHead.x += velocityX; // перемещаем голову
        snakeHead.y += velocityY;

        for (Square snakePart : snakeBody) {
            if (collision(snakeHead, snakePart)) { // змея столкнулась с самой собой
                gameOver = true;
            }
        }

        if (snakeHead.x < 0 || snakeHead.x * squareSize >= boardWidth || // выход за левую или правую границу
                snakeHead.y < 0 || snakeHead.y * squareSize >= boardHeight) { // выход за верхнюю или нижнюю границу
            gameOver = true;
        }
    }

    public void updatePositions() { // вызывается при завершении интерполяции, чтобы скопировать логические координаты в координаты отрисовки
        snakeHead.currentX = snakeHead.x;
        snakeHead.currentY = snakeHead.y;
        for (Square part : snakeBody) {
            part.currentX = part.x;
            part.currentY = part.y;
        }
    }

    public void resetGame() {
        snakeHead = new Square(5, 5);
        snakeBody.clear();
        velocityX = 1;
        velocityY = 0;
        gameOver = false;
        placeFood();
        updatePositions();
    }
}
