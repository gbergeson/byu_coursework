package server.model;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;
import java.util.Set;
import java.util.TreeSet;

public class Events {
    private static Events instance = null;
    private Events(){}

    public static Events getInstance() {
        if (instance == null)
            instance = new Events();
        return instance;
    }

    private Event[] data;

    public Event[] getData() {
        return data;
    }

    public void setData(Event[] data) {
        this.data = data;
    }

    public Event getEvent(String EventId) {
        for(Event p : data) {
            if (p.getEventID().equals(EventId))
                return p;
        }
        return null;
    }

    public Event[] getEvents(String personId) {
        ArrayList<Event> events = new ArrayList<>();

        for (Event e : data) {
            if (e.getPersonID().equals(personId))
                events.add(e);
        }

        events = sort(events);

        return events.toArray(new Event[events.size()]);
    }

    private ArrayList<Event> sort(ArrayList<Event> events) {
        Collections.sort(events, new SortByYear());

        for (int i = 0; i < events.size(); i++) {
            if (events.get(i).getEventType().equals("birth")) {
                Event temp = events.get(i);
                events.remove(i);
                events.add(0, temp);
            }
            if (events.get(i).getEventType().equals("death")) {
                Event temp = events.get(i);
                events.remove(i);
                events.add(temp);
            }
        }

        return events;
    }

    public List<String> getEventTypes() {
        Set<String> databaseEventTypes = new TreeSet<>();

        for (Event e : data)
            databaseEventTypes.add(capitalizeFirstLetter(e.getEventType()) + " Events");

        List<String> eventTypes = new ArrayList<>();

        eventTypes.addAll(databaseEventTypes);

        eventTypes.add("Father's Side");
        eventTypes.add("Mother's Side");
        eventTypes.add("Male Events");
        eventTypes.add("Female Events");

        return eventTypes;
    }

    private String capitalizeFirstLetter(String s) {
        return s.substring(0, 1).toUpperCase() + s.substring(1);
    }

    public boolean isMothersSide(Event e) {
        return People.getInstance().isMothersSide(e.getPersonID());
    }

    public boolean isFathersSide(Event e) {
        return People.getInstance().isFathersSide(e.getPersonID());
    }

    public List<Event> search(String searchValue) {
        ArrayList<Event> results = new ArrayList<>();

        searchValue = searchValue.toLowerCase();

        for (Event e : data) {
            if (e.getCity() != null && e.getCity().toLowerCase().contains(searchValue))
                results.add(e);
            else if (e.getCountry() != null && e.getCountry().toLowerCase().contains(searchValue))
                results.add(e);
            else if (e.getEventType() != null && e.getEventType().contains(searchValue))
                results.add(e);
            else if (e.getYear() != null && e.getYear().contains(searchValue))
                results.add(e);
        }

        return results;
    }

    public void destroy() {
        instance = null;
    }

    class SortByYear implements Comparator<Event> {
        @Override
        public int compare(Event a, Event b) {
            if (a.getYear() == null)
                return 0;
            if (b.getYear() == null)
                return 0;
            return Integer.parseInt(a.getYear()) - Integer.parseInt(b.getYear());
        }
    }

    public void normalizeEventTypes() {
        for (Event e : data) {
            String eventType = e.getEventType();
            e.setEventType(eventType.toLowerCase());
        }
    }
}