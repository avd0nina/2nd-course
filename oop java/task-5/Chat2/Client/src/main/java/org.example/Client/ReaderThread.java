package org.example.Client;

import org.example.Message.Message;
import org.example.View.ClientWindow;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.xml.sax.InputSource;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import java.io.DataInputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.StringReader;
import java.util.ArrayList;
import java.util.Objects;

public class ReaderThread extends Thread {
    private DataInputStream in;
    private ClientWindow window;
    private boolean isWorking;

    public ReaderThread(DataInputStream in, ClientWindow window) {
        this.in = in;
        this.window = window;
    }

    public void stopWorking() {
        isWorking = false;
    }

    private String readXml() throws IOException {
        int length = in.readInt();
        byte[] buf = new byte[length];
        in.readFully(buf);
        return new String(buf, "UTF-8");
    }

    private Document parseXml(String xml) throws Exception {
        DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
        DocumentBuilder builder = factory.newDocumentBuilder();
        return builder.parse(new InputSource(new StringReader(xml)));
    }

    private void processMessage(Document document) {
        Element cmd = document.getDocumentElement();
        String name = cmd.getAttribute("name");
        if (Objects.equals(name, "message")) {
            String text = cmd.getElementsByTagName("text").item(0).getTextContent();
            String time = cmd.getElementsByTagName("time").item(0).getTextContent();
            String username = cmd.getElementsByTagName("username").item(0).getTextContent();
            window.addMessage(new Message(username, text, time));
        }
        else if (Objects.equals(name, "userlist")) {
            ArrayList<String> usernames = new ArrayList<>();
            var userNodes = cmd.getElementsByTagName("user");
            for (int i = 0; i < userNodes.getLength(); i++) {
                String user = userNodes.item(i).getTextContent();
                if (user != null && !user.isEmpty()) {
                    usernames.add(user);
                }
            }
            window.updateUserList(usernames);
        }
        else if(Objects.equals(name, "messagelist")) {
            var textNodes = cmd.getElementsByTagName("text");
            var userNodes = cmd.getElementsByTagName("username");
            var timeNodes = cmd.getElementsByTagName("time");
            for (int i = 0; i < textNodes.getLength(); i++) {
                String text = textNodes.item(i).getTextContent();
                String username = userNodes.item(i).getTextContent();
                String time = timeNodes.item(i).getTextContent();
                if (username != null && text != null && time != null) {
                    Message message = new Message(username, text, time);
                    window.addMessage(message);
                }
            }
        }
    }

    @Override
    public void run() {
        isWorking = true;
        while (isWorking) {
            try {
                Document doc = parseXml(readXml());
                processMessage(doc);
            }
            catch (Exception e) {
                throw new RuntimeException(e);
            }
        }
    }
}
