package server.model;

public class AuthToken {
    private static AuthToken instance = null;
    private AuthToken(){}

    public static AuthToken getInstance() {
        if (instance == null)
            instance = new AuthToken();
        return instance;
    }

    private String authToken, username;

    public void setAuthToken(String authToken) {
        this.authToken = authToken;
    }

    public void setUsername(String username) {
        this.username = username;
    }

    public String getAuthToken() {
        return authToken;
    }

    public String getUsername() {
        return username;
    }
}
