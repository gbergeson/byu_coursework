package server.model;

import android.content.Intent;

import org.junit.Before;
import org.junit.Test;

import java.util.List;

import server.ProxyServer;
import server.result.Login;

import static org.junit.Assert.*;

//NOTE: I cannot easily test that the events are being filtered correctly as that is happening
//in my custom map fragment. Believe me, however, I tested it well by trial and error.

public class EventsTest {
    @Before
    public void setUp() throws Exception {
        ProxyServer ps = ProxyServer.getInstance();

        //login user Sheila Parker and set the correct data in the Events singleton.
        server.request.Login loginReq = new server.request.Login("sheila", "parker");
        server.result.Result loginResult = ps.loginUser(loginReq);

        String authToken = ((Login)loginResult).authToken;
        String personID = ((Login)loginResult).personId;
        User.getInstance().setAuthToken(authToken);
        User.getInstance().setPersonId(personID);

        ps.retrieveUserFamilyData();
    }

    @Test
    public void getEvents() throws Exception {
        Events events = Events.getInstance();

        //test that Sheila's are ordered chronologically, birth is first, death is last
        String sheilaID = User.getInstance().getPersonId();
        Event[] sheilaEvents = events.getEvents(sheilaID);

        for (int i = 1; i < sheilaEvents.length; i++) {
            int first = Integer.parseInt(sheilaEvents[i].getYear());
            int second = Integer.parseInt(sheilaEvents[i-1].getYear());
            int compare = first - second;
            if (compare < 0)
                assert(false);
        }

        assertEquals("birth", sheilaEvents[0].getEventType());
        assertEquals("death", sheilaEvents[sheilaEvents.length - 1].getEventType());

        //test that Davis' birth is first (removed its year)
        Event[] davisEvents = events.getEvents("Davis_Hyer");
        assertEquals("birth", davisEvents[0].getEventType());

        //test that Betty's death is last (removed its year)
        Event[] bettyEvents = events.getEvents("Betty_White");
        assertEquals("death", bettyEvents[bettyEvents.length - 1].getEventType());
    }

    @Test
    public void search() throws Exception {
        Events events = Events.getInstance();

        List<Event> searchNone = events.search("z");
        assertEquals(0, searchNone.size());

        List<Event> searchCity = events.search("qaanaaq");
        assertEquals(2, searchCity.size());

        List<Event> searchCountry = events.search("united states");
        assertEquals(2, searchCountry.size());

        List<Event> searchEventType = events.search("birth");
        assertEquals(3, searchEventType.size());

        List<Event> searchYear = events.search("2014");
        assertEquals(2, searchYear.size());
    }

}