package client.familymap;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

import server.model.Events;

public class Filter {
    private static Filter instance;

    private Filter() {
        filters = new HashMap<>();

        List<String> eventTypes = Events.getInstance().getEventTypes();
        for (String s : eventTypes)
            filters.put(s, true);
    }

    public static Filter getInstance() {
        if (instance == null)
            instance = new Filter();
        return instance;
    }

    private Map<String, Boolean> filters;

    public void update(String key, boolean newValue) {
        filters.put(key, newValue);
    }

    public boolean valueOf(String key) {
        return filters.get(key);
    }

    public List<String> getKeys() {
        Set<String> eventTypes = filters.keySet();

        ArrayList<String> keys = new ArrayList<>();
        keys.addAll(filters.keySet());

        //This is to present them in the correct order.
        keys.remove("Father's Side");
        keys.remove("Mother's Side");
        keys.remove("Male Events");
        keys.remove("Female Events");

        keys.add("Father's Side");
        keys.add("Mother's Side");
        keys.add("Male Events");
        keys.add("Female Events");

        return keys;
    }

    public void destroy() {
        instance = null;
    }
}
