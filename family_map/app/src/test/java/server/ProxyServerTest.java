package server;

import org.junit.Test;

import server.model.Events;
import server.model.People;
import server.model.User;
import server.result.Login;

import static org.junit.Assert.*;

public class ProxyServerTest {
    private String serverHost = "10.24.194.97", serverPort = "8889";

    @Test
    public void getInstanceTest() {
        assert(ProxyServer.getInstance() != null);
    }

    @Test
    public void setServerTest() {
        try {
            ProxyServer.getInstance().setServer("10.4.132.37", "8889");
        } catch (Exception e) {
            assert(false);
        }
        assertEquals("http://10.4.132.37:8889", ProxyServer.getInstance().getServerURL().toString());
    }

    @Test
    public void registerUserTest() {
        try {
            ProxyServer ps = ProxyServer.getInstance();
            ps.setServer(serverHost, serverPort);

            //non-unique username
            server.request.Register errorOne = new server.request.Register("sheila", "me", "g@g", "s", "p", "f");

            //missing value in request
            server.request.Register errorTwo = new server.request.Register("gb", "me", "g@g", null, "p", "m");

            server.result.Result resultOne = ps.registerUser(errorOne);
            server.result.Result resultTwo = ps.registerUser(errorTwo);

            assert(resultOne instanceof server.result.Error);
            assert(resultTwo instanceof server.result.Error);

            server.request.Register noError = new server.request.Register("gb", "me", "g@g", "g", "p", "m");

            server.result.Result resultThree = ps.registerUser(noError);
            assert(resultThree instanceof server.result.Login);
        } catch (Exception e) {
            assert(false);
        }
    }

    @Test
    public void loginUserTest() {
        try {
            ProxyServer ps = ProxyServer.getInstance();
            ps.setServer(serverHost, serverPort);

            //invalid username
            server.request.Login invalidUsername = new server.request.Login("sheil", "parker");
            server.result.Result errorOne = ps.loginUser(invalidUsername);
            assert(errorOne instanceof server.result.Error);

            //invalid password
            server.request.Login invalidPassword = new server.request.Login("sheila", "parke");
            server.result.Result errorTwo = ps.loginUser(invalidPassword);
            assert(errorTwo instanceof server.result.Error);

            //Login result
            server.request.Login correct = new server.request.Login("sheila", "parker");
            server.result.Result noError = ps.loginUser(correct);
            assert(noError instanceof server.result.Login);
        } catch (Exception e) {
            assert(false);
        }
    }

    @Test
    public void retrieveUserFamilyDataTest() {
        try {
            ProxyServer ps = ProxyServer.getInstance();
            ps.setServer(serverHost, serverPort);

            People people = People.getInstance();
            Events events = Events.getInstance();
            User user = User.getInstance();

            people.destroy();
            events.destroy();

            assert(people.getData() == null);
            assert(events.getData() == null);

            //wrong auth token
            user.setAuthToken("thisIsNotValid");
            server.result.Result invalidAuthToken = ps.retrieveUserFamilyData();
            assert(invalidAuthToken instanceof server.result.Error);

            //valid auth token
            server.request.Login request = new server.request.Login("sheila", "parker");
            server.result.Result result = ps.loginUser(request);
            String validAuth = ((Login)result).authToken;
            user.setAuthToken(validAuth);
            ps.retrieveUserFamilyData();

            assert(people.getData() != null);
            assert(events.getData() != null);

        } catch (Exception e) {
            assert(false);
        }
    }
}