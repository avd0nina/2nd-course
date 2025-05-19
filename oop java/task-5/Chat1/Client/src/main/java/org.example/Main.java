package org.example;

import org.example.Client.ClientThread;
import org.example.View.ClientWindow;
import org.example.View.ClientWindowListener;

import javax.swing.*;

public class Main {
    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> {
            ClientWindow window = new ClientWindow();
            String username = JOptionPane.showInputDialog(window, "Введите ваше имя:");
            if (username == null || username.isEmpty()) {
                System.exit(0);
            }
            ClientThread clientThread = new ClientThread(username, window);
            clientThread.start();
            window.setListener(new ClientWindowListener() {
                @Override
                public void sendMessage(String messageText) {
                    clientThread.sendMessageFromGUI(messageText);
                }
                @Override
                public void stopWorking() {
                    clientThread.stopWorking();
                }
            });
        });
    }
}
