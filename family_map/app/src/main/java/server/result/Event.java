package server.result;

public class Event {
    public String descendant, eventID, personID, country, city, eventType, year;
    public float latitude, longitude;

    public server.model.Event makeModel() {
        return new server.model.Event(eventID, descendant, personID, latitude, longitude, country, city, eventType, year);
    }
}
