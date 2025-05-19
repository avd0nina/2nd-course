package org.example.Server;

import org.example.Message.Message;

import java.io.*;
import java.net.Socket;
import java.util.Objects;

public class UserThread extends Thread {
    private Socket socket;
    private ObjectInputStream in;
    private ObjectOutputStream out;
    private boolean isWorking;
    private String username;

    public UserThread(Socket socket) throws Exception {
        this.socket = socket;
        out = new ObjectOutputStream(socket.getOutputStream());
        out.flush();
        in = new ObjectInputStream(socket.getInputStream());
    }

    private void sendListMessages() {
        try {
            out.writeObject(Server.messages);
        }
        catch (Exception e) {
            System.err.println("Failed to send list of messages");
        }
    }

    private void sendUserList() throws IOException {
        StringBuilder str = new StringBuilder();
        for (int i = 0; i < Server.usersList.size(); i++) {
            System.out.println(Server.usersList.get(i));
            str.append(Server.usersList.get(i));
            str.append("$");
        }
        out.writeObject("userlist");
        out.writeObject(str);
        out.flush();
        synchronized (Server.logger) {
            Server.logger.sendListUsers();
        }
    }

    private void broadcastUserList() { // рассылка списка юзеров
        synchronized (Server.userThreads) {
            for (UserThread thread : Server.userThreads) {
                try {
                    if (thread != null) {
                        thread.sendUserList();
                    }
                } catch (Exception e) {
                    System.err.println("Failed to send user list");
                }
            }
        }
    }

    private void getNameUser() {
        try {
            username = (String) in.readObject();
            synchronized (Server.logger) { // synchronized обеспечивает, что только один поток за раз может выполнять код внутри блока (удерживает Server.logger)
                Server.logger.connectUser(username);
            }
            synchronized (Server.usersList) {
                Server.usersList.add(username);
            }
            broadcastUserList();
        } catch (Exception e) {
            System.err.println("Failed to get username");
        }
    }

    private void sendMessage(Message message) throws IOException {
        out.writeObject("message");
        out.writeObject(message);
        out.flush();
        synchronized (Server.logger) {
            Server.logger.sendMessage(message);
        }
    }

    private void broadcastMessage(Message message) {
        synchronized (Server.userThreads) {
            for (UserThread thread : Server.userThreads) {
                try {
                    if (thread != null) {
                        thread.sendMessage(message);
                    }
                } catch (Exception e) {
                    System.err.println("Failed to send message to client");
                }
            }
        }
    }

    private void stopWorking() {
        isWorking = false;
        try {
            if (socket != null) socket.close();
            if (in != null) in.close();
            if (out != null) out.close();
        }
        catch (IOException e) {
            System.err.println("Failed to close resources");
        }
        synchronized (Server.logger) {
            Server.logger.deleteUser(username);
        }
        synchronized (Server.usersList) {
            Server.usersList.remove(username);
        }
        synchronized (Server.userThreads) {
            Server.userThreads.remove(this);
        }
        broadcastUserList();
    }

    @Override
    public void run() {
        isWorking = true;
        sendListMessages();
        getNameUser();
        Message message;
        try {
            while (isWorking) {
                Object received = in.readObject();
                if (received instanceof Message) {
                    message = (Message) received;
                    synchronized (Server.logger) {
                        Server.logger.getMessage(message);
                    }
                    if (Objects.equals(message.getText(), "Disconnect") &&
                            Objects.equals(message.getUsername(), "System")) {
                        stopWorking();
                        break;
                    }
                    synchronized (Server.messages) {
                        Server.messages.add(message);
                    }
                    broadcastMessage(message);
                }
            }
        } catch (Exception e) {
            System.err.println("Client disconnected unexpectedly");
            stopWorking();
        }
    }
}
