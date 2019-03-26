package server.model;

import org.junit.Before;
import org.junit.Test;

import java.util.List;

import server.ProxyServer;
import server.result.Login;

import static org.junit.Assert.*;

public class PeopleTest {
    @Before
    public void setUp() throws Exception {
        ProxyServer ps = ProxyServer.getInstance();

        //login user Sheila Parker and set the correct data in the People singleton.
        server.request.Login loginReq = new server.request.Login("sheila", "parker");
        server.result.Result loginResult = ps.loginUser(loginReq);

        String authToken = ((Login)loginResult).authToken;
        String personID = ((Login)loginResult).personId;
        User.getInstance().setAuthToken(authToken);
        User.getInstance().setPersonId(personID);

        ps.retrieveUserFamilyData();
    }

    @Test
    public void getFamily() throws Exception {
        String sheilaID = User.getInstance().getPersonId();
        Person[] family = People.getInstance().getFamily(sheilaID);

        //Sheila's family is of size three (doesn't count her)
        assertEquals(3, family.length);

        //Her father has the PersonID Blaine_McGary
        for (Person p : family)
            if (p.getPersonID().equals("Blaine_McGary") && !p.getRelationship().equals("Father"))
                assert(false);

        //Her mother has the PersonID Betty_White
        for (Person p : family)
            if (p.getPersonID().equals("Betty_White") && !p.getRelationship().equals("Mother"))
                assert(false);

        //Her spouse has the PersonID Davis_Hyer
        for (Person p : family)
            if (p.getPersonID().equals("Davis_Hyer") && !p.getRelationship().equals("Spouse"))
                assert(false);

        String fatherID = People.getInstance().getPerson(sheilaID).getFather();
        Person[] extended = People.getInstance().getFamily(fatherID);

        //Sheila's father's family is of size two (doesn't count him)
        assertEquals(2, extended.length);

        //His spouse has the PersonID Betty_White
        for (Person p : extended)
            if (p.getPersonID().equals("Betty_White") && !p.getRelationship().equals("Spouse"))
                assert(false);

        //His child has the PersonID Sheila_Parker
        for (Person p : extended)
            if (p.getPersonID().equals("Sheila_Parker") && !p.getRelationship().equals("Child"))
                assert(false);
    }

    @Test
    public void search() throws Exception {
        People people = People.getInstance();

        List<Person> searchNone = people.search("z");
        assertEquals(0, searchNone.size());

        List<Person> searchAll = people.search("e");
        assertEquals(4, searchAll.size());

        List<Person> searchOne = people.search("Sheila Parker");
        assertEquals(1, searchOne.size());
    }

}