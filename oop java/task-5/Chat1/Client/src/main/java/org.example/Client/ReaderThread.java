package org.example.Client;

import org.example.Message.Message;
import org.example.View.ClientWindow;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.util.ArrayList;
import java.util.Objects;

public class ReaderThread extends Thread {
    public static Boolean usersMessage = false;
    private ObjectInputStream in;
    private ClientWindow window;
    private boolean isWorking;

    public ReaderThread(ObjectInputStream in, ClientWindow window) {
        this.in = in;
        this.window = window;
    }

    private void getListNames() {
        try {
            ArrayList<String> usernames = new ArrayList<>();
            StringBuilder str = (StringBuilder) in.readObject();
            StringBuilder word = new StringBuilder();
            for(int i = 0; i < str.length(); i++) {
                Character elem = str.charAt(i);
                if (elem.equals('$')) {
                    usernames.add(word.toString());
                    word.setLength(0);
                }
                else {
                    word.append(elem);
                }
            }
            for (String username : usernames) {
                System.out.println(username);
            }
            window.updateUserList(usernames);
        }
        catch (Exception e) {
            System.err.println("Failed get array messages");
        }
    }

    public void stopWorking() {
        isWorking = false;
    }

    @Override
    public void run() {
        isWorking = true;
        while (true) {
            try {
                String answer = (String)in.readObject();
                if(Objects.equals(answer, "message")) {
                    Message messageAnswer = (Message)in.readObject();
                    window.addMessage(messageAnswer);
                    System.out.println("Cообщение :" + messageAnswer.getText());
                }
                else if(Objects.equals(answer, "userlist")) {
                    getListNames();
                }
            }
            catch (Exception e) {
                throw new RuntimeException(e);
            }
        }
    }
}
