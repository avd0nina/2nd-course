package org.example.Server;

import org.example.Message.Message;

import java.io.BufferedWriter;
import java.io.FileWriter;

public class Logger {
    private BufferedWriter writer;
    private String log;

    public Logger() {
        try {
            writer = new BufferedWriter(new FileWriter("/Users/a1234/Desktop/NSU/programming/2course/oop java/Chat1/Server/src/main/resources/log.log", true));
        }
        catch (Exception e) {
            System.err.println(e.getLocalizedMessage());
        }
    }

    private void writeLog() {
        try {
            writer.write(log);
            writer.newLine();
            writer.flush();
        }
        catch (Exception e) {
            System.err.println("Failed to write log to file");
        }
    }

    public void getMessage(Message message) {
        log = message.getSendingTime() + " : " + message.getUsername() + " get message";
        writeLog();
    }

    public void sendMessage(Message message) {
        log = message.getSendingTime() + " : " + message.getUsername() + " send message";
        writeLog();
    }

    public void connectUser(String user) {
        log = user + " connected";
        writeLog();
    }

    public void deleteUser(String user) {
        log = user + " deleted";
        writeLog();

    }

    public void sendListUsers() {
        log = "Send list users";
        writeLog();
    }
}
