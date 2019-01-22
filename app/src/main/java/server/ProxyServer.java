package server;

import com.google.gson.Gson;
import com.google.gson.JsonSyntaxException;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.URL;

import server.model.Events;
import server.model.People;
import server.model.User;

public class ProxyServer {
    private static ProxyServer instance = null;
    //data member that contains the connection to the server
    private URL serverURL;

    private ProxyServer(){}

    public static ProxyServer getInstance() {
        if(instance == null) {
            instance = new ProxyServer();
        }
        return instance;
    }

    public void setServer(String serverHost, String serverPort) throws Exception {
        serverURL = new URL("http://" + serverHost+ ":" +serverPort);
    }

    public URL getServerURL() {
        return serverURL;
    }

    public server.result.Result registerUser(server.request.Register r) throws Exception {
        String json = new Gson().toJson(r);
        String registerURL = serverURL.toString() + "/user/register";
        URL regURL = new URL(registerURL);

        HttpURLConnection con = (HttpURLConnection) regURL.openConnection();
        con.setRequestMethod("POST");

        con.setDoOutput(true);
        OutputStream os = con.getOutputStream();
        os.write(json.getBytes());
        os.flush();
        os.close();

        int responseCode = con.getResponseCode();
        System.out.println("Register Response Code :: " + responseCode);

        server.result.Result serverResponse = null;

        if (responseCode == HttpURLConnection.HTTP_OK) {
            BufferedReader in = new BufferedReader(new InputStreamReader(con.getInputStream()));
            String inputLine;
            StringBuilder response = new StringBuilder();

            while ((inputLine = in.readLine()) != null) {
                response.append(inputLine);
            }
            in.close();

            String responseJson = response.toString();
            System.out.println(responseJson);

            if (isErrorResult(responseJson))
                serverResponse = new Gson().fromJson(responseJson, server.result.Error.class);
            else
                serverResponse = new Gson().fromJson(responseJson, server.result.Login.class);
        }

        return serverResponse;
    }

    public server.result.Result loginUser(server.request.Login r) throws Exception {
        String json = new Gson().toJson(r);
        String registerURL = serverURL.toString() + "/user/login";
        URL regURL = new URL(registerURL);

        HttpURLConnection con = (HttpURLConnection) regURL.openConnection();
        con.setRequestMethod("POST");

        con.setDoOutput(true);
        OutputStream os = con.getOutputStream();
        os.write(json.getBytes());
        os.flush();
        os.close();

        int responseCode = con.getResponseCode();
        System.out.println("Register Response Code :: " + responseCode);

        server.result.Result serverResponse = null;

        if (responseCode == HttpURLConnection.HTTP_OK) {
            BufferedReader in = new BufferedReader(new InputStreamReader(con.getInputStream()));
            String inputLine;
            StringBuilder response = new StringBuilder();

            while ((inputLine = in.readLine()) != null) {
                response.append(inputLine);
            }
            in.close();

            String responseJson = response.toString();
            System.out.println(responseJson);

            if (isErrorResult(responseJson))
                serverResponse = new Gson().fromJson(responseJson, server.result.Error.class);
            else
                serverResponse = new Gson().fromJson(responseJson, server.result.Login.class);
        }

        return serverResponse;
    }

    public server.result.Result retrieveUserFamilyData() throws Exception {
        String peopleURL = serverURL.toString() + "/person/";
        String peopleResponse = sendGetRequest(peopleURL);
        if (peopleResponse.equals(""))
            return new server.result.Error("Internal server error.");
        else if (isErrorResult(peopleResponse))
            return new Gson().fromJson(peopleResponse, server.result.Error.class);
        else {
            server.result.People peopleResult = new Gson().fromJson(peopleResponse, server.result.People.class);
            People.getInstance().setData(peopleResult.makeModel());
        }

        String eventsURL = serverURL.toString() + "/event/";
        String eventsResponse = sendGetRequest(eventsURL);
        if (eventsResponse.equals(""))
            return new server.result.Error("Internal server error.");
        if (isErrorResult(eventsResponse))
            return new Gson().fromJson(eventsResponse, server.result.Error.class);
        else {
            server.result.Events eventsResult = new Gson().fromJson(eventsResponse, server.result.Events.class);
            Events.getInstance().setData(eventsResult.makeModel());
        }

        return new server.result.Result();
    }

    private String sendGetRequest(String url) throws Exception {
        HttpURLConnection con = (HttpURLConnection) new URL(url).openConnection();
        con.setRequestMethod("GET");
        con.setRequestProperty("Authorization", User.getInstance().getAuthToken());

        int responseCode = con.getResponseCode();
        System.out.println("Register Response Code :: " + responseCode);

        if (responseCode == HttpURLConnection.HTTP_OK) {
            BufferedReader in = new BufferedReader(new InputStreamReader(con.getInputStream()));
            String inputLine;
            StringBuilder response = new StringBuilder();

            while ((inputLine = in.readLine()) != null) {
                response.append(inputLine);
            }
            in.close();

            return response.toString();
        }

        return "";
    }

    private boolean isErrorResult(String json) {
        server.result.Error temp = new Gson().fromJson(json, server.result.Error.class);
        return temp.message != null;
    }
}