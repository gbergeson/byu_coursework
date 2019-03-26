package server.model;

public class Event {
    private String eventID, descendant, personID, country, city, eventType, year;
    private float latitude, longitude;

    public Event(String eventID, String descendant, String personID, float latitude, float longitude, String country, String city, String eventType, String year) {
        this.eventID = eventID;
        this.descendant = descendant;
        this.personID = personID;
        this.latitude = latitude;
        this.longitude = longitude;
        this.country = country;
        this.city = city;
        this.eventType = eventType;
        this.year = year;
    }

    void setEventType(String eventType) {
        this.eventType = eventType;
    }

    public String getPersonID() {
        return personID;
    }

    public String getDescendant() {
        return descendant;
    }

    public float getLatitude() {
        return latitude;
    }

    public float getLongitude() {
        return longitude;
    }

    public String getCity() {
        return city;
    }

    public String getCountry() {
        return country;
    }

    public String getEventID() {
        return eventID;
    }

    public String getEventType() {
        return eventType;
    }

    public String getYear() {
        return year;
    }
}
