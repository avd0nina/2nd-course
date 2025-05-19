package org.example.View;

import javax.swing.*;
import java.awt.*;
import java.util.List;

public class UserListPanel extends JPanel {
    private DefaultListModel<String> userListModel;
    private JList<String> userList;
    private String currentUsername;

    public UserListPanel() {
        setLayout(new BorderLayout());
        setBorder(BorderFactory.createTitledBorder("Users"));
        userListModel = new DefaultListModel<>();
        userList = new JList<>(userListModel);
        userList.setFont(new Font("Arial", Font.PLAIN, 14));
        JScrollPane scrollPane = new JScrollPane(userList);
        add(scrollPane, BorderLayout.CENTER);
    }

    public void updateUsers(List<String> users) {
        userListModel.clear();
        for (String user : users) {
            if(user != null) {
                userListModel.addElement(user);
            }
        }
        highlightCurrentUser();
    }

    public void setCurrentUsername(String username) {
        this.currentUsername = username;
        highlightCurrentUser();
    }

    private void highlightCurrentUser() {
        if (currentUsername != null) {
            for (int i = 0; i < userListModel.getSize(); i++) {
                String user = userListModel.getElementAt(i);
                if (user.equals(currentUsername)) {
                    userList.setSelectionBackground(new Color(180, 112, 111, 70));
                    userList.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
                    userList.setSelectedIndex(i);
                    break;
                }
            }
        }
    }
}
