package server.model;

public class User {
    private static User instance = null;
    private User(){}

    public static User getInstance() {
        if (instance == null)
            instance = new User();
        return instance;
    }

    private String authToken, userName, personId;
    private boolean loggedIn;

    public void setAuthToken(String authToken) {
        this.authToken = authToken;
    }

    public void setPersonId(String personId) {
        this.personId = personId;
    }

    public void setUserName(String userName) {
        this.userName = userName;
    }

    public String getUserName() {
        return userName;
    }

    public String getPersonId() {
        return personId;
    }

    public String getAuthToken() {
        return authToken;
    }

    public boolean isLoggedIn() {
        return loggedIn;
    }

    public void setLoggedIn(boolean value) {
        loggedIn = value;
    }

    public void destroy() {
        instance = null;
    }
}
