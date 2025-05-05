package org.example;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.image.BufferedImage;
import java.util.Set;

public class SnakeGame extends JPanel implements ActionListener, KeyListener {
    SnakeController controller;
    Timer gameLoop;
    JButton restartButton; // кнопка рестарта
    BufferedImage buffer1;
    BufferedImage buffer2;
    BufferedImage activeBuffer;
    BufferedImage nextBuffer;
    private float interpolationProgress = 0.0f; // прогресс интерполяции (от 0.0 до 1.0)
    private final float interpolationSpeed = 0.3f; // скорость интерполяции

    public SnakeGame(int boardWidth, int boardHeight) {
        controller = new SnakeController(boardWidth, boardHeight);

        setPreferredSize(new Dimension(boardWidth, boardHeight)); // размер поля
        setBackground(Color.black);
        addKeyListener(this); // подключаем обработку клавиш
        setFocusable(true); // разрешаем панели слушать клавиши
        setLayout(null); // нужно для точного позиционирования кнопки

        buffer1 = new BufferedImage(boardWidth, boardHeight, BufferedImage.TYPE_INT_ARGB); // инициализация буферов
        buffer2 = new BufferedImage(boardWidth, boardHeight, BufferedImage.TYPE_INT_ARGB);
        activeBuffer = buffer1;
        nextBuffer = buffer2;

        Graphics2D g2d = activeBuffer.createGraphics(); // отрисовка начального кадра в activeBuffer
        draw(g2d);
        g2d.dispose();

        gameLoop = new Timer(10, this);
        gameLoop.start();

        restartButton = new JButton("Restart"); // создание кнопки
        restartButton.setFocusable(false);
        restartButton.setBounds(boardWidth / 2 - 60, boardHeight / 2 - 20, 120, 40); // по центру
        restartButton.setVisible(false);
        restartButton.addActionListener(e -> resetGame());
        this.add(restartButton);

        requestFocusInWindow();
    }

    public void draw(Graphics g) {
        Graphics2D g2d = (Graphics2D) g;
        g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
        // Полная очистка буфера черным цветом
        g2d.setColor(Color.black);
        g2d.fillRect(0, 0, controller.boardWidth, controller.boardHeight);

        g2d.setColor(Color.darkGray); // сетка
        for (int i = 0; i < controller.boardWidth / controller.squareSize; i++) {
            g2d.drawLine(i * controller.squareSize, 0, i * controller.squareSize, controller.boardHeight); // вертикальная линия
            g2d.drawLine(0, i * controller.squareSize, controller.boardWidth, i * controller.squareSize); // горизонтальная линия
        }

        g2d.setColor(Color.red); // еда
        g2d.fillOval(
                controller.food.x * controller.squareSize,
                controller.food.y * controller.squareSize,
                controller.squareSize,
                controller.squareSize
        );

        g2d.setColor(new Color(0, 180, 0)); // тело змеи
        g2d.setStroke(new BasicStroke(controller.squareSize, BasicStroke.CAP_ROUND, BasicStroke.JOIN_ROUND)); // толщина линии

        float[] interpolatedX = new float[controller.snakeBody.size() + 1]; // массивы для интерполяционных координат
        float[] interpolatedY = new float[controller.snakeBody.size() + 1];

        // интерполяция головы змеи
        interpolatedX[0] = controller.snakeHead.currentX + (controller.snakeHead.x - controller.snakeHead.currentX) * interpolationProgress;
        interpolatedY[0] = controller.snakeHead.currentY + (controller.snakeHead.y - controller.snakeHead.currentY) * interpolationProgress;

        // интерполяция тела змеи
        for (int i = 0; i < controller.snakeBody.size(); i++) {
            Square part = controller.snakeBody.get(i);
            interpolatedX[i + 1] = part.currentX + (part.x - part.currentX) * interpolationProgress;
            interpolatedY[i + 1] = part.currentY + (part.y - part.currentY) * interpolationProgress;
        }

        // отрисовка линий между частями тела
        for (int i = 0; i < interpolatedX.length - 1; i++) {
            int x1 = (int) (interpolatedX[i] * controller.squareSize + controller.squareSize / 2); // преобразуем координаты клеток в пиксели, умножая на размер клетки
            int y1 = (int) (interpolatedY[i] * controller.squareSize + controller.squareSize / 2); // прибавляем squareSize / 2, чтобы попасть в центр клетки, а не в верхний левый угол
            int x2 = (int) (interpolatedX[i + 1] * controller.squareSize + controller.squareSize / 2);
            int y2 = (int) (interpolatedY[i + 1] * controller.squareSize + controller.squareSize / 2);
            g2d.drawLine(x1, y1, x2, y2);
        }

        g2d.setColor(Color.green); // голова змеи
        int headX = (int) (interpolatedX[0] * controller.squareSize);
        int headY = (int) (interpolatedY[0] * controller.squareSize);
        g2d.fillOval(headX, headY, controller.squareSize, controller.squareSize);

        g2d.setFont(new Font("Arial", Font.PLAIN, 16)); // очки
        g2d.setColor(Color.white);
        g2d.drawString("Score: " + controller.snakeBody.size(), controller.squareSize - 16, controller.squareSize);
        g2d.drawString("Max: " + controller.maxScore, controller.squareSize - 16, controller.squareSize + 20);

        if (controller.gameOver) {
            g2d.setColor(Color.red);
            g2d.drawString("Game Over", controller.boardWidth / 2 - 40, controller.boardHeight / 2 - 40);
        }
    }

    public void paintComponent(Graphics g) {
        super.paintComponent(g);
        g.drawImage(activeBuffer, 0, 0, this); // рисуем активный буфер на экран
    }

    @Override
    public void actionPerformed(ActionEvent e) {
        interpolationProgress += interpolationSpeed;
        if (interpolationProgress >= 1.0f) {
            interpolationProgress = 0.0f;
            controller.updatePositions(); // обновляем позиции
            controller.move(); // перемещаем змею
        }

        Graphics2D g2d = nextBuffer.createGraphics(); // рисуем следующую сцену в невидимый буфер
        draw(g2d);
        g2d.dispose();

        BufferedImage temp = activeBuffer; // переключаем буферы
        activeBuffer = nextBuffer;
        nextBuffer = temp;
        repaint();

        int snakeLength = controller.snakeBody.size(); // скорость змеи
        int delay = Math.max(20, 60 - (snakeLength * 10));
        gameLoop.setDelay(delay);

        if (controller.gameOver) {
            gameLoop.stop();
            if (controller.snakeBody.size() > controller.maxScore) {
                controller.maxScore = controller.snakeBody.size();
            }
            restartButton.setVisible(true); // показ кнопки рестарта
        }
    }

    public void resetGame() {
        controller.resetGame();
        interpolationProgress = 0.0f;
        restartButton.setVisible(false);
        gameLoop.setDelay(10);
        gameLoop.start();

        Graphics2D g2d = activeBuffer.createGraphics(); // рисуем начальный кадр после рестарта
        draw(g2d);
        g2d.dispose();
        repaint();
        requestFocus();
    }

    @Override
    public void keyPressed(KeyEvent e) {
        Set<Integer> pressedKeys = controller.pressedKeys;
        pressedKeys.add(e.getKeyCode());
        boolean up = pressedKeys.contains(KeyEvent.VK_UP);
        boolean down = pressedKeys.contains(KeyEvent.VK_DOWN);
        boolean left = pressedKeys.contains(KeyEvent.VK_LEFT);
        boolean right = pressedKeys.contains(KeyEvent.VK_RIGHT);
            // диагонали
        if (up && right && controller.velocityY != 1 && controller.velocityX != -1) {
            controller.velocityX = 1;
            controller.velocityY = -1;
        } else if (up && left && controller.velocityY != 1 && controller.velocityX != 1) {
            controller.velocityX = -1;
            controller.velocityY = -1;
        } else if (down && right && controller.velocityY != -1 && controller.velocityX != -1) {
            controller.velocityX = 1;
            controller.velocityY = 1;
        } else if (down && left && controller.velocityY != -1 && controller.velocityX != 1) {
            controller.velocityX = -1;
            controller.velocityY = 1;
            // обычные направления
        } else if (up && controller.velocityY != 1) {
            controller.velocityX = 0;
            controller.velocityY = -1;
        } else if (down && controller.velocityY != -1) {
            controller.velocityX = 0;
            controller.velocityY = 1;
        } else if (left && controller.velocityX != 1) {
            controller.velocityX = -1;
            controller.velocityY = 0;
        } else if (right && controller.velocityX != -1) {
            controller.velocityX = 1;
            controller.velocityY = 0;
        }
    }

    @Override
    public void keyReleased(KeyEvent e) {
        controller.pressedKeys.remove(e.getKeyCode());
    }

    @Override
    public void keyTyped(KeyEvent e) {}
}
