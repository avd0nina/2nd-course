package org.example;

public class Square {
    public int x;
    public int y;
    public float currentX; // позиция для интерполяции
    public float currentY;

    public Square(int x, int y) {
        this.x = x;
        this.y = y;
        this.currentX = x;
        this.currentY = y;
    }
}
