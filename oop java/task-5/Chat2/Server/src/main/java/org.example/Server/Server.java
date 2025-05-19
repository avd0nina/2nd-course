package org.example.Server;

import org.example.Message.Message;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;

public class Server {
    private final int port = 8080;
    private int countUsers;
    public static final ArrayList<String> usersList = new ArrayList<>();
    public static final ArrayList<Message> messages = new ArrayList<>();
    public static final ArrayList<UserThread> userThreads = new ArrayList<>();
    public static final Logger logger = new Logger();

    public Server() {
        countUsers = 0;
    }

    public void onServer() throws IOException {
        try (ServerSocket server = new ServerSocket(port)) {
            while (true) {
                try {
                    System.out.println("Server is waiting for connection...");
                    Socket socket = server.accept();
                    UserThread userThread = new UserThread(socket);
                    userThreads.add(userThread);
                    countUsers++;
                    System.out.println("Connection established");
                    userThread.start();
                } catch (Exception e) {
                    System.out.println("Failed to connect to server");
                }

            }
        } catch (Exception e) {
            System.err.println("Failed to start server");
        }
    }

}
