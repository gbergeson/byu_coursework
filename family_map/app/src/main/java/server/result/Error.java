package server.result;


public class Error extends Result {
    public String message;

    public Error(String message) {
        this.message = message;
    }
}
